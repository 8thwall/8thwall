// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "medium";
  deps = {
    ":g8-api.capnp-cc",
    ":g8-git",
    ":g8-testing",
    ":g8-testing-strings",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8/io:capnp-messages",
    "//c8/io:file-io",
    "//c8:process",
    "//c8/string:format",
    "//c8/string:trim",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x413b6d3e);

#include <capnp/pretty-print.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sstream>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/git/g8-api.capnp.h"
#include "c8/git/g8-testing-strings.h"
#include "c8/git/g8-testing.h"
#include "c8/io/capnp-messages.h"
#include "c8/io/file-io.h"
#include "c8/process.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/format.h"
#include "c8/string/trim.h"
#include "gmock/gmock.h"

namespace fs = std::filesystem;

using testing::StrEq;

namespace c8 {

class G8ApiTest : public ::testing::Test {
public:
  // Create a test repo for each test.
  LocalOriginRepo repo;
  G8ApiTest() : repo(testRepo()) {}
};

TEST_F(G8ApiTest, CreateChangeset) {
  // create initial client
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // And switch to it
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Make an initial file
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "a");

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::ADDED, "a.txt", true}}}}));

  // Create the changeset with this file
  auto createCsRes = createChangeset(repo, "test changeset", {"a.txt"});
  EXPECT_THAT(createCsRes.reader(), IsValidCreatedChangeset());
  String cs1 = changesetId(createCsRes.reader());

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{cs1, {{G8FileInfo::Status::ADDED, "a.txt", false}}}, {"working", {}}}));
}

TEST_F(G8ApiTest, MultipleStatus) {
  // Switch to initial client
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Create a file
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "a");

  // Switch to c2
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c1"));
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Create a different file
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "b");

  // My other still has the original
  EXPECT_THAT(
    statusForClient(repo, "c1").reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::ADDED, "a.txt", false}}}}));

  // I have a status now
  EXPECT_THAT(
    statusForClient(repo, "c2").reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::ADDED, "b.txt", true}}}}));
}

TEST_F(G8ApiTest, ValidClient) {
  // Create a valid client.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // Create another valid client with a number.
  EXPECT_THAT(createClient(repo, "0").reader(), EqualsClients({"0", "c1"}));

  // Create another valid client.
  EXPECT_THAT(createClient(repo, "a").reader(), EqualsClients({"0", "a", "c1"}));

  // Create another valid client with an uppercase.
  EXPECT_THAT(createClient(repo, "B").reader(), EqualsClients({"0", "a", "B", "c1"}));

  // Create another valid client with a dash.
  EXPECT_THAT(createClient(repo, "0-0").reader(), EqualsClients({"0", "0-0", "a", "B", "c1"}));

  // Create another valid client with an underscore and a dash.
  EXPECT_THAT(
    createClient(repo, "0-_0").reader(), EqualsClients({"0", "0-0", "0-_0", "a", "B", "c1"}));

  // Ensure that case-insensitive switching works.
  EXPECT_THAT(
    switchClient(repo, "A").reader(), EqualsClients({"0", "0-0", "0-_0", "a", "B", "c1"}, "a"));

  // Ensure that case-insensitive switching works.
  EXPECT_THAT(
    switchClient(repo, "b").reader(), EqualsClients({"0", "0-0", "0-_0", "a", "B", "c1"}, "B"));

  // Create an invalid client with a only a dash.
  createClient(repo, "-");

  // Create an invalid client with a only an underscore.
  createClient(repo, "_");

  // Create an invalid client with an invalid character.
  createClient(repo, "+");

  // Create another invalid client with an invalid character.
  createClient(repo, ".");

  // Create an invalid client ending with a dash.
  createClient(repo, "a-");

  // Create an invalid client starting with a dash.
  createClient(repo, "-a");

  // Ensure that delete was successful.
  EXPECT_THAT(
    switchClient(repo, "0").reader(), EqualsClients({"0", "0-0", "0-_0", "a", "B", "c1"}, "0"));
}

TEST_F(G8ApiTest, Client) {
  // Create a client.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // Change to the client.
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Create another client.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));

  // Switch to the second client.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Write a file in the second client.
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "a");

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::ADDED, "a.txt", true}}}}));

  // Switch to the first client.
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  // Check client status
  EXPECT_THAT(statusWithRenames(repo).reader(), EqualsStatus({{"working", {}}}));

  // Write a file in the first client.
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "b");

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::ADDED, "b.txt", true}}}}));

  // Switch to the second client.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::ADDED, "a.txt", false}}}}));

  // Delete a client.
  EXPECT_THAT(deleteClient(repo, "c1").reader(), IsOk());

  // Ensure that delete was successful.
  EXPECT_THAT(listClients(repo).reader(), EqualsClients({"c2"}, "c2"));
}

TEST_F(G8ApiTest, Save) {
  // Create a client.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // Create another client.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));

  // Switch to the second client.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Write a file in the second client.
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "a");

  // Save the client with the file.
  EXPECT_THAT(saveClient(repo, "c2").reader(), IsOk());

  // Clear all git state in the local repo.
  EXPECT_TRUE(isInitializedTestRepo(repo));
  clearRepo(repo);
  EXPECT_FALSE(isInitializedTestRepo(repo));

  // Re-clone the repo.
  EXPECT_THAT(cloneRepo(repo).reader(), IsOk());
  EXPECT_TRUE(isInitializedTestRepo(repo));

  // Switch to the first client.
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  // Check client status, no files in this client.
  EXPECT_THAT(statusWithRenames(repo).reader(), EqualsStatus({{"working", {}}}));

  // Switch to the second client.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Check client status, this client has our file.
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::ADDED, "a.txt", false}}}}));
}

TEST_F(G8ApiTest, LandUpdatedChangeset) {
  // create initial client
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // And switch to it
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Make two files
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "a");
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "b");

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{"working",
        {{G8FileInfo::Status::ADDED, "a.txt", true},
         {G8FileInfo::Status::ADDED, "b.txt", true}}}}));

  // Create the changeset with this file
  String cs1 = changesetId(createChangeset(repo, "test changeset", {"a.txt", "b.txt"}).reader());

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{cs1,
        {{G8FileInfo::Status::ADDED, "a.txt", false}, {G8FileInfo::Status::ADDED, "b.txt", false}}},
       {"working", {}}}));

  // Make additional changes
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "b modified");

  // Check client status has dirty bit for b
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{cs1,
        {{G8FileInfo::Status::ADDED, "a.txt", false}, {G8FileInfo::Status::ADDED, "b.txt", true}}},
       {"working", {}}}));

  // Update changeset
  EXPECT_THAT(update(repo, "c1", cs1.c_str()).reader(), IsOk());

  // Check client status has cleared dirty bit (change was saved)
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{cs1,
        {{G8FileInfo::Status::ADDED, "a.txt", false}, {G8FileInfo::Status::ADDED, "b.txt", false}}},
       {"working", {}}}));

  // Land and ensure a clean sync
  EXPECT_THAT(land(repo, "c1", cs1.c_str(), "test changeset").reader(), EqualsMerge({}));

  // Check client status
  EXPECT_THAT(statusWithRenames(repo).reader(), EqualsStatus({{"working", {}}}));
}

TEST_F(G8ApiTest, LandChangesetLocalDirty) {
  // create initial client
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // And switch to it
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Make two files
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "a");
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{"working",
        {{G8FileInfo::Status::ADDED, "a.txt", true},
         {G8FileInfo::Status::ADDED, "b.txt", true}}}}));

  // Create the changeset with this file
  String cs1 = changesetId(createChangeset(repo, "test changeset", {"a.txt", "b.txt"}).reader());

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{cs1,
        {{G8FileInfo::Status::ADDED, "a.txt", false}, {G8FileInfo::Status::ADDED, "b.txt", false}}},
       {"working", {}}}));

  // Make a small change
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n99");

  // Land and ensure that our dirty changes conflict with what was landed
  auto landStatus = land(repo, "c1", cs1.c_str(), "test changeset");
  EXPECT_THAT(
    landStatus.reader(), EqualsMerge({{G8MergeAnalysisInfo::Status::CONFLICTED, "b.txt", ""}}));

  // TODO this describes the current way things work. ideally there should be no
  // conflict in this case

  // Check client status
  // EXPECT_THAT(statusWithRenames(repo).reader(),
  // EqualsStatus({{"working", {}}}));
}

TEST_F(G8ApiTest, RenameDetection) {
  // create initial client
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // And switch to it
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Make initial file
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");

  // create changeset
  String cs1 = changesetId(createChangeset(repo, "initial version of a", {"a.txt"}).reader());

  // Land initial version of a
  auto landStatus = land(repo, "c1", cs1.c_str(), "initial version of a");
  EXPECT_THAT(landStatus.reader(), EqualsMerge({}));

  // Now delete a and write the same contents to b
  deleteFile(format("%s/a.txt", repo.path.c_str()).c_str());
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");

  // Check client status
  // TODO this should show RENAMED as well
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{"working",
        {{G8FileInfo::Status::DELETED, "a.txt", true},
         {G8FileInfo::Status::ADDED, "b.txt", true}}}}));

  // Make a changeset with b
  String cs2 = changesetId(createChangeset(repo, "renamed to b", {"b.txt"}).reader());

  // Check client status
  // Currently we disallow showing renames in the working changeset
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{"working", {{G8FileInfo::Status::DELETED, "a.txt", false}}},
       {cs2.c_str(), {{G8FileInfo::Status::ADDED, "b.txt", false}}}}));

  // Now add the "deleted" file a to the changeset
  EXPECT_THAT(setFiles(repo, "c1", cs2.c_str(), {"a.txt", "b.txt"}).reader(), IsOk());

  // Check client status
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({
      {"working", {}},
      {cs2.c_str(), {{G8FileInfo::Status::RENAMED, "b.txt", false}}},
    }));
}

