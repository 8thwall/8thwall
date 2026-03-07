// Copyright (c) 2019 8th Wall, Inc.

#pragma once

#include <map>
#include <optional>
#include <regex>
#include <set>
#include <vector>

#include "c8/git/g8-api.capnp.h"
#include "c8/io/capnp-messages.h"
#include "c8/scope-exit.h"
#include "c8/string/format.h"
#include "c8/string/strcat.h"
#include "c8/vector.h"
#include "git2.h"

#define GIT_PTR(type, name)    \
  git_##type *name = nullptr;  \
  SCOPE_EXIT([&name] {         \
    if (name)                  \
      git_##type##_free(name); \
  })

#define CHECK_GIT(result)                                      \
  {                                                            \
    if ((result) != 0) {                                       \
      const git_error *err = git_error_last();                 \
      response.builder().getStatus().setCode(err->klass);      \
      response.builder().getStatus().setMessage(err->message); \
      return response;                                         \
    }                                                          \
  }

#define VOID_CHECK_GIT(result)                                 \
  {                                                            \
    if ((result) != 0) {                                       \
      const git_error *err = git_error_last();                 \
      response.builder().getStatus().setCode(err->klass);      \
      response.builder().getStatus().setMessage(err->message); \
      return;                                                  \
    }                                                          \
  }

#define GIT_STATUS_RETURN(result) \
  {                               \
    if ((result) != 0) {          \
      return result;              \
    }                             \
  }

// For use with a pair in which the gitStatus is the first element.
#define GIT_STATUS_RETURN_PAIR(expr, defaultValue) \
  {                                                \
    auto res = expr;                               \
    if (res != 0) {                                \
      return {res, defaultValue};                  \
    }                                              \
  }

#define RESPOND_ERROR(msg)                        \
  response.builder().getStatus().setCode(1);      \
  response.builder().getStatus().setMessage(msg); \
  return response

#define GPTR(type, name) \
  std::unique_ptr<git_##type, std::function<void(git_##type *)>> name{nullptr, git_##type##_free};

template <typename Ptr, typename Func, class... Args>
int assign_ptr(Func func, Ptr &ptr, Args &&...args) {
  typename std::remove_reference<decltype(*Ptr())>::type *temp;
  int ret = func(&temp, std::forward<Args>(args)...);
  ptr.reset(temp);
  return ret;
}

#define CHECK_ASSIGN(...)                                          \
  {                                                                \
    if ((assign_ptr(__VA_ARGS__)) != 0) {                          \
      const git_error *err = git_error_last();                     \
      ctx.response.builder().getStatus().setCode(err->klass);      \
      ctx.response.builder().getStatus().setMessage(err->message); \
      return ctx.response;                                         \
    }                                                              \
  }

namespace c8 {
namespace g8 {

int gitRepositoryInit();

struct InitializedLibGit2 {
  InitializedLibGit2() { gitRepositoryInit(); }

