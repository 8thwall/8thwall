// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Tools for running g8 unit tests and verifying expected outcomes.

#pragma once

#include "c8/git/g8-api.capnp.h"
#include "c8/io/capnp-messages.h"
#include "c8/map.h"
#include "c8/scope-exit.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "gmock/gmock.h"

namespace c8 {

struct LocalOriginRepo {
  // Root path for the repo.
  String path;

  // Daemon that will serve the repo until the LocalOriginRepo goes out of scope.
  ScopeExit closeDaemon;

  // Clean up local and remote repo directories when the LocalOriginRepo goes out of scope.
  ScopeExit cleanDirectories;
};

// Inline definition of G8FileInfo.
struct FileInfo {
  G8FileInfo::Status status = G8FileInfo::Status::UNMODIFIED;
  String path = "";
  bool dirty = false;
};

// Inline definition of G8MergeDecision
struct MergeDecision {
  String path = "";
  G8MergeDecision::Choice choice = G8MergeDecision::Choice::UNDECIDED;
  ConstRootMessage<G8MergeTriplet> fileId;
  String blobId = "";  // used for MANUAL_MERGE
};

struct AdditionalChange {
  String path;
  String blobId;
};

// Inline partial definition of G8MergeAnalysisInfo
struct MergeAnalysisInfo {
  G8MergeAnalysisInfo::Status status;
  String path = "";
  String previousPath = "";
};

// Inline partial definition of G8MergeAnalysisInspect.
struct MergeAnalysisInspect {
  String path;
  String previousBlobId;
  String nextBlobId;
  G8MergeAnalysisInspect::ChangeSource source;
  G8MergeAnalysisInspect::Status status;
};

struct MergeAnalysis {
  Vector<MergeAnalysisInfo> info;
  Vector<MergeAnalysisInspect> inspect;
};

String getHashForClient(const LocalOriginRepo &repo, String client);

// Pipes to git hash-object --stdin to generate a blob id.
String getHashForString(String &data);

// Create a new empty repository for unit tests. This repo is hosted locally and is backed
// by a subprocess git daemon that serves the origin. The repo lasts as long as the returned
// object stays in scope, and then the subprocess is killed and resources are reclaimed.
LocalOriginRepo testRepo();

// Checks whether the LocalOriginRepo is a valid and initialized test repo.
bool isInitializedTestRepo(const LocalOriginRepo &repo);

// Empty the contents of the local repo of all git state.
void clearRepo(const LocalOriginRepo &repo);

// Delete a file in the repo
void deleteFile(const char *path);

// Checks if file exists.
bool fileExists(const String &path);

mode_t getFileMode(const String &path);

int setFileMode(const String &path, mode_t mode);

// Checks if client or client's parent is the merge base.
bool branchIsValidClient(const LocalOriginRepo &repo, const char *client);

// Land a changeset and return id of the commit
String simulateRemoteLand(
  const LocalOriginRepo &repo, const char *client, const char *changeset, const char *msg);

// Empty the contents of the local repo of all git state.
ConstRootMessage<G8RepositoryResponse> cloneRepo(const LocalOriginRepo &repo);

// Create a new client with the specified name in the repo.
ConstRootMessage<G8ClientResponse> createClient(const LocalOriginRepo &repo, const char *name);

// Create a new client with the specified name in the repo.
void createRemoteClient(const LocalOriginRepo &repo, const char *name);

// Delete client specified.
ConstRootMessage<G8ClientResponse> deleteClient(const LocalOriginRepo &repo, const char *name);

ConstRootMessage<G8FileResponse> revertFiles(
  const LocalOriginRepo &repo, const Vector<String> &files);

// Run the G8FileRequest::LIST action for listing files in the the working directory and index.
ConstRootMessage<G8FileResponse> lsFiles(const LocalOriginRepo &repo, const Vector<String> &paths);

// Lists clients in repo.
ConstRootMessage<G8ClientResponse> listClients(const LocalOriginRepo &repo);

// List local and remote clients in repo.
ConstRootMessage<G8ClientResponse> listRemoteClients(const LocalOriginRepo &repo);

// Switch to the client with the specified name in the repo.
ConstRootMessage<G8ClientResponse> switchClient(const LocalOriginRepo &repo, const char *name);

// Save the client with the specified name to origin.
ConstRootMessage<G8ClientResponse> saveClient(const LocalOriginRepo &repo, const char *name);

// Get the status of files in the client.
ConstRootMessage<G8ChangesetResponse> statusWithoutRenames(const LocalOriginRepo &repo);

// Get status of files in the client with renames
ConstRootMessage<G8ChangesetResponse> statusWithRenames(const LocalOriginRepo &repo);

// Get the status of an inactive client.
ConstRootMessage<G8ChangesetResponse> statusForClient(
  const LocalOriginRepo &repo, const char *client);

// Create a changeset; this EXPECTS_THAT the changeset was validly created
ConstRootMessage<G8ChangesetResponse> createChangeset(
  const LocalOriginRepo &repo,
  const String &description,
  const Vector<String> &files,
  bool includeCommitId = false);

// given a changeset, return the id
String changesetId(G8ChangesetResponse::Reader r);

// Perform a finish land operation
ConstRootMessage<G8ChangesetResponse> finishLand(
  const LocalOriginRepo &repo, const char *client, const char *changeset);

// Canonical land operation, including syncing
ConstRootMessage<G8ClientResponse> land(
  const LocalOriginRepo &repo, const char *client, const char *changeset, const char *msg);

// Update Changeset
ConstRootMessage<G8ChangesetResponse> update(
  const LocalOriginRepo &repo, const char *client, const char *changeset);

// Add files to changeset
ConstRootMessage<G8ChangesetResponse> setFiles(
  const LocalOriginRepo &repo,
  const char *client,
  const char *changeset,
  const Vector<String> &files);

// Perform a sync operation on the current client given a list of MergeDecisions
ConstRootMessage<G8ClientResponse> syncClient(
  const LocalOriginRepo &repo,
  const String &client,
  const String &headId,
  const Vector<MergeDecision> &decisions,
  const String &inspectRegex = "",
  const bool inspectComplete = false,
  const Vector<AdditionalChange> & = {});

// Perform a patch operation on the current client given a list of MergeDecisions.
ConstRootMessage<G8ClientResponse> patchClient(
  const LocalOriginRepo &repo,
  const String &client,
  const String &changeSourceBranch,
  const Vector<MergeDecision> &decisions);

// Perform a patch operation on the current client given a list of MergeDecisions.
ConstRootMessage<G8ClientResponse> patchRef(
  const LocalOriginRepo &repo,
  const String &client,
  const String &ref,
  const Vector<MergeDecision> &decisions);

// Creates a blob
ConstRootMessage<G8CreateBlobResponse> createBlob(const LocalOriginRepo &repo, String data);

// Perform house keeping.
ConstRootMessage<G8RepositoryResponse> keepHouse(const LocalOriginRepo &repo);

// Helper function for matchers that call a function that returns a non-empty string on failure.
bool checkErrorString(const String &error, ::testing::MatchResultListener *result_listener);

// Expect the supplied list of clients, and that the named client is active. If active is not set,
// don't make any assertions about which client is active. Returns a non-empty error string on
// failure.
String equalsClients(
  G8ClientResponse::Reader r, const Vector<String> &clients, const char *active = nullptr);

MATCHER_P2(EqualsClientsTempl, expectedClients, expectedActive, "") {
  return checkErrorString(equalsClients(arg, expectedClients, expectedActive), result_listener);
}

inline decltype(auto) EqualsClients(const Vector<String> &clients, const char *active = nullptr) {
  return EqualsClientsTempl<const Vector<String> &, const char *>(clients, active);
}

// Expect the supplied list of clients, remoteClients, and the active client. If active is
// not set, don't make any assertions about which client is active. Returns a non-empty error string
// on failure.
String equalsRemoteClients(
  G8ClientResponse::Reader r,
  const Vector<String> &clients,
  const Vector<String> &remoteClients,
  const char *active = nullptr);

MATCHER_P3(EqualsRemoteClientsTempl, expectedClients, expectedRemoteClients, expectedActive, "") {
  return checkErrorString(
    equalsRemoteClients(arg, expectedClients, expectedRemoteClients, expectedActive),
    result_listener);
}

inline decltype(auto) EqualsRemoteClients(
  const Vector<String> &clients,
  const Vector<String> &remoteClients,
  const char *active = nullptr) {
  return EqualsRemoteClientsTempl<const Vector<String> &, const Vector<String> &, const char *>(
    clients, remoteClients, active);
}

// Expect the supplied lists of files (sorted by changeset) to be returned from status(...).
// The input is a map from changeset id to files in that changeset. Since non-working changeset
// ids are generated dynamically, organizing them as a map saves callers the requirement of needing
// to sort them lexicographically at runtime. Returns a non-empty error string on failure.
String equalsStatus(
  G8ChangesetResponse::Reader r, const TreeMap<String, Vector<FileInfo>> &changesetFiles);

MATCHER_P(EqualsStatusTempl, expected, "") {
  return checkErrorString(equalsStatus(arg, expected), result_listener);
}

inline decltype(auto) EqualsStatus(const TreeMap<String, Vector<FileInfo>> &expected) {
  return EqualsStatusTempl<const TreeMap<String, Vector<FileInfo>> &>(expected);
}

// Expect the list of file statuses to be returned by a sync command
// An empty list means a successful merge
String equalsMerge(
  G8ClientResponse::Reader r,
  const Vector<MergeAnalysisInfo> &mergeFiles,
  const Vector<MergeAnalysisInspect> &inspectFiles = {});

MATCHER_P(EqualsMergeTempl, expected, "") {
  return checkErrorString(equalsMerge(arg, expected), result_listener);
}

inline decltype(auto) EqualsMerge(const Vector<MergeAnalysisInfo> &expected) {
  return EqualsMergeTempl<const Vector<MergeAnalysisInfo>>(expected);
}

MATCHER_P(EqualsMergeTempl2, expected, "") {
  return checkErrorString(equalsMerge(arg, expected.info, expected.inspect), result_listener);
}

inline decltype(auto) EqualsMerge2(const MergeAnalysis &expected) {
  return EqualsMergeTempl2<const MergeAnalysis>(expected);
}

String equalsFiles(G8FileResponse::Reader r, const Vector<const char *> &paths);

MATCHER_P(EqualsFilesTempl, expectedFiles, "") {
  return checkErrorString(equalsFiles(arg, expectedFiles), result_listener);
}

inline decltype(auto) EqualsFiles(const Vector<const char *> &paths) {
  return EqualsFilesTempl<const Vector<const char *>>(paths);
}

// Expect that the g8 response is OK
MATCHER(IsOk, "") {
  *result_listener << "Status code is " << arg.getStatus().getCode() << ", message is \""
                   << arg.getStatus().getMessage().cStr() << "\".";
  return arg.getStatus().getCode() == 0;
}

// Expect that sync resolved all conflicts.
MATCHER(SyncComplete, "") {
  return arg.getMerge().getStatus() == G8MergeAnalysis::MergeStatus::COMPLETE;
}

// Expect that sync did not complete due to unresolved conflicts.
MATCHER(SyncNeedsAction, "") {
  return arg.getMerge().getStatus() == G8MergeAnalysis::MergeStatus::NEEDS_ACTION;
}

// Expect a non-failure and a single changeset info to be returned with an id
String isValidCreatedChangeset(G8ChangesetResponse::Reader r);

MATCHER(IsValidCreatedChangeset, "") {
  return checkErrorString(isValidCreatedChangeset(arg), result_listener);
}

// Format a G8GlientResponse::Reader for debug printing.
void PrintTo(const c8::G8ClientResponse::Reader &response, std::ostream *os);

// Format a G8ChangesetResponse::Reader for debug printing.
void PrintTo(const c8::G8ChangesetResponse::Reader &response, std::ostream *os);

// Format a G8FilesResponse::Reader for debug printing.
void PrintTo(const c8::G8FileResponse::Reader &response, std::ostream *os);

}  // namespace c8