TEST_F(G8ApiTest, MoveFileAcrossChangeset) {
  // create initial client
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // and switch to it
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // make initial file
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");

  // create changeset
  String cs1 = changesetId(createChangeset(repo, "initial version of a", {"a.txt"}).reader());

  // and land it
  EXPECT_THAT(land(repo, "c1", cs1.c_str(), "initial version of a").reader(), EqualsMerge({}));

  // Now delete a and write the same contents to b (mimic rename)
  deleteFile(format("%s/a.txt", repo.path.c_str()).c_str());
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");

  // Check client status
  // TODO we expect this to be RENAMED as well
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{"working",
        {{G8FileInfo::Status::DELETED, "a.txt", true},
         {G8FileInfo::Status::ADDED, "b.txt", true}}}}));

  // make a changeset with the deleted file
  String cs2 = changesetId(createChangeset(repo, "deleted file", {"a.txt"}).reader());

  // deleted a should be in changeset
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{"working", {{G8FileInfo::Status::ADDED, "b.txt", false}}},
       {cs2.c_str(), {{G8FileInfo::Status::DELETED, "a.txt", false}}}}));

  // make a changeset with just the added b
  String cs3 = changesetId(createChangeset(repo, "added file", {"b.txt"}).reader());

  // deleted a should be in changeset
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{"working", {}},
       {cs3.c_str(), {{G8FileInfo::Status::ADDED, "b.txt", false}}},
       {cs2.c_str(), {{G8FileInfo::Status::DELETED, "a.txt", false}}}}));

  // remove files from cs2
  EXPECT_THAT(setFiles(repo, "c1", cs2.c_str(), {}).reader(), IsOk());

  // and put them into cs3
  // api-changeset.cc logic is to automatically exclude files in other
  // changesets so we have to remove them from cs2
  EXPECT_THAT(setFiles(repo, "c1", cs3.c_str(), {"a.txt", "b.txt"}).reader(), IsOk());

  // deleted a should be in changeset
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{"working", {}},
       {cs3.c_str(), {{G8FileInfo::Status::RENAMED, "b.txt", false}}},
       {cs2.c_str(), {}}}));
}

TEST_F(G8ApiTest, SyncNoConflicts) {
  // Make a client A.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // Make a client B.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));

  // Switch to client A.
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  // client A lands some files
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "a");

  // Create changeset
  String cs1 = changesetId(createChangeset(repo, "ChangeA", {"a.txt"}).reader());

  // And land it.
  EXPECT_THAT(land(repo, "c1", cs1.c_str(), "LandA").reader(), EqualsMerge({}));

  // Switch to client B.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // client B makes other changes
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "b");

  // Sync changes from client A.
  EXPECT_THAT(syncClient(repo, "c2", "", {}).reader(), EqualsMerge({}));

  // Expect that both the synced file and the current file coexist.
  EXPECT_EQ(readTextFile(format("%s/a.txt", repo.path.c_str())), "a");
  EXPECT_EQ(readTextFile(format("%s/b.txt", repo.path.c_str())), "b");

  // Ensure that the sync operations resulted in a valid clients.
  EXPECT_TRUE(branchIsValidClient(repo, "c1"));
  EXPECT_TRUE(branchIsValidClient(repo, "c2"));
}

TEST_F(G8ApiTest, SyncWithConflicts) {
  String ancestorOriginal = "1\n2\n3\n4\n5\n6\n7\n8\n9\n";
  String ancestorSideA = "1\n2\n33\n4\n5\n6\n7\n8\n9\n";
  String ancestorSideB = "1\n2\n3\n4\n5\n6\n7\n88\n9\n";
  String ancestorAutoMerge = "1\n2\n33\n4\n5\n6\n7\n88\n9\n";
  String conflictSideA = "conflict-side-a";
  String conflictSideB = "conflict-side-b";
  String theirsSideA = "theirs-side-a";
  String theirsSideB = "theirs-side-b";
  String oursSideA = "ours-side-a";
  String oursSideB = "ours-side-b";

  // Make a client A.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // Switch to client A.
  // NOTE(pawel) YOU MUST BE ON A CLIENT BEFORE CREATING A CHANGESET
  // (until createChangeset is updated to take in a client name).
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Seed an ancestor file for conflicts.
  writeTextFile(format("%s/ancestor.txt", repo.path.c_str()), ancestorOriginal.c_str());
  String csSeed = changesetId(createChangeset(repo, "seed", {"ancestor.txt"}).reader());
  EXPECT_THAT(land(repo, "c1", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));

  // Make a client B.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));

  // Switch to client A.
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  // client A lands some files
  writeTextFile(format("%s/conflict.txt", repo.path.c_str()), conflictSideA.c_str());
  writeTextFile(format("%s/theirs.txt", repo.path.c_str()), theirsSideA.c_str());
  writeTextFile(format("%s/ours.txt", repo.path.c_str()), oursSideA.c_str());
  writeTextFile(format("%s/ancestor.txt", repo.path.c_str()), ancestorSideA.c_str());

  // Create changeset
  String cs1 = changesetId(
    createChangeset(repo, "ChangeA", {"conflict.txt", "theirs.txt", "ours.txt", "ancestor.txt"})
      .reader());

  // And land it.
  EXPECT_THAT(land(repo, "c1", cs1.c_str(), "LandA").reader(), EqualsMerge({}));

  // Switch to client B.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // client B makes other changes
  writeTextFile(format("%s/conflict.txt", repo.path.c_str()), conflictSideB.c_str());
  writeTextFile(format("%s/theirs.txt", repo.path.c_str()), theirsSideB.c_str());
  writeTextFile(format("%s/ours.txt", repo.path.c_str()), oursSideB.c_str());
  writeTextFile(format("%s/ancestor.txt", repo.path.c_str()), ancestorSideB.c_str());

  // We need to save the client because a sync operation does not do so
  // which can result in loss of local work.
  EXPECT_THAT(saveClient(repo, "c2").reader(), IsOk());

  // Initial sync should show conflict
  auto initialSync = syncClient(repo, "c2", "", {});

  // Expect a conflict on sync.
  EXPECT_THAT(initialSync.reader(), SyncNeedsAction());

  // Check to ensure that the expected file is confliced.
  EXPECT_THAT(
    initialSync.reader(),
    EqualsMerge({
      {G8MergeAnalysisInfo::Status::CONFLICTED, "conflict.txt", ""},
      {G8MergeAnalysisInfo::Status::CONFLICTED, "theirs.txt", ""},
      {G8MergeAnalysisInfo::Status::CONFLICTED, "ours.txt", ""},
      {G8MergeAnalysisInfo::Status::MERGEABLE, "ancestor.txt", ""},
    }));

  // Resolve the conflict by hand
  auto createdBlob = createBlob(repo, "conflict-resolved\n");
  EXPECT_THAT(createdBlob.reader(), IsOk());
  EXPECT_TRUE(createdBlob.reader().hasId());

  std::map<String, G8MergeAnalysisInfo::Reader> mergeInfoMap;

  for (auto info : initialSync.reader().getMerge().getInfo()) {
    mergeInfoMap[info.getPath()] = info;
  }

  // Ensure that file blob ids correspond to the correct side.
  // Client A landed side A and is considered theirs.
  // We are syncing client B which means that side b is ours.
  auto conflictFileId = mergeInfoMap["conflict.txt"].getFileId();
  EXPECT_EQ(conflictFileId.getTheirs().cStr(), getHashForString(conflictSideA));
  EXPECT_EQ(conflictFileId.getYours().cStr(), getHashForString(conflictSideB));
  EXPECT_EQ(conflictFileId.getOriginal().cStr(), String(""));

  auto theirsFileId = mergeInfoMap["theirs.txt"].getFileId();
  EXPECT_EQ(theirsFileId.getTheirs().cStr(), getHashForString(theirsSideA));
  EXPECT_EQ(theirsFileId.getYours().cStr(), getHashForString(theirsSideB));
  EXPECT_EQ(theirsFileId.getOriginal().cStr(), String(""));

  auto oursFileId = mergeInfoMap["ours.txt"].getFileId();
  EXPECT_EQ(oursFileId.getTheirs().cStr(), getHashForString(oursSideA));
  EXPECT_EQ(oursFileId.getYours().cStr(), getHashForString(oursSideB));
  EXPECT_EQ(oursFileId.getOriginal().cStr(), String(""));

  auto ancestorFileId = mergeInfoMap["ancestor.txt"].getFileId();
  EXPECT_EQ(ancestorFileId.getTheirs().cStr(), getHashForString(ancestorSideA));
  EXPECT_EQ(ancestorFileId.getYours().cStr(), getHashForString(ancestorSideB));
  EXPECT_EQ(ancestorFileId.getOriginal().cStr(), getHashForString(ancestorOriginal));

  // Ensure that all decisions are filled out.
  Vector<MergeDecision> decisions(initialSync.reader().getMerge().getInfo().size());

  auto &decision1 = decisions[0];
  decision1.path = "conflict.txt";
  decision1.choice = G8MergeDecision::Choice::MANUAL_MERGE;
  decision1.fileId = mergeInfoMap["conflict.txt"].getFileId();
  decision1.blobId = createdBlob.reader().getId();

  auto &decision2 = decisions[1];
  decision2.path = "theirs.txt";
  decision2.choice = G8MergeDecision::Choice::THEIR_CHANGE;
  decision2.fileId = mergeInfoMap["theirs.txt"].getFileId();

  auto &decision3 = decisions[2];
  decision3.path = "ours.txt";
  decision3.choice = G8MergeDecision::Choice::YOUR_CHANGE;
  decision3.fileId = mergeInfoMap["ours.txt"].getFileId();

  auto &decision4 = decisions[3];
  decision4.path = "ancestor.txt";
  decision4.choice = G8MergeDecision::Choice::AUTO_MERGE;
  decision4.fileId = mergeInfoMap["ancestor.txt"].getFileId();

  // Sync to resolve the conflict.
  EXPECT_THAT(syncClient(repo, "c2", "", decisions).reader(), SyncComplete());

  // Expect that sync resolved the conflict as requested.
  EXPECT_EQ(readTextFile(format("%s/conflict.txt", repo.path.c_str())), "conflict-resolved\n");
  EXPECT_EQ(readTextFile(format("%s/theirs.txt", repo.path.c_str())), theirsSideA.c_str());
  EXPECT_EQ(readTextFile(format("%s/ours.txt", repo.path.c_str())), oursSideB.c_str());
  EXPECT_EQ(readTextFile(format("%s/ancestor.txt", repo.path.c_str())), ancestorAutoMerge.c_str());

  // Create changeset
  String cs2 = changesetId(createChangeset(repo, "Resolved", {"conflict.txt"}).reader());

  // We need to save the client because a sync operation does not do so
  // which can result in loss of local work.
  EXPECT_THAT(saveClient(repo, "c2").reader(), IsOk());

  // And land it. An empty MergeInfo means that conflicts were properly resolved by previous sync.
  EXPECT_THAT(land(repo, "c2", cs2.c_str(), "Resolved").reader(), SyncComplete());

  // Expect that land preserved the local state.
  EXPECT_EQ(readTextFile(format("%s/conflict.txt", repo.path.c_str())), "conflict-resolved\n");

  // Ensure that the sync operations resulted in a valid clients.
  EXPECT_TRUE(branchIsValidClient(repo, "c1"));
  EXPECT_TRUE(branchIsValidClient(repo, "c2"));
}

