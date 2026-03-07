// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Pawel Czarnecki (pawel@8thwall.com)

// In all cases, hook execution is optional
// if a hook fails to execute, the optional is empty (std::nullopt)
// if execution succeeds, the hook process status code is returned

#include <optional>

#include "c8/string.h"
#include "git2.h"

namespace c8 {
namespace g8 {

enum class CheckoutType {
  BRANCH,
  FILE,
};

enum class MergeType {
  STANDARD,
  SQUASH,
};

// run after the worktree has been updated; the command is passed the following args
// PREV_HEAD, NEW_HEAD, 1
// 1 means branch checkout, 0 means file checkout
std::optional<int> postCheckoutHook(
  git_repository *repo, const char *prevHead, const char *newHead, CheckoutType checkoutType);

// Invoked after a commit is made; takes no args
std::optional<int> postCommitHook(git_repository *repo);

// invoked after a pull/sync operation; single arg, whether or not it was a squash merge
std::optional<int> postMergeHook(git_repository *repo, MergeType mergeType);

// invoked before a push; can be used to prevent the push; pass the following args
// REMOTE_NAME REMOTE_LOCATION
// also receives the following line on stdin
// <local ref> SP <local sha1> SP <remote ref> SP <remote sha1> LF
// if remote ref does not exist, remote sha1 == 0
std::optional<int> prePushHook(
  git_repository *repo,
  const char *remoteName,
  const char *remoteLocation,
  const char *localRef,
  const char *localOid,
  const char *remoteRef,
  const char *remoteOid);

}  // namespace g8
}  // namespace c8
