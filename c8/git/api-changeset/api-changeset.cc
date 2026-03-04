// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//c8/git:internal",
  };
  deps = {
    "//c8:c8-log-proto",
    "//c8:c8-log",
    "//c8:scope-exit",
    "//c8:vector",
    "//c8/git:api-common",
    "//c8/git:g8-api.capnp-cc",
    "//c8/git:hooks",
    "//c8/io:capnp-messages",
    "//c8/stats:scope-timer",
    "//c8/string:format",
    "//c8/string:strcat",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0x5c1c0c29);

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
#include <unordered_set>
#include <vector>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/git/api-common.h"
#include "c8/git/hooks.h"
#include "c8/io/capnp-messages.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/format.h"
#include "c8/string/strcat.h"
#include "c8/vector.h"

namespace c8 {
using namespace g8;
using Action = G8ChangesetRequest::Action;
using FileAction = G8ChangesetRequest::FileUpdateAction;

// Previous versions of g8 had an alternate changeset structure that was incompatible with GitLab,
// so we need to fix it.
// One of the ways we can tell that previous g8 managed a changeset branch is by detecting whether
// a particular commit is parent to more than one commit on the changeset branch.
int fixPreviousBranch(git_commit **out, const git_commit *head, const git_reference *master) {
  auto repo = git_commit_owner(head);
  std::unordered_set<String> seenParents;

  bool branchNeedsFixing{false};

  GIT_PTR(commit, masterCommit);
  GIT_STATUS_RETURN(
    git_reference_peel(reinterpret_cast<git_object **>(&masterCommit), master, GIT_OBJECT_COMMIT));
  const git_oid *masterOid = git_commit_id(masterCommit);
  git_oid currentOid = *git_commit_id(head);
  git_oid latestForkOid{0};

  while (true) {
    git_oid mergeBaseOid;
    GIT_STATUS_RETURN(git_merge_base(&mergeBaseOid, repo, masterOid, &currentOid));

    // End condition is when we end up on the master line.
    if (git_oid_equal(&mergeBaseOid, &currentOid)) {
      break;
    }

    GIT_PTR(commit, current);
    GIT_STATUS_RETURN(git_commit_lookup(&current, repo, &currentOid));

    // Figure out what was the latest point on master we synced to.
    if (git_oid_is_zero(&latestForkOid) && git_commit_parentcount(current) > 1) {
      git_oid_cpy(&latestForkOid, git_commit_parent_id(current, 1));
    }

    for (auto p = 0; p < git_commit_parentcount(current); p++) {
      String parentOid = git_oid_tostr_s(git_commit_parent_id(current, p));
      if (seenParents.count(parentOid)) {
        branchNeedsFixing = true;
      }
      seenParents.insert(parentOid);
    }

    // The first parent follows the branch.
    currentOid = *git_commit_parent_id(current, 0);
  }

  if (branchNeedsFixing) {
    GIT_PTR(signature, signature);
    git_signature_default(&signature, repo);

    GIT_PTR(tree, headTree);
    GIT_STATUS_RETURN(git_commit_tree(&headTree, head));

    GIT_PTR(commit, forkCommit);
    GIT_STATUS_RETURN(git_commit_lookup(&forkCommit, repo, &latestForkOid));
    const git_commit *parents[] = {forkCommit};

    git_oid fixedOid;
    GIT_STATUS_RETURN(git_commit_create(
      &fixedOid,
      repo,
      nullptr,
      signature,
      signature,
      nullptr,
      git_commit_message(head),
      headTree,
      1,
      parents));

    GIT_STATUS_RETURN(git_commit_lookup(out, repo, &fixedOid));
  }

  return 0;
}

ConstRootMessage<G8ChangesetResponse> g8Changeset(G8ChangesetRequest::Reader req) {
  ScopeTimer t("api-changeset");
  MutableRootMessage<G8ChangesetRequest> rewrittenRequestMessage(req);
  req = rewrittenRequestMessage.reader();
  ChangesetContext ctx;
  auto &response = ctx.response;
  auto action = req.getAction();
  auto fileAction = req.getFileUpdateAction();

  if (action == Action::CREATE && req.getId().size() != 0) {
    RESPOND_ERROR("Don't specify an id for a Changeset CREATE action.");
  }

  if (action == Action::CREATE && req.getDescription().size() == 0) {
    RESPOND_ERROR("Must specify the changeset description.");
  }

  if (action == Action::DELETE && req.getId().size() == 0) {
    RESPOND_ERROR("Must specify Changeset id for a DELETE action.");
  }

  if (action == Action::UPDATE && fileAction != FileAction::NONE && req.getId().size() > 1) {
    RESPOND_ERROR("Only one id is allowed for this action.");
  }

  CHECK_ASSIGN(git_repository_open, ctx.repo, req.getRepo().cStr());
  CHECK_ASSIGN(lookupMainBranch, ctx.master, ctx.repo.get());
  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(ctx.repo.get()));