TEST_F(G8ApiTest, SyncBackInTime) {
  // Make a client A and switch to it.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // This hash has an empty tree.
  String initialHash = getHashForClient(repo, "c1");

  // Let's get an original file here
  writeTextFile(format("%s/original.txt", repo.path.c_str()), "original file");
  {
    String cs = changesetId(createChangeset(repo, "Original", {"original.txt"}).reader());
    EXPECT_THAT(land(repo, "c1", cs.c_str(), "Original").reader(), SyncComplete());
  }

  String backHash = getHashForClient(repo, "c1");

  // Make a bunch of lands to generate a reasonable master line.
  std::stringstream ss;
  ss << "initial file\n";
  String midHash;
  String midAContents;
  for (auto i = 0; i < 10; i++) {
    ss << i << "\n";
    writeTextFile(format("%s/a.txt", repo.path.c_str()), ss.str().c_str());
    String cs =
      changesetId(createChangeset(repo, format("Change %d", i).c_str(), {"a.txt"}).reader());
    EXPECT_THAT(
      land(repo, "c1", cs.c_str(), format("Land %d", i).c_str()).reader(), SyncComplete());
    if (i == 5) {
      midHash = getHashForClient(repo, "c1");
      midAContents = ss.str();
    }
  }

  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  writeTextFile(format("%s/saved.txt", repo.path.c_str()), "some local saved changes");
  EXPECT_THAT(saveClient(repo, "c2").reader(), IsOk());

  writeTextFile(format("%s/unsaved.txt", repo.path.c_str()), "some unsaved changes");

  // Sync to the point where we added original file.
  EXPECT_THAT(syncClient(repo, "c2", backHash.c_str(), {}).reader(), EqualsMerge({}));

  // Ensure that state of files are as expected.
  EXPECT_THAT(readTextFile(format("%s/saved.txt", repo.path.c_str())), "some local saved changes");
  EXPECT_THAT(readTextFile(format("%s/unsaved.txt", repo.path.c_str())), "some unsaved changes");
  EXPECT_THAT(readTextFile(format("%s/original.txt", repo.path.c_str())), "original file");

  // Sync forward to a midpoint 50; also tests giving short hashes.
  EXPECT_THAT(syncClient(repo, "c2", midHash.substr(0, 6).c_str(), {}).reader(), EqualsMerge({}));

  // Ensure client files remains the same and that a.txt is in the expected state.
  EXPECT_THAT(readTextFile(format("%s/saved.txt", repo.path.c_str())), "some local saved changes");
  EXPECT_THAT(readTextFile(format("%s/unsaved.txt", repo.path.c_str())), "some unsaved changes");
  EXPECT_THAT(readTextFile(format("%s/original.txt", repo.path.c_str())), "original file");
  EXPECT_THAT(readTextFile(format("%s/a.txt", repo.path.c_str())), midAContents.c_str());

  // Now land changes, sync to master and ensure that state looks as expected.
  {
    String cs = changesetId(createChangeset(repo, "LandSaved", {"saved.txt"}).reader());
    // TODO(pawel) Figure out why git rev-parse can't fin the changeset.
    // EXPECT_THAT(land(repo, "c1", cs.c_str(), "LandSaved").reader(), SyncComplete());
  }

  EXPECT_THAT(readTextFile(format("%s/saved.txt", repo.path.c_str())), "some local saved changes");
  EXPECT_THAT(readTextFile(format("%s/unsaved.txt", repo.path.c_str())), "some unsaved changes");
  EXPECT_THAT(readTextFile(format("%s/original.txt", repo.path.c_str())), "original file");
  // TODO(pawel) After figuring out why git rev-parse can't find the changeset, uncomment this.
  // EXPECT_THAT(readTextFile(format("%s/a.txt", repo.path.c_str())), ss.str().c_str());

  // Ensure that the sync operations resulted in a valid clients.
  EXPECT_TRUE(branchIsValidClient(repo, "c1"));
  EXPECT_TRUE(branchIsValidClient(repo, "c2"));
}

TEST_F(G8ApiTest, SyncNonActiveClient) {
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));
  String pathA = format("%s/a.txt", repo.path.c_str());

  writeTextFile(pathA, "cheese burger");

  // Switching clients performs a save.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  writeTextFile(pathA, "milk shake");
  String cs1 = changesetId(createChangeset(repo, "land milk shake", {"a.txt"}).reader());
  C8Log("cs1 %s", cs1.c_str());
  EXPECT_THAT(land(repo, "c2", cs1.c_str(), "land milk shake").reader(), EqualsMerge({}));

  // Now, syncing the original client should yield merge conflicts.
  auto syncRes = syncClient(repo, "c1", "", {});
  EXPECT_THAT(
    syncRes.reader(),
    EqualsMerge({
      {G8MergeAnalysisInfo::Status::CONFLICTED, "a.txt", ""},
    }));

  EXPECT_THAT(syncRes.reader(), EqualsClients({"c1"}, "c1"));

  auto createdBlob = createBlob(repo, "manual resolution");
  EXPECT_THAT(createdBlob.reader(), IsOk());
  EXPECT_TRUE(createdBlob.reader().hasId());

  Vector<MergeDecision> decisions(1);
  auto &decision = decisions[0];
  decision.path = "a.txt";
  decision.choice = G8MergeDecision::Choice::MANUAL_MERGE;
  decision.blobId = createdBlob.reader().getId();
  decision.fileId = syncRes.reader().getMerge().getInfo()[0].getFileId();

  EXPECT_THAT(syncClient(repo, "c1", "", decisions).reader(), SyncComplete());

  EXPECT_EQ(readTextFile(pathA), "milk shake");
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));
  EXPECT_EQ(readTextFile(pathA), "manual resolution");

  // Also ensure that the change was pushed remotely by re-cloning.
  clearRepo(repo);
  EXPECT_FALSE(isInitializedTestRepo(repo));
  EXPECT_THAT(cloneRepo(repo).reader(), IsOk());
  EXPECT_TRUE(isInitializedTestRepo(repo));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));
  EXPECT_EQ(readTextFile(pathA), "manual resolution");
}

