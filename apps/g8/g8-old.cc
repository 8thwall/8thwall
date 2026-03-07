// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "g8-old.h",
  };
  deps = {
    ":git-service-factory",
    ":g8-helpers",
    ":g8-plumbing",
    "//c8:c8-log",
    "//c8:process",
    "//c8:map",
    "//c8:set",
    "//c8:vector",
    "//c8/git:g8-api.capnp-cc",
    "//c8/git:g8-git",
    "//c8/io:capnp-messages",
    "//c8/io:file-io",
    "//c8/pixels:base64",
    "//c8/stats:scope-timer",
    "//c8/string:format",
    "//c8/string:join",
    "//c8/string:split",
    "//c8/string:strcat",
    "//c8/string:trim",
    "@cli11//:cli11",
    "@json//:json",
    "@curl//:curl",
  };
  linkopts = {
    "-framework Security",
  };
}
cc_end(0x0980e395);

#include <capnp/pretty-print.h>
#include <libgen.h>
#include <unistd.h>

#include <filesystem>

// NOTE(pawel) POSIX.
#include <sys/resource.h>

#include <CLI/CLI.hpp>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <experimental/type_traits>
#include <mutex>
#include <nlohmann/json.hpp>
#include <queue>
#include <regex>
#include <string>
#include <vector>

#include "apps/g8/g8-helpers.h"
#include "apps/g8/g8-plumbing.h"
#include "apps/g8/git-service-factory.h"
#include "c8/c8-log.h"
#include "c8/git/g8-api.capnp.h"
#include "c8/git/g8-git.h"
#include "c8/io/capnp-messages.h"
#include "c8/io/file-io.h"
#include "c8/map.h"
#include "c8/pixels/base64.h"
#include "c8/process.h"
#include "c8/set.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/format.h"
#include "c8/string/join.h"
#include "c8/string/split.h"
#include "c8/string/strcat.h"
#include "c8/string/trim.h"
#include "c8/vector.h"
#include "curl/curl.h"

namespace fs = std::filesystem;
using namespace c8;

// TODO(pawel) Put back anonymous namespace in the right place.
// namespace {

// Call printf without adding a newline, and flush output.
template <class... Args>
static void prompt(const char *fmt, Args &&...args) {
  printf(fmt, std::forward<Args>(args)...);
  fflush(stdout);
}

template <class... Args>
static void prompt(const char *fmt) {
  prompt("%s", fmt);
}

std::ifstream launchTemporaryEditor(String message) {
  auto tmpEnv = std::getenv("TMPDIR");

  String tmpDir = tmpEnv ? tmpEnv : P_tmpdir;
  String editor = resolveEditor();

  constexpr char tmpName[] = "g8-temp-XXXXXX";
  auto filePath = format("%s%s", tmpDir.c_str(), tmpName);
  int fd = mkstemp(const_cast<char *>(filePath.c_str()));

  write(fd, message.c_str(), message.size());
  close(fd);

  String editorCmd = editor + " " + filePath;
  system(editorCmd.c_str());

  return std::ifstream(filePath);
}

template <class G8ResponseType>
void checkResponse(const G8ResponseType &response) {
  if (0 != response.reader().getStatus().getCode()) {
    C8Log("ERROR: %s", response.reader().getStatus().getMessage().cStr());
    throw CLI::RuntimeError(response.reader().getStatus().getCode());
  }
}

template <class G8RequestType, class G8ResponseType>
void verboseCheckResponse(
  const G8RequestType &req, const G8ResponseType &res, const MainContext &ctx) {
  if (ctx.verbose) {
    C8Log("Request:\n%s", capnp::prettyPrint(req.reader()).flatten().cStr());
    C8Log("Response:\n%s", capnp::prettyPrint(res.reader()).flatten().cStr());
  }
  checkResponse(res);
}

auto getGitService(const MainContext &ctx) {
  MutableRootMessage<G8RepositoryRequest> repoReq;
  repoReq.builder().setRepo(repoPath());
  repoReq.builder().setAction(G8RepositoryRequest::Action::GET_INFO);

  auto repoRes = g8Repository(repoReq.reader());
  verboseCheckResponse(repoReq, repoRes, ctx);

  GitServiceFactoryArgs args;
  switch (repoRes.reader().getRemote().getApi()) {
    case G8GitRemote::Api::GITHUB: {
      args.serviceType = GitServiceType::GITHUB;
      break;
    }
    case G8GitRemote::Api::GITLAB: {
      args.serviceType = GitServiceType::GITLAB;
      break;
    }
    default: {
      C8Log("ERROR: Unknown git service api");
      throw CLI::RuntimeError(1);
    }
  }

  String remoteName = repoRes.reader().getRemote().getRepoName().cStr();
  args.repoName = remoteName.substr(0, remoteName.rfind("."));
  args.endpoint = repoRes.reader().getRemote().getHost().cStr();
  args.mainBranchName = repoRes.reader().getMainBranchName();
  args.verbose = ctx.verbose;

  auto [_, token] = getCredentialsFromSecureStorageForHost(args.endpoint);
  args.credential = token;

  return GitServiceFactory::create(args);
}

String colorLine(G8DiffLine::Reader line) {
  String out;

  switch (line.getOrigin()) {
    case G8DiffLine::Origin::BINARY:
      out += line.getContent();
      break;
    case G8DiffLine::Origin::CONTEXT:
      out += " ";
      out += line.getContent();
      break;
    case G8DiffLine::Origin::ADDITION:
      out += "\033[32m+";
      out += line.getContent();
      out += "\033[0m";
      break;
    case G8DiffLine::Origin::DELETION:
      out += "\033[31m-";
      out += line.getContent();
      out += "\033[0m";
      break;
    case G8DiffLine::Origin::CONTEXT_EOFNL:
      out += "=";
      out += line.getContent();
      break;
    case G8DiffLine::Origin::ADD_EOFNL:
      out += "\033[32m>";
      out += line.getContent();
      out += "\033[0m";
      break;
    case G8DiffLine::Origin::DEL_EOFNL:
      out += "\033[31m<";
      out += line.getContent();
      out += "\033[0m";
      break;
    case G8DiffLine::Origin::FILE_HDR:
      out += "\033[33m";
      out += line.getContent();
      out += "\033[0m";
      break;
    case G8DiffLine::Origin::HUNK_HDR:
      out += "\033[33m";
      out += line.getContent();
      out += "\033[0m";
      break;
  }

  return out;
}

const char *getStatusChar(G8FileInfo::Status status) {
  switch (status) {
    case G8FileInfo::Status::UNMODIFIED:
      return " ";
    case G8FileInfo::Status::ADDED:
      return "\033[32mA\033[0m";
    case G8FileInfo::Status::COPIED:
      return "\033[32mC\033[0m";
    case G8FileInfo::Status::DELETED:
      return "\033[31mD\033[0m";
    case G8FileInfo::Status::MODIFIED:
      return "\033[33mM\033[0m";
    case G8FileInfo::Status::RENAMED:
      return "\033[33mR\033[0m";
    case G8FileInfo::Status::TYPE_CHANGED:
      return "\033[33mT\033[0m";
    case G8FileInfo::Status::IGNORED:
      return "\033[37mI\033[0m";
    case G8FileInfo::Status::CONFLICTED:
      return "\033[31mU\033[0m";
    case G8FileInfo::Status::UNREADABLE:
      return "\033[31mX\033[0m";
  }
}

const char *getMergeStatusChar(G8MergeAnalysisInfo::Status status) {
  switch (status) {
    case G8MergeAnalysisInfo::Status::MERGEABLE:
      return "M";
    case G8MergeAnalysisInfo::Status::CONFLICTED:
      return "C";
    default:
      return "D";
  }
}

void cleanupMergedChangesets(const MainContext &ctx, String clientName) {
  auto gitService = getGitService(ctx);

  MutableRootMessage<G8ChangesetRequest> listReq;
  listReq.builder().setRepo(repoPath());
  listReq.builder().setIncludeRemoteBranch(true);
  listReq.builder().setClient(clientName);
  listReq.builder().setAction(G8ChangesetRequest::Action::LIST);
  auto listRes = g8Changeset(listReq.reader());
  verboseCheckResponse(listReq, listRes, ctx);

  Vector<String> changesetsToDelete = {};
  for (auto cs : listRes.reader().getChangeset()) {
    // TODO(christoph): Add parallelization
    auto pr = gitService->pullRequestByBranch(cs.getRemoteBranch());
    if (pr && pr->isMerged) {
      changesetsToDelete.push_back(cs.getId());
    }
  }

  if (changesetsToDelete.size()) {
    MutableRootMessage<G8ChangesetRequest> deleteReq;
    deleteReq.builder().setRepo(repoPath());
    deleteReq.builder().setClient(clientName);
    deleteReq.builder().setAction(G8ChangesetRequest::Action::DELETE);
    deleteReq.builder().initId(changesetsToDelete.size());
    for (int i = 0; i < changesetsToDelete.size(); ++i) {
      deleteReq.builder().getId().set(i, changesetsToDelete[i]);
    }
    auto deleteRes = g8Changeset(deleteReq.reader());
    verboseCheckResponse(deleteReq, deleteRes, ctx);
  }
}

void cleanupMergedChangesets(
  const MainContext &ctx, ConstRootMessage<G8ClientResponse> &clientRes) {
  ScopeTimer t("cleanupMergedChangesets");

  for (auto client : clientRes.reader().getClient()) {
    cleanupMergedChangesets(ctx, client.getName());
  }
}