  {
    GIT_PTR(remote, remote);
    CHECK_GIT(git_remote_lookup(&remote, ctx.repo.get(), ORIGIN));
    setRemoteInfo(response.builder().getRemote(), remote);
  }

  if (action == Action::CREATE || action == Action::UPDATE) {
    ScopeTimer t1("g8-git-save");
    // There is no reason to push an empty save commit when creating/updating changesets so
    // don't force a save. As long as save commit has latest working changes we good.
    CHECK_GIT(g8GitSave(req.getAuth(), ctx.repo.get(), false));
  }

  bool clientIsHead = true;
  GIT_PTR(reference, client);
  if (req.getClient() == "HEAD" || req.getClient() == "head") {
    const char *clientFullName = nullptr;
    GIT_PTR(reference, head);
    CHECK_GIT(git_reference_lookup(&head, ctx.repo.get(), "HEAD"));

    clientFullName = git_reference_symbolic_target(head);
    if (!clientFullName) {
      RESPOND_ERROR("No active client in repository");
    }

    CHECK_GIT(git_reference_lookup(&client, ctx.repo.get(), clientFullName));
    rewrittenRequestMessage.builder().setClient(git_reference_shorthand(client));
  } else {
    CHECK_GIT(git_branch_lookup(&client, ctx.repo.get(), req.getClient().cStr(), GIT_BRANCH_LOCAL));
    const int isHeadRes = git_branch_is_head(client);
    if (isHeadRes < 0) {
      RESPOND_ERROR("Error checking if client is HEAD");
    }
    clientIsHead = isHeadRes == 1;
  }

  if (!clientNameIsValid(req.getClient().cStr())) {
    RESPOND_ERROR(format("Invalid client %s", req.getClient().cStr()));
  }

  response.builder().setClient(req.getClient());

  GIT_PTR(reference, changeset);
  GIT_PTR(commit, clientForkCommit);
  GIT_PTR(commit, clientCommit);

  CHECK_GIT(gitFindMergeBase(&clientForkCommit, ctx.repo.get(), ctx.master.get(), client));

  CHECK_GIT(
    git_reference_peel(reinterpret_cast<git_object **>(&clientCommit), client, GIT_OBJECT_COMMIT));

  const auto [lsGitRes, changesetList] = g8GitLsChangesets(ctx.repo.get(), req.getClient().cStr());
  CHECK_GIT(lsGitRes);

  if (req.getId().size() > 0 && action != Action::LIST) {
    for (const auto id : req.getId()) {
      if (id == WORKING_CHANGESET) {
        // LIST is  the only supported action for the default work tree.
        RESPOND_ERROR("The only supported action for the 'working' changeset is LIST");
      }
    }
  }