// This patches in a client who is ahead of us on the master line.
// We expect to see only the changes in the change branch
TEST_F(G8ApiTest, PatchNoConflicts) {
  // Create client A which will hang out in back.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // Create client B which will land a bunch of things and have changes for us to patch in.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Seed a master line.
  for (int i = 1; i <= 3; i++) {
    String fileName = format("seed-%d.txt", i);
    writeTextFile(format("%s/%s", repo.path.c_str(), fileName.c_str()), "seed");
    String csSeed = changesetId(createChangeset(repo, "seed", {fileName.c_str()}).reader());
    EXPECT_THAT(land(repo, "c2", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));
  }

  // No make some changes in client B, that we want to patch into client A.
  writeTextFile(format("%s/patch-1.txt", repo.path.c_str()), "patch file");

  // Switching clients saves changes automatically; go back to client A.
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  EXPECT_FALSE(fileExists(format("%s/seed-1.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/seed-2.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/seed-3.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/patch-1.txt", repo.path.c_str())));

  // Now make some of our own changes (saved).
  writeTextFile(format("%s/saved.txt", repo.path.c_str()), "saved changes");

  EXPECT_THAT(saveClient(repo, "c1").reader(), IsOk());

  // Now some unsaved changes.
  writeTextFile(format("%s/unsaved.txt", repo.path.c_str()), "unsaved changes");

  // Now patch changes from client B into client A.
  EXPECT_THAT(patchClient(repo, "c1", "c2", {}).reader(), EqualsMerge({}));

  // Now expect files from both clients and nothing from master.
  EXPECT_THAT(readTextFile(format("%s/saved.txt", repo.path.c_str())), "saved changes");
  EXPECT_THAT(readTextFile(format("%s/unsaved.txt", repo.path.c_str())), "unsaved changes");
  EXPECT_THAT(readTextFile(format("%s/patch-1.txt", repo.path.c_str())), "patch file");

  // Make sure that none of the intermeidary files showed up.
  EXPECT_FALSE(fileExists(format("%s/seed-1.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/seed-2.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/seed-3.txt", repo.path.c_str())));
}

TEST_F(G8ApiTest, PatchLocalBranch) {
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  writeTextFile(format("%s/a.txt", repo.path.c_str()), "patch file");

  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  EXPECT_FALSE(fileExists(format("%s/a.txt", repo.path.c_str())));

  EXPECT_THAT(patchRef(repo, "c1", "c2", {}).reader(), EqualsMerge({}));

  EXPECT_TRUE(fileExists(format("%s/a.txt", repo.path.c_str())));
}

TEST_F(G8ApiTest, PatchRemoteBranch) {
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  writeTextFile(format("%s/a.txt", repo.path.c_str()), "patch file");

  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  EXPECT_FALSE(fileExists(format("%s/a.txt", repo.path.c_str())));

  EXPECT_THAT(patchRef(repo, "c1", "origin/jenkins-c2", {}).reader(), EqualsMerge({}));

  EXPECT_TRUE(fileExists(format("%s/a.txt", repo.path.c_str())));
}

TEST_F(G8ApiTest, PatchCommit) {
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  writeTextFile(format("%s/a.txt", repo.path.c_str()), "patch file");

  EXPECT_THAT(saveClient(repo, "c2").reader(), IsOk());

  auto changesetRes = createChangeset(repo, "message", {"a.txt"}, true);

  EXPECT_THAT(changesetRes.reader(), IsOk());

  String commitToPatch = changesetRes.reader().getChangeset()[0].getCommitId();
  EXPECT_NE(commitToPatch.size(), 0);

  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  EXPECT_FALSE(fileExists(format("%s/a.txt", repo.path.c_str())));

  EXPECT_THAT(patchRef(repo, "c1", commitToPatch, {}).reader(), EqualsMerge({}));

  EXPECT_TRUE(fileExists(format("%s/a.txt", repo.path.c_str())));
}

// This test is different from PatchWithConflictOnMaterLine below.
// patch operations bring in the diff from the tip of the incoming branch against their
// fork point off master. This test case ensures that when the incoming patch only brings in its
// changes. So if the client has a file that conflicts with something along the master line,
// the patch succeeds because it does not bring in that file.
TEST_F(G8ApiTest, PatchWithConflicts) {
  // Create client A which will hang out in back.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // Create client B which will land a bunch of things and have changes for us to patch in.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Seed a master line.
  for (int i = 1; i <= 3; i++) {
    String fileName = format("seed-%d.txt", i);
    writeTextFile(format("%s/%s", repo.path.c_str(), fileName.c_str()), "seed");
    String csSeed = changesetId(createChangeset(repo, "seed", {fileName.c_str()}).reader());
    EXPECT_THAT(land(repo, "c2", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));
  }

  // No make some changes in client B, that we want to patch into client A.
  writeTextFile(format("%s/patch-1.txt", repo.path.c_str()), "patch file");

  // Switching clients saves changes automatically; go back to client A.
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  EXPECT_FALSE(fileExists(format("%s/seed-1.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/seed-2.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/seed-3.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/patch-1.txt", repo.path.c_str())));

  // Now make some of our own changes (saved).
  writeTextFile(format("%s/saved.txt", repo.path.c_str()), "saved changes");

  EXPECT_THAT(saveClient(repo, "c1").reader(), IsOk());

  // Now some unsaved changes.
  writeTextFile(format("%s/unsaved.txt", repo.path.c_str()), "unsaved changes");

  // Let's make sure that we can make files that conflict on master.
  writeTextFile(format("%s/seed-3.txt", repo.path.c_str()), "conflict with master");

  // Now patch changes from client B into client A.
  EXPECT_THAT(patchClient(repo, "c1", "c2", {}).reader(), EqualsMerge({}));

  // Now expect files from both clients and nothing from master.
  EXPECT_THAT(readTextFile(format("%s/saved.txt", repo.path.c_str())), "saved changes");
  EXPECT_THAT(readTextFile(format("%s/unsaved.txt", repo.path.c_str())), "unsaved changes");
  EXPECT_THAT(readTextFile(format("%s/patch-1.txt", repo.path.c_str())), "patch file");
  EXPECT_THAT(readTextFile(format("%s/seed-3.txt", repo.path.c_str())), "conflict with master");

  // Make sure that none of the intermeidary files showed up.
  EXPECT_FALSE(fileExists(format("%s/seed-1.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/seed-2.txt", repo.path.c_str())));
}

TEST_F(G8ApiTest, DetectChangeAfterRevert) {
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Seed an original file.
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "original");
  String csSeed = changesetId(createChangeset(repo, "seed", {"a.txt"}).reader());
  EXPECT_THAT(land(repo, "c1", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));

  // Make a change.
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "change");

  // Unsaved change appears dirty.
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::MODIFIED, "a.txt", true}}}}));

  // Save the client.
  EXPECT_THAT(saveClient(repo, "c1").reader(), IsOk());

  // Perform status to ensure that file exists and is not dirty (which means it is saved).
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::MODIFIED, "a.txt", false}}}}));

  // Revert the file to its original contents.
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "original");

  // Ensure that status shows this file and reports it as dirty.
  // Because the file is reverted and has original contents the status should show
  // no changes.
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::UNMODIFIED, "a.txt", true}}}}));
}

TEST_F(G8ApiTest, CanRevertFileMode) {
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  auto fileNameA = "a.txt";
  auto filePathA = format("%s/%s", repo.path.c_str(), fileNameA);
  auto fileNameB = "b.txt";
  auto filePathB = format("%s/%s", repo.path.c_str(), fileNameB);

  // Create an initial commit
  {
    writeTextFile(filePathA, "before");
    writeTextFile(filePathB, "before");
    setFileMode(filePathB, 0100755);

    String csSeed = changesetId(createChangeset(repo, "seed", {fileNameA, fileNameB}).reader());
    EXPECT_THAT(land(repo, "c1", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));

    EXPECT_THAT(statusWithRenames(repo).reader(), EqualsStatus({{"working", {}}}));

    EXPECT_EQ(getFileMode(filePathA), 0100644);
    EXPECT_EQ(getFileMode(filePathB), 0100755);
  }

  // Make some modifications
  {
    setFileMode(filePathA, 0100755);
    writeTextFile(filePathA, "after");

    setFileMode(filePathB, 0100644);
    writeTextFile(filePathB, "after");

    EXPECT_THAT(
      statusWithRenames(repo).reader(),
      EqualsStatus(
        {{"working",
          {{G8FileInfo::Status::MODIFIED, fileNameA, true},
           {G8FileInfo::Status::MODIFIED, fileNameB, true}}}}));

    EXPECT_EQ(getFileMode(filePathA), 0100755);
    EXPECT_EQ(getFileMode(filePathB), 0100644);
  }

  // Revert a.txt
  {
    revertFiles(repo, {fileNameA});

    EXPECT_THAT(
      statusWithRenames(repo).reader(),
      EqualsStatus({{"working", {{G8FileInfo::Status::MODIFIED, fileNameB, true}}}}));

    EXPECT_EQ(getFileMode(filePathA), 0100644);
    EXPECT_EQ(readTextFile(filePathA), "before");

    EXPECT_EQ(getFileMode(filePathB), 0100644);
    EXPECT_EQ(readTextFile(filePathB), "after");
  }

  // Revert b.txt
  {
    revertFiles(repo, {fileNameB});

    EXPECT_THAT(statusWithRenames(repo).reader(), EqualsStatus({{"working", {}}}));

    EXPECT_EQ(getFileMode(filePathA), 0100644);
    EXPECT_EQ(readTextFile(filePathA), "before");

    EXPECT_EQ(getFileMode(filePathB), 0100755);
    EXPECT_EQ(readTextFile(filePathB), "before");
  }
}