// Resolve merge conflicts as result of sync or patch.
void resolveMerges(
  const MainContext &ctx, G8MergeAnalysis::Reader merge, G8SyncParams::Builder syncParams) {
  // Get feedback from the user on how to handle merge conflicts before syncing again.
  auto tmpEnv = std::getenv("TMPDIR");

  String tmpDir = tmpEnv ? tmpEnv : P_tmpdir;
  String mergeTool = resolveMergeTool();

  const std::regex markerRegex("(^>>>>.*)|(^====.*)|(^<<<<.*)");

  int decisionIdx{0};
  syncParams.initDecision(merge.getInfo().size());
  for (auto mergeInfo : merge.getInfo()) {
    auto decision = syncParams.getDecision()[decisionIdx++];
    decision.setFileId(mergeInfo.getFileId());
    bool haveMergeContent = false;
    String action;
    bool hasConflictMarkers =
      mergeInfo.getStatus() == G8MergeAnalysisInfo::Status::CONFLICTED ? true : false;
    std::unique_ptr<char[]> mergeFilePath(nullptr);
    bool showHelp = false;
    while (1) {
      if (showHelp) {
        // Show help if the user requested or if they had invalid input.
        C8Log("\nMerge conflict detected, please choose from the following:");
        C8Log("  ?  (help)          - Print this message.");
        C8Log("  c  (cancel)        - Exit without syncing.");
        C8Log("  d  (diff)          - Print diff from yours to merge.");
        C8Log("  e  (edit)          - Edit the proposed merge and manually resolve conflicts.");
        C8Log("  ad (accept delete) - Accept their delete [if applicable].");
        C8Log("  ae (accept edits)  - Accept your edits [if applicable].");
        C8Log("  am (accept merge)  - Accept the auto merge results [if applicable].");
        C8Log("  at (accept theirs) - Accept their version, discard your changes.");
        C8Log("  ay (accept yours)  - Accept your version, discard their changes.");
        C8Log("  dt (diff theirs)   - Print diff from original to theirs.");
        C8Log("  dy (diff yours)    - Print diff from original to yours.\n");
        showHelp = false;
      }

      // Check whether all of the conflict markers have been removed.
      if (mergeFilePath.get()) {
        std::ifstream stream(mergeFilePath.get());
        String line;
        hasConflictMarkers = false;
        while (std::getline(stream, line)) {
          if (regex_match(line, markerRegex)) {
            hasConflictMarkers = true;
            break;
          }
        }
      }

      String defaultAction;
      if (!mergeInfo.getFileId().hasTheirs()) {
        C8Log(
          "\nModified file %s%s%s has been deleted. Accept delete?",
          BOLD,
          mergeInfo.getPath().cStr(),
          CLEAR);
        prompt("Accept Delete (ad) Accept Yours (ay) Cancel(c) Help(?) ad: ");
        defaultAction = "ad";
      } else if (!mergeInfo.getFileId().hasYours()) {
        C8Log(
          "\nDeleted file %s%s%s has been modified. Accept changes?",
          BOLD,
          mergeInfo.getPath().cStr(),
          CLEAR);
        prompt("Accept Theirs (at) Keep Delete (kd) Cancel (c) Help(?) at: ");
        defaultAction = "at";
      } else {
        switch (mergeInfo.getStatus()) {
          case G8MergeAnalysisInfo::Status::MERGEABLE: {
            C8Log(
              "\nModified file %s%s%s has changed. Accept merge?",
              BOLD,
              mergeInfo.getPath().cStr(),
              CLEAR);
            prompt(
              "Accept Merge (am) Accept Theirs (at) "
              "Accept Yours (ay) Edit(e) Cancel(c) Help(?) am: ");
            defaultAction = "am";
            break;
          }
          case G8MergeAnalysisInfo::Status::CONFLICTED: {
            if (hasConflictMarkers) {
              // Review hunks or edit directly?
              C8Log(
                "\nModified file %s%s%s has merge conflicts. Edit?",
                BOLD,
                mergeInfo.getPath().cStr(),
                CLEAR);
              prompt("Edit(e) Accept Theirs (at) Accept Yours (ay) Cancel(c) Help(?) e: ");
              defaultAction = "e";
            } else {
              C8Log(
                "\nFile %s%s%s has been edited. Accept edit?",
                BOLD,
                mergeInfo.getPath().cStr(),
                CLEAR);
              prompt(
                "Accept Edits (ae) Accept Theirs (at) "
                "Accept Yours (ay) Edit(e) Cancel(c) Help(?) ae: ");
              defaultAction = "ae";
            }
            break;
          }
        }
      }

      std::getline(std::cin, action);
      action = action.empty() ? defaultAction : action;
      if (action == "?") {
        showHelp = true;
      } else if (action == "e") {
        if (!haveMergeContent) {
          // Request the merge file.
          MutableRootMessage<G8MergeFileRequest> mergeReq;
          auto mergeReqBuilder = mergeReq.builder();
          mergeReqBuilder.setRepo(repoPath());
          mergeReqBuilder.setMergeId(merge.getMergeId());
          mergeReqBuilder.setFileId(mergeInfo.getFileId());
          mergeReqBuilder.setPath(mergeInfo.getPath());

          auto mergeRes = g8MergeFile(mergeReq.reader());
          verboseCheckResponse(mergeReq, mergeRes, ctx);  // merge file

          fs::path path(mergeInfo.getPath().cStr());
          auto ext = path.extension().string();

          // Create a temp file with the merge conflict.
          constexpr char tmpName[] = "g8-merge-XXXXXX";
          mergeFilePath.reset(new char[tmpDir.size() + sizeof(tmpName) + ext.size()]);
          strcpy(mergeFilePath.get(), tmpDir.c_str());
          strcpy(mergeFilePath.get() + tmpDir.size(), tmpName);
          // -1 to account for null byte in tmpName
          strcpy(mergeFilePath.get() + tmpDir.size() + sizeof(tmpName) - 1, ext.c_str());

          // Write out the merge conflict file to a temporary file.
          int fd = mkstemps(mergeFilePath.get(), ext.size());
          C8Log("%s", mergeFilePath.get());
          auto mergeContent = mergeRes.reader().getMerge().getContent();
          write(fd, mergeContent.cStr(), mergeContent.size());

          // Close the file.
          close(fd);
          haveMergeContent = true;
        }

        String command = mergeTool + " ";
        command += mergeFilePath.get();
        system(command.c_str());
      } else if (action == "ae") {
        if (hasConflictMarkers) {
          continue;
        }

        // Write out the blob to disk and add to the accepted conflicts
        MutableRootMessage<G8CreateBlobRequest> blobReq;
        auto blobReqBuilder = blobReq.builder();
        blobReqBuilder.setRepo(repoPath());

        std::ifstream blobStream(mergeFilePath.get(), std::ios::binary | std::ios::ate);
        std::ifstream::pos_type blobSize = blobStream.tellg();

        blobReqBuilder.initData(blobSize);

        blobStream.seekg(0, std::ios::beg);
        blobStream.read(reinterpret_cast<char *>(blobReqBuilder.getData().begin()), blobSize);

        auto blobRes = g8CreateBlob(blobReq.reader());
        verboseCheckResponse(blobReq, blobRes, ctx);  // create blob

        decision.setBlobId(blobRes.reader().getId());
        decision.setChoice(G8MergeDecision::Choice::MANUAL_MERGE);
        break;
      } else if (action == "ad") {
        decision.setChoice(G8MergeDecision::Choice::THEIR_DELETE);
        break;
      } else if (action == "am") {
        if (mergeInfo.getStatus() == G8MergeAnalysisInfo::Status::CONFLICTED) {
          C8Log("Unable to auto merge (am) on a conflicted file");
          continue;
        }
        decision.setChoice(G8MergeDecision::Choice::AUTO_MERGE);
        break;
      } else if (action == "at") {
        decision.setChoice(G8MergeDecision::Choice::THEIR_CHANGE);
        break;
      } else if (action == "ay" || action == "kd") {
        decision.setChoice(G8MergeDecision::Choice::YOUR_CHANGE);
        break;
      } else if (action == "dt") {
        MutableRootMessage<G8DiffBlobsRequest> diffReq;

        diffReq.builder().setRepo(repoPath());
        diffReq.builder().setBaseId(mergeInfo.getFileId().getOriginal());
        diffReq.builder().setNewId(mergeInfo.getFileId().getTheirs());

        auto diffRes = g8DiffBlobs(diffReq.reader());
        verboseCheckResponse(diffReq, diffRes, ctx);  // diff blob

        for (auto line : diffRes.reader().getLine()) {
          C8Log("%s", colorLine(line).c_str());
        }
      } else if (action == "dy") {
        MutableRootMessage<G8DiffBlobsRequest> diffReq;

        diffReq.builder().setRepo(repoPath());
        diffReq.builder().setBaseId(mergeInfo.getFileId().getOriginal());
        diffReq.builder().setNewId(mergeInfo.getFileId().getYours());

        auto diffRes = g8DiffBlobs(diffReq.reader());
        verboseCheckResponse(diffReq, diffRes, ctx);  // diffBlob

        for (auto line : diffRes.reader().getLine()) {
          C8Log("%s", colorLine(line).c_str());
        }
      } else if (action == "d") {
        if (mergeInfo.hasMergeBlobId()) {
          MutableRootMessage<G8DiffBlobsRequest> diffReq;

          diffReq.builder().setRepo(repoPath());
          diffReq.builder().setBaseId(mergeInfo.getFileId().getYours());
          diffReq.builder().setNewId(mergeInfo.getMergeBlobId());

          auto diffRes = g8DiffBlobs(diffReq.reader());

          verboseCheckResponse(diffReq, diffRes, ctx);  // diff blob

          for (auto line : diffRes.reader().getLine()) {
            C8Log("%s", colorLine(line).c_str());
          }

        } else {
          C8Log("Currently unsupported diff type for conflicted entry");
          exit(1);
        }
      } else if (action == "c") {
        // User chose to cancel. Stop sync.
        C8Log("Canceled sync");
        return;
      } else {
        C8Log("\n%sWARNING: %sUnsupported action: %s%s", RED, YELLOW, action.c_str(), CLEAR);
        showHelp = true;
      }
    }
  }
}

// Sync remote and rebase client.
//
// Example usage:
//   # Sync active client
//   g8 sync
//
//   # Sync a client by name
//   g8 sync myclient