  if (req.getId().size() == 0) {
    if (req.getIncludeWorkingChanges()) {
      // TODO(pawel) Should this be -1?
      rewrittenRequestMessage.builder().initId(changesetList.changesets.size() + 1);
    } else {
      rewrittenRequestMessage.builder().initId(changesetList.changesets.size());
    }
    int idx = 0;
    for (const auto &changeset : changesetList.changesets) {
      rewrittenRequestMessage.builder().getId().set(idx++, changeset.name);
    }
    if (req.getIncludeWorkingChanges()) {
      rewrittenRequestMessage.builder().getId().set(idx++, WORKING_CHANGESET);
    }
  }

  if (action == Action::CREATE) {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(100000, 999999);
    rewrittenRequestMessage.builder().initId(1).set(0, std::to_string(dist(rd)));

    String changesetFullName =
      strCat(CHANGESETS_PREFIX, req.getClient().cStr(), "-", CHANGESET_CODE, req.getId()[0].cStr());

    CHECK_GIT(git_reference_create(
      &changeset,
      ctx.repo.get(),
      changesetFullName.c_str(),
      git_commit_id(clientForkCommit),
      0,
      strCat("Create ref for Changeset ", req.getId()[0].cStr()).c_str()));

    git_push_options pushOpts = GIT_PUSH_OPTIONS_INIT;
    auto authReader = req.getAuth();
    pushOpts.callbacks = gitRemoteCallbacks(authReader);
    pushOpts.callbacks.payload = reinterpret_cast<void *>(&authReader);

    GIT_PTR(reference, remoteRef);
    CHECK_GIT(git_branch_upstream(&remoteRef, client));

    String upstreamName =
      strCat(git_reference_shorthand(remoteRef), "-", CHANGESET_CODE, req.getId()[0].cStr());

    // NOTE(pawel) Manually create the remote tracking ref instead of calling git_remote_push()
    // to prevent a needless network operation. We push the branch after we add files to it below.
    // The refs/remotes prefix is by convention where git keeps its remote tracking branches.
    // We are making the (fairly) reasonable assumption here that this will always be the case.
    GIT_PTR(reference, remoteChangesetRef);
    CHECK_GIT(git_reference_create(
      &remoteChangesetRef,
      ctx.repo.get(),
      strCat("refs/remotes/", upstreamName.c_str()).c_str(),
      git_commit_id(clientForkCommit),
      true,  // force
      "Create remote tracking branch for changeset"));

    CHECK_GIT(git_branch_set_upstream(changeset, upstreamName.c_str()));
  }

  response.builder().initChangeset(req.getId().size());

  if (req.getId().size() == 0) {
    return response;
  }

  size_t changesetIndex = 0;

  GIT_PTR(tree, clientForkTree);
  GIT_PTR(index, workIndex);
  GIT_PTR(tree, nonActiveClientTree);

  if (req.getIncludeFileInfo()) {
    CHECK_GIT(git_commit_tree(&clientForkTree, clientForkCommit));
    if (clientIsHead) {
      CHECK_GIT(makeWorkIndex(ctx.repo.get(), &workIndex, clientForkTree));
    } else {
      CHECK_GIT(git_reference_peel(
        reinterpret_cast<git_object **>(&nonActiveClientTree), client, GIT_OBJECT_TREE));
    }
  }