TEST_F(G8ApiTest, PatchWithConflictOnMaterLine) {
  // This test checks the case where there is no change for a file in the client but the diff
  // between start end end cannot be applied because there is  a conflicting change along master.
  // These files should be included in the merge flow otherwise we'll get an error indicating
  // that a tree cannot be created from a not fully merged index.

  String originalContent = "original";
  String patchContent = "diff";
  String conflictingContent = "conflict introduced";

  String testFilePath = format("%s/file.txt", repo.path.c_str());
  String bystanderPath = format("%s/bystander.txt", repo.path.c_str());

  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Seed original file.
  writeTextFile(testFilePath, originalContent);
  String csSeed = changesetId(createChangeset(repo, "seed", {"file.txt"}).reader());
  EXPECT_THAT(land(repo, "c1", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));

  // Now leave this client in a state where it would have conflicts with master.
  writeTextFile(testFilePath, patchContent);
  EXPECT_THAT(saveClient(repo, "c1").reader(), IsOk());

  // Now create the client that does the conflicts.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Make the conflicting change.
  writeTextFile(testFilePath, conflictingContent);
  String csConflict = changesetId(createChangeset(repo, "conflicting", {"file.txt"}).reader());
  EXPECT_THAT(land(repo, "c2", csConflict.c_str(), "land conflict").reader(), EqualsMerge({}));

  // Make an unrelated change just so we are further "removed" from the scene of the crime.
  writeTextFile(bystanderPath, "innocent changes");
  String csBystander = changesetId(createChangeset(repo, "innocent", {"bystander.txt"}).reader());
  EXPECT_THAT(land(repo, "c2", csBystander.c_str(), "land bystander").reader(), EqualsMerge({}));

  // Eliminate client for good meassure.
  EXPECT_THAT(deleteClient(repo, "c2").reader(), IsOk());

  // Create the test client.
  EXPECT_THAT(createClient(repo, "test").reader(), EqualsClients({"c1", "test"}));
  EXPECT_THAT(switchClient(repo, "test").reader(), EqualsClients({"c1", "test"}, "test"));

  // Now that we have a clean client with no changes.
  EXPECT_THAT(statusWithRenames(repo).reader(), EqualsStatus({{"working", {}}}));

  // Perform a patch operation.
  auto patchResult = patchClient(repo, "test", "c1", {});

  // First sign of trouble.
  EXPECT_THAT(patchResult.reader(), IsOk());

  // We need the conflicted file to appear.
  EXPECT_THAT(
    patchResult.reader(), EqualsMerge({{G8MergeAnalysisInfo::Status::CONFLICTED, "file.txt", ""}}));
}

TEST_F(G8ApiTest, SyncWithConflictsOnMaster) {
  String testFilePath = format("%s/test.txt", repo.path.c_str());
  String testFile2Path = format("%s/test2.txt", repo.path.c_str());
  String originalContent = "original content\nlots of lines\nnot a small thing\nrename maybe";

  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Seed the original file.
  writeTextFile(testFilePath, originalContent);
  String csSeed = changesetId(createChangeset(repo, "seed", {"test.txt"}).reader());
  EXPECT_THAT(land(repo, "c1", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));

  // Dummy commit in between to simulate more history
  {
    writeTextFile(format("%s/dummy.txt", repo.path.c_str()), "dummy text");
    String csDummy = changesetId(createChangeset(repo, "dummy", {"dummy.txt"}).reader());
    EXPECT_THAT(land(repo, "c1", csDummy.c_str(), "land dummy").reader(), EqualsMerge({}));
  }

  // Delete it and save the client.
  deleteFile(testFilePath.c_str());
  EXPECT_THAT(saveClient(repo, "c1").reader(), IsOk());
  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::DELETED, "test.txt", false}}}}));

  // Now create a new client.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Create another file with same content but different file name.
  writeTextFile(testFile2Path, originalContent);
  deleteFile(testFilePath.c_str());
  String csAddTest2 =
    changesetId(createChangeset(repo, "add test2", {"test.txt", "test2.txt"}).reader());
  EXPECT_THAT(land(repo, "c2", csAddTest2.c_str(), "land added").reader(), EqualsMerge({}));

  // Now master has file.txt and file2.txt, with the same exact content.
  EXPECT_FALSE(fileExists(testFilePath));
  EXPECT_TRUE(fileExists(testFile2Path));

  // Now head back to original client, land the delete and try to sync.
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  EXPECT_FALSE(fileExists(testFilePath));
  EXPECT_FALSE(fileExists(testFile2Path));

  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus({{"working", {{G8FileInfo::Status::DELETED, "test.txt", false}}}}));

  auto syncResult = syncClient(repo, "c1", "", {});
  EXPECT_THAT(syncResult.reader(), IsOk());
  EXPECT_THAT(syncResult.reader(), EqualsMerge({}));
  EXPECT_THAT(syncResult.reader(), EqualsClients({"c1"}, "c1"));

  EXPECT_FALSE(fileExists(testFilePath));
}

TEST_F(G8ApiTest, SyncUnchangeDuplicateRename) {
  // create initial client
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));

  // and switch to it
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // make initial file
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");

  // create changeset
  String cs1 = changesetId(createChangeset(repo, "initial version of a", {"a.txt"}).reader());

  // and land it
  EXPECT_THAT(land(repo, "c1", cs1.c_str(), "initial version of a").reader(), EqualsMerge({}));

  // create second client
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c1"));

  // duplicate the file
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");

  // create changeset
  String cs2 = changesetId(createChangeset(repo, "duplicate version of a", {"b.txt"}).reader());

  // and land it
  EXPECT_THAT(land(repo, "c1", cs2.c_str(), "duplicate a to be b").reader(), EqualsMerge({}));

  // now switch to c2
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  // Now delete a and write the same contents to b
  deleteFile(format("%s/a.txt", repo.path.c_str()).c_str());
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");

  {
    auto res = syncClient(repo, "c2", "", {});
    EXPECT_THAT(res.reader(), EqualsMerge({}));
  }
}

TEST_F(G8ApiTest, DeleteRenameSync) {
  // A rename occurs on master and we delete the file.

  String testFilePath = format("%s/test.txt", repo.path.c_str());
  String testFile2Path = format("%s/test2.txt", repo.path.c_str());
  String originalContent = "a text file\nnamed foo";
  String modifiedContent = "a test file\nnamed foo";

  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Seed the original file.
  writeTextFile(testFilePath, originalContent);
  String csSeed = changesetId(createChangeset(repo, "seed", {"test.txt"}).reader());
  EXPECT_THAT(land(repo, "c1", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));

  // Create another client which is based off this original state.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c1"));

  // Add the file.
  writeTextFile(testFile2Path, originalContent);
  deleteFile(testFilePath.c_str());
  String csRename =
    changesetId(createChangeset(repo, "add test2", {"test.txt", "test2.txt"}).reader());

  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{csRename, {{G8FileInfo::Status::RENAMED, "test2.txt", false}}}, {"working", {}}}));

  EXPECT_THAT(land(repo, "c1", csRename.c_str(), "land rename").reader(), EqualsMerge({}));

  // Go to client 2 and delete the file.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));
  deleteFile(testFilePath.c_str());

  String csLandDelete = changesetId(createChangeset(repo, "delete", {"test.txt"}).reader());

  EXPECT_THAT(
    statusWithRenames(repo).reader(),
    EqualsStatus(
      {{csLandDelete, {{G8FileInfo::Status::DELETED, "test.txt", false}}}, {"working", {}}}));

  // auto landId = simulateRemoteLand(repo, "c2", csLandDelete.c_str(), "land deleted");
  // EXPECT_THAT(finishLand(repo, "c2", csLandDelete.c_str()).reader(), IsOk());

  auto syncResult = syncClient(repo, "c2", "", {});
  EXPECT_THAT(syncResult.reader(), IsOk());
  EXPECT_THAT(syncResult.reader(), EqualsMerge({}));
}

// We can totally obliterate the local config file because each test case gets a fresh repo.
TEST_F(G8ApiTest, HouseKeeping) {
  const String configFilePath = format("%s/.git/config", repo.path.c_str());
  const auto originalConfigFileContents = readTextFile(configFilePath.c_str());

  // Clean up empty config subsections.
  writeTextFile(configFilePath.c_str(), g8testing::configFileA1);
  ASSERT_THAT(keepHouse(repo).reader(), IsOk());
  const auto cleanEmptyBranchSections = readTextFile(configFilePath.c_str());
  EXPECT_EQ(cleanEmptyBranchSections, g8testing::configFileA2);

  // TODO(pawel) Write test for setting missing remote and merge entries.
  // TODO(pawel) Write test for creating missing remote references.
  // TODO(pawel) Write test for cleaning up sections for branches that no longer exist.
}

