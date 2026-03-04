// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "g8-testing.h",
  };
  visibility = {
    "//visibility:public",
  };
  testonly = 1;
  deps = {
    ":g8-api.capnp-cc",
    ":g8-git",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:process",
    "//c8:string",
    "//c8:map",
    "//c8:vector",
    "//c8/io:capnp-messages",
    "//c8/string:format",
    "//c8/string:join",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x39a2b0f5);

#include <capnp/pretty-print.h>

#include <filesystem>
#include <thread>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/git/g8-git.h"
#include "c8/git/g8-testing.h"
#include "c8/map.h"
#include "c8/process.h"
#include "c8/scope-exit.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/string/join.h"

using namespace std::chrono_literals;

namespace c8 {

namespace {

// Set to true for verbose printing from git daemon, useful for debugging connection issues.
const bool VERBOSE_GIT_DAEMON = false;

// Get the bazel test temp path.
static String tmpPath() {
  auto bazelTmp = std::getenv("TEST_TMPDIR");
  if (bazelTmp != nullptr) {
    return bazelTmp;
  }
  return std::filesystem::temp_directory_path();
}

// Wrapper to capture a process and its pipes. When a process is run, the pipes are created
// and returned by the caller who owns them. We need to maintain both a handle to the process
// (to kill it or inspect its exit code), and to its pipes (to read their output). Note that
// PipeMap is an unmoveable type, since std::map represents its keys as std::pair<const K, V>,
// and const K is always unmoveable even if K and V are both moveable. This means ProcessWithPipes
// can be constructed, but not copied or moved.
struct ProcessWithPipes {
  process::PipeMap pipes;  // pipes comes before process so we can move process after pipes.
  process::Process process;
};

// Start a subprocess of the supplied program with the supplied arguments. If workDir is specified,
// run it in the specified workDir. If capturedDescriptors are set, the specified descriptors
// will be captured in pipes that are returned in a PipeMap along with the Process.
ProcessWithPipes run(
  const String &progName,
  const Vector<String> &args,
  const char *workDir,
  const TreeSet<int> &capturedDescriptors = {}) {
  Vector<String> argsWithName;
  argsWithName.push_back(progName);
  argsWithName.insert(argsWithName.end(), args.begin(), args.end());
  process::Process p{progName, argsWithName, capturedDescriptors};
  if (workDir != nullptr) {
    p.setWorkDirectory(workDir);
  }
  return {p(), std::move(p)};
}

// Execute the named pogram with the supplied arguments and block until its completion. Return the
// contents of std::out, and don't raise any excpetions on failure.
String execGetOutputErrorsOk(
  const String &progName, const Vector<String> &args, const char *workDir) {
  auto p = run(progName, args, workDir, {process::C8_STDOUT});
  p.process.getExitCode();  // block until completion but ignore output status.
  return getOutput(p.pipes[process::C8_STDOUT]);
}

// Execute the named program with the supplied arguments and block until its completion. Return the
// contents of std::out, expect zero exit status
String execGetOutput(const String &progName, const Vector<String> &args, const char *workDir) {
  auto p = run(progName, args, workDir, {process::C8_STDOUT});
  EXPECT_EQ(0, p.process.getExitCode()) << progName << " " << strJoin(args, " ");
  return getOutput(p.pipes[process::C8_STDOUT]);
}

// Execute the named program with the supplied arguments and block until its completion.
void exec(const String &progName, const Vector<String> &args, const char *workDir) {
  auto p = run(progName, args, workDir);
  EXPECT_EQ(0, p.process.getExitCode()) << progName << " " << strJoin(args, " ");
}

// Execute a process in the background, and return a handle to kill it when scope is lost.
ScopeExit execAsync(const String &progName, const Vector<String> &args, const char *workDir) {
  auto p = run(progName, args, workDir);
  // Move the process to the exit function so we can kill it later. The pipes are not moved because
  // std::maps are unmovable, because their entries are represented as std::pair<const Key, V>, and
  // even if V is movable, const K prevents the key from being moved.
  return ScopeExit(
    PackagedTask([p{std::move(p.process)}, progName{progName}, args{args}]() mutable {
      p.kill();
      EXPECT_EQ(0, p.getExitCode()) << progName << " " << strJoin(args, " ");
    }));
}

// Gets the first token (stripping away rest of output and trailing newlines)
String getFirstToken(const String &str) {
  String token;
  std::istringstream iss(str);
  iss >> token;
  return token;
}

// Given a number of lines, the property is the first token on the line; the value is the rest
// of the line
String getProperty(const String &str, const String &targetProp) {
  std::istringstream iss(str);
  String line;
  String currentProp;
  while (true) {
    std::getline(iss, line);
    if (line.empty()) {
      break;
    }

    std::istringstream lineStream(line);
    lineStream >> currentProp;
    if (currentProp == targetProp) {
      lineStream.ignore();             // move past space
      std::getline(lineStream, line);  // get the rest of the line
      return line;
    }
  }

  return "";
}

}  // namespace

String getHashForString(String &data) {
  auto [stdin, stdout] = process::execute2("git", "hash-object", "--stdin");
  stdin << data;
  stdin.close();
  Vector<uint8_t> res;
  stdout >> res;
  return getFirstToken(String(res.begin(), res.end()));
}

String getHashForClient(const LocalOriginRepo &repo, String client) {
  return getFirstToken(execGetOutput("git", {"rev-parse", client.c_str()}, repo.path.c_str()));
}

// Create a new empty repository for unit tests. This repo is hosted locally and is backed
// by a subprocess git daemon that serves the origin. The repo lasts as long as the returned
// object stays in scope, and then the subprocess is killed and resources are reclaimed.
LocalOriginRepo testRepo() {
  auto tmp = tmpPath();
  auto remoteDir = format("%s/remote", tmp.c_str());
  auto remoteGitDir = format("%s/repo.git", remoteDir.c_str());
  auto localDir = format("%s/local", tmp.c_str());
  auto localGitDir = format("%s/repo", localDir.c_str());

  // Prepare the remote directory for serving by git daemon.
  exec("rm", {"-rf", remoteDir}, nullptr);
  exec("mkdir", {remoteDir}, nullptr);
  exec("mkdir", {remoteGitDir}, nullptr);
  exec("git", {"init", "--bare", "--quiet"}, remoteGitDir.c_str());
  exec("touch", {"git-daemon-export-ok"}, remoteGitDir.c_str());

  // Start a self-killing subprocess to run the git daemon and capture it (along with other
  // associated cleanup) in the repo object that will be returned.
  LocalOriginRepo repo{// path
                       localGitDir,
                       // closeDaemon
                       execAsync(
                         "git",
                         {
                           "daemon",
                           "--reuseaddr",
                           format("--base-path=%s/", remoteDir.c_str()),
                           "--enable=receive-pack",
                           VERBOSE_GIT_DAEMON ? "--verbose" : "",
                           format("%s/", remoteDir.c_str()),
                         },
                         nullptr),
                       // cleanDirectories
                       {[remoteDir{remoteDir}, localDir{localDir}]() {
                         exec("rm", {"-rf", remoteDir}, nullptr);
                         exec("rm", {"-rf", localDir}, nullptr);
                       }}};

  // Prepare the local directory.
  exec("mkdir", {localDir}, nullptr);
  exec("mkdir", {repo.path}, nullptr);
  auto p = run("git", {"init", "--quiet", "--initial-branch", "master"}, repo.path.c_str());
  if (p.process.getExitCode() != 0) {
    // NOTE(christoph): For versions of git that don't support --initial-branch, retry without it.
    //   We use GET_INFO to double-check that master is detected as the main branch below.
    exec("git", {"init", "--quiet"}, repo.path.c_str());
  }

  int tries = 0;
  while (tries++ < 10) {
    auto p = run("git", {"ls-remote", "git://localhost/repo.git"}, nullptr);
    if (p.process.getExitCode() == 0) {
      break;
    }
    std::this_thread::sleep_for(200ms);
  }

  exec("git", {"config", "--local", "user.name", "Leeroy", "Jenkins"}, repo.path.c_str());
  exec("git", {"config", "--local", "user.email", "jenkins@8thwall.com"}, repo.path.c_str());
  exec("git", {"remote", "add", "origin", "git://localhost/repo.git"}, repo.path.c_str());

  // Push an initial commit to seed repository refs.
  // G8 initial commit, equivalent to:
  //  * exec("git", {"commit", "--allow-empty", "-m", "Initial commit."}, repo.path.c_str());
  //  * exec("git", {"push", "origin", "master"}, repo.path.c_str());
  MutableRootMessage<G8RepositoryRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setMessage("Initial commit");
  req.builder().setAction(G8RepositoryRequest::Action::INITIAL_COMMIT);
  auto res = g8Repository(req.reader());
  EXPECT_THAT(res.reader(), IsOk());

  MutableRootMessage<G8RepositoryRequest> infoReq;
  infoReq.builder().setAction(G8RepositoryRequest::Action::GET_INFO);
  infoReq.builder().setRepo(repo.path);
  auto infoRes = g8Repository(infoReq.reader());
  EXPECT_THAT(infoRes.reader(), IsOk());
  EXPECT_EQ(infoRes.reader().getMainBranchName(), "master");

  return repo;
}

// Checks whether the LocalOriginRepo is a valid and initialized test repo.
bool isInitializedTestRepo(const LocalOriginRepo &repo) {
  // Check that the path string ends in /local/repo, which is ths the suffix used when creating
  // the repo in test repo.
  if (repo.path.rfind("/local/repo") != (repo.path.size() - 11)) {
    return false;
  }
  // Check that the path contains a ".git" object. This stat command will print out ".git\n" if
  // .git is present or nothing otherwise.
  if (execGetOutputErrorsOk("stat", {"-f", "%N", "-q", ".git"}, repo.path.c_str()) != ".git\n") {
    return false;
  }

  return true;
}

// Empty the contents of the local repo of all git state.
void clearRepo(const LocalOriginRepo &repo) {
  bool isValid = isInitializedTestRepo(repo);
  EXPECT_TRUE(isValid);
  if (!isValid) {
    return;
  }
  exec("rm", {"-rf", repo.path}, nullptr);
  exec("mkdir", {repo.path}, nullptr);
}

void deleteFile(const char *path) { exec("rm", {path}, nullptr); }

mode_t getFileMode(const String &path) {
  struct stat fileStat;
  if (stat(path.c_str(), &fileStat) < 0)
    return -1;
  return fileStat.st_mode;
}

int setFileMode(const String &path, mode_t mode) { return chmod(path.c_str(), mode); }

// Checks if client or client's parent is the merge base.
bool branchIsValidClient(const LocalOriginRepo &repo, const char *client) {
  // Get fork point commit
  String forkId =
    getFirstToken(execGetOutput("git", {"merge-base", client, "master"}, repo.path.c_str()));

  // Get client commit
  String clientId = getFirstToken(execGetOutput("git", {"rev-parse", client}, repo.path.c_str()));

  // Get client parent commit
  String clientParentId =
    getFirstToken(execGetOutput("git", {"rev-parse", format("%s^", client)}, repo.path.c_str()));

  return forkId == clientId || forkId == clientParentId;
}

// Takes the changeset tree and make a commit with it on top of master
String simulateRemoteLand(
  const LocalOriginRepo &repo, const char *client, const char *changeset, const char *msg) {
  // get commit id of tip of master
  String masterId = getFirstToken(execGetOutput("git", {"rev-parse", "master"}, repo.path.c_str()));

  // get id of the latest commit on the changeset branch
  String commitId = getFirstToken(
    execGetOutput("git", {"rev-parse", format("%s-CS%s", client, changeset)}, repo.path.c_str()));

  // get tree id
  String treeId = getProperty(
    execGetOutput("git", {"cat-file", "-p", commitId.c_str()}, repo.path.c_str()), "tree");

  // commit the tree on top of master
  String landedId = getFirstToken(execGetOutput(
    "git", {"commit-tree", treeId.c_str(), "-p", masterId.c_str(), "-m", msg}, repo.path.c_str()));

  exec("git", {"branch", "-f", "master", landedId.c_str()}, repo.path.c_str());

  exec("git", {"push", "origin", "master"}, repo.path.c_str());

  return landedId;
}

// Empty the contents of the local repo of all git state.
ConstRootMessage<G8RepositoryResponse> cloneRepo(const LocalOriginRepo &repo) {
  MutableRootMessage<G8RepositoryRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setUrl("git://localhost/repo.git");
  req.builder().setUser("jenkins");
  req.builder().setAction(G8RepositoryRequest::Action::CLONE);
  auto res = g8Repository(req.reader());

  exec("git", {"config", "--local", "user.name", "Leeroy", "Jenkins"}, repo.path.c_str());
  exec("git", {"config", "--local", "user.email", "jenkins@8thwall.com"}, repo.path.c_str());
  return res;
}

// Create a new client with the specified name in the repo.
ConstRootMessage<G8ClientResponse> createClient(const LocalOriginRepo &repo, const char *name) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().initClient(1);
  req.builder().getClient().set(0, name);
  req.builder().setAction(G8ClientRequest::Action::CREATE);
  req.builder().setUser("jenkins");