  ~InitializedLibGit2() { git_libgit2_shutdown(); }
};

struct FunctionContext {
private:
  InitializedLibGit2 libgit2;

public:
  GPTR(repository, repo);
  GPTR(reference, master);
};

struct ChangesetContext : FunctionContext {
  MutableRootMessage<G8ChangesetResponse> response;
};
struct ClientContext : FunctionContext {
  MutableRootMessage<G8ClientResponse> response;
  MutableRootMessage<G8ClientRequest> request;
  String activeClientName;
};

constexpr char ORIGIN[] = "origin";
constexpr char CLIENT_PREFIX[] = "";
constexpr char HEADS_PREFIX[] = "refs/heads/";
constexpr char CHANGESETS_PREFIX[] = "refs/heads/";
constexpr char CHANGESET_CODE[] = "CS";
constexpr char WORKING_CHANGESET[] = "working";

//
// Naming helpers
//

bool clientNameIsValid(const char *client);

bool changesetNameIsValid(const char *changeset);

bool userNameIsValid(const char *user);

// Returns (gitStatus, bool). Check gitStatus for error from underlying git functions.
std::pair<int, bool> branchIsValidClient(git_repository *repo, const char *client);

namespace {
inline String makeClientName(const char *name) { return strCat(CLIENT_PREFIX, name); }

inline String makeChangesetName(const char *client, const char *cs) {
  return strCat(client, "-", CHANGESET_CODE, cs);
}

inline String makeRemoteClientName(const char *user, const char *client) {
  return strCat(user, "-", client);
}

inline String makeRemoteChangesetName(const char *user, const char *client, const char *cs) {
  return strCat(user, "-", client, "-", CHANGESET_CODE, cs);
}

inline String makeBranchFullName(const char *branchShortName) {
  return strCat(HEADS_PREFIX, branchShortName);
}

inline bool clientNameIsValid(const char *client, const char *user) {
  return g8::clientNameIsValid(client) && strncmp(client, user, strlen(user)) == 0;
}

inline int lookupMainBranch(git_reference **ref, git_repository *repo) {
  static const char *branchCandidates[] = {"main", "master", "trunk"};
  int res;
  for (int i = 0; i < sizeof(branchCandidates) / sizeof(const char *); i++) {
    res = git_branch_lookup(ref, repo, branchCandidates[i], GIT_BRANCH_LOCAL);
    if (res == 0 || res != GIT_ENOTFOUND) {
      return res;
    }
  }
  git_error_set_str(GIT_ENOTFOUND, "no main branch found in this repo");
  return GIT_ENOTFOUND;
}

}  // namespace

//
// Classes and structures
//

enum class MergeSegmentType {
  COMMON,
  ORIGINAL,
  YOURS,
  THEIRS,
};

struct MergeFileState {
  git_oid commitOid;
  git_oid fileOid;
  git_commit *commit = nullptr;
  git_tree *tree = nullptr;
  git_tree_entry *treeEntry = nullptr;
  git_odb_object *odbObject = nullptr;
  git_merge_file_input mergeFileInput = GIT_MERGE_FILE_INPUT_INIT;

  MergeFileState() = default;

  // Delete copy and assign.
  MergeFileState(const MergeFileState &) = delete;
  MergeFileState &operator=(const MergeFileState &) = delete;

  // Free up git resources.
  ~MergeFileState() {
    git_commit_free(commit);
    git_tree_free(tree);
    git_tree_entry_free(treeEntry);
    git_odb_object_free(odbObject);
  }
};

struct DiffCallbackPayload {
  int index = 0;
  G8FileInfoList::Builder *builder = nullptr;
};

struct GitChangeset {
  String name;
  String summary;
  String branchName;
};

struct GitChangesetList {
  std::vector<GitChangeset> changesets;
};

struct GitDiff {
  G8FileInfo::Status status;
  String oldPath;
  String newPath;
  bool dirty;