// Check the inspect file logic cases.
// +-----------------+------+-----+------+-------+------------+--------+
// |      File       | Seed | You | They | Merge |   Status   | Source |
// +-----------------+------+-----+------+-------+------------+--------+
// | unmodified.txt  | A    | A   | A    | A     | unmodified | none   |
// | modify-you.txt  | A    | B   | A    | B     | modified   | you    |
// | modify-they.txt | A    | A   | B    | B     | modified   | they   |
// | modify-both.txt | A    | B   | B    | B     | modified   | both   |
// | add-you.txt     | -    | B   | -    | B     | added      | you    |
// | add-they.txt    | -    | -   | B    | B     | added      | they   |
// | add-both.txt    | -    | B   | B    | B     | added      | both   |
// | delete-you.txt  | A    | -   | A    | -     | deleted    | you    |
// | delete-they.txt | A    | A   | -    | -     | deleted    | they   |
// | delete-both.txt | A    | -   | -    | -     | deleted    | both   |
// +-----------------+------+-----+------+-------+------------+--------+
TEST_F(G8ApiTest, InspectFilesOnSync) {
  // NOTE(pawel) The hashes of this character contain no eol character since writeTextFile()
  // doesn't write it. To generate this in vim, first do a :set binary then :set noeol.
  // Hash of a file containing single character (no eol) "A".
  String hashA = "8c7e5a667f1b771847fe88c01c3de34413a1b220";
  // Hash of a file containing single character (no eol) "B".
  String hashB = "7371f47a6f8bd23a8fa1a8b2a9479cdd76380e54";

  EXPECT_THAT(createClient(repo, "seed").reader(), EqualsClients({"seed"}));
  EXPECT_THAT(switchClient(repo, "seed").reader(), EqualsClients({"seed"}, "seed"));

  auto write = [&](const char *file, const char *contents) {
    writeTextFile(format("%s/%s", repo.path.c_str(), file), contents);
  };

  auto remove = [&](const char *file) {
    deleteFile(format("%s/%s", repo.path.c_str(), file).c_str());
  };

  Vector<String> seedFiles = {
    "mergeable.asc",
    "unmodified.txt",
    "modify-you.txt",
    "modify-they.txt",
    "modify-both.txt",
    "delete-you.txt",
    "delete-they.txt",
    "delete-both.txt"};

  // Seed Fork
  for (const auto &file : seedFiles) {
    write(file.c_str(), "A");
  }

  // NOTE(pawel) Mergeable has its own contents. This is to test the auto-merge and regex features.
  write("mergeable.asc", "1\n2\n3\n4\n5\n6\n7\n8\n9\n");

  // Sanity check to ensure that a sync isn't performed when it is not needed.
  auto preSeedSyncRes = syncClient(repo, "seed", "", {}, ".*");
  EXPECT_THAT(preSeedSyncRes.reader(), IsOk());
  EXPECT_THAT(preSeedSyncRes.reader(), EqualsMerge({}));
  EXPECT_THAT(preSeedSyncRes.reader(), EqualsClients({"seed"}, "seed"));

  // Seed the files.
  String csSeed = changesetId(createChangeset(repo, "seed", seedFiles).reader());
  EXPECT_THAT(land(repo, "seed", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));

  EXPECT_THAT(deleteClient(repo, "seed").reader(), EqualsClients({}));

  EXPECT_THAT(createClient(repo, "you").reader(), EqualsClients({"you"}));
  EXPECT_THAT(createClient(repo, "they").reader(), EqualsClients({"they", "you"}));
  EXPECT_THAT(switchClient(repo, "they").reader(), EqualsClients({"they", "you"}, "they"));

  Vector<String> theyFiles = {
    "mergeable.asc",
    "modify-they.txt",
    "modify-both.txt",
    "add-they.txt",
    "add-both.txt",
    "delete-they.txt",
    "delete-both.txt"};

  for (const auto &file : theyFiles) {
    write(file.c_str(), "B");
  }

  remove("delete-they.txt");
  remove("delete-both.txt");

  // NOTE(pawel) Mergeable has its own contents. This is to test the auto-merge and regex features.
  write("mergeable.asc", "they\n2\n3\n4\n5\n6\n7\n8\n9\n");

  // Land these they changes.
  String csThey = changesetId(createChangeset(repo, "they", theyFiles).reader());
  EXPECT_THAT(land(repo, "they", csThey.c_str(), "land they").reader(), EqualsMerge({}));

  EXPECT_THAT(switchClient(repo, "you").reader(), EqualsClients({"they", "you"}, "you"));
  Vector<String> youFiles = {// Not including mergeable.asc
                             "modify-you.txt",
                             "modify-both.txt",
                             "add-you.txt",
                             "add-both.txt",
                             "delete-you.txt",
                             "delete-both.txt"};

  for (const auto &file : youFiles) {
    write(file.c_str(), "B");
  }

  remove("delete-you.txt");
  remove("delete-both.txt");

  using Source = G8MergeAnalysisInspect::ChangeSource;
  using Status = G8MergeAnalysisInspect::Status;
  String empty = "";

  // The real test.
  auto youSyncRes = syncClient(repo, "you", "", {}, ".*\\.txt");
  EXPECT_THAT(youSyncRes.reader(), IsOk());
  EXPECT_THAT(
    youSyncRes.reader(),
    EqualsMerge2(
      {{},
       {
         // NOTE(pawel) Ordering of hashes is previousBlobId -> nextBlobId.
         {"unmodified.txt", hashA, hashA, Source::NONE, Status::UNMODIFIED},
         {"modify-you.txt", hashA, hashB, Source::YOU, Status::MODIFIED},
         {"modify-they.txt", hashA, hashB, Source::THEY, Status::MODIFIED},
         {"modify-both.txt", hashA, hashB, Source::BOTH, Status::MODIFIED},
         {"add-you.txt", empty, hashB, Source::YOU, Status::ADDED},
         {"add-they.txt", empty, hashB, Source::THEY, Status::ADDED},
         {"add-both.txt", empty, hashB, Source::BOTH, Status::ADDED},
         {"delete-you.txt", hashA, empty, Source::YOU, Status::DELETED},
         {"delete-they.txt", hashA, empty, Source::THEY, Status::DELETED},
         {"delete-both.txt", hashA, empty, Source::BOTH, Status::DELETED},
       }}));

  // When inspectRegex is passed, sync should not auto complete.
  EXPECT_EQ(youSyncRes.reader().getMerge().getStatus(), G8MergeAnalysis::MergeStatus::NEEDS_ACTION);

  // Make a conflict and ensure that it does not appear in the inspect list.
  write("add-they.txt", "conflict");

  // NOTE(pawel) Mergeable has its own contents. This is to test the auto-merge and regex features.
  write("mergeable.asc", "1\n2\n3\n4\5\n6\n7\n8\n9\nyou");

  youSyncRes = syncClient(repo, "you", "", {}, ".*");
  EXPECT_THAT(youSyncRes.reader(), IsOk());
  EXPECT_THAT(
    youSyncRes.reader(),
    EqualsMerge2(
      {{{G8MergeAnalysisInfo::Status::CONFLICTED, "add-they.txt", ""},
        {G8MergeAnalysisInfo::Status::MERGEABLE, "mergeable.asc", ""}},
       {
         {"unmodified.txt", hashA, hashA, Source::NONE, Status::UNMODIFIED},
         {"modify-you.txt", hashA, hashB, Source::YOU, Status::MODIFIED},
         {"modify-they.txt", hashA, hashB, Source::THEY, Status::MODIFIED},
         {"modify-both.txt", hashA, hashB, Source::BOTH, Status::MODIFIED},
         // add-they.txt is missing from this list.
         {"add-you.txt", empty, hashB, Source::YOU, Status::ADDED},
         {"add-both.txt", empty, hashB, Source::BOTH, Status::ADDED},
         {"delete-you.txt", hashA, empty, Source::YOU, Status::DELETED},
         {"delete-they.txt", hashA, empty, Source::THEY, Status::DELETED},
         {"delete-both.txt", hashA, empty, Source::BOTH, Status::DELETED},
       }}));

  remove("add-they.txt");
  write("mergeable.asc", "1\n2\n3\n4\n5\n6\n7\n8\n9\n");  // Revert to seed value.

  // Now repeat the original request but mark inspectComplete.
  youSyncRes = syncClient(repo, "you", "", {}, ".*\\.txt", true);
  EXPECT_THAT(youSyncRes.reader(), IsOk());
  EXPECT_THAT(
    youSyncRes.reader(),
    EqualsMerge2(
      {{},
       {
         {"unmodified.txt", hashA, hashA, Source::NONE, Status::UNMODIFIED},
         {"modify-you.txt", hashA, hashB, Source::YOU, Status::MODIFIED},
         {"modify-they.txt", hashA, hashB, Source::THEY, Status::MODIFIED},
         {"modify-both.txt", hashA, hashB, Source::BOTH, Status::MODIFIED},
         {"add-you.txt", empty, hashB, Source::YOU, Status::ADDED},
         {"add-they.txt", empty, hashB, Source::THEY, Status::ADDED},
         {"add-both.txt", empty, hashB, Source::BOTH, Status::ADDED},
         {"delete-you.txt", hashA, empty, Source::YOU, Status::DELETED},
         {"delete-they.txt", hashA, empty, Source::THEY, Status::DELETED},
         {"delete-both.txt", hashA, empty, Source::BOTH, Status::DELETED},
       }}));

  // When inspectRegex is passed, sync should not auto complete.
  EXPECT_EQ(youSyncRes.reader().getMerge().getStatus(), G8MergeAnalysis::MergeStatus::COMPLETE);
}

