// Copyright (c) 2022 8th Wall, LLC.
// Author: Dale Ross (daleross@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "g8-plumbing.h",
  };
  deps = {
    "//c8/git:g8-api.capnp-cc",
    "//c8/git:g8-git",
    "//c8:string",
    ":g8-helpers",
    "//c8/io:capnp-messages",
  };
}
cc_end(0x1f609a6e);

#include "apps/g8/g8-helpers.h"
#include "apps/g8/g8-plumbing.h"
#include "c8/git/g8-git.h"
#include "c8/string.h"

using namespace c8;

ConstRootMessage<G8ChangesetResponse> listChangesets(
  const MainContext &ctx, const bool findRenames, const String &client = "HEAD") {
  MutableRootMessage<G8ChangesetRequest> req;
  setCredsForCwdRepo(req.builder().getAuth());
  req.builder().setRepo(repoPath());
  req.builder().setClient(client);
  req.builder().setIncludeSummary(true);
  req.builder().setIncludeDescription(true);
  req.builder().setIncludeBody(true);
  req.builder().setIncludeFileInfo(true);
  req.builder().setIncludeWorkingChanges(true);
  req.builder().setFindRenamesAndCopies(findRenames);
  req.builder().setIncludeCommitId(true);
  req.builder().setAction(G8ChangesetRequest::Action::LIST);
  CHECK_DECLARE_G8(res, req, c8::g8Changeset);
  return res;
}