  return g8Client(req.reader());
}

// Create a new client with the specified name in the repo.
void createRemoteClient(const LocalOriginRepo &repo, const char *name) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().initClient(1);
  req.builder().getClient().set(0, name);
  req.builder().setAction(G8ClientRequest::Action::CREATE);
  req.builder().setUser("johnny");
  g8Client(req.reader());

  // Delete the local branch
  exec("git", {"branch", "-D", name}, repo.path.c_str());
}

// Delete client specified.
ConstRootMessage<G8ClientResponse> deleteClient(const LocalOriginRepo &repo, const char *name) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().initClient(1);
  req.builder().getClient().set(0, name);
  req.builder().setAction(G8ClientRequest::Action::DELETE);

  return g8Client(req.reader());
}

ConstRootMessage<G8FileResponse> revertFiles(
  const LocalOriginRepo &repo, const Vector<String> &files) {
  MutableRootMessage<G8FileRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setClient("HEAD");
  req.builder().setAction(G8FileRequest::Action::REVERT);
  req.builder().initPaths(files.size());
  for (int i = 0; i < files.size(); i++) {
    req.builder().getPaths().set(i, files[i]);
  }
  return g8FileRequest(req.reader());
}

ConstRootMessage<G8FileResponse> lsFiles(const LocalOriginRepo &repo, const Vector<String> &paths) {
  MutableRootMessage<G8FileRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setClient("HEAD");
  req.builder().setAction(G8FileRequest::Action::LIST);
  req.builder().initPaths(paths.size());
  for (int i = 0; i < paths.size(); i++) {
    req.builder().getPaths().set(i, paths[i]);
  }
  return g8FileRequest(req.reader());
}