TEST_F(G8ApiTest, AdditionalChangesOnSync) {
  String testFilePath = format("%s/test.txt", repo.path.c_str());
  String addFilePath = format("%s/add.txt", repo.path.c_str());
  String deleteFilePath = format("%s/delete.txt", repo.path.c_str());
  String modifyFilePath = format("%s/modify.txt", repo.path.c_str());

  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  // Seed the original file.
  writeTextFile(testFilePath, "original");
  writeTextFile(deleteFilePath, "original");
  writeTextFile(modifyFilePath, "original");
  String csSeed =
    changesetId(createChangeset(repo, "seed", {"test.txt", "delete.txt", "modify.txt"}).reader());
  EXPECT_THAT(land(repo, "c1", csSeed.c_str(), "land seed").reader(), EqualsMerge({}));

  // Create another client which is based off this original state.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c1"));

  writeTextFile(testFilePath, "change");
  String cs2 = changesetId(createChangeset(repo, "modify", {"test.txt"}).reader());
  EXPECT_THAT(land(repo, "c1", cs2.c_str(), "land modify").reader(), EqualsMerge({}));

  // Now create conflict to ensure merge flow.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));
  writeTextFile(testFilePath, "conflict");
  auto syncResult = syncClient(repo, "c2", "", {}, "", false, {});
  EXPECT_THAT(
    syncResult.reader(), EqualsMerge({{G8MergeAnalysisInfo::Status::CONFLICTED, "test.txt", ""}}));

  Vector<MergeDecision> decisions(1);
  auto &decision = decisions[0];
  decision.path = "test.txt";
  decision.choice = G8MergeDecision::Choice::THEIR_CHANGE;
  decision.fileId = syncResult.reader().getMerge().getInfo()[0].getFileId();

  Vector<AdditionalChange> additionalChanges(3);
  additionalChanges[0].path = "add.txt";
  additionalChanges[0].blobId = createBlob(repo, "post-sync added").reader().getId().cStr();
  additionalChanges[1].path = "modify.txt";
  additionalChanges[1].blobId = createBlob(repo, "post-sync modified").reader().getId().cStr();
  additionalChanges[2].path = "delete.txt";
  additionalChanges[2].blobId = "";

  EXPECT_EQ(readTextFile(modifyFilePath), "original");
  EXPECT_FALSE(fileExists(addFilePath));
  EXPECT_TRUE(fileExists(deleteFilePath));

  EXPECT_THAT(
    syncClient(repo, "c2", "", decisions, "", true, additionalChanges).reader(), SyncComplete());

  EXPECT_EQ(readTextFile(modifyFilePath), "post-sync modified");
  EXPECT_EQ(readTextFile(addFilePath), "post-sync added");
  EXPECT_FALSE(fileExists(deleteFilePath));
}

TEST_F(G8ApiTest, AddFileAfterSyncBeforeUpdate) {
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));
  writeTextFile(format("%s/a.txt", repo.path.c_str()), "a");
  String cs1 = changesetId(createChangeset(repo, "a", {"a.txt"}).reader());
  EXPECT_THAT(land(repo, "c1", cs1.c_str(), "Land a").reader(), EqualsMerge({}));

  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));
  writeTextFile(format("%s/b.txt", repo.path.c_str()), "b");
  String cs2 = changesetId(createChangeset(repo, "b", {"b.txt"}).reader());
  EXPECT_THAT(syncClient(repo, "c2", "", {}).reader(), EqualsMerge({}));

  writeTextFile(format("%s/c.txt", repo.path.c_str()), "c");
  EXPECT_THAT(setFiles(repo, "c2", cs2.c_str(), {"c.txt"}).reader(), IsOk());
}

// Ensure that the index tree matches the commit tree when switching clients in
// a sparse-checkout repo.
// NOTE: Currently this does not work properly in all cases, and there are some
// EXPECT_THAT statements commented out in this test.
TEST_F(G8ApiTest, SparseCheckoutIndexTreeConsistency) {
  // Create a first client and switch to it.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  const char *reporoot = repo.path.c_str();

  const auto repopath = [reporoot](const char *file) -> String {
    return format("%s/%s", reporoot, file);
  };

  // Create a subdirectory to put the files in.
  ASSERT_TRUE(fs::create_directory(repopath("subdir")));

  // Create a separate subdirectory that is not checked out in the sparse checkout.
  ASSERT_TRUE(fs::create_directory(repopath("hidden")));

  // Add some top-level files, with 1.txt, subdir/b.txt, and hidden/bb.txt executable.
  writeTextFile(repopath("0.txt"), "a\nb\nc\nd\ne\nf\ng\nh\ni\nj\n");
  writeTextFile(repopath("1.txt"), "1");
  setFileMode(repopath("1.txt"), 0100755);

  // Add two files to subdir and hidden dir.
  writeTextFile(repopath("subdir/a.txt"), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");
  writeTextFile(repopath("subdir/b.txt"), "b");
  setFileMode(repopath("subdir/b.txt"), 0100755);
  writeTextFile(repopath("hidden/aa.txt"), "aa");
  writeTextFile(repopath("hidden/bb.txt"), "bb");
  setFileMode(repopath("hidden/bb.txt"), 0100755);
  String cs1 = changesetId(
    createChangeset(
      repo,
      "ab",
      {"0.txt", "1.txt", "subdir/a.txt", "subdir/b.txt", "hidden/aa.txt", "hidden/bb.txt"})
      .reader());
  EXPECT_THAT(land(repo, "c1", cs1.c_str(), "Land ab").reader(), EqualsMerge({}));

  auto indexTree = [](const char *repoPath) -> String {
    auto [input, output] =
      process::execute2(process::Execute2Options{"git", repoPath}, "write-tree", "--missing-ok");
    return strTrim(process::getOutput(output));
  };

  auto branchTree = [](const char *repoPath, const char *branch) -> String {
    auto [input, output] = process::execute2(
      process::Execute2Options{"git", repoPath}, "rev-parse", format("%s^{tree}", branch));
    return strTrim(process::getOutput(output));
  };

  EXPECT_THAT(indexTree(reporoot), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));
  EXPECT_THAT(branchTree(reporoot, "c1"), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));

  // Create a second client but stay in client 1.
  EXPECT_THAT(createClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}));

  // Now move 0.txt and a.txt in client 1 to the hidden directory, modify
  // subdir/b.txt and hidden/aa.txt and add hidden/cc.txt.
  deleteFile(repopath("subdir/a.txt").c_str());
  writeTextFile(repopath("hidden/a.txt"), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");
  deleteFile(repopath("0.txt").c_str());
  writeTextFile(repopath("hidden/0.txt"), "a\nb\nc\nd\ne\nf\ng\nh\ni\nj\n");
  writeTextFile(repopath("subdir/b.txt"), "updated b");
  writeTextFile(repopath("hidden/aa.txt"), "updated aa");
  writeTextFile(repopath("hidden/cc.txt"), "cc");
  setFileMode(repopath("hidden/cc.txt"), 0100755);
  String cs2 = changesetId(createChangeset(
                             repo,
                             "a-move",
                             {"0.txt",
                              "subdir/b.txt",
                              "hidden/0.txt",
                              "subdir/a.txt",
                              "hidden/a.txt",
                              "hidden/aa.txt",
                              "hidden/cc.txt"})
                             .reader());
  EXPECT_THAT(land(repo, "c1", cs2.c_str(), "Land a-move").reader(), EqualsMerge({}));

  EXPECT_THAT(indexTree(reporoot), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c1"), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c2"), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));

  // Initialize sparse checkout.
  {
    process::ExecuteResult result =
      process::execute({"git", reporoot}, {"sparse-checkout", "init", "--no-cone"});
    ASSERT_EQ(result.process.getExitCode(), 0);
  }

  // Write a sparse checkout file that includes top-level files and the subdir
  // files with no recursion.
  writeTextFile(repopath(".git/info/sparse-checkout"), "/*\n!/*/\n/subdir/\n!/subdir/*/");

  // Reapply sparse checkout.
  {
    process::ExecuteResult result =
      process::execute({"git", reporoot}, {"sparse-checkout", "reapply", "--no-cone"});
    ASSERT_EQ(result.process.getExitCode(), 0);
  }

  auto isSkipWorktree = [reporoot](const char *filename) -> bool {
    auto [input, output] = process::execute2(
      process::Execute2Options{"git", reporoot}, "ls-files", "-s", "-v", "--", filename);
    String outputStr = process::getOutput(output);
    return outputStr.substr(0, 8) == "S 100644";
  };

  auto isSkipWorktreeExe = [reporoot](const char *filename) -> bool {
    auto [input, output] = process::execute2(
      process::Execute2Options{"git", reporoot}, "ls-files", "-s", "-v", "--", filename);
    String outputStr = process::getOutput(output);
    return outputStr.substr(0, 8) == "S 100755";
  };

  auto repoFileExists = [&repopath](const char *filename) -> bool {
    return fileExists(repopath(filename));
  };

  // Confirm that the index and commit trees are unaffected by the sparse checkout.
  EXPECT_THAT(indexTree(reporoot), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c1"), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c2"), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));

  // Confirm that the index and commit trees are unaffected by a call to statusWithRenames.
  EXPECT_THAT(statusWithRenames(repo).reader(), EqualsStatus({{"working", {}}}));

  EXPECT_FALSE(repoFileExists("hidden/0.txt"));
  EXPECT_FALSE(repoFileExists("hidden/a.txt"));
  EXPECT_FALSE(repoFileExists("hidden/aa.txt"));
  EXPECT_FALSE(repoFileExists("hidden/bb.txt"));
  EXPECT_FALSE(repoFileExists("hidden/cc.txt"));

  EXPECT_TRUE(isSkipWorktree("hidden/0.txt"));
  EXPECT_TRUE(isSkipWorktree("hidden/a.txt"));
  EXPECT_TRUE(isSkipWorktree("hidden/aa.txt"));
  EXPECT_TRUE(isSkipWorktreeExe("hidden/bb.txt"));
  EXPECT_TRUE(isSkipWorktreeExe("hidden/cc.txt"));

  EXPECT_THAT(indexTree(reporoot), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c1"), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c2"), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));

  // Now switch to client 2 and ensure the index matches the branch.
  EXPECT_THAT(switchClient(repo, "c2").reader(), EqualsClients({"c1", "c2"}, "c2"));

  EXPECT_THAT(indexTree(reporoot), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));
  EXPECT_THAT(branchTree(reporoot, "c1"), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c2"), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));

  EXPECT_FALSE(repoFileExists("hidden/0.txt"));
  EXPECT_FALSE(repoFileExists("hidden/a.txt"));
  EXPECT_FALSE(repoFileExists("hidden/aa.txt"));
  EXPECT_FALSE(repoFileExists("hidden/bb.txt"));
  EXPECT_FALSE(repoFileExists("hidden/cc.txt"));

  EXPECT_TRUE(isSkipWorktree("hidden/aa.txt"));
  EXPECT_TRUE(isSkipWorktreeExe("hidden/bb.txt"));

  // Confirm that the index and commit trees are unaffected by a call to statusWithRenames.
  EXPECT_THAT(statusWithRenames(repo).reader(), EqualsStatus({{"working", {}}}));

  EXPECT_THAT(indexTree(reporoot), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));
  EXPECT_THAT(branchTree(reporoot, "c1"), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c2"), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));

  // Now switch to client 1 and ensure the index matches the branch.
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1", "c2"}, "c1"));

  EXPECT_THAT(indexTree(reporoot), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c1"), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c2"), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));

  EXPECT_FALSE(repoFileExists("hidden/0.txt"));
  EXPECT_FALSE(repoFileExists("hidden/a.txt"));
  EXPECT_FALSE(repoFileExists("hidden/aa.txt"));
  EXPECT_FALSE(repoFileExists("hidden/bb.txt"));
  EXPECT_FALSE(repoFileExists("hidden/cc.txt"));

  EXPECT_TRUE(isSkipWorktree("hidden/0.txt"));
  EXPECT_TRUE(isSkipWorktree("hidden/a.txt"));
  EXPECT_TRUE(isSkipWorktree("hidden/aa.txt"));
  EXPECT_TRUE(isSkipWorktreeExe("hidden/bb.txt"));
  EXPECT_TRUE(isSkipWorktreeExe("hidden/cc.txt"));

  // Confirm that the index and commit trees are unaffected by a call to statusWithRenames.
  EXPECT_THAT(statusWithRenames(repo).reader(), EqualsStatus({{"working", {}}}));
  EXPECT_THAT(indexTree(reporoot), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c1"), StrEq("932c180ada3ac75425929a4039a7c188a42f0fb7"));
  EXPECT_THAT(branchTree(reporoot, "c2"), StrEq("edc4ff2fac428a04d94de086da8a4e1831862a35"));
}

