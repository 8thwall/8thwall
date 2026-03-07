// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Pawel Czarnecki (pawel@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "hooks.h",
  };
  visibility = {
    "//c8/git:internal",
  };
  deps = {
    "//c8:process",
    "//c8:scope-exit",
    "//c8/string:strcat",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0x0a793e94);

#include <filesystem>

#include "c8/git/hooks.h"
#include "c8/process.h"
#include "c8/scope-exit.h"
#include "c8/string/strcat.h"

#define GIT_PTR(type, name)    \
  git_##type *name = nullptr;  \
  SCOPE_EXIT([name] {          \
    if (name)                  \
      git_##type##_free(name); \
  })

namespace {
c8::String repoWorkDirectory(git_repository *repo) { return git_repository_workdir(repo); }

c8::String hooksDirectory(git_repository *repo) {
  // first try to see what the config thinks
  GIT_PTR(config, config);
  if (git_repository_config_snapshot(&config, repo) == 0) {
    const char *path;

    if (git_config_get_string(&path, config, "core.hookspath") == 0) {
      return path;
    }
  }
  // otherwise use a sane default
  auto workDirectory = git_repository_workdir(repo);
  return c8::strCat(workDirectory, ".git/hooks");
}
}  // namespace

namespace c8 {
namespace g8 {

// NOTE(pawel)
// Because execution of hooks involves launching another process; we make stubs for JAVASCRIPT.
// a result of std::nullopt means the hook could not be executed
#ifdef JAVASCRIPT

std::optional<int> postCheckoutHook(
  git_repository *repo, const char *prevHead, const char *newHead, CheckoutType checkoutType) {
  return std::nullopt;
}

std::optional<int> postCommitHook(git_repository *repo) { return std::nullopt; }

std::optional<int> postMergeHook(git_repository *repo, MergeType mergeType) { return std::nullopt; }

std::optional<int> prePushHook(
  git_repository *repo,
  const char *remoteName,
  const char *remoteLocation,
  const char *localRef,
  const char *localOid,
  const char *remoteRef,
  const char *remoteOid) {
  return std::nullopt;
}

#else  // implementation for platforms supporting c8:process

// post-checkout
// This hook is invoked when a git-checkout[1] or git-switch[1] is run after having updated the
// worktree. The hook is given three parameters: the ref of the previous HEAD, the ref of the new
// HEAD (which may or may not have changed), and a flag indicating whether the checkout was a branch
// checkout (changing branches, flag=1) or a file checkout (retrieving a file from the index,
// flag=0). This hook cannot affect the outcome of git switch or git checkout.
//
// It is also run after git-clone[1], unless the --no-checkout (-n) option is used. The first
// parameter given to the hook is the null-ref, the second the ref of the new HEAD and the flag is
// always 1. Likewise for git worktree add unless --no-checkout is used.
//
// This hook can be used to perform repository validity checks, auto-display differences from the
// previous HEAD if different, or set working dir metadata properties.
std::optional<int> postCheckoutHook(
  git_repository *repo, const char *prevHead, const char *newHead, CheckoutType checkoutType) {

  process::ExecuteOptions opts;
  opts.file = strCat(hooksDirectory(repo), "/post-checkout");
  opts.workDirectory = repoWorkDirectory(repo);

  if (!std::filesystem::exists(opts.file)) {
    // Don't try to run the hook if it doesn't exist.
    return 0;
  }

  try {
    auto res =
      process::execute(opts, {prevHead, newHead, checkoutType == CheckoutType::BRANCH ? "1" : "0"});

    return res.process.getExitCode();  // blocks until process returns
  } catch (const ExecError &err) {
    return std::nullopt;
  }
}

// post-commit
// This hook is invoked by git-commit[1]. It takes no parameters, and is invoked after a commit is
// made.
//
// This hook is meant primarily for notification, and cannot affect the outcome of git commit.
std::optional<int> postCommitHook(git_repository *repo) {

  process::ExecuteOptions opts;
  opts.file = strCat(hooksDirectory(repo), "/post-commit");
  opts.workDirectory = repoWorkDirectory(repo);

  if (!std::filesystem::exists(opts.file)) {
    // Don't try to run the hook if it doesn't exist.
    return 0;
  }

  try {
    auto res = process::execute(opts);
    return res.process.getExitCode();

  } catch (const ExecError &err) {
    return std::nullopt;
  }
}

// post-merge
// This hook is invoked by git-merge[1], which happens when a git pull is done on a local
// repository. The hook takes a single parameter, a status flag specifying whether or not the merge
// being done was a squash merge. This hook cannot affect the outcome of git merge and is not
// executed, if the merge failed due to conflicts.
//
// This hook can be used in conjunction with a corresponding pre-commit hook to save and restore any
// form of metadata associated with the working tree (e.g.: permissions/ownership, ACLS, etc). See
// contrib/hooks/setgitperms.perl for an example of how to do this.
std::optional<int> postMergeHook(git_repository *repo, MergeType mergeType = MergeType::STANDARD) {

  process::ExecuteOptions opts;
  opts.file = strCat(hooksDirectory(repo), "/post-merge");
  opts.workDirectory = repoWorkDirectory(repo);

  if (!std::filesystem::exists(opts.file)) {
    // Don't try to run the hook if it doesn't exist.
    return 0;
  }

  try {
    auto res = process::execute(opts, {mergeType == MergeType::SQUASH ? "1" : "0"});
    return res.process.getExitCode();

  } catch (const ExecError &err) {
    return std::nullopt;
  }
}

// pre-push
// This hook is called by git-push[1] and can be used to prevent a push from taking place. The hook
// is called with two parameters which provide the name and location of the destination remote, if a
// named remote is not being used both values will be the same.
//
// Information about what is to be pushed is provided on the hook’s standard input with lines of the
// form:
//
//<local ref> SP <local sha1> SP <remote ref> SP <remote sha1> LF
// For instance, if the command git push origin master:foreign were run the hook would receive a
// line like the following:
//
// refs/heads/master 67890 refs/heads/foreign 12345
// although the full, 40-character SHA-1s would be supplied. If the foreign ref does not yet exist
// the <remote SHA-1> will be 40 0. If a ref is to be deleted, the <local ref> will be supplied as
// (delete) and the <local SHA-1> will be 40 0. If the local commit was specified by something other
// than a name which could be expanded (such as HEAD~, or a SHA-1) it will be supplied as it was
// originally given.
//
// If this hook exits with a non-zero status, git push will abort without pushing anything.
// Information about why the push is rejected may be sent to the user by writing to standard error.
std::optional<int> prePushHook(
  git_repository *repo,
  const char *remoteName,
  const char *remoteLocation,
  const char *localRef,
  const char *localOid,
  const char *remoteRef,
  const char *remoteOid) {

  process::ExecuteOptions opts;
  opts.file = strCat(hooksDirectory(repo), "/pre-push");
  opts.workDirectory = repoWorkDirectory(repo);
  opts.redirectStdin = true;

  if (!std::filesystem::exists(opts.file)) {
    // Don't try to run the hook if it doesn't exist.
    return 0;
  }

  try {
    auto res = process::execute(opts, {remoteName, remoteLocation});
    *res.stdin << format("%s %s %s %s\n", localRef, localOid, remoteRef, remoteOid);
    res.stdin->close();
    return res.process.getExitCode();

  } catch (const ExecError &err) {
    return std::nullopt;
  }
}

#endif  // JAVASCRIPT

}  // namespace g8
}  // namespace c8
