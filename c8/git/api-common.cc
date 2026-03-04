// Copyright (c) 2020 8th Wall, Inc.

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "api-common.h",
  };
  visibility = {
    "//c8/git:internal",
  };
  deps = {
    ":filters",
    ":g8-api.capnp-cc",
    ":hooks",
    "//c8:scope-exit",
    "//c8/io:capnp-messages",
    "//c8/stats:scope-timer",
    "//c8/string:strcat",
    "//c8/string:format",
    "//c8:vector",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0xc7c347e6);

// #include <git2.h>
// #include <git2/oid.h>
// #include <git2/refs.h>
// #include <git2/types.h>

#include "c8/git/api-common.h"
#include "c8/git/filters.h"
#include "c8/git/hooks.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"

namespace c8 {
namespace g8 {

int gitRepositoryInit() {
  // NOTE(pawel) this returns the number of times the library has been initialized
  int ret = git_libgit2_init();

  // NOTE(pawel) we'll probably want to compile git-lfs (written in go) for the browser
  // alternatively we can write custom support for git-lfs
  // for now, we don't do lfs on javascript
#ifndef JAVASCRIPT
  GIT_STATUS_RETURN(register_lfs_filter());
#endif
  return ret;
}

bool clientNameIsValid(const char *client) {
  const std::string notClientRegex =
    format("(^main$)|(^master$)|(^.*-fork$)|(^head$)|(^.*-%s[0-9]{6,}$)", CHANGESET_CODE);
  const std::string validCharacterRegex = "[a-zA-Z0-9]([a-zA-Z0-9-_]*[a-zA-Z0-9])?";
  const std::regex invalidClientRegex(notClientRegex, std::regex_constants::icase);
  const std::regex validClientRegex(validCharacterRegex, std::regex_constants::icase);
  if (std::regex_match(client, invalidClientRegex)) {
    return false;
  }
  if (!std::regex_match(client, validClientRegex)) {
    return false;
  }
  return true;
}

bool changesetNameIsValid(const char *changeset) {
  const String changesetRegex = "^[0-9]{6}$";
  const std::regex validChangesetRegex(changesetRegex);
  if (std::regex_match(changeset, validChangesetRegex)) {
    return true;
  }
  return false;
}

bool userNameIsValid(const char *user) {
  const String userRegex = "^[a-zA-Z0-9]{1,}$";
  const std::regex validUserRegex(userRegex);
  if (std::regex_match(user, validUserRegex)) {
    return true;
  }
  return false;
}

// A valid g8 client is either a commit that is on the master line or its immediate parent is on the
// master line. Returns (gitStatus, valid). Check gitStatus for error from underlying git functions.
std::pair<int, bool> branchIsValidClient(git_repository *repo, const char *clientName) {
  ScopeTimer t(format("branch-is-valid-client-%s", clientName));
  GIT_PTR(reference, master);
  GIT_PTR(reference, client);

  if (!clientNameIsValid(clientName)) {
    return {0, false};
  }

  GIT_STATUS_RETURN_PAIR(lookupMainBranch(&master, repo), false);
  GIT_STATUS_RETURN_PAIR(git_branch_lookup(&client, repo, clientName, GIT_BRANCH_LOCAL), false);

  GIT_PTR(commit, masterCommit);
  GIT_PTR(commit, clientCommit);

  GIT_STATUS_RETURN_PAIR(
    git_reference_peel(reinterpret_cast<git_object **>(&masterCommit), master, GIT_OBJECT_COMMIT),
    false);
  GIT_STATUS_RETURN_PAIR(
    git_reference_peel(reinterpret_cast<git_object **>(&clientCommit), client, GIT_OBJECT_COMMIT),
    false);

  git_oid forkCommitOid;
  // If we fail to find a merge base, we aren't valid and don't want to make this an error case.
  if (
    0
    != git_merge_base(
      &forkCommitOid, repo, git_commit_id(masterCommit), git_commit_id(clientCommit))) {
    return {GIT_OK, false};
  }
  // Branch commit is on master line.
  if (git_oid_equal(git_commit_id(clientCommit), &forkCommitOid)) {
    return {GIT_OK, true};
  }

  // Branch commit's parent is on the master line.
  if (git_oid_equal(git_commit_parent_id(clientCommit, 0), &forkCommitOid)) {
    return {GIT_OK, true};
  }

  // Branch is not a G8 client.
  return {GIT_OK, false};
}

G8FileInfo::Status g8FileStatusFromDeltaStatus(git_delta_t deltaStatus) {
  switch (deltaStatus) {
    case GIT_DELTA_UNMODIFIED:
      return G8FileInfo::Status::UNMODIFIED;
    case GIT_DELTA_ADDED:
      return G8FileInfo::Status::ADDED;
    case GIT_DELTA_MODIFIED:
      return G8FileInfo::Status::MODIFIED;
    case GIT_DELTA_DELETED:
      return G8FileInfo::Status::DELETED;
    case GIT_DELTA_RENAMED:
      return G8FileInfo::Status::RENAMED;
    case GIT_DELTA_COPIED:
      return G8FileInfo::Status::COPIED;
    case GIT_DELTA_TYPECHANGE:
      return G8FileInfo::Status::TYPE_CHANGED;
    case GIT_DELTA_IGNORED:
      return G8FileInfo::Status::IGNORED;
    case GIT_DELTA_CONFLICTED:
      return G8FileInfo::Status::CONFLICTED;
    case GIT_DELTA_UNTRACKED:
      // For purposes of g8, untracked files are the same as added files.
      return G8FileInfo::Status::ADDED;
    case GIT_DELTA_UNREADABLE:
      return G8FileInfo::Status::UNREADABLE;
  }
}

G8DiffLine::Origin diffLineOrigin(git_diff_line_t type) {
  switch (type) {
    case GIT_DIFF_LINE_CONTEXT:
      return G8DiffLine::Origin::CONTEXT;
    case GIT_DIFF_LINE_ADDITION:
      return G8DiffLine::Origin::ADDITION;
    case GIT_DIFF_LINE_DELETION:
      return G8DiffLine::Origin::DELETION;
    case GIT_DIFF_LINE_CONTEXT_EOFNL:
      return G8DiffLine::Origin::CONTEXT_EOFNL;
    case GIT_DIFF_LINE_ADD_EOFNL:
      return G8DiffLine::Origin::ADD_EOFNL;
    case GIT_DIFF_LINE_DEL_EOFNL:
      return G8DiffLine::Origin::DEL_EOFNL;
    case GIT_DIFF_LINE_FILE_HDR:
      return G8DiffLine::Origin::FILE_HDR;
    case GIT_DIFF_LINE_HUNK_HDR:
      return G8DiffLine::Origin::HUNK_HDR;
    case GIT_DIFF_LINE_BINARY:
      return G8DiffLine::Origin::BINARY;
  }
}

static const String HTTPS_PREFIX = "https://";

std::pair<std::string, std::string> extractHttpsHost(const String &url) {
  if (!url.starts_with(HTTPS_PREFIX)) {
    return std::pair{"", ""};
  }

  auto slashIndex = url.find("/", HTTPS_PREFIX.size());
  auto atIndex = url.rfind("@", slashIndex);

  auto startIndex = atIndex == String::npos ? HTTPS_PREFIX.size() : atIndex + 1;

  auto originEndIndex = slashIndex == String::npos ? url.size() : slashIndex;

  String origin = url.substr(startIndex, originEndIndex - startIndex);

  String remainder = slashIndex == String::npos ? "" : url.substr(slashIndex + 1);

  return std::pair{origin, remainder};
}

void setRemoteInfo(G8GitRemote::Builder builder, const git_remote *remote) {
  String remoteUrl = git_remote_url(remote);
  setRemoteInfo(builder, remoteUrl);
}

void setRemoteInfo(G8GitRemote::Builder builder, const String &remoteUrl) {
  auto [host, remainder] = extractHttpsHost(remoteUrl);
  builder.setHost(host);

  if (host == "github.com") {
    builder.setApi(G8GitRemote::Api::GITHUB);
    builder.setRepoName(remainder);
  } else if (host.starts_with("gitlab.")) {
    builder.setApi(G8GitRemote::Api::GITLAB);
    // TODO(pawel) Do we also want to strip the trailing .git?
    builder.setRepoName(remainder);
  } else {
    builder.setApi(G8GitRemote::Api::UNKNOWN);
  }
}

int gitDiffTreeToTree(
  git_repository *repo,
  git_tree *oldTree,
  git_tree *newTree,
  bool findRenames,
  G8FileInfoList::Builder output) {
  ScopeTimer t("diff-tree-tree");
  GIT_PTR(diff, diff);

  git_diff_options diffOpts = GIT_DIFF_OPTIONS_INIT;
  diffOpts.flags = GIT_DIFF_INCLUDE_TYPECHANGE | GIT_DIFF_SKIP_BINARY_CHECK
    | GIT_DIFF_INCLUDE_UNREADABLE | GIT_DIFF_UPDATE_INDEX;

  // Compute the diff between the two trees.
  GIT_STATUS_RETURN(git_diff_tree_to_tree(&diff, repo, oldTree, newTree, &diffOpts));

  if (findRenames) {
    // Find copies and renames.
    git_diff_find_options diffFindOpts = GIT_DIFF_FIND_OPTIONS_INIT;
    diffFindOpts.flags = GIT_DIFF_FIND_RENAMES | GIT_DIFF_FIND_COPIES;
    GIT_STATUS_RETURN(git_diff_find_similar(diff, &diffFindOpts));
  }

  GIT_PTR(diff_stats, stats);

  GIT_STATUS_RETURN(git_diff_get_stats(&stats, diff));

  const int filesChanged = git_diff_stats_files_changed(stats);
  if (filesChanged > 0) {
    output.initInfo(filesChanged);
  }

  DiffCallbackPayload payload;
  payload.builder = &output;

  GIT_STATUS_RETURN(git_diff_foreach(
    diff,
    [](const git_diff_delta *delta, float progress, void *payload) -> int {
      auto *pl = reinterpret_cast<DiffCallbackPayload *>(payload);
      G8FileInfo::Builder info = pl->builder->getInfo()[pl->index];
      info.setStatus(g8FileStatusFromDeltaStatus(delta->status));
      if (info.getStatus() == G8FileInfo::Status::DELETED) {
        // For deleted files, use their old path.
        info.setPath(delta->old_file.path);
      } else {
        // For all other files, use their new path.
        info.setPath(delta->new_file.path);
      }
      if (
        info.getStatus() == G8FileInfo::Status::RENAMED
        || info.getStatus() == G8FileInfo::Status::COPIED) {
        // Save the previous path for renames and copies.
        info.setPreviousPath(delta->old_file.path);
      }
      ++pl->index;
      return 0;
    },
    nullptr,
    nullptr,
    nullptr,
    &payload));
  return 0;
}

int gitDiffRefToRef(
  git_repository *repo,
  const char *oldName,
  const char *newName,
  bool findRenames,
  G8FileInfoList::Builder output) {
  GIT_PTR(reference, oldRef);
  GIT_PTR(reference, newRef);

  GIT_STATUS_RETURN(git_reference_dwim(&oldRef, repo, oldName));
  GIT_STATUS_RETURN(git_reference_dwim(&newRef, repo, newName));

  GIT_PTR(tree, oldTree);
  GIT_PTR(tree, newTree);

  GIT_STATUS_RETURN(git_reference_peel((git_object **)&oldTree, oldRef, GIT_OBJECT_TREE));
  GIT_STATUS_RETURN(git_reference_peel((git_object **)&newTree, newRef, GIT_OBJECT_TREE));

  return gitDiffTreeToTree(repo, oldTree, newTree, findRenames, output);
}

int gitDiffCommitToCommit(
  git_repository *repo,
  git_commit *oldCommit,
  git_commit *newCommit,
  bool findRenames,
  G8FileInfoList::Builder output) {
  GIT_PTR(tree, oldTree);
  GIT_PTR(tree, newTree);

  GIT_STATUS_RETURN(git_commit_tree(&oldTree, oldCommit));
  GIT_STATUS_RETURN(git_commit_tree(&newTree, newCommit));

  return gitDiffTreeToTree(repo, oldTree, newTree, findRenames, output);
}

// Diffs the commits to a merge base
int gitDiffToMergeBase(
  git_repository *repo,
  git_commit *base,
  git_commit *branch,
  bool findRenames,
  G8FileInfoList::Builder output) {
  ScopeTimer t("diff-to-merge-base");

  GIT_PTR(commit, baseCommit);
  git_oidarray bases;

  GIT_STATUS_RETURN(git_merge_bases(&bases, repo, git_commit_id(base), git_commit_id(branch)));
  GIT_STATUS_RETURN(git_commit_lookup(&baseCommit, repo, &bases.ids[0]));
  return gitDiffCommitToCommit(repo, baseCommit, branch, findRenames, output);
}

// Diffs the referenced branches to a merge base
int gitDiffToMergeBase(
  git_repository *repo,
  git_reference *client,
  git_reference *changeset,
  bool findRenames,
  G8FileInfoList::Builder output) {

  GIT_PTR(commit, clientCommit);
  GIT_PTR(commit, changesetCommit);

  GIT_STATUS_RETURN(
    git_reference_peel(reinterpret_cast<git_object **>(&clientCommit), client, GIT_OBJECT_COMMIT));
  GIT_STATUS_RETURN(git_reference_peel(
    reinterpret_cast<git_object **>(&changesetCommit), changeset, GIT_OBJECT_COMMIT));

  return gitDiffToMergeBase(repo, clientCommit, changesetCommit, findRenames, output);
}

int gitDiffTreeToIndex(
  git_repository *repo,
  git_tree *tree,
  git_index *index,
  bool findRenames,
  const git_strarray *pathspec,
  G8FileInfoList::Builder output) {
  ScopeTimer t("diff-tree-to-index");
  GIT_PTR(diff, diff);

  git_diff_options diffOpts = GIT_DIFF_OPTIONS_INIT;
  diffOpts.flags = GIT_DIFF_INCLUDE_UNTRACKED | GIT_DIFF_RECURSE_UNTRACKED_DIRS
    | GIT_DIFF_INCLUDE_TYPECHANGE | GIT_DIFF_SKIP_BINARY_CHECK | GIT_DIFF_INCLUDE_UNREADABLE;
  if (pathspec != nullptr) {
    diffOpts.pathspec = *pathspec;
  }

  GIT_STATUS_RETURN(git_diff_tree_to_index(&diff, repo, tree, index, &diffOpts));

  if (findRenames) {
    git_diff_find_options diffFindOpts = GIT_DIFF_FIND_OPTIONS_INIT;
    diffFindOpts.flags = GIT_DIFF_FIND_FOR_UNTRACKED | GIT_DIFF_FIND_RENAMES | GIT_DIFF_FIND_COPIES;
    GIT_STATUS_RETURN(git_diff_find_similar(diff, &diffFindOpts));
  }

  GIT_PTR(diff_stats, stats);

  GIT_STATUS_RETURN(git_diff_get_stats(&stats, diff));
  const int filesChanged = git_diff_stats_files_changed(stats);
  if (filesChanged > 0) {
    output.initInfo(filesChanged);
  }

  DiffCallbackPayload payload;
  payload.builder = &output;

  GIT_STATUS_RETURN(git_diff_foreach(
    diff,
    [](const git_diff_delta *delta, float progress, void *payload) -> int {
      auto *pl = reinterpret_cast<DiffCallbackPayload *>(payload);
      G8FileInfo::Builder info = pl->builder->getInfo()[pl->index];
      info.setStatus(g8FileStatusFromDeltaStatus(delta->status));
      if (info.getStatus() == G8FileInfo::Status::DELETED) {
        // For deleted files, use their old path.
        info.setPath(delta->old_file.path);
      } else {
        // For all other files, use their new path.
        info.setPath(delta->new_file.path);
      }
      if (
        info.getStatus() == G8FileInfo::Status::RENAMED
        || info.getStatus() == G8FileInfo::Status::COPIED) {
        // Save the previous path for renames and copies.
        info.setPreviousPath(delta->old_file.path);
      }
      ++pl->index;
      return 0;
    },
    nullptr,
    nullptr,
    nullptr,
    &payload));
  return 0;
}

int gitFindMergeBase(
  git_commit **mergeBaseCommit, git_repository *repo, git_commit *one, git_commit *two) {
  ScopeTimer t("find-merge-base");
  git_oidarray bases;
  GIT_STATUS_RETURN(git_merge_bases(&bases, repo, git_commit_id(one), git_commit_id(two)));
  GIT_STATUS_RETURN(git_commit_lookup(mergeBaseCommit, repo, &bases.ids[0]));
  return 0;
}

int gitFindMergeBase(
  git_commit **mergeBaseCommit, git_repository *repo, git_reference *one, git_reference *two) {
  ScopeTimer t(
    format("find-merge-base-%s-%s", git_reference_shorthand(one), git_reference_shorthand(two)));
  GIT_PTR(commit, commitA);
  GIT_PTR(commit, commitB);

  GIT_STATUS_RETURN(
    git_reference_peel(reinterpret_cast<git_object **>(&commitA), one, GIT_OBJECT_COMMIT));
  GIT_STATUS_RETURN(
    git_reference_peel(reinterpret_cast<git_object **>(&commitB), two, GIT_OBJECT_COMMIT));

  return gitFindMergeBase(mergeBaseCommit, repo, commitA, commitB);
}

// If override tree is present, us the entries found in it.
int writeChangesetTree(
  git_oid *out,
  git_repository *repo,
  const git_commit *clientCommit,
  const git_commit *clientForkCommit,
  const std::set<String> *filesToInclude,
  const std::set<String> &filesToExclude,
  const git_tree *clientOverrideTree) {

  git_tree *clientTree = nullptr;
  git_tree *clientForkTree = nullptr;

  GIT_STATUS_RETURN(git_commit_tree(&clientTree, clientCommit));
  GIT_STATUS_RETURN(git_commit_tree(&clientForkTree, clientForkCommit));

  const git_tree *fileVersionTree = clientOverrideTree ? clientOverrideTree : clientTree;

  GitDiffList fileList;

  git_tree *tree = nullptr;
  GIT_STATUS_RETURN(git_commit_tree(&tree, clientForkCommit));

  fileList.oldName = git_oid_tostr_s(git_commit_id(clientForkCommit));
  fileList.newName = WORKING_CHANGESET;

  git_diff *diff = nullptr;

  git_diff_options diffOpts = GIT_DIFF_OPTIONS_INIT;
  diffOpts.flags = GIT_DIFF_INCLUDE_UNTRACKED | GIT_DIFF_RECURSE_UNTRACKED_DIRS
    | GIT_DIFF_INCLUDE_TYPECHANGE | GIT_DIFF_SKIP_BINARY_CHECK | GIT_DIFF_UPDATE_INDEX
    | GIT_DIFF_INCLUDE_UNREADABLE;

  GIT_PTR(tree, workingTree);
  GIT_STATUS_RETURN(resolveTreeForChangePoint(repo, &workingTree, "WORKING"));

  GIT_STATUS_RETURN(git_diff_tree_to_tree(&diff, repo, tree, workingTree, &diffOpts));

  GIT_STATUS_RETURN(git_diff_foreach(
    diff,
    [](const git_diff_delta *delta, float progress, void *payload) -> int {
      GitDiffList *fileList = reinterpret_cast<GitDiffList *>(payload);
      fileList->files.emplace_back(
        g8FileStatusFromDeltaStatus(delta->status), delta->old_file.path, delta->new_file.path);
      return 0;
    },
    nullptr,
    nullptr,
    nullptr,
    &fileList));

  git_tree_free(tree);
  git_diff_free(diff);

  std::vector<git_tree_update> updates;
  updates.reserve(fileList.files.size());
  for (const auto &file : fileList.files) {
    if (file.status == G8FileInfo::Status::DELETED) {
      if (!filesToInclude || (filesToInclude->find(file.oldPath) != filesToInclude->end())) {
        if (filesToExclude.find(file.oldPath) == filesToExclude.end()) {
          updates.push_back(git_tree_update());
          git_tree_update &update = updates.back();
          update.action = GIT_TREE_UPDATE_REMOVE;
          update.path = file.oldPath.c_str();
        }
      }
    } else {
      if (!filesToInclude || (filesToInclude->find(file.newPath) != filesToInclude->end())) {
        if (filesToExclude.find(file.newPath) == filesToExclude.end()) {
          GIT_PTR(tree_entry, entry);
          // NOTE(pawel) Since the point of the clientOverrideTree is to make the files as they
          // are in this tree... non-existing files should simply be ignored.
          int byPathStatus = git_tree_entry_bypath(&entry, fileVersionTree, file.newPath.c_str());
          if (clientOverrideTree && byPathStatus == GIT_ENOTFOUND) {
            continue;
          }
          GIT_STATUS_RETURN(byPathStatus);

          updates.emplace_back();
          git_tree_update &update = updates.back();

          update.action = GIT_TREE_UPDATE_UPSERT;
          update.path = file.newPath.c_str();
          git_oid_cpy(&update.id, git_tree_entry_id(entry));
          update.filemode = git_tree_entry_filemode(entry);
        }
      }
    }
  }

  GIT_STATUS_RETURN(
    git_tree_create_updated(out, repo, clientForkTree, updates.size(), updates.data()));

  // CLEAN UP GIT_TREE_BUILDER.
  git_tree_free(clientForkTree);
  git_tree_free(clientTree);

  return 0;
}

void setRemoteProgressCallbacks(git_remote_callbacks *callbacks) {
  // Don't print stuff in the browser.
#ifdef JAVASCRIPT
  return;
#endif

  // TODO(mc): Replace with better version.
  static git_transport_message_cb sidebandProgressCb =
    [](const char *str, int len, void *data) -> int {
    (void)data;
    printf("remote: %.*s", len, str);
    fflush(stdout); /* We don't have the \n to force the flush */
    return 0;
  };

  // TODO(mc): Replace with better version.
  static auto updateTipsCb =
    [](const char *refname, const git_oid *a, const git_oid *b, void *data) -> int {
    char a_str[GIT_OID_HEXSZ + 1], b_str[GIT_OID_HEXSZ + 1];
    (void)data;

    git_oid_fmt(b_str, b);
    b_str[GIT_OID_HEXSZ] = '\0';

    if (git_oid_is_zero(a)) {
      printf("[new]     %.20s %s\n", b_str, refname);
    } else {
      git_oid_fmt(a_str, a);
      a_str[GIT_OID_HEXSZ] = '\0';
      printf("[updated] %.10s..%.10s %s\n", a_str, b_str, refname);
    }

    return 0;
  };

  static git_indexer_progress_cb transferProgressCb =
    [](const git_indexer_progress *stats, void *payload) -> int {
    (void)payload;

    if (stats->received_objects == stats->total_objects) {
      printf("Resolving deltas %d/%d\r", stats->indexed_deltas, stats->total_deltas);
    } else if (stats->total_objects > 0) {
      printf(
        "Received %d/%d objects (%d) in %zu bytes\r",
        stats->received_objects,
        stats->total_objects,
        stats->indexed_objects,
        stats->received_bytes);
    }
    return 0;
  };

  callbacks->update_tips = updateTipsCb;
  callbacks->sideband_progress = sidebandProgressCb;
  callbacks->transfer_progress = transferProgressCb;
}

int fetchRemote(
  git_repository *repo, G8Authentication::Reader auth, const String &user, bool includeRemote) {
  GIT_PTR(remote, remote);
  GIT_STATUS_RETURN(git_remote_lookup(&remote, repo, ORIGIN));

  git_fetch_options fetchOpts = GIT_FETCH_OPTIONS_INIT;
#ifdef JAVASCRIPT
  // TODO(pawel) Should we remove this directive?
#else
  git_transport_certificate_check_cb certCheckCb =
    [](git_cert *cert, int valid, const char *host, void *payload) -> int { return valid; };

  git_cred_acquire_cb credAcquireCb = [](
                                        git_cred **cred,
                                        const char *url,
                                        const char *username_from_url,
                                        unsigned int allowed_types,
                                        void *payload) -> int {
    G8Authentication::Reader *req = (G8Authentication::Reader *)payload;
    return git_credential_userpass_plaintext_new(
      cred, req->getUser().cStr(), req->getPass().cStr());
  };

  git_remote_callbacks remoteCallbacks = GIT_REMOTE_CALLBACKS_INIT;
  setRemoteProgressCallbacks(&remoteCallbacks);

  remoteCallbacks.certificate_check = certCheckCb;
  remoteCallbacks.credentials = credAcquireCb;
  remoteCallbacks.payload = reinterpret_cast<void *>(&auth);

  fetchOpts.callbacks = remoteCallbacks;

  String cred = auth.hasCred() ? auth.getCred() : "";
  bool useCreds = !cred.empty();
  auto authStr = format("Authorization: %s", cred.c_str());
  char *authCStr = const_cast<char *>(authStr.c_str());
  if (useCreds) {
    fetchOpts.custom_headers.count = 1;
    fetchOpts.custom_headers.strings = &authCStr;
  }
#endif

  git_strarray refs;
  std::vector<const char *> refstr;

  String userClientRefspec =
    format("+refs/heads/%s-*:refs/remotes/origin/%s-*", user.c_str(), user.c_str());

  if (includeRemote) {
    refstr.push_back("+refs/heads/*:refs/remotes/origin/*");
    // Enable pruning of remote-tracking references that no longer exist on the remote
    fetchOpts.prune = GIT_FETCH_PRUNE;
  } else {
    refstr.push_back(userClientRefspec.c_str());
    refstr.push_back("+refs/heads/master:refs/remotes/origin/master");
    refstr.push_back("+refs/heads/main:refs/remotes/origin/main");
    refstr.push_back("+refs/heads/trunk:refs/remotes/origin/trunk");
  }

  refs.strings = const_cast<char **>(refstr.data());
  refs.count = refstr.size();
  GIT_STATUS_RETURN(git_remote_fetch(remote, &refs, &fetchOpts, nullptr));

  const git_indexer_progress *stats = git_remote_stats(remote);
  if (stats->total_objects > 0) {
    if (stats->local_objects > 0) {
      printf(
        "\rReceived %d/%d objects in %zu bytes (used %d local objects)\n",
        stats->indexed_objects,
        stats->total_objects,
        stats->received_bytes,
        stats->local_objects);
    } else {
      printf(
        "\rReceived %d/%d objects in %zu bytes\n",
        stats->indexed_objects,
        stats->total_objects,
        stats->received_bytes);
    }
  }

  return 0;
}

int g8SyncMaster(git_repository *repo, G8Authentication::Reader auth, const String &user) {
  GIT_PTR(reference, master);
  GIT_STATUS_RETURN(fetchRemote(repo, auth, user));

  GIT_STATUS_RETURN(lookupMainBranch(&master, repo));
  String mainBranchName = git_reference_shorthand(master);

  // Investigate whether a main appeared on remote and needs to be checked out locally.
  if (mainBranchName == "master") {
    GIT_PTR(reference, originMain);
    int lookupRes =
      git_branch_lookup(&originMain, repo, strCat(ORIGIN, "/", "main").c_str(), GIT_BRANCH_REMOTE);
    if (0 == lookupRes) {
      GIT_PTR(reference, localMain);
      GIT_STATUS_RETURN(git_reference_create(
        &localMain,
        repo,
        strCat(HEADS_PREFIX, "main").c_str(),
        git_reference_target(originMain),
        true,
        "Checkout origin/main"));
      mainBranchName = git_reference_shorthand(localMain);
      std::swap(localMain, master);
    }
  }

  GIT_PTR(reference, originMaster);
  String originMasterName = strCat(ORIGIN, "/", mainBranchName);
  GIT_STATUS_RETURN(
    git_branch_lookup(&originMaster, repo, originMasterName.c_str(), GIT_BRANCH_REMOTE));

  if (0 != git_reference_cmp(master, originMaster)) {
    GIT_PTR(reference, newMaster);
    GIT_PTR(commit, originMasterCommit);
    GIT_STATUS_RETURN(git_reference_peel(
      reinterpret_cast<git_object **>(&originMasterCommit), originMaster, GIT_OBJECT_COMMIT));

    const git_oid *originMasterOid = git_commit_id(originMasterCommit);
    GIT_STATUS_RETURN(git_reference_set_target(
      &newMaster,
      master,
      originMasterOid,
      strCat("advance ", mainBranchName, " to origin/", mainBranchName).c_str()));

    std::swap(newMaster, master);
  }

  return 0;
}

GitLsClients g8GitLsClients(git_repository *repo, bool includeRemote) {
  ScopeTimer t("ls-clients");
  Vector<String> clientBranches;
  Vector<String> remoteBranches;
  GIT_PTR(branch_iterator, iter);
  GIT_STATUS_RETURN_PAIR(git_branch_iterator_new(&iter, repo, GIT_BRANCH_LOCAL), {});
  git_branch_t unusedType;
  git_reference *iterRef = nullptr;  // Not GIT_PTR.
  while (0 == git_branch_next(&iterRef, &unusedType, iter)) {
    const char *branchName = git_reference_shorthand(iterRef);
    auto [gitStatus, validClient] = branchIsValidClient(repo, branchName);
    GIT_STATUS_RETURN_PAIR(gitStatus, {});
    if (clientNameIsValid(branchName) && validClient) {
      clientBranches.push_back(branchName);
    }
    // Must free on each iteration.
    git_reference_free(iterRef);
  }

  // NOTE(johnny): Include all remote branches.
  if (includeRemote) {
    GIT_STATUS_RETURN_PAIR(git_branch_iterator_new(&iter, repo, GIT_BRANCH_REMOTE), {});
    while (0 == git_branch_next(&iterRef, &unusedType, iter)) {
      const char *branchName = git_reference_shorthand(iterRef);
      // TODO(johnny): There are more possible remote branches like main this is ok for now
      // since studio repos are all master.
      if (strcmp(branchName, "origin/master") != 0 && strcmp(branchName, "origin/HEAD") != 0) {
        remoteBranches.push_back(branchName);
      }
      // Must free on each iteration.
      git_reference_free(iterRef);
    }
  }

  return {GIT_OK, clientBranches, remoteBranches};
}

std::pair<int, GitChangesetList> g8GitLsChangesets(git_repository *repo, const char *clientName) {
  GitChangesetList list;
  git_reference *head = nullptr;
  GIT_STATUS_RETURN_PAIR(git_reference_lookup(&head, repo, "HEAD"), {});
  // !!!ENSURE head is of type git_reference_symbolic_target() (not detached head).

  String clientFullName;
  if (clientName) {
    clientFullName = makeBranchFullName(makeClientName(clientName).c_str());
  } else {
    clientFullName = git_reference_symbolic_target(head);
    GIT_PTR(reference, clientReference);
    GIT_STATUS_RETURN_PAIR(
      git_reference_lookup(&clientReference, repo, clientFullName.c_str()), {});
    clientName = git_reference_shorthand(clientReference);
  }

  GitLsLookupPayload payload;

  payload.prefixLength =
    std::strlen(CHANGESETS_PREFIX) + std::strlen(clientName) + sizeof(CHANGESET_CODE);
  payload.list = &list;
  payload.repo = repo;
  String csPattern = strCat(CHANGESETS_PREFIX, clientName, "-", CHANGESET_CODE, "*");

  git_strarray *changesets = nullptr;

  int globError = git_reference_foreach_glob(
    repo,
    csPattern.c_str(),
    [](const char *name, void *payload) -> int {
      GitLsLookupPayload *pl = reinterpret_cast<GitLsLookupPayload *>(payload);
      git_reference *csRef = nullptr;
      git_commit *csCommit = nullptr;

      pl->gitError = git_reference_lookup(&csRef, pl->repo, name);
      GIT_STATUS_RETURN(pl->gitError);

      pl->gitError =
        git_reference_peel(reinterpret_cast<git_object **>(&csCommit), csRef, GIT_OBJECT_COMMIT);
      GIT_STATUS_RETURN(pl->gitError);

      GitChangeset changeset;
      changeset.name = &name[pl->prefixLength];
      changeset.summary = git_commit_summary(csCommit);
      changeset.branchName = name;

      pl->list->changesets.push_back(std::move(changeset));

      git_commit_free(csCommit);
      git_reference_free(csRef);
      return 0;
    },
    &payload);

  if (globError == GIT_EUSER) {
    GIT_STATUS_RETURN_PAIR(payload.gitError, {});
  } else {
    GIT_STATUS_RETURN_PAIR(globError, {})
  }

  git_strarray_dispose(changesets);
  git_reference_free(head);

  return {0, list};
}

int expandOid(git_repository *repo, git_oid *oid, String shortened) {
  GIT_PTR(odb, odb);
  GIT_STATUS_RETURN(git_repository_odb(&odb, repo));
  GIT_STATUS_RETURN(git_oid_fromstrp(oid, shortened.c_str()));
  git_odb_expand_id expand = {
    .id = *oid,
    .length = static_cast<unsigned short>(shortened.size()),
    .type = GIT_OBJECT_ANY,
  };

  GIT_STATUS_RETURN(git_odb_expand_ids(odb, &expand, 1));
  *oid = expand.id;
  return 0;
}

git_remote_callbacks gitRemoteCallbacks(G8Authentication::Reader auth) {
  git_transport_certificate_check_cb certCheckCb =
    [](git_cert *cert, int valid, const char *host, void *payload) -> int {
    // If libgit2 thinks this certificate is valid we'll take its word for it.
    return valid;
  };

  git_cred_acquire_cb credAcquireCb = [](
                                        git_cred **cred,
                                        const char *url,
                                        const char *username_from_url,
                                        unsigned int allowed_types,
                                        void *payload) -> int {
    G8Authentication::Reader *req = (G8Authentication::Reader *)payload;
    return git_credential_userpass_plaintext_new(
      cred, req->getUser().cStr(), req->getPass().cStr());
  };

  git_remote_callbacks remoteCallbacks = GIT_REMOTE_CALLBACKS_INIT;
  remoteCallbacks.certificate_check = certCheckCb;
  remoteCallbacks.credentials = credAcquireCb;
  return remoteCallbacks;
}

int g8GitForcePush(G8Authentication::Reader auth, git_repository *repo, git_reference *client) {
  GIT_PTR(remote, remote);
  GIT_STATUS_RETURN(git_remote_lookup(&remote, repo, ORIGIN));

  git_push_options opts = GIT_PUSH_OPTIONS_INIT;
  opts.callbacks = gitRemoteCallbacks(auth);
  opts.callbacks.payload = reinterpret_cast<void *>(&auth);

  String cred = auth.hasCred() ? auth.getCred() : "";
  bool useCreds = !cred.empty();
  auto authStr = format("Authorization: %s", cred.c_str());
  char *authCStr = const_cast<char *>(authStr.c_str());
  if (useCreds) {
    opts.custom_headers.count = 1;
    opts.custom_headers.strings = &authCStr;
  }

  // printf("g8GitPush for %s %s\n", branch.c_str(), user.c_str());
  GIT_PTR(reference, remoteRef);
  GIT_STATUS_RETURN(git_branch_upstream(&remoteRef, client));
  git_buf remoteRefName = {0};
  SCOPE_EXIT([&remoteRefName] { git_buf_dispose(&remoteRefName); });

  GIT_STATUS_RETURN(git_refspec_rtransform(
    &remoteRefName, git_remote_get_refspec(remote, 0), git_reference_name(remoteRef)));

  // Push refspec which is doing a force push of the branch.
  std::vector<char *> refs;
  String ref = strCat("+", git_reference_name(client), ":", remoteRefName.ptr);
  refs.push_back(const_cast<char *>(ref.c_str()));

  // pre-push
  // we do a pre-push on the client
  {
    GIT_PTR(commit, localCommit);
    GIT_PTR(commit, remoteCommit);

    GIT_STATUS_RETURN(
      git_reference_peel(reinterpret_cast<git_object **>(&localCommit), client, GIT_OBJECT_COMMIT));
    GIT_STATUS_RETURN(git_reference_peel(
      reinterpret_cast<git_object **>(&remoteCommit), remoteRef, GIT_OBJECT_COMMIT));

    auto localOid = String(git_oid_tostr_s(git_commit_id(localCommit)));
    auto remoteOid = String(git_oid_tostr_s(git_commit_id(remoteCommit)));

    auto prePushRes = prePushHook(
      repo,
      git_remote_name(remote),
      git_remote_url(remote),
      git_reference_name(client),
      localOid.c_str(),
      git_reference_name(remoteRef),
      remoteOid.c_str());
    if (prePushRes && *prePushRes != 0) {
      // TODO(pawel) should we respect a failed push? is it possible that our local state could be
      // off?
      // TODO(pawel) does that means this should be run before even making local changes?\
      // TODO(pawel) what does it even mean to reject a push when a local client has been updated?
    }
  }

  git_strarray specs = {refs.data(), refs.size()};
  GIT_STATUS_RETURN(git_remote_push(remote, &specs, &opts));

  return 0;
}

int g8GitDeleteRemoteBranch(
  G8Authentication::Reader auth, git_repository *repo, git_reference *branch) {
  GIT_PTR(remote, remote);
  GIT_STATUS_RETURN(git_remote_lookup(&remote, repo, ORIGIN));

  git_push_options opts = GIT_PUSH_OPTIONS_INIT;
  opts.callbacks = gitRemoteCallbacks(auth);
  opts.callbacks.payload = reinterpret_cast<void *>(&auth);

  String cred = auth.hasCred() ? auth.getCred() : "";
  bool useCreds = !cred.empty();
  auto authStr = format("Authorization: %s", cred.c_str());
  char *authCStr = const_cast<char *>(authStr.c_str());
  if (useCreds) {
    opts.custom_headers.count = 1;
    opts.custom_headers.strings = &authCStr;
  }

  GIT_PTR(reference, remoteRef);
  GIT_STATUS_RETURN(git_branch_upstream(&remoteRef, branch));
  git_buf remoteRefName = {0};
  SCOPE_EXIT([&remoteRefName] { git_buf_dispose(&remoteRefName); });

  GIT_STATUS_RETURN(git_refspec_rtransform(
    &remoteRefName, git_remote_get_refspec(remote, 0), git_reference_name(remoteRef)));

  // let pre-push hook know we are about to perform a remote delete
  {
    GIT_PTR(commit, remoteCommit);
    GIT_STATUS_RETURN(git_reference_peel(
      reinterpret_cast<git_object **>(&remoteCommit), remoteRef, GIT_OBJECT_COMMIT));
    auto remoteOid = String(git_oid_tostr_s(git_commit_id(remoteCommit)));

    auto prePushRes = prePushHook(
      repo,
      git_remote_name(remote),
      git_remote_url(remote),
      "(delete)",
      "0000000000000000000000000000000000000000",  // 40 0's, a blank oid
      git_reference_name(remoteRef),
      remoteOid.c_str());
    if (prePushRes && *prePushRes != 0) {
      // TODO(pawel) should we respect an aborted delete push? presumable local ref not longer
      // exists
      // TODO(pawel) what would this even mean? it would leave us in an inconsistent state
    }
  }

  // Push refspec which is doing a force push of the branch.
  String ref = strCat(":", remoteRefName.ptr);
  char *refs[] = {const_cast<char *>(ref.c_str())};
  git_strarray specs = {refs, 1};
  GIT_STATUS_RETURN(git_remote_push(remote, &specs, &opts));

  return 0;
}

int fetchRemoteBranch(git_repository *repo, G8Authentication::Reader auth, const char *branch) {
  GIT_PTR(remote, remote);
  GIT_STATUS_RETURN(git_remote_lookup(&remote, repo, ORIGIN));

  git_fetch_options opts = GIT_FETCH_OPTIONS_INIT;
  opts.callbacks = gitRemoteCallbacks(auth);
  opts.callbacks.payload = reinterpret_cast<void *>(&auth);

  String cred = auth.hasCred() ? auth.getCred() : "";
  bool useCreds = !cred.empty();
  auto authStr = format("Authorization: %s", cred.c_str());
  char *authCStr = const_cast<char *>(authStr.c_str());
  if (useCreds) {
    opts.custom_headers.count = 1;
    opts.custom_headers.strings = &authCStr;
  }

  git_strarray refspec;
  Vector<const char *> refs;
  String remoteRef = c8::format("+refs/heads/%s:refs/remotes/origin/%s", branch, branch);
  refs.push_back(remoteRef.c_str());
  refspec.count = refs.size();
  refspec.strings = const_cast<char **>(refs.data());

  GIT_STATUS_RETURN(git_remote_fetch(remote, &refspec, &opts, nullptr));

  return 0;
}

int g8GitSave(G8Authentication::Reader auth, git_repository *repo, bool force) {
  ScopeTimer t("g8-git-save");
  GIT_PTR(tree, tree);
  GIT_STATUS_RETURN(resolveTreeForChangePoint(repo, &tree, "WORKING"));

  git_reference *head = nullptr;
  GIT_STATUS_RETURN(git_reference_lookup(&head, repo, "HEAD"));

  const char *clientName = git_reference_symbolic_target(head);

  GIT_PTR(reference, client);
  GIT_PTR(commit, clientCommit);

  GIT_STATUS_RETURN(git_reference_lookup(&client, repo, clientName));

  GIT_STATUS_RETURN(
    git_reference_peel(reinterpret_cast<git_object **>(&clientCommit), client, GIT_OBJECT_COMMIT));

  // Punt early if client tree is the same as the new save tree and we don't force save.
  GIT_PTR(tree, clientTree);
  GIT_STATUS_RETURN(git_commit_tree(&clientTree, clientCommit));
  if (!force && git_oid_equal(git_tree_id(tree), git_tree_id(clientTree))) {
    return 0;
  }

  // Create an action signature with the default user and current timestamp.
  GIT_PTR(signature, signature);
  GIT_STATUS_RETURN(git_signature_default(&signature, repo));

  String commitMessage = strCat("[g8] save ", git_reference_shorthand(client));

  git_oid commitOid;
  git_oid parentOid;
  GIT_PTR(commit, parent);

  GIT_STATUS_RETURN(git_reference_name_to_id(&parentOid, repo, "HEAD"));
  GIT_STATUS_RETURN(git_commit_lookup(&parent, repo, &parentOid));

  GIT_PTR(reference, master);
  GIT_STATUS_RETURN(lookupMainBranch(&master, repo));

  GIT_PTR(commit, clientForkCommit);

  GIT_STATUS_RETURN(gitFindMergeBase(&clientForkCommit, repo, master, client));

  // Decide whether to create a new commit or amend the previous.
  if (git_oid_equal(git_commit_id(clientCommit), git_commit_id(clientForkCommit))) {
    GIT_STATUS_RETURN(git_commit_create(
      &commitOid,
      repo,
      "HEAD",
      signature,
      signature,
      nullptr,
      commitMessage.c_str(),
      tree,
      1,
      const_cast<const git_commit **>(&parent)));
  } else {
    GIT_STATUS_RETURN(git_commit_amend(
      &commitOid, parent, "HEAD", signature, signature, nullptr, commitMessage.c_str(), tree));
  }

  if (auto postCommitRes = postCommitHook(repo); postCommitRes && *postCommitRes != 0) {
    // TODO(pawel) report non-zero exit status?
  }

  // Push changes to remote
  GIT_STATUS_RETURN(g8GitForcePush(auth, repo, client));

  return 0;
}

int makeWorkIndex(git_repository *repo, git_index **index, git_tree *backingTree) {
  ScopeTimer t("make-work-index");
  const char *glob = "*";
  git_strarray strs = {.strings = (char **)&glob, .count = 1};
  GIT_STATUS_RETURN(git_repository_index(index, repo));
  GIT_STATUS_RETURN(git_index_read(*index, true));
  if (backingTree) {
    GIT_STATUS_RETURN(git_index_read_tree(*index, backingTree));
  }
  GIT_STATUS_RETURN(git_index_add_all(*index, &strs, GIT_INDEX_UPDATE_INDEX, nullptr, nullptr));
  // After adding all to the index, write the updated index back to disk so that
  // subsequent runs are faster and the git status command is correct.
  GIT_STATUS_RETURN(git_index_write(*index));
  return 0;
}

// Attempts to resolve a tree for a given change point
int resolveTreeForChangePoint(git_repository *repo, git_tree **tree, const std::string name) {
  git_oid oid;

  // Working changes are backed by the client fork in cases of sparse checkout.
  if (name == "WORKING") {
    GIT_PTR(tree, clientForkTree);
    GIT_PTR(index, workingIndex);
    GIT_STATUS_RETURN(resolveTreeForChangePoint(repo, &clientForkTree, "FORK"));
    GIT_STATUS_RETURN(makeWorkIndex(repo, &workingIndex, clientForkTree));

    git_oid workingTreeOid;
    GIT_STATUS_RETURN(git_index_write_tree(&workingTreeOid, workingIndex));
    GIT_STATUS_RETURN(git_tree_lookup(tree, repo, &workingTreeOid));
    return 0;
  }

  if (name == "FORK") {
    const char *clientFullName{nullptr};
    GIT_PTR(reference, head);
    GIT_PTR(reference, master);
    GIT_STATUS_RETURN(lookupMainBranch(&master, repo));
    GIT_STATUS_RETURN(git_reference_lookup(&head, repo, "HEAD"));
    clientFullName = git_reference_symbolic_target(head);

    if (clientFullName) {
      GIT_PTR(reference, client);
      GIT_PTR(commit, clientForkCommit);
      GIT_STATUS_RETURN(git_reference_lookup(&client, repo, clientFullName));
      GIT_STATUS_RETURN(gitFindMergeBase(&clientForkCommit, repo, master, client));
      GIT_STATUS_RETURN(git_commit_tree(tree, clientForkCommit));
      return 0;
    }
  }

  if (name == "EMPTY") {
    GIT_STATUS_RETURN(git_oid_fromstr(&oid, "4b825dc642cb6eb9a060e54bf8d69288fbee4904"));
    GIT_STATUS_RETURN(git_tree_lookup(tree, repo, &oid));
    return 0;
  }

  // see if we can find by direct reference
  GIT_PTR(reference, reference);
  auto res = git_reference_lookup(&reference, repo, name.c_str());
  if (res == 0) {
    GIT_STATUS_RETURN(
      git_reference_peel(reinterpret_cast<git_object **>(tree), reference, GIT_OBJECT_TREE));
    return 0;
  }

  // Try to look up by client name
  String clientFullName = makeBranchFullName(makeClientName(name.c_str()).c_str());
  res = git_reference_lookup(&reference, repo, clientFullName.c_str());
  if (res == 0) {
    GIT_STATUS_RETURN(
      git_reference_peel(reinterpret_cast<git_object **>(tree), reference, GIT_OBJECT_TREE));
    return 0;
  }

  // maybe we are a changeset?
  const auto [lsGitRes, changesetList] = g8GitLsChangesets(repo, nullptr);
  GIT_STATUS_RETURN(lsGitRes);
  for (const auto &cs : changesetList.changesets) {
    if (name == cs.name) {
      GIT_PTR(reference, csReference);
      GIT_STATUS_RETURN(git_reference_lookup(&csReference, repo, cs.branchName.c_str()));
      GIT_STATUS_RETURN(
        git_reference_peel(reinterpret_cast<git_object **>(tree), csReference, GIT_OBJECT_TREE));
      return 0;
    }
  }

  // finally try to get by commit hash
  res = git_oid_fromstrp(&oid, name.c_str());
  if (res == 0) {
    if (name.size() < 40) {
      GIT_STATUS_RETURN(expandOid(repo, &oid, name));
    }
    GIT_PTR(commit, commit);
    if (0 == git_commit_lookup(&commit, repo, &oid)) {
      GIT_STATUS_RETURN(git_commit_tree(tree, commit));
      return 0;
    }
  }

  git_error_set_str(GIT_ERROR_INVALID, strCat("could not resolve a tree for ", name).c_str());
  return GIT_ERROR_INVALID;
}

void writeClientInfo(
  ClientContext &ctx,
  G8Client::Builder responseClient,
  const String &branchName,
  git_branch_t gitBranch) {
  responseClient.setName(branchName);
  responseClient.setActive(branchName == ctx.activeClientName);
  // Interrogate the save commit; do not fail request if one of these calls fails
  GIT_PTR(reference, clientRef);
  if (!git_branch_lookup(&clientRef, ctx.repo.get(), branchName.c_str(), gitBranch)) {
    // This is either the save commit or the client fork commit (if no save has been made yet)
    GIT_PTR(commit, annotatedEndCommit);
    if (!git_reference_peel(
          reinterpret_cast<git_object **>(&annotatedEndCommit), clientRef, GIT_OBJECT_COMMIT)) {
      auto saveSignature = git_commit_author(annotatedEndCommit);
      responseClient.setLastSaveTimeSeconds(saveSignature->when.time);

      GIT_PTR(commit, ancestorCommit);
      // We could fail to find a merge base if something wild happens, like a history rewrite.
      if (0 == gitFindMergeBase(&ancestorCommit, ctx.repo.get(), clientRef, ctx.master.get())) {
        responseClient.setForkId(git_oid_tostr_s(git_commit_id(ancestorCommit)));
      }
    }
  }
}

void writeResponseClients(ClientContext &ctx) {
  auto req = ctx.request.reader();
  auto &response = ctx.response;
  bool includeRemote = req.getIncludeRemote();

  if (includeRemote || req.getIncludeCloudSaveInfo()) {
    VOID_CHECK_GIT(fetchRemote(ctx.repo.get(), req.getAuth(), req.getUser().cStr(), includeRemote));
  }
  const auto [lsClientsRes, clientBranches, remoteBranches] =
    g8GitLsClients(ctx.repo.get(), includeRemote);
  VOID_CHECK_GIT(lsClientsRes);

  {
    ScopeTimer t("write-response-clients");
    auto responseClient = response.builder().initClient(clientBranches.size());

    for (int i = 0; i < clientBranches.size(); i++) {
      writeClientInfo(ctx, responseClient[i], clientBranches[i], GIT_BRANCH_LOCAL);

      if (req.getIncludeCloudSaveInfo()) {
        // Figure out timestamp of latest remote save commit.
        GIT_PTR(reference, remoteClientRef);
        String remoteRefName =
          format("%s/%s-%s", ORIGIN, req.getUser().cStr(), clientBranches[i].c_str());
        if (!git_branch_lookup(
              &remoteClientRef, ctx.repo.get(), remoteRefName.c_str(), GIT_BRANCH_REMOTE)) {
          GIT_PTR(commit, remoteSaveCommit);
          if (!git_reference_peel(
                reinterpret_cast<git_object **>(&remoteSaveCommit),
                remoteClientRef,
                GIT_OBJECT_COMMIT)) {
            auto saveSignature = git_commit_author(remoteSaveCommit);
            responseClient[i].setLastCloudSaveTimeSeconds(saveSignature->when.time);
          }
        }
      }
    }

    if (includeRemote) {
    auto responseRemoteClient = response.builder().initRemoteClient(remoteBranches.size());

      for (int i = 0; i < remoteBranches.size(); i++) {
        writeClientInfo(ctx, responseRemoteClient[i], remoteBranches[i], GIT_BRANCH_REMOTE);
      }
    }
  }
}

std::pair<const char *, bool> findBranchByName(git_repository *repo, const char *inputClient) {
  git_branch_iterator *iter;
  git_branch_iterator_new(&iter, repo, GIT_BRANCH_LOCAL);

  git_reference *ref;
  git_branch_t branch_type;

  const char *branchName = NULL;
  bool found = false;

  while (git_branch_next(&ref, &branch_type, iter) == 0) {
    const char *tempBranchName;
    git_branch_name(&tempBranchName, ref);
    int result = strcasecmp(inputClient, tempBranchName);
    if (result == 0) {
      branchName = tempBranchName;
      found = true;
      break;
    }
    git_reference_free(ref);
  }

  return std::make_pair(branchName, found);
}

}  // namespace g8
}  // namespace c8