// List clients in repo.
ConstRootMessage<G8ClientResponse> listClients(const LocalOriginRepo &repo) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setAction(G8ClientRequest::Action::LIST);

  return g8Client(req.reader());
}

// List local and remote clients in repo.
ConstRootMessage<G8ClientResponse> listRemoteClients(const LocalOriginRepo &repo) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setIncludeRemote(true);
  req.builder().setUser("jenkins");
  req.builder().setAction(G8ClientRequest::Action::LIST);

  return g8Client(req.reader());
}

// Switch to the client with the specified name in the repo.
ConstRootMessage<G8ClientResponse> switchClient(const LocalOriginRepo &repo, const char *name) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().initClient(1);
  req.builder().getClient().set(0, name);
  req.builder().setAction(G8ClientRequest::Action::SWITCH);
  req.builder().setUser("jenkins");

  return g8Client(req.reader());
}

// Save the client with the specified name to origin.
ConstRootMessage<G8ClientResponse> saveClient(const LocalOriginRepo &repo, const char *name) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().initClient(1);
  req.builder().getClient().set(0, name);
  req.builder().setAction(G8ClientRequest::Action::SAVE);
  req.builder().setUser("jenkins");

  return g8Client(req.reader());
}

// Get the status of files in the client.
ConstRootMessage<G8ChangesetResponse> statusWithoutRenames(const LocalOriginRepo &repo) {
  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setClient("HEAD");
  req.builder().setIncludeSummary(true);
  req.builder().setIncludeDescription(true);
  req.builder().setIncludeBody(true);
  req.builder().setIncludeFileInfo(true);
  req.builder().setIncludeWorkingChanges(true);
  req.builder().setAction(G8ChangesetRequest::Action::LIST);
  return g8Changeset(req.reader());
}