void sync(const MainContext &ctx, Vector<String> clientNames, String syncCommitId) {
  ScopeTimer t("sync");
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setAction(G8ClientRequest::Action::SYNC);
  req.builder().setUser(getenv("USER"));
  req.builder().initClient(clientNames.size());
  setCredsForCwdRepo(req.builder().getAuth());

  if (!syncCommitId.empty()) {
    req.builder().getSyncParams().setSyncCommitId(syncCommitId);
  }

  for (int i = 0; i < clientNames.size(); ++i) {
    req.builder().getClient().set(i, clientNames[i].c_str());
  }

  // note(pawel) temporary to speed up g8 sync.
  process::ExecuteOptions gitExecOpts;
  gitExecOpts.file = "git";
  gitExecOpts.redirectStderr = true;
  process::execute(gitExecOpts, {"fetch", "-q", "origin", "main"});
  process::execute(gitExecOpts, {"fetch", "-q", "origin", "master"});

  auto res = g8Client(req.reader());
  verboseCheckResponse(req, res, ctx);

  auto merge = res.reader().getMerge();
  if (0 == merge.getInfo().size()) {
    // PRINT SYNCED TO XXXX
  } else {
    resolveMerges(ctx, res.reader().getMerge(), req.builder().getSyncParams());

    // In case client was not specified, the res returns the client operated on.
    req.builder().initClient(1).set(0, res.reader().getClient()[0].getName());

    // In case sync param was not specified on sync and master moved.
    req.builder().getSyncParams().setSyncCommitId(res.reader().getMerge().getMergeId().getTheirs());

    res = g8Client(req.reader());
    verboseCheckResponse(req, res, ctx);
  }

  cleanupMergedChangesets(ctx, res);
}

MutableRootMessage<G8ClientRequest> makeClientRequest() {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setUser(getenv("USER"));
  setCredsForCwdRepo(req.builder().getAuth());
  return req;
}

MutableRootMessage<G8ClientRequest> makeClientRequest(String client) {
  auto req = makeClientRequest();
  req.builder().initClient(1);
  req.builder().getClient().set(0, client);
  return req;
}

String autoSync(const MainContext &ctx, String client) {
  auto syncReq = makeClientRequest(client);
  syncReq.builder().setAction(G8ClientRequest::Action::SYNC);

  auto syncRes = g8Client(syncReq.reader());
  verboseCheckResponse(syncReq, syncRes, ctx);

  auto merge = syncRes.reader().getMerge();
  if (merge.getStatus() == G8MergeAnalysis::MergeStatus::COMPLETE) {
    return merge.getMergeId().getTheirs();
  } else {
    return "";
  }
}

void syncAll(const MainContext &ctx, Vector<String> clientNames, String syncCommitId) {
  ScopeTimer t("syncAll");

  if (!clientNames.empty()) {
    C8Log("Must not provide client names with --all.");
    throw CLI::RuntimeError(1);
  }

  if (!syncCommitId.empty()) {
    C8Log("Must not provide commit ID with --all.");
    throw CLI::RuntimeError(1);
  }

  auto listReq = makeClientRequest();
  listReq.builder().setAction(G8ClientRequest::Action::LIST);
  auto listRes = g8Client(listReq.reader());
  verboseCheckResponse(listReq, listRes, ctx);

  String latestForkId;
  Vector<String> failedSyncs;

  for (const auto &client : listRes.reader().getClient()) {
    auto isSynced = latestForkId.compare(client.getForkId()) == 0;
    if (!isSynced) {
      auto newForkId = autoSync(ctx, client.getName());
      if (newForkId.empty()) {
        failedSyncs.push_back(client.getName());
      } else {
        latestForkId = newForkId;
      }
    }
  }

  cleanupMergedChangesets(ctx, listRes);

  if (!failedSyncs.empty()) {
    C8Log("Manual sync required for: %s", strJoin(failedSyncs, ", ").c_str());
  }
}

CLI::App_p syncCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Sync and rebase client", "sync");

  static Vector<String> clientNames;
  cmd->add_option("client", clientNames, "Client names");

  static String syncCommitId;
  cmd->add_option("-c,--commit", syncCommitId, "Commit ID to sync to");

  static bool syncAllClients;
  cmd->add_flag("--all", syncAllClients, "Sync all clients");

  cmd->callback([&] {
    ScopeTimer t("g8");

    if (syncAllClients) {
      syncAll(ctx, clientNames, syncCommitId);
    } else {
      sync(ctx, clientNames, syncCommitId);
    }
  });

  return cmd;
}

void patch(
  const MainContext &ctx,
  Vector<String> clients,
  const String &sourceUser,
  const String &sourceClient,
  const String &sourceChangeset,
  const String &sourceRef) {
  ScopeTimer t("patch");
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setAction(G8ClientRequest::Action::PATCH);

  req.builder().initClient(clients.size());
  for (int i = 0; i < clients.size(); ++i) {
    req.builder().getClient().set(i, clients[i].c_str());
  }

  if (!sourceClient.empty()) {
    req.builder().getPatchParams().setClient(sourceClient);
  }
  if (!sourceUser.empty()) {
    req.builder().getPatchParams().setUser(sourceUser);
  }
  if (!sourceChangeset.empty()) {
    req.builder().getPatchParams().setChangeset(sourceChangeset);
  }
  if (!sourceRef.empty()) {
    req.builder().getPatchParams().setRef(sourceRef);
  }
  setCredsForCwdRepo(req.builder().getAuth());

  auto res = g8Client(req.reader());
  verboseCheckResponse(req, res, ctx);

  auto merge = res.reader().getMerge();
  if (0 == merge.getInfo().size()) {
    // Auto-patch successful.
  } else {
    resolveMerges(ctx, res.reader().getMerge(), req.builder().getSyncParams());
    res = g8Client(req.reader());
    verboseCheckResponse(req, res, ctx);
  }
}

CLI::App_p patchCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Patch changes", "patch");

  static Vector<String> clients;
  cmd->add_option("client", clients, "Client names");

  static String sourceClient;
  cmd->add_option("-c,--client", sourceClient, "Source client to get changes from");

  static String sourceChangeset;
  cmd->add_option("-s,--changeset", sourceChangeset, "Source changeset to get changes from");

  static String sourceUser;
  cmd->add_option("-u,--user", sourceUser, "Source user to get changes from");

  static String prNumber;
  cmd->add_option("--pr", prNumber, "GitHub PR number to patch changes from");
  cmd->add_option("--mr", prNumber, "GitLab MR number to patch changes from");

  static String sourceRef;
  cmd->add_option("-b,--branch,-r,--ref", sourceRef, "Branch or reference to patch changes from");

  cmd->callback([&] {
    ScopeTimer t("g8");

    // Look up the user/client/changeset from PR number.
    if (!prNumber.empty()) {
      if (!sourceClient.empty() || !sourceChangeset.empty() || !sourceUser.empty()) {
        C8Log("If PR number is specified, client/changeset/user fields must be empty");
        throw CLI::RuntimeError(1);
      }

      auto gitService = getGitService(ctx);
      auto prInfo = gitService->pullRequestById(prNumber);
      if (!prInfo) {
        C8Log("Pull request not found %s", prNumber.c_str());
        throw CLI::RuntimeError(1);
      }

      sourceRef = strCat("origin/", prInfo->branchName);
    }
    patch(ctx, clients, sourceUser, sourceClient, sourceChangeset, sourceRef);
  });

  return cmd;
}

// Do a dry sync to obtain mergeability of files in the client
//
// Example usage:
//  g8 dry-sync

void drySync(const MainContext &ctx, String client, String syncCommitId) {
  ScopeTimer t("dry-sync");
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setUser(getenv("USER"));
  req.builder().setAction(G8ClientRequest::Action::DRY_SYNC);
  setCredsForCwdRepo(req.builder().getAuth());

  if (!client.empty()) {
    req.builder().initClient(1);
    req.builder().getClient().set(0, client);
  }

  if (!syncCommitId.empty()) {
    req.builder().getSyncParams().setSyncCommitId(syncCommitId);
  }

  auto res = g8Client(req.reader());
  verboseCheckResponse(req, res, ctx);

  for (const auto mergeInfo : res.reader().getMerge().getInfo()) {
    C8Log("%s %s", getMergeStatusChar(mergeInfo.getStatus()), mergeInfo.getPath().cStr());
  }
}

CLI::App_p drySyncCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Dry sync to uncover mergeability", "dry-sync");

  static String client;
  cmd->add_option("client", client, "client name");

  cmd->callback([&] {
    ScopeTimer t("g8");
    drySync(ctx, client, "");
  });

  return cmd;
}

// Create a new changeset.
//
// Example usage:
//   # Create a new changeset with all added/modified/deleted files.
//   g8 newchange -m "Adding my cool change."