  GitDiff(G8FileInfo::Status inStatus, const char *inOldPath, const char *inNewPath)
      : status(inStatus), oldPath(inOldPath), newPath(inNewPath), dirty(false) {}
};

struct GitDiffList {
  String oldName;
  String newName;
  std::vector<GitDiff> files;
};

struct GitLsLookupPayload {
  size_t prefixLength;
  GitChangesetList *list;
  git_repository *repo;
  int gitError{0};
};

struct GitTreeBuilder {
  git_oid oid;
  git_treebuilder *builder = nullptr;
  git_tree_entry *entry = nullptr;
  std::vector<GitTreeBuilder *> subtrees;
};

struct GitTreeWalkPayload {
  git_repository *repo;
  std::map<String, GitTreeBuilder> *builderMap;
  const std::set<String> *files;
};

struct G8DiffPayloadEntry {
  std::optional<MutableRootMessage<G8FileInfo>> info;
  std::vector<MutableRootMessage<G8DiffLine>> lines;
};

struct G8DiffPayload {
  std::map<String, G8DiffPayloadEntry> entries;
  git_tree *baseTree;
  git_tree *changeTree;
};

struct GitLsClients {
  int status;
  Vector<String> clientBranches;
  Vector<String> remoteBranches;
};

//
// Git helpers
//

G8FileInfo::Status g8FileStatusFromDeltaStatus(git_delta_t deltaStatus);

G8DiffLine::Origin diffLineOrigin(git_diff_line_t type);

int gitDiffTreeToTree(
  git_repository *repo,
  git_tree *oldTree,
  git_tree *newTree,
  bool findRenames,
  G8FileInfoList::Builder output);

int gitDiffRefToRef(
  git_repository *repo,
  const char *oldName,
  const char *newName,
  bool findRenames,
  G8FileInfoList::Builder output);

int gitDiffCommitToCommit(
  git_repository *repo,
  git_commit *oldCommit,
  git_commit *newCommit,
  bool findRenames,
  G8FileInfoList::Builder output);

int gitDiffToMergeBase(
  git_repository *repo,
  git_commit *base,
  git_commit *branch,
  bool findRenames,
  G8FileInfoList::Builder output);

int gitDiffToMergeBase(
  git_repository *repo,
  git_reference *client,
  git_reference *changeset,
  bool findRenames,
  G8FileInfoList::Builder output);

int gitDiffTreeToIndex(
  git_repository *repo,
  git_tree *tree,
  git_index *index,
  bool findRenames,
  const git_strarray *pathspec,
  G8FileInfoList::Builder output);

int gitFindMergeBase(
  git_commit **mergeBaseCommit, git_repository *repo, git_reference *one, git_reference *two);

int gitFindMergeBase(
  git_commit **mergeBaseCommit, git_repository *repo, git_commit *one, git_commit *two);

int writeChangesetTree(
  git_oid *out,
  git_repository *repo,
  const git_commit *clientCommit,
  const git_commit *clientForkCommit,
  const std::set<String> *filesToInclude,
  const std::set<String> &filesToExclude,
  const git_tree *clientTreeOverride);

void setRemoteInfo(G8GitRemote::Builder builder, const c8::String &remoteUrl);
void setRemoteInfo(G8GitRemote::Builder builder, const git_remote *remote);

int fetchRemote(
  git_repository *repo,
  G8Authentication::Reader auth,
  const String &user,
  bool includeRemote = false);

int g8SyncMaster(git_repository *repo, G8Authentication::Reader auth, const String &user);

GitLsClients g8GitLsClients(git_repository *repo, bool includeRemote = false);

std::pair<int, GitChangesetList> g8GitLsChangesets(git_repository *repo, const char *clientName);

int expandOid(git_repository *repo, git_oid *oid, String shortened);

git_remote_callbacks gitRemoteCallbacks(G8Authentication::Reader reader);

int g8GitForcePush(G8Authentication::Reader auth, git_repository *repo, git_reference *client);

int g8GitDeleteRemoteBranch(
  G8Authentication::Reader auth, git_repository *repo, git_reference *branch);

// This will just fetch the remote branch info and will be accessible as GIT_BRANCH_REMOTE.
int fetchRemoteBranch(git_repository *repoo, G8Authentication::Reader auth, const char *branch);

int g8GitSave(G8Authentication::Reader auth, git_repository *repo, bool force);

// Replacement for makeWorkingTree() which preserves sparse status on entries.
// The backingTree is what is used for sparse entries.
int makeWorkIndex(git_repository *repo, git_index **index, git_tree *backingTree);

int resolveTreeForChangePoint(git_repository *repo, git_tree **tree, const std::string name);

void setRemoteProgressCallbacks(git_remote_callbacks *callbacks);

void writeClientInfo(
  ClientContext &ctx, G8Client::Builder responseClient, const String &branchName, git_branch_t gitBranch);

// NOTE(christoph): Errors from this function are not checked because it is the last thing
// called by consumers before returning the response.
void writeResponseClients(ClientContext &ctx);

std::pair<const char *, bool> findBranchByName(git_repository *repo, const char *inputClient);

};  // namespace g8
};  // namespace c8