// Get status of files in the client with renames
ConstRootMessage<G8ChangesetResponse> statusWithRenames(const LocalOriginRepo &repo) {
  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setClient("HEAD");
  req.builder().setIncludeSummary(true);
  req.builder().setIncludeDescription(true);
  req.builder().setIncludeBody(true);
  req.builder().setIncludeFileInfo(true);
  req.builder().setIncludeWorkingChanges(true);
  req.builder().setFindRenamesAndCopies(true);
  req.builder().setAction(G8ChangesetRequest::Action::LIST);
  return g8Changeset(req.reader());
}

ConstRootMessage<G8ChangesetResponse> statusForClient(
  const LocalOriginRepo &repo, const char *client) {
  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setClient(client);
  req.builder().setIncludeSummary(true);
  req.builder().setIncludeDescription(true);
  req.builder().setIncludeBody(true);
  req.builder().setIncludeFileInfo(true);
  req.builder().setIncludeWorkingChanges(true);
  req.builder().setFindRenamesAndCopies(true);
  req.builder().setAction(G8ChangesetRequest::Action::LIST);
  return g8Changeset(req.reader());
}

// Create a changeset with given files; this EXPECTS_THAT the changeset was validly created
ConstRootMessage<G8ChangesetResponse> createChangeset(
  const LocalOriginRepo &repo,
  const String &description,
  const Vector<String> &files,
  bool includeCommitId) {

  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setClient("HEAD");
  req.builder().setIncludeCommitId(includeCommitId);
  req.builder().setAction(G8ChangesetRequest::Action::CREATE);
  req.builder().setFileUpdateAction(G8ChangesetRequest::FileUpdateAction::REPLACE);

  req.builder().setDescription(description);
  req.builder().initFileUpdate(files.size());
  for (int i = 0; i < files.size(); i++) {
    req.builder().getFileUpdate().set(i, files[i]);
  }

  auto resp = g8Changeset(req.reader());
  EXPECT_THAT(resp.reader(), IsValidCreatedChangeset());
  return resp;
}