void newchange(const MainContext &ctx, String description) {
  ScopeTimer t("newchange");
  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setClient("HEAD");
  req.builder().setIncludeSummary(true);
  req.builder().setIncludeDescription(true);
  req.builder().setIncludeBody(true);
  req.builder().setIncludeFileInfo(true);
  req.builder().setIncludeRemoteBranch(true);
  setCredsForCwdRepo(req.builder().getAuth());

  req.builder().setIncludeWorkingChanges(true);
  req.builder().setAction(G8ChangesetRequest::Action::LIST);

  auto res = g8Changeset(req.reader());
  verboseCheckResponse(req, res, ctx);

  String message = "# The following files will be included in the changeset.\n";
  for (auto changeset : res.reader().getChangeset()) {
    for (auto file : changeset.getFileList().getInfo()) {
      if (changeset.getId() == "working") {
        message += String(file.getPath().cStr()) + "\n";
      }
    }
  }

  auto stream = launchTemporaryEditor(message);

  String line;
  Vector<String> fileList;
  while (std::getline(stream, line)) {
    if (!line.empty() && line[0] != '#') {
      fileList.push_back(line);
    }
  }

  message = format(
    "%s\n"
    "# Please enter the description for your changeset. Lines starting\n"
    "# with '#' will be ignored, and an empty message will cancel.\n"
    "#\n"
    "# In client '%s'\n"
    "#\n"
    "# Files in the changeset:\n"
    "#   %s\n",
    description.c_str(),
    res.reader().getClient().cStr(),
    strJoin(fileList, "\n#   ").c_str());

  stream = launchTemporaryEditor(message);
  description.clear();
  while (std::getline(stream, line)) {
    if (!line.empty() && line[0] == '#') {
      // Ignore comment lines.
      continue;
    }
    description += line + "\n";
  }

  description = strTrim(description);

  if (description.empty()) {
    C8Log("Operation canceled.");
    exit(1);
  }

  req.builder().setDescription(description);

  req.builder().initFileUpdate(fileList.size());
  for (int i = 0; i < fileList.size(); i++) {
    req.builder().getFileUpdate().set(i, fileList[i]);
  }

  req.builder().setIncludeWorkingChanges(false);
  req.builder().setAction(G8ChangesetRequest::Action::CREATE);
  req.builder().setFileUpdateAction(G8ChangesetRequest::FileUpdateAction::REPLACE);

  res = g8Changeset(req.reader());
  verboseCheckResponse(req, res, ctx);

  auto changeset = res.reader().getChangeset()[0];
  C8Log("Created changeset \033[33m%s\033[0m", changeset.getId().cStr());
  for (auto info : changeset.getFileList().getInfo()) {
    if (
      info.getStatus() == G8FileInfo::Status::COPIED
      || info.getStatus() == G8FileInfo::Status::RENAMED) {
      C8Log(
        "  %s %s --> %s",
        getStatusChar(info.getStatus()),
        info.getPreviousPath().cStr(),
        info.getPath().cStr());
    } else {
      C8Log("  %s %s", getStatusChar(info.getStatus()), info.getPath().cStr());
    }
  }

  auto gitService = getGitService(ctx);
  CreatePullRequestArgs args;
  args.headRef = changeset.getRemoteBranch().cStr();
  args.title = changeset.getSummary().cStr();
  args.body = changeset.getBody().cStr();
  args.draft = false;
  auto success = gitService->createPullRequest(args);

  if (!success) {
    C8Log("ERROR: failure in making Github PR request");
  }
}

CLI::App_p newchangeCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Create new changeset", "newchange");

  static String description;
  cmd->add_option("-m", description, "Changeset description");

  cmd->callback([&] {
    ScopeTimer t("g8");
    newchange(ctx, description);
  });
  return cmd;
}

// Update one or more changesets.
//
// Example usage:
//   # Update all changesets in client
//   g8 update
//
//   # Update 1 changeset and change description
//   g8 update 231146 -m "Updated change message"
CLI::App_p updateCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Update changesets", "update");

  // If specified, update description too.
  static String description;
  cmd->add_option("-m", description, "Changeset description");

  static Vector<String> changesetId;
  cmd->add_option("id", changesetId, "Changeset id");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8ChangesetRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setDescription(description);
    req.builder().setClient("HEAD");
    req.builder().setIncludeSummary(true);
    req.builder().setIncludeDescription(true);
    req.builder().setIncludeBody(true);
    req.builder().setIncludeRemoteBranch(true);
    setCredsForCwdRepo(req.builder().getAuth());

    if (!description.empty() && changesetId.empty()) {
      req.builder().setAction(G8ChangesetRequest::Action::LIST);
      CHECK_DECLARE_G8(listRes, req, c8::g8Changeset);
      // We don't request the working changeset so we don't need to account for it here.
      if (listRes.reader().getChangeset().size() > 1) {
        C8Log("Multiple changesets found. Specify which one you want to update the message for.");
        exit(1);
      }
    }

    req.builder().initId(changesetId.size());
    for (int i = 0; i < changesetId.size(); ++i) {
      req.builder().getId().set(i, changesetId[i]);
    }

    req.builder().setAction(G8ChangesetRequest::Action::UPDATE);
    if (!description.empty()) {
      req.builder().setDescription(description);
    }

    CHECK_DECLARE_G8(res, req, c8::g8Changeset);

    int changesetUpdates = 0;
    for (auto changeset : res.reader().getChangeset()) {
      if (changeset.getUpdated()) {
        changesetUpdates++;
      }
    }

    if (changesetUpdates == 1) {
      C8Log("Updated changeset");
    } else if (changesetUpdates > 1) {
      C8Log("Updated changesets");
    }

    auto gitService = getGitService(ctx);

    for (auto changeset : res.reader().getChangeset()) {
      if (changeset.getUpdated()) {
        C8Log("\033[33m%s\033[0m %s", changeset.getId().cStr(), changeset.getSummary().cStr());

        String head = changeset.getRemoteBranch().cStr();
        auto pr = gitService->pullRequestByBranch(head);
        if (!pr) {
          if (ctx.verbose) {
            C8Log("Github PR does not exist for %s, creating", head.c_str());
          }

          CreatePullRequestArgs args;
          args.headRef = head;
          args.title = changeset.getSummary().cStr();
          args.body = changeset.getBody().cStr();
          args.draft = false;
          auto success = gitService->createPullRequest(args);
          if (!success) {
            C8Log("WARNING: Failed to create GitHub PR for %s.", head.c_str());
          }
        } else if (!description.empty()) {
          TitleRequest titleRequest;
          titleRequest.pullRequestId = pr->pullRequestId;
          titleRequest.message = description;
          gitService->updateTitle(titleRequest);
        }
      }
    }
  });

  return cmd;
}

// View or delete changesets.
//
// Example usage:
//   # List all changesets.
//   g8 change
//
//   # Get a detailed description of a changelist
//   g8 change 7654123
//
//   # Delete a changeset.
//   g8 change -d 7654123
CLI::App_p changeCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Query changeset", "change");

  static bool deleteChangeset;
  cmd->add_flag("-d", deleteChangeset, "Delete changeset");

  static Vector<String> changesetId;
  cmd->add_option("id", changesetId, "Changeset id");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8ChangesetRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setClient("HEAD");
    setCredsForCwdRepo(req.builder().getAuth());

    if (!deleteChangeset) {
      req.builder().setIncludeSummary(true);

      if (changesetId.empty()) {
        req.builder().setIncludeSummary(true);
      } else {
        req.builder().setIncludeDescription(true);
      }
    }

    req.builder().initId(changesetId.size());
    for (int i = 0; i < changesetId.size(); ++i) {
      req.builder().getId().set(i, changesetId[i]);
    }

    if (deleteChangeset) {
      // Set the action as delete.
      req.builder().setAction(G8ChangesetRequest::Action::DELETE);
      if (changesetId.empty()) {
        C8Log("Missing changeset id");
        exit(1);
      }
    } else {
      // List information about the changeset
      req.builder().setAction(G8ChangesetRequest::Action::LIST);
    }

    CHECK_DECLARE_G8(res, req, c8::g8Changeset);

    if (req.reader().getAction() == G8ChangesetRequest::Action::DELETE) {
      // We assume here that if g8Changeset didn't return an error that it deleted all the
      // requested Changesets.
      for (auto changesetId : req.reader().getId()) {
        C8Log("Deleted changeset \033[33m%s\033[0m", changesetId.cStr());
      }
    } else if (0 == res.reader().getChangeset().size()) {
      C8Log("No changesets in client");
    } else {
      if (!req.reader().getIncludeDescription()) {
        for (auto changeset : res.reader().getChangeset()) {
          C8Log("  \033[33m%s\033[0m %s", changeset.getId().cStr(), changeset.getSummary().cStr());
        }
      } else {
        for (auto changeset : res.reader().getChangeset()) {
          C8Log("\033[33mChangeset %s\033[0m", changeset.getId().cStr());
          // TODO(mc): Indent each line of the description by 2-chars.
          C8Log("%s", changeset.getDescription().cStr());
        }
      }
    }
  });

  return cmd;
}

// Change to a client or delete a client with -d.
//
// Example usage:
//   # Change the active client to dingo.
//   g8 client ripley
//
//   # Delete the ripley client.
//   g8 client -d ripley.
CLI::App_p clientCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Change client", "client");

  static Vector<String> clientNames;
  cmd->add_option("names", clientNames, "Client names");

  static bool deleteClient;
  cmd->add_flag("-d", deleteClient, "Delete client");

  static bool fetchCloudInfo;
  cmd->add_flag("--cloud-info", fetchCloudInfo, "Fetch time of latest remote save commit");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8ClientRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().initClient(clientNames.size());
    setCredsForCwdRepo(req.builder().getAuth());

    for (auto &singleName : clientNames) {
      auto nameParts = split(singleName, "-");
      if (nameParts[0] == getenv("USER") && nameParts.size() == 3) {
        C8Log(
          "Using client name %s from remote changeset %s",
          nameParts[1].c_str(),
          singleName.c_str());
        singleName = nameParts[1];
      }
    }

    for (int i = 0; i < clientNames.size(); ++i) {
      req.builder().getClient().set(i, clientNames[i].c_str());
    }

    if (deleteClient) {
      req.builder().setAction(G8ClientRequest::Action::DELETE);
      // req.builder().setUser(getenv("USER"));
    } else if (!clientNames.empty()) {
      // see if we need to create the client
      req.builder().setAction(G8ClientRequest::Action::LIST);
      CHECK_DECLARE_G8(res, req, c8::g8Client);
      if (res.reader().getStatus().getCode() != 0) {
        C8Log("Failed to list clients");
        return;
      }

      bool exists = false;
      for (auto client : res.reader().getClient()) {
        int result = strcasecmp(client.getName().cStr(), clientNames[0].c_str());
        if (result == 0) {
          exists = true;
          break;
        }
      }

      if (!exists) {
        prompt("This client does not exist, create it now? (Y/n): ");
        String reply;
        std::getline(std::cin, reply);
        if (std::tolower(reply[0] == 'n')) {
          return;
        }

        req.builder().setUser(getenv("USER"));
        req.builder().setAction(G8ClientRequest::Action::CREATE);
        CHECK_G8(res, req, c8::g8Client);
        if (res.reader().getStatus().getCode() != 0) {
          C8Log("Failed to create client");
          return;
        }
      }

      req.builder().setAction(G8ClientRequest::Action::SWITCH);
    } else {
      if (fetchCloudInfo) {
        req.builder().setIncludeCloudSaveInfo(true);
        req.builder().setUser(getenv("USER"));
      }
      req.builder().setAction(G8ClientRequest::Action::LIST);
    }

    CHECK_DECLARE_G8(res, req, c8::g8Client);

    if (req.reader().getAction() == G8ClientRequest::Action::SWITCH) {
      for (auto client : res.reader().getClient()) {
        if (client.getActive()) {
          C8Log("Switched to client \033[32m%s\033[0m", req.reader().getClient()[0].cStr());
        }
      }
    } else if (req.reader().getAction() == G8ClientRequest::Action::DELETE) {
      // We assume here that if g8Client DELETE action didn't return an error that it deleted all
      // the requested clients.
      for (auto client : req.reader().getClient()) {
        C8Log("Deleted client \033[31m%s\033[0m", client.cStr());
      }
      // TODO(mc): Add 'Switched to client X'.
    } else {
      for (auto client : res.reader().getClient()) {
        String staleMarker = "";
        if (fetchCloudInfo) {
          if (client.getLastSaveTimeSeconds() != client.getLastCloudSaveTimeSeconds()) {
            staleMarker = "\033[31mS \033\[0m";
          } else {
            staleMarker = "  ";
          }
        }
        if (client.getActive()) {
          C8Log("%s* \033[32m%s\033[0m", staleMarker.c_str(), client.getName().cStr());
        } else {
          C8Log("%s  %s", staleMarker.c_str(), client.getName().cStr());
        }
      }
    }
  });

  return cmd;
}