  for (const auto id : req.getId()) {
    ScopeTimer t1("process-changesets");
    auto changesetResponse = response.builder().getChangeset()[changesetIndex++];
    changesetResponse.setId(id);

    if (id == WORKING_CHANGESET) {
      if (req.getIncludeFileInfo()) {
        const auto [lsGitRes, csList] = g8GitLsChangesets(ctx.repo.get(), req.getClient().cStr());
        CHECK_GIT(lsGitRes);

        std::set<String> filesInChangesets;
        for (const auto &cs : csList.changesets) {
          auto csName = format(
            "%s%s-%s%s",
            CHANGESETS_PREFIX,
            req.getClient().cStr(),
            CHANGESET_CODE,
            cs.name.c_str());

          GIT_PTR(reference, cs_ref);
          CHECK_GIT(git_reference_lookup(&cs_ref, ctx.repo.get(), csName.c_str()));

          MutableRootMessage<G8FileInfoList> fileList;
          CHECK_GIT(gitDiffToMergeBase(ctx.repo.get(), client, cs_ref, false, fileList.builder()));

          for (const auto info : fileList.reader().getInfo()) {
            filesInChangesets.insert(info.getPath().cStr());
          }
        }

        std::set<String> dirtyWorkFiles;
        std::map<String, G8FileInfo::Builder> dirtyFileMap;
        MutableRootMessage<G8FileInfoList> dirtyList;

        if (clientIsHead) {
          GIT_PTR(tree, clientTree);
          CHECK_GIT(git_commit_tree(&clientTree, clientCommit));
          // NOTE(pawel) It doesn't matter which side is from and to since we only care about the
          // paths of the changed files here.
          CHECK_GIT(gitDiffTreeToIndex(
            ctx.repo.get(), clientTree, workIndex, false, nullptr, dirtyList.builder()));

          for (auto info : dirtyList.builder().getInfo()) {
            dirtyWorkFiles.insert(info.getPath().cStr());
            dirtyFileMap.insert(std::make_pair(info.getPath().cStr(), info));
          }
        }

        std::map<String, G8FileInfo::Builder> outFiles;
        MutableRootMessage<G8FileInfoList> workFiles;
        // NOTE(pawel) Doesn't matter which side is which for diff since we only care about paths.
        if (clientIsHead) {
          CHECK_GIT(gitDiffTreeToIndex(
            ctx.repo.get(), clientForkTree, workIndex, false, nullptr, workFiles.builder()));
        } else {
          CHECK_GIT(gitDiffTreeToTree(
            ctx.repo.get(), clientForkTree, nonActiveClientTree, false, workFiles.builder()));
        }

        // NOTE(pawel) Diffing clientFork to working does not show client files that were reverted.

        for (auto file : workFiles.builder().getInfo()) {
          if (filesInChangesets.find(file.getPath().cStr()) == filesInChangesets.end()) {
            // Only add the files that aren't in other changesets.
            auto fileIsDirty = dirtyWorkFiles.find(file.getPath().cStr());
            if (fileIsDirty != dirtyWorkFiles.end()) {
              file.setDirty(true);
            }
            outFiles.insert(std::make_pair(file.getPath().cStr(), file));
          }
        }

        // Now go through the dirtyWorkFiles to see if any are missing from the outFiles.
        for (const auto &dirtyFile : dirtyWorkFiles) {
          if (filesInChangesets.find(dirtyFile) == filesInChangesets.end()) {
            if (outFiles.count(dirtyFile.c_str()) == 0) {
              auto dirtyFileIt = dirtyFileMap.find(dirtyFile);
              dirtyFileIt->second.setDirty(true);
              // A dirtyWorkFile that is not in the outFiles is a reverted file.
              // Its status should be unmodified.
              dirtyFileIt->second.setStatus(G8FileInfo::Status::UNMODIFIED);
              outFiles.insert(std::make_pair(dirtyFile, dirtyFileIt->second));
            }
          }
        }

        changesetResponse.getFileList().initInfo(outFiles.size());
        int idx = 0;
        for (auto outFile : outFiles) {
          changesetResponse.getFileList().getInfo().setWithCaveats(idx++, outFile.second);
        }
      }
    } else {  // id != WORKING_CHANGESET
      CHECK_GIT(git_reference_lookup(
        &changeset,
        ctx.repo.get(),
        strCat(CHANGESETS_PREFIX, req.getClient().cStr(), "-", CHANGESET_CODE, id.cStr()).c_str()));

// Only native g8 needs to migrate and move changesets.
// TODO(pawel) Remove after a reasonable time.
#ifndef JAVASCRIPT
      {
        GIT_PTR(commit, changesetCommit);
        CHECK_GIT(git_reference_peel(
          reinterpret_cast<git_object **>(&changesetCommit), changeset, GIT_OBJECT_COMMIT));

        GIT_PTR(commit, fixedChangesetCommit);
        CHECK_GIT(fixPreviousBranch(&fixedChangesetCommit, changesetCommit, ctx.master.get()));

        if (fixedChangesetCommit) {
          String prevId = git_oid_tostr_s(git_commit_id(changesetCommit));
          String nextId = git_oid_tostr_s(git_commit_id(fixedChangesetCommit));
          C8Log("Fixed changeset commit %s -> %s", prevId.c_str(), nextId.c_str());
          GIT_PTR(reference, fixedChangeset);
          CHECK_GIT(git_reference_set_target(
            &fixedChangeset,
            changeset,
            git_commit_id(fixedChangesetCommit),
            "Fixed changeset branch"));
          std::swap(changeset, fixedChangeset);
          CHECK_GIT(g8GitForcePush(req.getAuth(), ctx.repo.get(), changeset));
        }
      }
#endif

      if (action == Action::DELETE) {
        g8GitDeleteRemoteBranch(req.getAuth(), ctx.repo.get(), changeset);
        CHECK_GIT(git_branch_set_upstream(changeset, nullptr));
        CHECK_GIT(git_reference_delete(changeset));
        continue;
      }

      if (action == Action::CREATE || action == Action::UPDATE) {
        GIT_PTR(tree, originalChangesetTree);
        CHECK_GIT(git_reference_peel(
          reinterpret_cast<git_object **>(&originalChangesetTree), changeset, GIT_OBJECT_TREE));

        std::set<String> filesInChangeset;
        std::set<String> filesToExclude;

        const auto [lsGitRes, csList] = g8GitLsChangesets(ctx.repo.get(), req.getClient().cStr());
        CHECK_GIT(lsGitRes);

        for (const auto &cs : csList.changesets) {
          auto csName = format(
            "%s%s-%s%s",
            CHANGESETS_PREFIX,
            req.getClient().cStr(),
            CHANGESET_CODE,
            cs.name.c_str());

          GIT_PTR(reference, cs_ref);
          CHECK_GIT(git_reference_lookup(&cs_ref, ctx.repo.get(), csName.c_str()));

          MutableRootMessage<G8FileInfoList> fileList;
          CHECK_GIT(gitDiffToMergeBase(ctx.repo.get(), client, cs_ref, false, fileList.builder()));

          for (const auto info : fileList.reader().getInfo()) {
            if (id == cs.name) {
              // Find all the files in this changeset if the changeset already exists.
              filesInChangeset.insert(info.getPath().cStr());
            } else {
              // Exclude all files that are in other Changesets.
              filesToExclude.insert(info.getPath().cStr());
            }
          }
        }

        std::set<String> filesToInclude;
        if (fileAction != FileAction::REPLACE) {
          filesToInclude = filesInChangeset;
        }

        if (
          fileAction == FileAction::REPLACE || fileAction == FileAction::INSERT
          || action == Action::CREATE) {
          for (const auto path : req.getFileUpdate()) {
            filesToInclude.insert(path);
          }
        }

        if (fileAction == FileAction::REMOVE) {
          for (const auto path : req.getFileUpdate()) {
            filesToInclude.erase(path);
          }
        }

        git_oid changesetOid;

        if (action == Action::CREATE && fileAction == FileAction::NONE) {
          CHECK_GIT(writeChangesetTree(
            &changesetOid,
            ctx.repo.get(),
            clientCommit,
            clientForkCommit,
            nullptr,
            filesToExclude,
            nullptr));
        } else {
          CHECK_GIT(writeChangesetTree(
            &changesetOid,
            ctx.repo.get(),
            clientCommit,
            clientForkCommit,
            &filesToInclude,
            filesToExclude,
            nullptr));
        }

        GIT_PTR(tree, changesetTree);
        CHECK_GIT(git_tree_lookup(&changesetTree, ctx.repo.get(), &changesetOid));

        GIT_PTR(commit, changesetCommit);
        CHECK_GIT(git_reference_peel(
          reinterpret_cast<git_object **>(&changesetCommit), changeset, GIT_OBJECT_COMMIT));

        const char *oldMessage = git_commit_message_raw(changesetCommit);

        git_buf prettyDescription = {nullptr, 0, 0};
        SCOPE_EXIT([&prettyDescription] { git_buf_dispose(&prettyDescription); });

        const char *message = nullptr;
        if (0 == req.getDescription().size()) {
          message = oldMessage;
        } else {
          CHECK_GIT(git_message_prettify(&prettyDescription, req.getDescription().cStr(), 1, '#'));
          message = prettyDescription.ptr;
        }
        const bool changesetsAreEqual =
          git_oid_equal(git_tree_id(originalChangesetTree), &changesetOid)
          && (req.getDescription().size() == 0 || 0 == strcmp(oldMessage, message));

        // Keep the changeset fork up to date with the client fork. Some repos have a fast-forward
        // land requirement which requires that branches are based off the latest master tip.
        // g8 sync will rebase the client on top of latest master and g8 update will update the
        // changeset fork to reflect the client fork.
        bool changesetForkIsClientFork{false};
        GIT_PTR(commit, changesetForkCommit);
        CHECK_GIT(
          gitFindMergeBase(&changesetForkCommit, ctx.repo.get(), ctx.master.get(), changeset));
        if (git_oid_equal(git_commit_id(changesetForkCommit), git_commit_id(clientForkCommit))) {
          changesetForkIsClientFork = true;
        }

        GIT_PTR(signature, signature);
        git_signature_default(&signature, ctx.repo.get());

        // Gitlab rebase operations totally ignore any changes in merge commits. It cherry-picks
        // the other commits. For the commit following the merge commit, its difference from the
        // merge commit appears to be taken. So what i'm going to try here is to send up the fork
        // point move but without changing the actual changeset files... so keep the files as
        // they were in originalChangesetTree and everything else as they are in the clientTree.
        if (!changesetForkIsClientFork) {
          const git_commit *parents[] = {changesetCommit, clientForkCommit};

          git_oid movedForkTreeOid;
          CHECK_GIT(writeChangesetTree(
            &movedForkTreeOid,
            ctx.repo.get(),
            clientCommit,
            clientForkCommit,
            &filesToInclude,
            filesToExclude,
            originalChangesetTree));

          GIT_PTR(tree, movedForkTree);
          CHECK_GIT(git_tree_lookup(&movedForkTree, ctx.repo.get(), &movedForkTreeOid));

          git_oid csOid;
          CHECK_GIT(git_commit_create(
            &csOid,
            ctx.repo.get(),
            git_reference_name(changeset),
            signature,
            signature,
            nullptr,
            message,
            movedForkTree,
            2,
            parents));
          GIT_PTR(reference, newChangeset);
          CHECK_GIT(
            git_reference_lookup(&newChangeset, ctx.repo.get(), git_reference_name(changeset)));
          std::swap(changeset, newChangeset);

          GIT_PTR(commit, newChangesetCommit);
          CHECK_GIT(git_reference_peel(
            reinterpret_cast<git_object **>(&newChangesetCommit), changeset, GIT_OBJECT_COMMIT));
          std::swap(changesetCommit, newChangesetCommit);

          changesetResponse.setUpdated(true);
        }

        if (!changesetsAreEqual) {
          const git_commit *parents[] = {changesetCommit};
          git_oid csOid;
          CHECK_GIT(git_commit_create(
            &csOid,
            ctx.repo.get(),
            git_reference_name(changeset),
            signature,
            signature,
            nullptr,
            message,
            changesetTree,
            1,
            parents));

          auto postCommitRes = postCommitHook(ctx.repo.get());
          if (postCommitRes && *postCommitRes != 0) {
            // TODO(pawel) report non-zero return?
          }

          // Fix the stale changeset reference post commit.
          GIT_PTR(reference, newChangeset);
          CHECK_GIT(
            git_reference_lookup(&newChangeset, ctx.repo.get(), git_reference_name(changeset)));
          std::swap(changeset, newChangeset);

          changesetResponse.setUpdated(true);
        }
        if (!changesetsAreEqual || !changesetForkIsClientFork || action == Action::CREATE) {
          // Force push any updates to the server.
          // TODO(mc): Add a way to force push everything if the client gets behind.
          CHECK_GIT(g8GitForcePush(req.getAuth(), ctx.repo.get(), changeset));
        }
      }

      GIT_PTR(commit, changesetPostCommit);
      CHECK_GIT(git_reference_peel(
        reinterpret_cast<git_object **>(&changesetPostCommit), changeset, GIT_OBJECT_COMMIT));

      if (req.getIncludeSummary()) {
        changesetResponse.setSummary(git_commit_summary(changesetPostCommit));
      }

      if (req.getIncludeDescription()) {
        changesetResponse.setDescription(git_commit_message(changesetPostCommit));
      }

      if (req.getIncludeBody()) {
        const char *commitBody = git_commit_body(changesetPostCommit);
        changesetResponse.setBody(commitBody ? commitBody : "");
      }

      if (req.getIncludeCommitId()) {
        changesetResponse.setCommitId(git_oid_tostr_s(git_commit_id(changesetPostCommit)));
      }

      if (req.getIncludeFileInfo()) {
        CHECK_GIT(gitDiffToMergeBase(
          ctx.repo.get(),
          client,
          changeset,
          req.getFindRenamesAndCopies(),
          changesetResponse.getFileList()));

        std::map<String, G8FileInfo::Builder> filesInThisChangeset;
        if (clientIsHead) {
          for (auto file : changesetResponse.getFileList().getInfo()) {
            filesInThisChangeset.insert(std::make_pair(file.getPath().cStr(), file));
          }

          GIT_PTR(commit, csCommit);
          CHECK_GIT(git_reference_peel(
            reinterpret_cast<git_object **>(&csCommit), changeset, GIT_OBJECT_COMMIT));

          // Only diff files in the changeset.
          std::vector<char *> csPathArray;
          for (const auto &[csPath, _csFileInfo] : filesInThisChangeset) {
            csPathArray.push_back(const_cast<char *>(csPath.c_str()));
          }
          git_strarray csPathspec = {.strings = csPathArray.data(), .count = csPathArray.size()};

          // Diff the changeset with the working directory to figure out if any files are dirty.
          MutableRootMessage<G8FileInfoList> workFiles;
          GIT_PTR(tree, csTree);
          CHECK_GIT(git_commit_tree(&csTree, csCommit));
          // NOTE(pawel) Doesn't matter which side is which for diff since we only care about paths.
          CHECK_GIT(gitDiffTreeToIndex(
            ctx.repo.get(), csTree, workIndex, false, &csPathspec, workFiles.builder()));
          for (const auto file : workFiles.reader().getInfo()) {
            auto fileInWorking = filesInThisChangeset.find(file.getPath());
            if (fileInWorking != filesInThisChangeset.end()) {
              // The file differs in the working tree. Mark it as dirty.
              fileInWorking->second.setDirty(true);
            }
          }
        }
      }

      if (req.getIncludeRemoteBranch()) {
        GIT_PTR(reference, remoteChangesetRef);
        CHECK_GIT(git_branch_upstream(&remoteChangesetRef, changeset));

        git_buf originName = {0};
        SCOPE_EXIT([&originName] { git_buf_dispose(&originName); });
        CHECK_GIT(
          git_branch_upstream_remote(&originName, ctx.repo.get(), git_reference_name(changeset)));
        changesetResponse.setRemoteBranch(
          &git_reference_shorthand(remoteChangesetRef)[originName.size + 1]);
      }
    }
  }
  return response;
}

}  // namespace c8