// given a changeset, return the id
String changesetId(G8ChangesetResponse::Reader r) { return r.getChangeset()[0].getId(); }

// Perform a finish land on the current client and given changeset
ConstRootMessage<G8ChangesetResponse> finishLand(
  const LocalOriginRepo &repo, const char *client, const char *changeset) {
  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setClient(client);
  req.builder().setAction(G8ChangesetRequest::Action::DELETE);
  req.builder().initId(1);
  req.builder().getId().set(0, changeset);

  return g8Changeset(req.reader());
}

// Canonical land operation, including syncing + returning result of sync
ConstRootMessage<G8ClientResponse> land(
  const LocalOriginRepo &repo, const char *client, const char *changeset, const char *msg) {
  auto landedId = simulateRemoteLand(repo, client, changeset, msg);
  EXPECT_THAT(finishLand(repo, client, changeset).reader(), IsOk());
  return syncClient(repo, client, landedId, {});
}

// Update Changeset
ConstRootMessage<G8ChangesetResponse> update(
  const LocalOriginRepo &repo, const char *client, const char *changeset) {
  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setClient(client);
  req.builder().initId(1);
  req.builder().getId().set(0, changeset);
  req.builder().setAction(G8ChangesetRequest::Action::UPDATE);
  return g8Changeset(req.reader());
}

// Add files to changeset
ConstRootMessage<G8ChangesetResponse> setFiles(
  const LocalOriginRepo &repo,
  const char *client,
  const char *changeset,
  const Vector<String> &files) {
  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setClient(client);
  req.builder().initId(1);
  req.builder().getId().set(0, changeset);
  req.builder().setAction(G8ChangesetRequest::Action::UPDATE);
  req.builder().setFileUpdateAction(G8ChangesetRequest::FileUpdateAction::REPLACE);
  req.builder().initFileUpdate(files.size());
  for (int i = 0; i < files.size(); i++) {
    req.builder().getFileUpdate().set(i, files[i]);
  }
  return g8Changeset(req.reader());
}

// Perform a sync operation on the current client
ConstRootMessage<G8ClientResponse> syncClient(
  const LocalOriginRepo &repo,
  const String &client,
  const String &landedId,
  const Vector<MergeDecision> &decisions,
  const String &inspectRegex,
  const bool inspectComplete,
  const Vector<AdditionalChange> &additionalChanges) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().initClient(1);
  req.builder().getClient().set(0, client);
  if (!landedId.empty()) {
    req.builder().getSyncParams().setSyncCommitId(landedId);
  }
  if (!inspectRegex.empty()) {
    req.builder().getSyncParams().setInspectRegex(inspectRegex);
  }
  req.builder().getSyncParams().setInspectComplete(inspectComplete);
  req.builder().setAction(G8ClientRequest::Action::SYNC);
  req.builder().setUser("jenkins");

  req.builder().getSyncParams().initDecision(decisions.size());
  int decisionIdx = 0;
  for (const auto &mergeDecision : decisions) {
    auto decisionBuilder = req.builder().getSyncParams().getDecision()[decisionIdx++];
    decisionBuilder.setChoice(mergeDecision.choice);
    decisionBuilder.setBlobId(mergeDecision.blobId);
    decisionBuilder.setFileId(mergeDecision.fileId.reader());
  }

  if (!additionalChanges.empty()) {
    req.builder().getSyncParams().initAdditionalChanges(additionalChanges.size());
    int changeIdx = 0;
    for (const auto &additionalChange : additionalChanges) {
      auto changeBuilder = req.builder().getSyncParams().getAdditionalChanges()[changeIdx++];
      changeBuilder.setPath(additionalChange.path);
      changeBuilder.setBlobId(additionalChange.blobId);
    }
  }

  return g8Client(req.reader());
}