// Save any work in the client.
//
// Example usage:
//   g8 save
CLI::App_p saveCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Save work", "save");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8ClientRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setAction(G8ClientRequest::Action::SAVE);
    req.builder().setForceSave(true);  // Always push even if locally there is no diff.
    setCredsForCwdRepo(req.builder().getAuth());

    // req.builder().setUser(getenv("USER"));

    // First create the client.
    CHECK_DECLARE_G8(res, req, c8::g8Client);

    C8Log("Saved client \033[32m%s\033[0m", res.reader().getClient()[0].getName().cStr());
  });

  return cmd;
}

// List all files and statuses.
//
// Example usage:
//   g8 status
CLI::App_p statusCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("List files and statuses", "status");

  static Vector<String> clientNames;
  cmd->add_option("names", clientNames, "Client names");

  static bool showAll = false;
  cmd->add_flag("-a,--all", showAll, "Show all clients");

  cmd->callback([&] {
    ScopeTimer t("g8");

    if (clientNames.empty()) {
      if (showAll) {
        MutableRootMessage<G8ClientRequest> req;
        req.builder().setRepo(repoPath());
        req.builder().setAction(G8ClientRequest::Action::LIST);
        req.builder().setUser(getenv("USER"));
        setCredsForCwdRepo(req.builder().getAuth());

        auto res = g8Client(req.reader());
        verboseCheckResponse(req, res, ctx);

        for (auto client : res.reader().getClient()) {
          clientNames.push_back(client.getName().cStr());
        }
      } else {
        clientNames.push_back("HEAD");
      }
    } else if (showAll) {
      C8Log("Cannot specify client names with --all");
      exit(1);
    }

    for (int i = 0; i < clientNames.size(); i++) {
      auto clientName = clientNames[i];

      auto res = listChangesets(ctx, true, clientName);

      if (clientNames.size() > 1) {
        if (i > 0) {
          C8Log("\n");
        }
        C8Log("\033[34mChanges in %s:\033[0m\n", clientName.c_str());
      }

      for (auto changeset : res.reader().getChangeset()) {
        if (changeset.getId() == "working") {
          if (changeset.getFileList().getInfo().size() == 0) {
            continue;
          }
          C8Log("\033[32mWorking changes\033[0m");
        } else {
          C8Log(
            "\033[33mChangeset %s \033[0m %s",
            changeset.getId().cStr(),
            changeset.getSummary().cStr());
          if (changeset.getFileList().getInfo().size() == 0) {
            C8Log("   \033[90m(empty)\033[0m");
          }
        }
        for (auto info : changeset.getFileList().getInfo()) {
          char dirty = info.getDirty() ? '*' : ' ';
          if (
            info.getStatus() == G8FileInfo::Status::COPIED
            || info.getStatus() == G8FileInfo::Status::RENAMED) {
            C8Log(
              " %s%c %s --> %s",
              getStatusChar(info.getStatus()),
              dirty,
              info.getPreviousPath().cStr(),
              info.getPath().cStr());
          } else {
            C8Log(" %s%c %s", getStatusChar(info.getStatus()), dirty, info.getPath().cStr());
          }
        }
      }
    }
  });

  return cmd;
}
// Request reviewers for the change
//
// Example usage:
//  # create a new change set, make a non-draft PR and request vinnyog as a reviewer
//  g8 send -m "added changed" -t vinnyog
//  # remove draft mark from specified changeset and request reviewers
//  g8 send 453123 -t vinnyog,erikmchut
//  g8 send 453123 -g usergroup1
CLI::App_p sendCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Send a changeset for review", "send");

  static Vector<String> reviewers;
  cmd->add_option("-t", reviewers, "Reviewers to notify")->delimiter(',');

  static Vector<String> reviewersGroups;
  cmd
    ->add_option(
      "-g",
      reviewersGroups,
      "Group of reviewers to notify defined in .git/info/user_groups.txt. Example format: "
      "group1=daleross,user2")
    ->delimiter(',');
  static std::map<std::string, std::vector<std::string>> userGroups;

  static String description;
  cmd->add_option("-m", description, "Changeset description");

  static String changesetId;
  cmd->add_option("id", changesetId, "changeset ID");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8ChangesetRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setClient("HEAD");
    req.builder().setIncludeSummary(true);
    req.builder().setIncludeDescription(true);
    req.builder().setIncludeBody(true);
    req.builder().setIncludeFileInfo(true);
    req.builder().setIncludeRemoteBranch(true);
    req.builder().setIncludeWorkingChanges(true);
    setCredsForCwdRepo(req.builder().getAuth());

    req.builder().setAction(G8ChangesetRequest::Action::LIST);

    CHECK_DECLARE_G8(res, req, c8::g8Changeset);

    auto changesets = res.reader().getChangeset();
    auto haveWorking{false};
    for (auto changeset : changesets) {
      if (changeset.getId() == "working") {
        if (changeset.getFileList().getInfo().size() != 0) {
          haveWorking = true;
          break;
        }
      }
    }

    req.builder().setIncludeWorkingChanges(false);
    CHECK_G8(res, req, c8::g8Changeset);
    changesets = res.reader().getChangeset();

    req.builder().setAction(G8ChangesetRequest::Action::UPDATE);

    if (!changesetId.empty()) {
      req.builder().initId(1);
      req.builder().getId().set(0, changesetId);
    } else if (haveWorking) {
      if (description.empty()) {
        C8Log("Missing description for new changeset (-m)");
        exit(1);
      }
      req.builder().setAction(G8ChangesetRequest::Action::CREATE);
    } else if (changesets.size() != 1) {
      C8Log(
        "ERROR: must provide a changeset ID to send when there are no working files and multiple "
        "changesets");
      exit(1);
    }

    if (!description.empty()) {
      req.builder().setDescription(description);
    }
    CHECK_G8(res, req, c8::g8Changeset);

    auto changeset = res.reader().getChangeset()[0];
    C8Log(
      "%s changeset \033[33m%s\033[0m",
      haveWorking ? "Created" : "Updated",
      changeset.getId().cStr());
    for (auto info : changeset.getFileList().getInfo()) {
      if (
        info.getStatus() == G8FileInfo::Status::COPIED
        || info.getStatus() == G8FileInfo::Status::RENAMED) {
        C8Log(
          "  %s %s --> %s",
          getStatusChar(info.getStatus()),
          info.getPreviousPath().cStr(),
          info.getPath().cStr());
      } else {
        C8Log("  %s %s", getStatusChar(info.getStatus()), info.getPath().cStr());
      }
    }

    if (!reviewersGroups.empty()) {
      bool configParsed = parseUserConfig(userGroups);
      if (configParsed) {
        for (const String &reviewersGroup : reviewersGroups) {
          if (userGroups.contains(reviewersGroup)) {
            reviewers.insert(
              reviewers.end(),
              userGroups[reviewersGroup].begin(),
              userGroups[reviewersGroup].end());
          } else {
            C8Log("ERROR: could not find group %s in reviewers config", reviewersGroup.c_str());
          }
        }
      } else {
        C8Log("ERROR: could not parse reviewers config");
      }
    }

    auto gitService = getGitService(ctx);
    String head = changeset.getRemoteBranch().cStr();

    auto pr = gitService->pullRequestByBranch(head);
    if (!pr) {
      CreatePullRequestArgs args;
      args.headRef = head;
      args.title = changeset.getSummary().cStr();
      args.body = changeset.getBody().cStr();
      args.draft = false;
      auto success = gitService->createPullRequest(args);

      if (!success) {
        C8Log("ERROR: failure in making Github PR request");
        exit(1);
      }

      auto pr = gitService->pullRequestByBranch(head);
      if (!pr) {
        C8Log("ERROR: could not get PR number of existing PR for %s", head.c_str());
        exit(1);
      }
    } else {
      DraftStatusRequest draftRequest;
      draftRequest.pullRequestId = pr->pullRequestId;
      draftRequest.shouldBeDraft = false;
      if (!gitService->setPullRequestDraftStatus(draftRequest)) {
        C8Log("Failed to mark PR ready for review.");
        exit(1);
      }
      if (!description.empty()) {
        TitleRequest titleRequest;
        titleRequest.pullRequestId = pr->pullRequestId;
        titleRequest.message = description;
        gitService->updateTitle(titleRequest);
      }
    }

    ReviewersRequest reviewersRequest;
    reviewersRequest.pullRequestId = pr->pullRequestId;
    reviewersRequest.reviewers = reviewers;
    gitService->addReviewers(reviewersRequest);
  });

  return cmd;
}

