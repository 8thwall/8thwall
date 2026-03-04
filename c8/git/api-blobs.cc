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
cc_end(0xd5bfeb64);

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
#include "c8/string/format.h"
#include "c8/string/strcat.h"
#include "c8/vector.h"
#include "capnp/pretty-print.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

using namespace g8;

ConstRootMessage<G8CreateBlobResponse> g8CreateBlob(G8CreateBlobRequest::Reader req) {
  ScopeTimer t("api-create-blob");
  MutableRootMessage<G8CreateBlobResponse> response;

  gitRepositoryInit();
  SCOPE_EXIT([] { git_libgit2_shutdown(); });

  // Open the repo.
  GIT_PTR(repository, repo);
  CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

  // Set the repo root in the response.
  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(repo));

  // Create the blob.
  git_oid oid;
  CHECK_GIT(git_blob_create_from_buffer(&oid, repo, req.getData().begin(), req.getData().size()));

  // Set the blob id in the response.
  response.builder().setId(git_oid_tostr_s(&oid));

  return response;
}

ConstRootMessage<G8BlobResponse> g8GetBlob(G8BlobRequest::Reader req) {
  ScopeTimer t("api-get-blob");
  MutableRootMessage<G8BlobResponse> response;

  gitRepositoryInit();
  SCOPE_EXIT([] { git_libgit2_shutdown(); });

  // Open the repo.
  GIT_PTR(repository, repo);
  CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

  // Set the repo root in the response.
  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(repo));

  if (req.getIds().size() == 0) {
    GIT_PTR(blob, theBlob);
    git_oid oid;

    CHECK_GIT(git_oid_fromstrn(&oid, req.getId().cStr(), req.getId().size()));
    CHECK_GIT(git_blob_lookup(&theBlob, repo, &oid));

    if (git_blob_is_binary(theBlob)) {
      response.builder().setData("Binary file");
    } else {
      auto blobSize = git_blob_rawsize(theBlob);
      response.builder().initData(blobSize);
      memcpy(response.builder().getData().begin(), git_blob_rawcontent(theBlob), blobSize);
    }
  } else {
    response.builder().initBlobData(req.getIds().size());
    for (int i = 0; i < req.getIds().size(); ++i) {
      GIT_PTR(blob, theBlob);
      git_oid oid;

      CHECK_GIT(git_oid_fromstrn(&oid, req.getIds()[i].cStr(), req.getIds()[i].size()));
      CHECK_GIT(git_blob_lookup(&theBlob, repo, &oid));

      if (git_blob_is_binary(theBlob)) {
        response.builder().getBlobData().set(i, "Binary file");
      } else {
        auto blobSize = git_blob_rawsize(theBlob);
        response.builder().getBlobData().init(i, blobSize);
        memcpy(response.builder().getBlobData()[i].begin(), git_blob_rawcontent(theBlob), blobSize);
      }
    }
  }

  return response;
}

ConstRootMessage<G8DiffBlobsResponse> g8DiffBlobs(G8DiffBlobsRequest::Reader req) {
  ScopeTimer t("api-diff-blobs");
  MutableRootMessage<G8DiffBlobsResponse> response;

  gitRepositoryInit();
  SCOPE_EXIT([] { git_libgit2_shutdown(); });

  // Open the repo.
  GIT_PTR(repository, repo);
  CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

  // Set the repo root in the response.
  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(repo));

  GIT_PTR(blob, baseBlob);
  GIT_PTR(blob, newBlob);
  git_oid baseOid, newOid;
  CHECK_GIT(git_oid_fromstrn(&baseOid, req.getBaseId().cStr(), req.getBaseId().size()));
  CHECK_GIT(git_oid_fromstrn(&newOid, req.getNewId().cStr(), req.getNewId().size()));
  CHECK_GIT(git_blob_lookup(&baseBlob, repo, &baseOid));
  CHECK_GIT(git_blob_lookup(&newBlob, repo, &newOid));

  git_diff_options diffOpts = GIT_DIFF_OPTIONS_INIT;
  diffOpts.flags |= GIT_DIFF_MINIMAL;
  diffOpts.flags |= GIT_DIFF_PATIENCE;
  diffOpts.flags |= GIT_DIFF_INDENT_HEURISTIC;

  using LinesType = std::vector<MutableRootMessage<G8DiffLine>>;
  LinesType lines;
  CHECK_GIT(git_diff_blobs(
    baseBlob,
    nullptr,
    newBlob,
    nullptr,
    &diffOpts,
    nullptr,
    // Binary callback.
    [](const git_diff_delta *delta, const git_diff_binary *binary, void *pl) -> int {
      auto lines = reinterpret_cast<LinesType *>(pl);
      MutableRootMessage<G8DiffLine> diffLine;
      diffLine.builder().setOrigin(G8DiffLine::Origin::BINARY);
      diffLine.builder().setContent("Binary files differ");
      lines->push_back(std::move(diffLine));
      return 0;
    },
    nullptr,
    // Line callback.
    [](const git_diff_delta *delta, const git_diff_hunk *hunk, const git_diff_line *line, void *pl)
      -> int {
      auto lines = reinterpret_cast<LinesType *>(pl);
      MutableRootMessage<G8DiffLine> diffLine;
      diffLine.builder().setOrigin(diffLineOrigin(static_cast<git_diff_line_t>(line->origin)));
      diffLine.builder().setContent(String(line->content, line->content_len));
      diffLine.builder().setBaseLineNumber(line->old_lineno);
      diffLine.builder().setNewLineNumber(line->new_lineno);
      lines->push_back(std::move(diffLine));
      return 0;
    },
    &lines));

  response.builder().initLine(lines.size());

  for (int i = 0; i < lines.size(); ++i) {
    response.builder().getLine().setWithCaveats(i, lines[i].reader());
  }

  return response;
}

}  // namespace c8