// Performs a patch operation on the client.
ConstRootMessage<G8ClientResponse> patchClient(
  const LocalOriginRepo &repo,
  const String &client,
  const String &changeSourceBranch,
  const Vector<MergeDecision> &decisions) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().initClient(1);
  req.builder().getClient().set(0, client);
  req.builder().getPatchParams().setClient(changeSourceBranch);
  req.builder().setAction(G8ClientRequest::Action::PATCH);
  req.builder().getSyncParams().initDecision(decisions.size());
  int decisionIdx{0};
  for (const auto &mergeDecision : decisions) {
    auto decisionBuilder = req.builder().getSyncParams().getDecision()[decisionIdx++];
    decisionBuilder.setChoice(mergeDecision.choice);
    decisionBuilder.setBlobId(mergeDecision.blobId);
    decisionBuilder.setFileId(mergeDecision.fileId.reader());
  }

  return g8Client(req.reader());
}

ConstRootMessage<G8ClientResponse> patchRef(
  const LocalOriginRepo &repo,
  const String &client,
  const String &ref,
  const Vector<MergeDecision> &decisions) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().initClient(1);
  req.builder().getClient().set(0, client);
  req.builder().getPatchParams().setRef(ref);
  req.builder().setAction(G8ClientRequest::Action::PATCH);
  req.builder().getSyncParams().initDecision(decisions.size());
  int decisionIdx{0};
  for (const auto &mergeDecision : decisions) {
    auto decisionBuilder = req.builder().getSyncParams().getDecision()[decisionIdx++];
    decisionBuilder.setChoice(mergeDecision.choice);
    decisionBuilder.setBlobId(mergeDecision.blobId);
    decisionBuilder.setFileId(mergeDecision.fileId.reader());
  }

  return g8Client(req.reader());
}

// Creates a blob
ConstRootMessage<G8CreateBlobResponse> createBlob(const LocalOriginRepo &repo, String data) {
  MutableRootMessage<G8CreateBlobRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().initData(data.size());
  std::copy(data.begin(), data.end(), req.builder().getData().begin());

  return g8CreateBlob(req.reader());
}

// Perform house keeping.
ConstRootMessage<G8RepositoryResponse> keepHouse(const LocalOriginRepo &repo) {
  MutableRootMessage<G8RepositoryRequest> req;
  req.builder().setRepo(repo.path);
  req.builder().setUser("jenkins");
  req.builder().setAction(G8RepositoryRequest::Action::KEEP_HOUSE);
  return g8Repository(req.reader());
}

// Helper function for matchers that call a function that returns a non-empty string on failure.
bool checkErrorString(const String &error, ::testing::MatchResultListener *result_listener) {
  if (error.size() > 0) {
    *result_listener << "FAILED: " << error;
    return false;
  }
  return true;
}

// Expect the supplied list of clients, and that the named client is active. If active is not set,
// don't make any assertions about which client is active. Returns a non-empty error string on
// failure.
String equalsClients(
  G8ClientResponse::Reader r, const Vector<String> &clients, const char *active) {
  if (r.getStatus().getCode() != 0) {
    return format("Non-zero status: %d", r.getStatus().getCode());
  }
  if (r.getClient().size() != clients.size()) {
    return format("Expected %d clients but was %d", clients.size(), r.getClient().size());
  }
  for (int i = 0; i < clients.size(); ++i) {
    if (r.getClient()[i].getName() != clients[i]) {
      return format(
        "Expected client %d to be %s but was %s",
        i,
        clients[i].c_str(),
        r.getClient()[i].getName().cStr());
    }
    if (active != nullptr) {
      if (((r.getClient()[i].getName() == active) != r.getClient()[i].getActive())) {
        return format("Expected client %s to be active it wasn't", active);
      }
    }
  }
  return "";  // pass.
}