// Land changes
//
// Example usage:
//  g8 land
//  g8 land 123876
CLI::App_p landCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Land a changeset", "land");

  static String changesetId;
  cmd->add_option("id", changesetId, "changeset ID");

  static bool withCi = false;
  cmd->add_flag("--with-ci", withCi, "with CI");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8ChangesetRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setClient("HEAD");
    setCredsForCwdRepo(req.builder().getAuth());
    req.builder().setIncludeCommitId(true);
    req.builder().setIncludeSummary(true);
    req.builder().setIncludeDescription(true);
    req.builder().setIncludeBody(true);
    req.builder().setIncludeWorkingChanges(true);
    req.builder().setIncludeFileInfo(true);
    req.builder().setIncludeRemoteBranch(true);

    req.builder().setAction(G8ChangesetRequest::Action::LIST);

    CHECK_DECLARE_G8(res, req, c8::g8Changeset);

    auto changesets = res.reader().getChangeset();
    auto haveWorking{false};
    for (auto changeset : changesets) {
      if (changeset.getId() == "working") {
        if (changeset.getFileList().getInfo().size() != 0) {
          haveWorking = true;
          break;
        }
      }
    }

    bool newChangesetCreated{false};
    if (res.reader().getChangeset().size() == 1) {
      if (!haveWorking) {
        C8Log("No changesets and no working files found. Please make changes\n");
        exit(1);
      }
      prompt("No changeset exists for changes, create one now? (Y/n): ");
      String reply;
      std::getline(std::cin, reply);
      if (std::tolower(reply[0]) == 'n') {
        exit(0);
      }
      newchange(ctx, "");
      CHECK_G8(res, req, c8::g8Changeset);
      newChangesetCreated = true;

      haveWorking = false;
      changesets = res.reader().getChangeset();
      for (auto changeset : changesets) {
        if (changeset.getId() == "working") {
          if (changeset.getFileList().getInfo().size() != 0) {
            haveWorking = true;
            break;
          }
        }
      }
    }

    if (changesetId.empty() && res.reader().getChangeset().size() == 2) {
      if (haveWorking) {
        auto cs = res.reader().getChangeset();
        changesetId = cs[0].getId() != "working" ? cs[0].getId() : cs[1].getId();
        prompt(strCat(
                 "You still have working changes that are not included in changeset ",
                 changesetId,
                 ". Do you wish to land it anyways? (y/N)")
                 .c_str());
        String reply;
        std::getline(std::cin, reply);
        if (std::tolower(reply[0] != 'y')) {
          exit(0);
        }
      }
    }

    if (!newChangesetCreated && (changesetId.empty() && res.reader().getChangeset().size() > 2)) {
      C8Log("More than one changeset found, please specify changesetId");
      exit(1);
    }

    if (!newChangesetCreated && haveWorking && changesetId.empty()) {
      C8Log("Working changes found. Please save them to a new changeset or specify a changeset ID");
      exit(1);
    }

    if (changesetId.empty()) {
      auto cs = res.reader().getChangeset();
      changesetId = cs[0].getId() != "working" ? cs[0].getId() : cs[1].getId();
    }

    req.builder().initId(1);
    req.builder().getId().set(0, changesetId);
    req.builder().setIncludeWorkingChanges(false);
    req.builder().setIncludeFileInfo(false);

    CHECK_G8(res, req, c8::g8Changeset);

    auto changeset = res.reader().getChangeset()[0];

    String head = changeset.getRemoteBranch().cStr();
    auto gitService = getGitService(ctx);

    // TODO(pawel) Should return pr id as a string.
    auto pr = gitService->pullRequestByBranch(head);
    if (!pr) {
      C8Log("PR/MR does not exist, use g8 update/send to create it");
      exit(1);
    }

    if (pr->isDraft) {
      DraftStatusRequest draftRequest;
      draftRequest.pullRequestId = pr->pullRequestId;
      draftRequest.shouldBeDraft = false;
      gitService->setPullRequestDraftStatus(draftRequest);
    }

    LandRequestArgs args;
    args.pullRequestId = pr->pullRequestId;
    args.expectedTipSha = changeset.getCommitId().cStr();
    args.commitTitle = changeset.getSummary().cStr();
    args.commitMessage = changeset.getBody().cStr();

    args.withCi = withCi;

    auto remoteMerge = gitService->landPullRequest(args);

    if (!remoteMerge) {
      C8Log("Error landing remotely.");
      exit(1);
    }

    if (withCi) {
      C8Log("Queued changeset for land: %s", changeset.getId().cStr());
    } else {
      if (ctx.verbose) {
        C8Log(
          "Squashed and merged Github PR. Merged commit SHA is %s\n",
          remoteMerge->landedMasterSha.c_str());
      }

      req.builder().setAction(G8ChangesetRequest::Action::DELETE);
      CHECK_DECLARE_G8(deleteChangesetRes, req, c8::g8Changeset);

      sync(ctx, {}, remoteMerge->landedMasterSha);

      C8Log("Landed Changeset %s", changeset.getId().cStr());
    }
  });

  return cmd;
}

void move(const MainContext &ctx, String changesetId, Vector<String> &files) {
  ScopeTimer t("move");

  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setClient("HEAD");
  req.builder().setIncludeFileInfo(true);
  req.builder().setIncludeWorkingChanges(true);

  req.builder().setAction(G8ChangesetRequest::Action::LIST);
  auto res = g8Changeset(req.reader());
  verboseCheckResponse(req, res, ctx);

  if (changesetId.empty()) {
    // NOTE(christoph): There is always a "working" changeset, but that doesn't count here.
    auto countExcludingWorking = res.reader().getChangeset().size() - 1;
    if (countExcludingWorking < 0) {
      C8Log("Unexpected changeset state");
      exit(1);
    }
    if (countExcludingWorking == 0) {
      C8Log("There is no available changeset");
      exit(1);
    }
    if (countExcludingWorking > 1) {
      C8Log("There is more than one changeset, please specify one");
      exit(1);
    }
    auto cs = res.reader().getChangeset();
    changesetId = cs[0].getId() != "working" ? cs[0].getId() : cs[1].getId();
  }

  bool foundId{false};
  for (auto changeset : res.reader().getChangeset()) {
    if (changeset.getId() == changesetId) {
      foundId = true;
    }
  }

  if (!foundId && changesetId != "working") {
    C8Log("ERROR: Changset %s does not exist", changesetId.c_str());
    exit(1);
  }

  MutableRootMessage<G8ChangesetRequest> changeReq;
  changeReq.builder().setRepo(repoPath());
  changeReq.builder().setClient("HEAD");
  changeReq.builder().setAction(G8ChangesetRequest::Action::UPDATE);
  changeReq.builder().initId(1);
  setCredsForCwdRepo(changeReq.builder().getAuth());

  // find where file currectly exists
  auto changesets = res.reader().getChangeset();
  for (auto changeset : changesets) {

    bool found{false};
    Vector<String> filesToRemove;

    for (auto f : changeset.getFileList().getInfo()) {
      std::for_each(begin(files), end(files), [&](auto file) {
        if (file.compare(f.getPath().cStr()) == 0) {
          filesToRemove.emplace_back(f.getPath().cStr());
          found = true;
        }
      });
    }

    if (!found) {
      continue;
    }

    changeReq.builder().setAction(G8ChangesetRequest::Action::UPDATE);
    changeReq.builder().initFileUpdate(filesToRemove.size());
    for (int i = 0; i < filesToRemove.size(); i++) {
      changeReq.builder().getFileUpdate().set(i, filesToRemove[i]);
    }

    if (String("working").compare(changeset.getId().cStr()) != 0) {
      changeReq.builder().getId().set(0, changeset.getId());
      changeReq.builder().setFileUpdateAction(G8ChangesetRequest::FileUpdateAction::REMOVE);

      auto changeRes = g8Changeset(changeReq.reader());
      verboseCheckResponse(changeReq, changeRes, ctx);
    }

    if (changesetId != "working") {
      changeReq.builder().getId().set(0, changesetId);
      changeReq.builder().setFileUpdateAction(G8ChangesetRequest::FileUpdateAction::INSERT);

      auto changeRes = g8Changeset(changeReq.reader());
      verboseCheckResponse(changeReq, changeRes, ctx);
    }
  }
}

void updateChangesetFiles(const MainContext &ctx, String changesetId, Vector<String> &files) {
  ScopeTimer t("updateChangesetFiles");

  MutableRootMessage<G8ChangesetRequest> changeReq;
  changeReq.builder().setRepo(repoPath());
  changeReq.builder().setClient("HEAD");
  changeReq.builder().setAction(G8ChangesetRequest::Action::UPDATE);
  setCredsForCwdRepo(changeReq.builder().getAuth());

  changeReq.builder().initId(1);
  changeReq.builder().getId().set(0, changesetId);

  changeReq.builder().setFileUpdateAction(G8ChangesetRequest::FileUpdateAction::REPLACE);
  changeReq.builder().initFileUpdate(files.size());
  for (int i = 0; i < files.size(); ++i) {
    changeReq.builder().getFileUpdate().set(i, files[i]);
  }

  auto changeRes = g8Changeset(changeReq.reader());
  verboseCheckResponse(changeReq, changeRes, ctx);
}