// Test the output of the G8FileRequest::LIST action.
TEST_F(G8ApiTest, LsFilesTest) {
  // Create a first client and switch to it.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  const char *reporoot = repo.path.c_str();

  const auto repopath = [reporoot](const char *file) -> String {
    return format("%s/%s", reporoot, file);
  };

  // Create a subdirectory to put the files in.
  ASSERT_TRUE(fs::create_directory(repopath("subdir")));

  // Create a separate subdirectory that is not checked out in the sparse checkout.
  ASSERT_TRUE(fs::create_directory(repopath("hidden")));

  // Add some top-level files, with 1.txt, subdir/b.txt, and hidden/bb.txt executable.
  writeTextFile(repopath("0.txt"), "a\nb\nc\nd\ne\nf\ng\nh\ni\nj\n");
  writeTextFile(repopath("1.txt"), "1");
  setFileMode(repopath("1.txt"), 0100755);

  // Add two files to subdir and hidden dir.
  writeTextFile(repopath("subdir/a.txt"), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9");
  writeTextFile(repopath("subdir/b.txt"), "b");
  setFileMode(repopath("subdir/b.txt"), 0100755);
  writeTextFile(repopath("hidden/aa.txt"), "aa");
  writeTextFile(repopath("hidden/bb.txt"), "bb");
  setFileMode(repopath("hidden/bb.txt"), 0100755);
  String cs1 = changesetId(
    createChangeset(
      repo,
      "ab",
      {"0.txt", "1.txt", "subdir/a.txt", "subdir/b.txt", "hidden/aa.txt", "hidden/bb.txt"})
      .reader());
  EXPECT_THAT(land(repo, "c1", cs1.c_str(), "Land ab").reader(), EqualsMerge({}));

  auto dotResult = lsFiles(repo, {"."});
  auto subdirResult = lsFiles(repo, {"subdir"});
  auto hiddenResult = lsFiles(repo, {"hidden"});

  EXPECT_THAT(dotResult.reader(), EqualsFiles({{"hidden", "subdir", "0.txt", "1.txt"}}));
  EXPECT_THAT(subdirResult.reader(), EqualsFiles({{"subdir/b.txt", "subdir/a.txt"}}));
  EXPECT_THAT(hiddenResult.reader(), EqualsFiles({{"hidden/aa.txt", "hidden/bb.txt"}}));

  // Initialize sparse checkout.
  {
    process::ExecuteResult result =
      process::execute({"git", reporoot}, {"sparse-checkout", "init"});
    ASSERT_EQ(result.process.getExitCode(), 0);
  }

  // Write a sparse checkout file that includes top-level files and the subdir
  // files with no recursion.
  writeTextFile(repopath(".git/info/sparse-checkout"), "/*\n!/*/\n/subdir/\n!/subdir/*/");

  // Reapply sparse checkout.
  {
    process::ExecuteResult result =
      process::execute({"git", reporoot}, {"sparse-checkout", "reapply"});
    ASSERT_EQ(result.process.getExitCode(), 0);
  }

  // These should be found from the working directory.
  EXPECT_THAT(dotResult.reader(), EqualsFiles({{"hidden", "subdir", "0.txt", "1.txt"}}));
  EXPECT_THAT(subdirResult.reader(), EqualsFiles({{"subdir/b.txt", "subdir/a.txt"}}));

  // These should be missing from the workdir but found in the index.
  EXPECT_FALSE(fileExists(format("%s/hidden/aa.txt", repo.path.c_str())));
  EXPECT_FALSE(fileExists(format("%s/hidden/bb.txt", repo.path.c_str())));
  EXPECT_THAT(hiddenResult.reader(), EqualsFiles({{"hidden/aa.txt", "hidden/bb.txt"}}));
}

// Test the output of the G8::LIST action with the includeRemote flag enabled.
TEST_F(G8ApiTest, ListRemoteClientsTest) {
  // Create a local client and switch to it.
  EXPECT_THAT(createClient(repo, "c1").reader(), EqualsClients({"c1"}));
  EXPECT_THAT(switchClient(repo, "c1").reader(), EqualsClients({"c1"}, "c1"));

  EXPECT_THAT(
    listRemoteClients(repo).reader(), EqualsRemoteClients({"c1"}, {"origin/jenkins-c1"}, "c1"));

  // List clients should not return remote clients.
  EXPECT_THAT(listClients(repo).reader(), EqualsRemoteClients({"c1"}, {}, "c1"));

  createRemoteClient(repo, "remote1");

  // List clients should not return remote clients.
  EXPECT_THAT(listClients(repo).reader(), EqualsRemoteClients({"c1"}, {}, "c1"));

  EXPECT_THAT(
    listRemoteClients(repo).reader(),
    EqualsRemoteClients({"c1"}, {"origin/jenkins-c1", "origin/johnny-remote1"}, "c1"));
}

}  // namespace c8