// Expect the supplied list of clients, remoteClients, and the active client. If active is
// not set, don't make any assertions about which client is active. Returns a non-empty error string
// on failure.
String equalsRemoteClients(
  G8ClientResponse::Reader r,
  const Vector<String> &clients,
  const Vector<String> &remoteClients,
  const char *active) {
  if (r.getStatus().getCode() != 0) {
    return format("Non-zero status: %d", r.getStatus().getCode());
  }
  if (r.getClient().size() != clients.size()) {
    return format("Expected %d clients but was %d", clients.size(), r.getClient().size());
  }
  if (r.getRemoteClient().size() != remoteClients.size()) {
    return format("Expected %d remote clients but was %d", clients.size(), r.getClient().size());
  }
  for (int i = 0; i < clients.size(); ++i) {
    if (r.getClient()[i].getName() != clients[i]) {
      return format(
        "Expected client %d to be %s but was %s",
        i,
        clients[i].c_str(),
        r.getClient()[i].getName().cStr());
    }
    if (active != nullptr) {
      if (((r.getClient()[i].getName() == active) != r.getClient()[i].getActive())) {
        return format("Expected client %s to be active it wasn't", active);
      }
    }
  }
  for (int i = 0; i < remoteClients.size(); ++i) {
    if (r.getRemoteClient()[i].getName() != remoteClients[i]) {
      return format(
        "Expected remote client %d to be %s but was %s",
        i,
        remoteClients[i].c_str(),
        r.getRemoteClient()[i].getName().cStr());
    }
  }
  return "";  // pass.
}

// Expect the supplied lists of files (sorted by changeset) to be returned from status(...).
// The input is a map from changeset id to files in that changeset. Since non-working changeset
// ids are generated dynamically, organizing them as a map saves callers the requirement of needing
// to sort them lexicographically at runtime. Returns a non-empty error string on failure.
String equalsStatus(
  G8ChangesetResponse::Reader r, const TreeMap<String, Vector<FileInfo>> &changesetFiles) {
  if (r.getStatus().getCode() != 0) {
    return format("Non-zero status: %d", r.getStatus().getCode());
  }
  if (r.getChangeset().size() != changesetFiles.size()) {
    return format(
      "Expected %d changesets but got %d", changesetFiles.size(), r.getChangeset().size());
  }
  for (auto c : r.getChangeset()) {
    auto changesetForId = changesetFiles.find(c.getId());
    if (changesetForId == changesetFiles.end()) {
      return format("Found unexpected changeset %s", c.getId().cStr());
    }
    if (c.getFileList().getInfo().size() != changesetForId->second.size()) {
      return format(
        "Expected changeset %s to have %d files but it had %d",
        c.getId().cStr(),
        changesetForId->second.size(),
        c.getFileList().getInfo().size());
    }
    for (int i = 0; i < c.getFileList().getInfo().size(); ++i) {
      if (
        c.getFileList().getInfo()[i].getStatus() != changesetForId->second[i].status
        || c.getFileList().getInfo()[i].getPath() != changesetForId->second[i].path
        || c.getFileList().getInfo()[i].getDirty() != changesetForId->second[i].dirty) {
        return format(
          "Expected changeset %s file %d to be {%d, %s, %d} but was {%d, %s, %d}",
          c.getId().cStr(),
          i,
          changesetForId->second[i].status,
          changesetForId->second[i].path.c_str(),
          changesetForId->second[i].dirty,
          c.getFileList().getInfo()[i].getStatus(),
          c.getFileList().getInfo()[i].getPath().cStr(),
          c.getFileList().getInfo()[i].getDirty());
      }
    }
  }
  return "";  // pass.
}

String equalsFiles(G8FileResponse::Reader r, const Vector<const char *> &paths) {
  if (r.getStatus().getCode() != 0) {
    return format("Non-zero status: %d", r.getStatus().getCode());
  }
  if (r.getFiles().getInfo().size() != paths.size()) {
    return format("Expected %d files but was %d", paths.size(), r.getFiles().getInfo().size());
  }
  for (int i = 0; i < paths.size(); ++i) {
    if (r.getFiles().getInfo()[i].getPath() != paths[i]) {
      return format(
        "Expected path %d to be %s but was %s",
        i,
        paths[i],
        r.getFiles().getInfo()[i].getPath().cStr());
    }
  }
  return "";  // pass.
}

