// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:private",
  };
  deps = {
    ":api-common",
    ":g8-api.capnp-cc",
    ":hooks",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:scope-exit",
    "//c8:vector",
    "//c8/io:capnp-messages",
    "//c8/stats:scope-timer",
    "//c8/string:format",
    "//c8/string:strcat",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0xc44e3519);

#include <capnp/pretty-print.h>
#include <git2.h>
#include <git2/oid.h>
#include <git2/refs.h>
#include <git2/types.h>

#include <algorithm>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/git/api-common.h"
#include "c8/git/hooks.h"
#include "c8/io/capnp-messages.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/format.h"
#include "c8/string/strcat.h"
#include "c8/vector.h"

namespace c8 {

using namespace g8;

ConstRootMessage<G8DiffResponse> g8Diff(G8DiffRequest::Reader req) {
  ScopeTimer t("api-diff");
  MutableRootMessage<G8DiffRequest> rewrittenRequestMessage(req);
  req = rewrittenRequestMessage.reader();
  MutableRootMessage<G8DiffResponse> response;

  if (!req.hasBasePoint() || !req.hasChangePoint()) {
    response.builder().getStatus().setCode(1);
    response.builder().getStatus().setMessage("Both basePoint and changePoint must be provided");
    return response;
  }

  gitRepositoryInit();
  SCOPE_EXIT([] { git_libgit2_shutdown(); });

  GIT_PTR(repository, repo);
  CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(repo));

  // Resolve the referenced commits (commit hash, special name, client name, changesetId, git
  // reference)
  GIT_PTR(tree, base);
  GIT_PTR(tree, change);
  CHECK_GIT(resolveTreeForChangePoint(repo, &base, req.getBasePoint().cStr()));
  CHECK_GIT(resolveTreeForChangePoint(repo, &change, req.getChangePoint().cStr()));

  // This will get us a list of files that have changed
  // CHECK_GIT(gitDiffTreeToTree(repo, base, change, true, response.builder().getFileList()));

  // Time to generate the diff!
  git_diff_options diffOpts = GIT_DIFF_OPTIONS_INIT;

  diffOpts.flags = GIT_DIFF_INCLUDE_UNTRACKED | GIT_DIFF_RECURSE_UNTRACKED_DIRS
    | GIT_DIFF_UPDATE_INDEX | GIT_DIFF_INCLUDE_TYPECHANGE | GIT_DIFF_SKIP_BINARY_CHECK
    | GIT_DIFF_INCLUDE_UNREADABLE;

  Vector<Vector<char>> paths;
  Vector<char *> pathPtrs;
  if (req.getPaths().size() > 0) {
    for (int i = 0; i < req.getPaths().size(); i++) {
      const char *str = req.getPaths()[i].cStr();
      const auto len = strlen(req.getPaths()[i].cStr());
      paths.emplace_back(len + 1);
      std::copy(str, str + len, begin(paths.back()));
      paths.back()[len] = 0;
    }
    for (auto i = 0; i < paths.size(); i++) {
      pathPtrs.push_back(paths[i].data());
    }
    diffOpts.pathspec.strings = pathPtrs.data();
    diffOpts.pathspec.count = pathPtrs.size();
  }

  GIT_PTR(diff, diff);
  GIT_PTR(diff_stats, stats);
  CHECK_GIT(git_diff_tree_to_tree(&diff, repo, base, change, &diffOpts));

  if (req.getFindRenamesAndCopies()) {
    git_diff_find_options diffFindOpts = GIT_DIFF_FIND_OPTIONS_INIT;
    diffFindOpts.flags = GIT_DIFF_FIND_FOR_UNTRACKED | GIT_DIFF_FIND_RENAMES | GIT_DIFF_FIND_COPIES;
    CHECK_GIT(git_diff_find_similar(diff, &diffFindOpts));
  }

  CHECK_GIT(git_diff_get_stats(&stats, diff));

  const int filesChanged = git_diff_stats_files_changed(stats);
  if (filesChanged > 0) {
    response.builder().initDiffList(filesChanged);
  } else {
    return response;
  }

  using PayloadType = G8DiffPayload;
  PayloadType payload;
  payload.baseTree = base;
  payload.changeTree = change;

  CHECK_GIT(git_diff_foreach(
    diff,
    [](const git_diff_delta *delta, float progress, void *pl) -> int {
      // build a list of file-level changes
      auto *payload = reinterpret_cast<PayloadType *>(pl);
      MutableRootMessage<G8FileInfo> info;

      GIT_PTR(tree_entry, baseEntry);
      GIT_PTR(tree_entry, changeEntry);
      if (0 == git_tree_entry_bypath(&baseEntry, payload->baseTree, delta->old_file.path)) {
        // 0 means we have an entry
        info.builder().setOldBlobId(git_oid_tostr_s(git_tree_entry_id(baseEntry)));
      }
      if (0 == git_tree_entry_bypath(&changeEntry, payload->changeTree, delta->new_file.path)) {
        info.builder().setBlobId(git_oid_tostr_s(git_tree_entry_id(changeEntry)));
      }

      info.builder().setStatus(g8FileStatusFromDeltaStatus(delta->status));
      if (info.builder().getStatus() == G8FileInfo::Status::DELETED) {
        info.builder().setPath(delta->old_file.path);
      } else {
        info.builder().setPath(delta->new_file.path);
      }

      if (
        info.builder().getStatus() == G8FileInfo::Status::RENAMED
        || info.builder().getStatus() == G8FileInfo::Status::COPIED) {
        info.builder().setPreviousPath(delta->old_file.path);
      }

      payload->entries[delta->new_file.path].info = std::move(info);
      return 0;
    },
    [](const git_diff_delta *delta, const git_diff_binary *binary, void *pl) -> int {
      auto *payload = reinterpret_cast<PayloadType *>(pl);
      MutableRootMessage<G8DiffLine> diffLine;
      diffLine.builder().setOrigin(G8DiffLine::Origin::BINARY);
      diffLine.builder().setContent("Binary files differ");
      payload->entries[delta->new_file.path].lines.push_back(std::move(diffLine));
      return 0;
    },
    nullptr,  // hunk callback
    [](const git_diff_delta *delta, const git_diff_hunk *hunk, const git_diff_line *line, void *pl)
      -> int {
      auto payload = reinterpret_cast<PayloadType *>(pl);
      MutableRootMessage<G8DiffLine> diffLine;
      diffLine.builder().setOrigin(diffLineOrigin(static_cast<git_diff_line_t>(line->origin)));
      diffLine.builder().setContent(String(line->content, line->content_len));
      diffLine.builder().setBaseLineNumber(line->old_lineno);
      diffLine.builder().setNewLineNumber(line->new_lineno);
      payload->entries[delta->new_file.path].lines.push_back(std::move(diffLine));
      return 0;
    },
    &payload));

  int idx{0};
  for (const auto &[path, diffPayload] : payload.entries) {
    auto diffResponse = response.builder().getDiffList()[idx++];
    diffResponse.setInfo(diffPayload.info->reader());

    int numLines = diffPayload.lines.size();
    diffResponse.initLines(numLines);
    for (int i = 0; i < numLines; i++) {
      diffResponse.getLines().setWithCaveats(i, diffPayload.lines[i].reader());
    }
  }

  return response;
}

}  // namespace c8