// Move files in and out of and between changesets.
//
// Example usage:
//   g8 files
CLI::App_p filesCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Move files in and out of changesets", "files");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8ChangesetRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setClient("HEAD");
    req.builder().setIncludeSummary(true);
    req.builder().setIncludeDescription(true);
    req.builder().setIncludeFileInfo(true);
    req.builder().setIncludeWorkingChanges(true);
    setCredsForCwdRepo(req.builder().getAuth());

    // First, get information about all of the files and changesets.
    req.builder().setAction(G8ChangesetRequest::Action::LIST);
    auto res = g8Changeset(req.reader());
    verboseCheckResponse(req, res, ctx);

    // Next, create lists of unique files, unique changesets, a map from changeset to files, and a
    // text buffer that will be presented to the user for editing.
    Vector<String> editorLines;
    TreeSet<String> openFiles;
    TreeMap<String, TreeSet<String>> preEdit;
    for (auto changeset : res.reader().getChangeset()) {
      if (changeset.getId() == "working") {
        if (changeset.getFileList().getInfo().size() == 0) {
          continue;
        }
        editorLines.emplace_back("# Changeset [working]");
      } else {
        editorLines.emplace_back(
          format("# Changeset [%s]  %s", changeset.getId().cStr(), changeset.getSummary().cStr()));
        preEdit[changeset.getId()];  // Insert empty set.
        if (changeset.getFileList().getInfo().size() == 0) {
          editorLines.emplace_back("");
        }
      }
      for (auto info : changeset.getFileList().getInfo()) {
        editorLines.emplace_back(info.getPath().cStr());
        openFiles.insert(info.getPath().cStr());
        if (changeset.getId() != "working") {
          preEdit[changeset.getId()].insert(info.getPath().cStr());
        }
      }
    }
    // Add empty line for trailing newline.
    editorLines.emplace_back("");

    // Launch the interactive editor.
    auto stream = launchTemporaryEditor(strJoin(editorLines, "\n"));

    const std::regex stripCommentRegex(R"(([^#]+).*)");
    const std::regex csRegex(R"(\s*#\s*Changeset \[(\w+)\].*)");

    // Parse the returned result from the client, with the logic that any filename that was
    // present in the original buffer is moved to the changeset whose comment it directly follows.
    String line;
    TreeMap<String, TreeSet<String>> postEdit;
    TreeSet<String> newOpenFiles;
    String marker = "working";
    bool changesetIsValid = false;
    while (std::getline(stream, line)) {
      std::smatch match;
      if (std::regex_match(line, match, csRegex)) {
        // This is a changeset marker comment. Update the changeset, assuming it was in the
        // original set.
        marker = match[1].str();
        changesetIsValid = (preEdit.find(marker) != preEdit.end());
        if (changesetIsValid) {
          postEdit[marker];  // Insert an empty set.
        }
      } else if (changesetIsValid && std::regex_match(line, match, stripCommentRegex)) {
        const auto &fileName = match[1];
        const bool fileIsOpen = (openFiles.find(fileName) != openFiles.end());
        if (fileIsOpen) {
          // Check for duplicates, allowing a file to move only once.
          const bool isDuplicate = (newOpenFiles.find(fileName) != newOpenFiles.end());
          if (!isDuplicate) {
            // Insert the file into the postEdit changeset->file mapping.
            newOpenFiles.insert(fileName);
            postEdit[marker].insert(fileName);
          }
        }
      }
    }

    // Update all changesets that have file changes.
    for (const auto &[cs, paths] : preEdit) {
      const auto &postPaths = postEdit[cs];
      if (paths != postPaths) {
        Vector<String> updatedFiles(postPaths.cbegin(), postPaths.cend());
        C8Log("Updating changeset %s", cs.c_str());
        updateChangesetFiles(ctx, cs, updatedFiles);
      }
    }
  });

  return cmd;
}

// Add files to changeset (removes from previous)
//
// Example Usage:
//  g8 add -c 695587 file1 file2...
//  g8 add file4
//
CLI::App_p addCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Add files to changeset", "add");

  static String changesetId;
  cmd->add_option("-c", changesetId, "changeset");

  static Vector<String> files;
  cmd->add_option("files", files, "files");

  cmd->callback([&] {
    ScopeTimer t("g8");
    move(ctx, changesetId, files);
  });

  return cmd;
}

// Takes files out of changesets.
//
// Example Usage:
//  g8 unadd file234
CLI::App_p unaddCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_shared<CLI::App>("Remove files from changeset", "unadd");

  static Vector<String> files;
  cmd->add_option("files", files, "files");

  cmd->callback([&] {
    ScopeTimer t("g8");
    move(ctx, "working", files);
  });

  return cmd;
}

void log(int depth, const MainContext &ctx) {
  FILE *lessProcess = popen("less -RFX", "w");
  {
    ScopeTimer t("log");
    MutableRootMessage<G8RepositoryRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setLogDepth(depth);
    req.builder().setAction(G8RepositoryRequest::Action::GET_COMMIT_LOG);

    auto res = g8Repository(req.reader());
    verboseCheckResponse(req, res, ctx);

    auto now = std::chrono::system_clock::now();
    using Minutes = std::chrono::minutes;
    using Hours = std::chrono::hours;
    using Days = std::chrono::duration<int, std::ratio<86400>>;
    using Weeks = std::chrono::duration<int, std::ratio<604800>>;
    using Months = std::chrono::duration<int, std::ratio<2629746>>;
    using Years = std::chrono::duration<int, std::ratio<31556952>>;

    for (auto entry : res.reader().getCommitLog()) {
      auto commitTime =
        std::chrono::system_clock::time_point(std::chrono::seconds(entry.getSignature().getWhen()));
      auto timeSince = now - commitTime;
      String timeMessage;
      if (timeSince < Minutes(1)) {
        timeMessage = "just now";
      } else {
        int value = 0;
        String unit;
        if (timeSince <= Minutes(90)) {
          unit = "minute";
          value = std::chrono::duration_cast<Minutes>(timeSince).count();
        } else if (timeSince <= Hours(36)) {
          unit = "hour";
          value = std::chrono::duration_cast<Hours>(timeSince).count();
        } else if (timeSince <= Days(10)) {
          unit = "day";
          value = std::chrono::duration_cast<Days>(timeSince).count();
        } else if (timeSince <= Weeks(6)) {
          unit = "week";
          value = std::chrono::duration_cast<Weeks>(timeSince).count();
        } else if (timeSince <= Months(18)) {
          unit = "month";
          value = std::chrono::duration_cast<Months>(timeSince).count();
        } else {
          unit = "year";
          value = std::chrono::duration_cast<Years>(timeSince).count();
        }
        timeMessage = strCat(value, " ", unit, value != 1 ? "s" : "", " ago");
      }

      fprintf(
        lessProcess,
        strCat(
          "* ", YELLOW, "%.7s", CLEAR, " %s ", MAGENTA, "<%s>", DIM, GREEN, " (%s)", CLEAR, "\n")
          .c_str(),
        entry.getId().cStr(),
        entry.getSummary().cStr(),
        entry.getSignature().getName().cStr(),
        timeMessage.c_str());
    }
  }
  pclose(lessProcess);
}

String colorForLineStatus(G8DiffLine::Origin type) {
  switch (type) {
    case G8DiffLine::Origin::ADDITION:
      return strCat(GREEN, "+");
    case G8DiffLine::Origin::DELETION:
      return strCat(RED, "-");
    default:
      return " ";
  }
}

void diff(
  const MainContext &ctx, String basePoint, String changePoint, const Vector<String> &files) {
  FILE *lessProcess = popen(resolveDiffTool().c_str(), "w");
  {
    ScopeTimer t("diff");
    MutableRootMessage<G8DiffRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setBasePoint(basePoint);
    req.builder().setChangePoint(changePoint);
    req.builder().setFindRenamesAndCopies(true);
    if (!files.empty()) {
      req.builder().initPaths(files.size());
      for (int i = 0; i < files.size(); i++) {
        req.builder().getPaths().set(i, files[i].c_str());
      }
    }

    auto res = g8Diff(req.reader());
    verboseCheckResponse(req, res, ctx);

    for (auto diff : res.reader().getDiffList()) {
      auto status = diff.getInfo().getStatus();

      String path =
        (status == G8FileInfo::Status::DELETED) ? "/dev/null" : diff.getInfo().getPath().cStr();
      String previousPath = (status == G8FileInfo::Status::ADDED)
        ? "/dev/null"
        : (
          (diff.getInfo().getPreviousPath().size() == 0) ? diff.getInfo().getPath().cStr()
                                                         : diff.getInfo().getPreviousPath().cStr());
      // Write the filenames and blob ids.
      fputs(
        strCat(
          RED,
          "--- ",
          previousPath,
          "\t",

          diff.getInfo().getOldBlobId().cStr(),
          "\n",
          GREEN,
          "+++ ",
          path,
          "\t",
          diff.getInfo().getBlobId().cStr(),
          CLEAR,
          "\n")
          .c_str(),
        lessProcess);

      // A Hunk is defined as DiffLineNo, OldLineNo, OldLineSize, NewLineNo, NewLineSize.
      std::queue<std::tuple<int, int, int, int, int>> hunks;

      int diffLine = 0;
      int hunkDiffLine = 1;
      auto currentLine = std::pair(-1, -1);
      auto hunkSize = std::pair(0, 0);
      auto hunkStart = std::pair(0, 0);
      for (const auto line : diff.getLines()) {
        ++diffLine;
        auto nextLine = std::pair(
          std::max(currentLine.first, line.getBaseLineNumber()),
          std::max(currentLine.second, line.getNewLineNumber()));

        if (std::pair(currentLine.first + 1, currentLine.second + 1) < nextLine) {
          // This nextList starts a new diff hunk.
          if (currentLine != std::pair(-1, -1)) {
            // Append the previous diff hunk.
            hunks.emplace(
              hunkDiffLine, hunkStart.first, hunkSize.first, hunkStart.second, hunkSize.second);
          }
          hunkStart = nextLine;
          hunkSize = std::pair(0, 0);
          hunkDiffLine = diffLine;
        } else {
          // This next line is part of the current hunk.
        }

        if (line.getBaseLineNumber() > 0) {
          ++hunkSize.first;
        }
        if (line.getNewLineNumber() > 0) {
          ++hunkSize.second;
        }
        currentLine = nextLine;
      }
      if (currentLine != std::pair(-1, -1)) {
        // Write the final hunk.
        hunks.emplace(
          hunkDiffLine, hunkStart.first, hunkSize.first, hunkStart.second, hunkSize.second);
      }

      int hunkLine = 0;
      auto getSizeString = [](int size) -> String { return size == 1 ? "" : strCat(",", size); };
      for (const auto line : diff.getLines()) {
        if (!hunks.empty()) {
          auto [diffLine, baseStart, baseSize, newStart, newSize] = hunks.front();
          if ((++hunkLine) == diffLine) {
            // Write the unified diff hunk header.
            fprintf(
              lessProcess,
              strCat(
                CYAN,
                "@@ -%d",
                getSizeString(baseSize),
                " +%d",
                getSizeString(newSize),
                " @@",
                CLEAR,
                "\n")
                .c_str(),
              baseStart,
              newStart);
            hunks.pop();
          }
        }

        // We want to clear the color on the line so that tools like grep will process
        // the clear character as well. Since in the fprintf we always output a newline
        // this also means that the shell prompt will always start on the new line.
        String lineToPrint = line.getContent();
        if (lineToPrint.back() == '\n') {
          lineToPrint.pop_back();
        }

        // Write the diff line.
        fprintf(
          lessProcess,
          "%s%s\033[0m\n",
          colorForLineStatus(line.getOrigin()).c_str(),
          lineToPrint.c_str());
      }
    }
  }
  pclose(lessProcess);
}