// Expect a non-failure and a single changeset info to be returned with an id
String isValidCreatedChangeset(G8ChangesetResponse::Reader r) {
  if (r.getStatus().getCode() != 0) {
    return format("Non-zero status: %d", r.getStatus().getCode());
  }
  if (r.getChangeset().size() != 1) {
    return format("Expected single changeset but got %d", r.getChangeset().size());
  }
  if (r.getChangeset()[0].getId().size() == 0) {
    return "Expected a changeset ID but none was found";
  }
  return "";
}

String equalsMerge(
  G8ClientResponse::Reader r,
  const Vector<MergeAnalysisInfo> &mergeFiles,
  const Vector<MergeAnalysisInspect> &inspectFiles) {
  if (r.getStatus().getCode() != 0) {
    return format("Non-zero status: %d", r.getStatus().getCode());
  }
  if (r.getMerge().getInfo().size() != mergeFiles.size()) {
    return format(
      "Expected %d merge files, got %d", mergeFiles.size(), r.getMerge().getInfo().size());
  }

  for (auto m : r.getMerge().getInfo()) {
    auto mergeFile =
      std::find_if(begin(mergeFiles), end(mergeFiles), [&](const MergeAnalysisInfo &info) {
        return info.path == String(m.getPath().cStr());
      });
    if (mergeFile == end(mergeFiles)) {
      return format(
        "Unexpected merge file '%s' (old path '%s') encountered",
        m.getPath().cStr(),
        m.getPreviousPath().cStr());
    }
    if (String(m.getPreviousPath().cStr()) != mergeFile->previousPath) {
      return format(
        "expected '%s' to have previous path '%s' but it has '%s'",
        m.getPath().cStr(),
        mergeFile->previousPath.c_str(),
        m.getPreviousPath().cStr());
    }
    if (m.getStatus() != mergeFile->status) {
      if (mergeFile->status == G8MergeAnalysisInfo::Status::MERGEABLE) {
        return format(
          "expected '%s' (previously '%s') to be mergeable but it is conflicted",
          m.getPath().cStr(),
          m.getPreviousPath().cStr());
      }
      if (mergeFile->status == G8MergeAnalysisInfo::Status::CONFLICTED) {
        return format(
          "expected '%s' (previously '%s') to be conflicted but it is mergeable",
          m.getPath().cStr(),
          m.getPreviousPath().cStr());
      }
    }
  }

  if (r.getMerge().getInspect().size() != inspectFiles.size()) {
    return format(
      "Expected %d inspect files, got %d", inspectFiles.size(), r.getMerge().getInspect().size());
  }

  for (auto inspectRecord : r.getMerge().getInspect()) {
    auto inspectFile =
      std::find_if(begin(inspectFiles), end(inspectFiles), [&](const MergeAnalysisInspect &i) {
        return i.path == inspectRecord.getPath().cStr();
      });
    if (inspectFile == end(inspectFiles)) {
      return format("Unexpected inspect record %s encountered", inspectRecord.getPath().cStr());
    }
    if (inspectFile->status != inspectRecord.getStatus()) {
      return format(
        "Inspect record %s expected status %d but got %d",
        inspectRecord.getPath().cStr(),
        inspectFile->status,
        inspectRecord.getStatus());
    }
    if (inspectFile->source != inspectRecord.getSource()) {
      return format(
        "Inspect record %s expected source %d but got %d",
        inspectRecord.getPath().cStr(),
        inspectFile->source,
        inspectRecord.getSource());
    }
    if (inspectFile->nextBlobId != inspectRecord.getNextBlobId().cStr()) {
      return format(
        "Inspect record %s expected nextBlobId %s but got %s",
        inspectFile->path.c_str(),
        inspectFile->nextBlobId.c_str(),
        inspectRecord.getNextBlobId().cStr());
    }

    if (inspectFile->previousBlobId != inspectRecord.getPreviousBlobId().cStr()) {
      return format(
        "Inspect record %s expected previous %s but got %s",
        inspectFile->path.c_str(),
        inspectFile->previousBlobId.c_str(),
        inspectRecord.getPreviousBlobId().cStr());
    }
  }

  return "";
}

void PrintTo(const c8::G8ClientResponse::Reader &response, std::ostream *os) {
  *os << capnp::prettyPrint(response).flatten().cStr();
}

void PrintTo(const c8::G8ChangesetResponse::Reader &response, std::ostream *os) {
  *os << capnp::prettyPrint(response).flatten().cStr();
}

void PrintTo(const c8::G8FileResponse::Reader &response, std::ostream *os) {
  *os << capnp::prettyPrint(response).flatten().cStr();
}

}  // namespace c8
