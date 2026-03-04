// Copyright (c) 8th Wall, LLC
// Original Author: Pawel Czarnecki (pawel@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:private",
  };
  deps = {
    ":api-common",
    ":g8-api.capnp-cc",
    "//c8:c8-log",
    "//c8:scope-exit",
    "//c8:vector",
    "//c8/stats:scope-timer",
    "//c8/string:strcat",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0x0bc86e0b);

#include <git2.h>

#include <regex>

#include "c8/c8-log.h"
#include "c8/git/api-common.h"
#include "c8/stats/scope-timer.h"

namespace c8 {
using namespace g8;

ConstRootMessage<G8InspectResponse> g8Inspect(G8InspectRequest::Reader req) {
  ScopeTimer t("api-inspect");
  MutableRootMessage<G8InspectResponse> response;

  if (req.getInspectPoint().size() == 0 || req.getInspectRegex().size() == 0) {
    response.builder().getStatus().setCode(1);
    response.builder().getStatus().setMessage("Change point and inspect regex are required.");
    return response;
  }

  gitRepositoryInit();
  SCOPE_EXIT([] { git_libgit2_shutdown(); });

  GIT_PTR(repository, repo);
  CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(repo));

  struct TreeWalkResult {
    String path;
    String blobId;

    TreeWalkResult(const String &p, const String &id) : path(p), blobId(id) {}
  };

  struct TreeWalkPayload {
    std::regex inspectRegex;
    Vector<TreeWalkResult> walkResults;
  };

  TreeWalkPayload walkPayload;
  walkPayload.inspectRegex = req.getInspectRegex().cStr();

  GIT_PTR(tree, tree);
  CHECK_GIT(resolveTreeForChangePoint(repo, &tree, req.getInspectPoint().cStr()));
  CHECK_GIT(git_tree_walk(
    tree,
    GIT_TREEWALK_PRE,
    [](const char *root, const git_tree_entry *entry, void *payload) -> int {
      // When walking pre, we can skip going into a particular tree by returning a positive number.
      // TODO(pawel) Investigate this as a possible optimization.
      auto pl = reinterpret_cast<TreeWalkPayload *>(payload);

      if (git_tree_entry_type(entry) == GIT_OBJECT_BLOB) {
        String path = strCat(root, git_tree_entry_name(entry));
        if (std::regex_match(path, pl->inspectRegex)) {
          pl->walkResults.emplace_back(path, git_oid_tostr_s(git_tree_entry_id(entry)));
        }
      }

      return 0;
    },
    &walkPayload));

  response.builder().initInfo(walkPayload.walkResults.size());
  for (int i = 0; i < walkPayload.walkResults.size(); i++) {
    auto info = response.builder().getInfo()[i];
    info.setPath(walkPayload.walkResults[i].path);
    info.setBlobId(walkPayload.walkResults[i].blobId);
  }

  return response;
}
}  // namespace c8