// Generate diff between two change points
// Example Usage:
//  g8 diff master client1
//  g8 diff master WORKING
//  g8 diff HEAD WORKING
CLI::App_p diffCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_shared<CLI::App>("Diff between changes", "diff");

  static String basePoint = "FORK";
  static String changePoint = "WORKING";
  static Vector<String> files;

  cmd->add_option(
    "--from",
    basePoint,
    "partial commit ID, branch/client names, changeset IDs, EMPTY, WORKING. default: FORK");

  cmd->add_option(
    "--to",
    changePoint,
    "partial commit ID, branch/client names, changeset IDs, EMPTY, FORK. default: WORKING");

  cmd->add_option(
    "files",
    files,
    "default diffs working changes against forkpoint. two arguments diff change points (like "
    "branches, changesets, etc), single argument diffs that file against fork point. three "
    "arguments diffs the file between two change points");

  cmd->callback([&] {
    ScopeTimer t("g8");
    diff(ctx, basePoint, changePoint, files);
  });

  return cmd;
}

CLI::App_p logCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Commit history for repo", "log");

  static int numCommits = 255;
  cmd->add_option("-n", numCommits, "The number of commits to output.");

  cmd->callback([&] {
    ScopeTimer t("g8");
    log(numCommits, ctx);
  });

  return cmd;
}

void revert(
  const MainContext &ctx,
  Vector<String> &paths,
  bool dryRun,
  String &revertPoint,
  bool preserveByDefault) {
  ScopeTimer t("revert");
  MutableRootMessage<G8FileRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setClient("HEAD");

  req.builder().initPaths(paths.size());
  for (int i = 0; i < paths.size(); i++) {
    req.builder().getPaths().set(i, paths[i]);
  }

  if (revertPoint.size() > 0) {
    req.builder().setRevertPoint(revertPoint);
  }

  req.builder().setAction(G8FileRequest::Action::REVERT);

  if (paths.empty()) {
    req.builder().setDryRun(true);
    auto res = g8FileRequest(req.reader());
    verboseCheckResponse(req, res, ctx);

    // Open temp editor and dump files names into it.
    Vector<String> filenames;
    for (const auto file : res.reader().getFiles().getInfo()) {
      filenames.emplace_back(file.getPath().cStr());
    }

    auto REVERT_LINE = strCat(
      "# Files in this section are reverted.",
      preserveByDefault ? "" : " Delete or move lines into the preserve section to retain.");
    auto KEEP_LINE = strCat(
      "# Files in this section are preserved.",
      preserveByDefault ? " Move lines into the revert section to revert." : "");

    String editorLines;

    if (preserveByDefault) {
      editorLines = strCat(REVERT_LINE, "\n\n\n", KEEP_LINE, "\n\n", strJoin(filenames, "\n"));
    } else {
      editorLines =
        strCat(REVERT_LINE, "\n\n", strJoin(filenames, "\n"), "\n\n", KEEP_LINE, "\n\n\n");
    }

    auto stream = launchTemporaryEditor(editorLines);

    // Rebuild the request with just the specified files.
    String line;
    Vector<String> filesToRevert;
    bool sawRevertLine = false;

    while (std::getline(stream, line)) {
      if (line.empty()) {
        continue;
      } else if (sawRevertLine) {
        if (line.compare(KEEP_LINE) == 0) {
          break;
        } else {
          filesToRevert.emplace_back(line);
        }
      } else {
        if (line.compare(REVERT_LINE) == 0) {
          sawRevertLine = true;
        } else {
          C8Log("ERROR: Encountered unexpected line in revert choices");
          throw CLI::RuntimeError(1);
        }
      }
    }

    if (filesToRevert.empty()) {
      return;
    }

    req.builder().initPaths(filesToRevert.size());
    for (int i = 0; i < filesToRevert.size(); i++) {
      req.builder().getPaths().set(i, filesToRevert[i]);
    }

    res = g8FileRequest(req.reader());
    verboseCheckResponse(req, res, ctx);

    // Print the files that will be reverted with a prominent visual treatment.
    C8Log(
      "%s",
      strCat(
        "\n",
        GREEN,
        "----------------------------------------------------------------------------\n>",
        CLEAR)
        .c_str());
    for (const auto file : res.reader().getFiles().getInfo()) {
      C8Log("%s", strCat(GREEN, ">  ", YELLOW, file.getPath().cStr(), CLEAR).c_str());
    }
    C8Log(
      "%s",
      strCat(
        GREEN,
        ">\n----------------------------------------------------------------------------\n",
        CLEAR)
        .c_str());

    if (dryRun) {
      return;
    }

    prompt(
      "Revert all of the above files in client \033[32m%s\033[0m (y/N)? ",
      res.reader().getClient().cStr());

    String action;
    if (getline(std::cin, action); std::tolower(action[0]) != 'y') {
      return;
    }
  }

  req.builder().setDryRun(false);
  auto res = g8FileRequest(req.reader());
  verboseCheckResponse(req, res, ctx);
}

CLI::App_p revertCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_shared<CLI::App>("Revert files.", "revert");

  static bool dryRun;
  cmd->add_flag("--dry-run", dryRun, "dry run");

  static bool preserveByDefault;
  cmd->add_flag("-p", preserveByDefault, "preserve changes by default");

  static Vector<String> paths;
  cmd->add_option("files", paths, "files");

  static String revertPoint;
  cmd->add_option("-c", revertPoint, "revert point");

  cmd->callback([&] {
    ScopeTimer t("g8");
    revert(ctx, paths, dryRun, revertPoint, preserveByDefault);
  });

  return cmd;
}

CLI::App_p fetchRemoteClientsCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Fetch new clients", "fetch-new-clients");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8RepositoryRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setUser(getenv("USER"));
    req.builder().setAction(G8RepositoryRequest::Action::FETCH_NEW_CLIENTS);
    setCredsForCwdRepo(req.builder().getAuth());

    CHECK_DECLARE_G8(res, req, c8::g8Repository);
  });

  return cmd;
}

CLI::App_p initialCommitCmd(MainContext &ctx) {
  CLI::App_p cmd =
    std::make_unique<CLI::App>("Create the initial commit on a new repo", "initial-commit");

  static String message;
  cmd->add_option("-m", message, "message");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8RepositoryRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setMessage(message);
    req.builder().setAction(G8RepositoryRequest::Action::INITIAL_COMMIT);
    setCredsForCwdRepo(req.builder().getAuth());

    CHECK_DECLARE_G8(res, req, c8::g8Repository);
  });

  return cmd;
}

const char *extractInfoValue(ConstRootMessage<G8RepositoryResponse> &res, const String &key) {
  if (key == "api") {
    switch (res.reader().getRemote().getApi()) {
      case G8GitRemote::Api::GITHUB:
        return "github";
      case G8GitRemote::Api::AWS_CODE_COMMIT:
        return "aws-codecommit";
      case G8GitRemote::Api::API8:
        return "8thwall";
      case G8GitRemote::Api::GITLAB:
        return "gitlab";
      default:
        return "unspecified";
    }
  } else if (key == "local") {
    return res.reader().getRepo().cStr();
  } else if (key == "host") {
    return res.reader().getRemote().getHost().cStr();
  } else if (key == "name") {
    return res.reader().getRemote().getRepoName().cStr();
  } else if (key == "main") {
    return res.reader().getMainBranchName().cStr();
  } else {
    return nullptr;
  }
}

void printInfoWithLabel(
  ConstRootMessage<G8RepositoryResponse> &res, const char *label, const char *key) {
  C8Log("%s: %s", label, extractInfoValue(res, key));
}

CLI::App_p infoCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Get information about the repository", "info");

  static String key;
  cmd->add_option("--key", key, "Print a single value from the data");

  cmd->callback([&] {
    ScopeTimer t("g8");
    MutableRootMessage<G8RepositoryRequest> req;
    req.builder().setRepo(repoPath());
    req.builder().setAction(G8RepositoryRequest::Action::GET_INFO);

    CHECK_DECLARE_G8(res, req, c8::g8Repository);
    if (key.size()) {
      auto value = extractInfoValue(res, key);
      if (value != nullptr) {
        C8Log("%s", value);
      } else {
        C8Log("Unexpected key: %s", key.c_str());
        exit(1);
      }
    } else {
      printInfoWithLabel(res, "local", "local");
      printInfoWithLabel(res, " host", "host");
      printInfoWithLabel(res, " name", "name");
      printInfoWithLabel(res, "  api", "api");
      printInfoWithLabel(res, " main", "main");
    }
  });

  return cmd;
}

void openPr(const MainContext &ctx, const String &id) {
  MutableRootMessage<G8ChangesetRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setClient("HEAD");
  req.builder().setIncludeRemoteBranch(true);
  auto res = g8Changeset(req.reader());
  verboseCheckResponse(res, res, ctx);

  auto gitService = getGitService(ctx);

  for (auto changeset : res.reader().getChangeset()) {
    if (id.empty() || id == changeset.getId().cStr()) {
      if (auto pr = gitService->pullRequestByBranch(changeset.getRemoteBranch()); pr) {
        system(strCat("open ", pr->url).c_str());
      }
    }
  }
}

CLI::App_p prCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Open PR in browser", "pr");

  static String changesetId;
  cmd->add_option("id", changesetId, "changeset id");

  cmd->callback([&] {
    ScopeTimer t("g8");
    openPr(ctx, changesetId);
  });
  return cmd;
}

CLI::App_p mrCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Open MR in browser", "mr");

  static String changesetId;
  cmd->add_option("id", changesetId, "changeset id");

  cmd->callback([&] {
    ScopeTimer t("g8");
    openPr(ctx, changesetId);
  });
  return cmd;
}

// TODO(pawel) Put back in the right place.
// }  // namespace
