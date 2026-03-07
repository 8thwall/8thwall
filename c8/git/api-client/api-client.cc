// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//c8/git:internal",
  };
  hdrs = {
    "api-client.h",
  };
  deps = {
    ":api-client-create",
    ":api-client-delete",
    ":api-client-list",
    ":api-client-save",
    ":api-client-switch",
    "//c8/git:api-common",
    "//c8/git:g8-api.capnp-cc",
    "//c8/git:hooks",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:scope-exit",
    "//c8:vector",
    "//c8/io:capnp-messages",
    "//c8/string:format",
    "//c8/string:strcat",
    "//c8/stats:scope-timer",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0x476ae38e);

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
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/git/api-client/api-client.h"
#include "c8/git/api-common.h"
#include "c8/git/hooks.h"
#include "c8/io/capnp-messages.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/format.h"
#include "c8/string/strcat.h"
#include "c8/vector.h"

namespace c8 {

using namespace g8;
using Action = G8ClientRequest::Action;

String extractRefFromParams(const G8PatchParams::Reader &params) {
  if (params.hasRef()) {
    return params.getRef();
  } else if (params.hasUser()) {
    if (params.hasChangeset()) {
      return strCat(
        "origin/",
        makeRemoteChangesetName(
          params.getUser().cStr(), params.getClient().cStr(), params.getChangeset().cStr()));
    } else {
      return strCat(
        "origin/", makeRemoteClientName(params.getUser().cStr(), params.getClient().cStr()));
    }
  } else {
    // No user provided so do a local branch.
    if (params.hasChangeset()) {
      return makeChangesetName(params.getClient().cStr(), params.getChangeset().cStr());
    } else {
      return makeClientName(params.getClient().cStr());
    }
  }
}

int resolveRef(const ClientContext &ctx, const String &sourceRef, git_commit **commit) {
  // If a remote ref is specified, perform a fetch operation to get latest remote state.
  if (sourceRef.starts_with("origin/")) {
    auto remoteBranch = sourceRef.substr(strlen("origin/"));
    GIT_STATUS_RETURN(
      fetchRemoteBranch(ctx.repo.get(), ctx.request.reader().getAuth(), remoteBranch.c_str()));
    GIT_PTR(reference, ref);
    GIT_STATUS_RETURN(
      git_branch_lookup(&ref, ctx.repo.get(), sourceRef.c_str(), GIT_BRANCH_REMOTE));
    GIT_STATUS_RETURN(
      git_reference_peel(reinterpret_cast<git_object **>(commit), ref, GIT_OBJECT_COMMIT));
  } else {
    // End commit is the tip of the branch we want to patch in.
    GIT_PTR(object, resolvedObject);
    GIT_STATUS_RETURN(git_revparse_single(&resolvedObject, ctx.repo.get(), sourceRef.c_str()));
    if (resolvedObject == nullptr) {
      return 0;
    }

    GIT_STATUS_RETURN(
      git_object_peel(reinterpret_cast<git_object **>(commit), resolvedObject, GIT_OBJECT_COMMIT));
  }

  return 0;
}

ConstRootMessage<G8ClientResponse> g8Client(G8ClientRequest::Reader req) {
  ScopeTimer t("api-client");
  ClientContext ctx;
  auto &response = ctx.response;

  ctx.request.setRoot(req);
  req = ctx.request.reader();

  const auto action = req.getAction();
  const bool isSyncAction = action == Action::SYNC || action == Action::DRY_SYNC;
  const bool isPatchAction = action == Action::PATCH || action == Action::DRY_PATCH;
  const bool isDry = action == Action::DRY_SYNC || action == Action::DRY_PATCH;

  const bool forceSave = req.getForceSave();

  CHECK_ASSIGN(git_repository_open, ctx.repo, req.getRepo().cStr());
  CHECK_ASSIGN(lookupMainBranch, ctx.master, ctx.repo.get());
  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(ctx.repo.get()));

  GIT_PTR(reference, activeClient);
  {
    GIT_PTR(reference, head);
    CHECK_GIT(git_reference_lookup(&head, ctx.repo.get(), "HEAD"));
    CHECK_GIT(git_reference_resolve(&activeClient, head));
  }

  ctx.activeClientName = git_reference_shorthand(activeClient);

  if (req.getClient().size() == 0) {
    ctx.request.builder().initClient(1).set(0, ctx.activeClientName);
  }

  bool onActiveClient = req.getClient()[0] == ctx.activeClientName;

  switch (action) {
    case Action::LIST:
      g8ClientList(ctx);
      return ctx.response;
    case Action::SAVE:
      g8ClientSave(ctx);
      return ctx.response;
    case Action::CREATE:
      g8ClientCreate(ctx);
      return ctx.response;
    case Action::SWITCH:
      g8ClientSwitch(ctx);
      return ctx.response;
    case Action::DELETE:
      g8ClientDelete(ctx);
      return ctx.response;
    case Action::SYNC:
    case Action::DRY_SYNC:
    case Action::PATCH:
    case Action::DRY_PATCH:
      break;
    default:
      RESPOND_ERROR("Unexpected action");
  }

  for (const auto client : req.getClient()) {
    if (!clientNameIsValid(client.cStr())) {
      RESPOND_ERROR(format("Invalid client name %s", client.cStr()));
    }
  }

  if (req.getIncludeCloudSaveInfo()) {
    if (!req.hasUser()) {
      RESPOND_ERROR("User is required for fetching cloud save info");
    }
  }

  {
    if (req.getClient().size() > 1) {
      RESPOND_ERROR("SYNC/PATCH action requires exactly one client");
    }

    if (isSyncAction && !req.hasUser()) {
      RESPOND_ERROR("SYNC action requires user specified");
    }

    // TODO(pawel) for dry sync this doesn't need to be the case.
    // Just make sure we use the client ref and not working changes.
    // Wet sync for current client use working changes.
    // Wet sync for other client uses that client's save commit.
    // Dry sync for current client uses working changes.
    // Dry sync for other client usee that client's save commit.
    // So can we simplify this to always use working changes for current client
    // and the save commit for non-active client?

    // Patch operations.
    // Patching to active client should always use working changes
    // Patching to non-active clients uses that client's save commit.

    // Looks like that for both sync and patch operations we can use working changes for active
    // client and save commit for non-active clients.
    // We just also don't checkout the result of the rebase for non-active.

    // But sync neither syncing nor patching performs a post-operation save what does that means
    // for the result of the operation? Perhaps a parameter to save successful result?
    // We can use the forceSave? Otherwise running wet operations on non-active clients doesn't
    // produce a meaningful result.
  }

  if (isPatchAction) {
    if (req.getPatchParams().hasRef()) {
      if (req.getPatchParams().hasClient()) {
        RESPOND_ERROR("Cannot specify client with ref");
      }
      if (req.getPatchParams().hasChangeset()) {
        RESPOND_ERROR("Cannot specify changeset with ref");
      }
      if (req.getPatchParams().hasUser()) {
        RESPOND_ERROR("Cannot specify user with ref");
      }
    } else {
      if (!req.getPatchParams().hasClient()) {
        RESPOND_ERROR("PATCH action requires a source client");
      }

      if (!clientNameIsValid(req.getPatchParams().getClient().cStr())) {
        RESPOND_ERROR("Invalid source client name");
      }

      if (req.getPatchParams().hasChangeset()) {
        if (!changesetNameIsValid(req.getPatchParams().getChangeset().cStr())) {
          RESPOND_ERROR("Invalid changeset name");
        }
      }

      if (req.getPatchParams().hasUser()) {
        if (!userNameIsValid(req.getPatchParams().getUser().cStr())) {
          RESPOND_ERROR("Invalid user name");
        }
      }
    }

    if (!onActiveClient) {
      // TODO(pawel) Enable patching to other clients as well.
      RESPOND_ERROR("PATCH requires you to be on the active client");
    }
  }

  if ((isSyncAction && !isDry) || (isPatchAction && req.getPatchParams().hasUser())) {
    ScopeTimer t("sync-master");
    CHECK_GIT(g8SyncMaster(ctx.repo.get(), req.getAuth(), req.getUser().cStr()));
    CHECK_ASSIGN(lookupMainBranch, ctx.master, ctx.repo.get());
  }

  if (!isDry && onActiveClient) {
    ScopeTimer t("save");
    CHECK_GIT(g8GitSave(req.getAuth(), ctx.repo.get(), forceSave));
  }

  // It is ensured above that exactly one client is present for a SYNC action.
  const char *client = req.getClient()[0].cStr();

  {
    GIT_PTR(reference, clientRef);
    CHECK_GIT(git_branch_lookup(&clientRef, ctx.repo.get(), client, GIT_BRANCH_LOCAL));

    GIT_PTR(object, masterCommitObj);
    CHECK_GIT(git_reference_peel(&masterCommitObj, ctx.master.get(), GIT_OBJECT_COMMIT));

    bool needsSync{false};

    // Both sync and patch are rebase operations. Rebase takes three inputs:
    // A destination commit onto which to apply changes, known as the unto commit.
    // A start point commit, which is known as upstream, this the point from which we compute diffs
    // that we want to apply to the destination.
    // An end commit, which is the last diff to apply. Known as the branch commit.
    // The changes between upstream and end commits is what is applied to destination.

    // For sync operations:
    // The destination is the sync point, which lives on the master line.
    // The end is the client save commit in which our changes live.
    // The start point is the client fork point.
    // This results in the diff between client and fork point being applied to the new base which
    // brings the changes in our client to the new base.

    // For patch operations:
    // The destination is the client.
    // The end commit is the target from which we are pulling changes.
    // The start point is the fork point off master for the terminal commit, which means that we
    // pull in only the changes that have been in that branch.

    GIT_PTR(commit, startCommit);
    GIT_PTR(commit, endCommit);
    GIT_PTR(commit, destCommit);
    GIT_PTR(commit, clientForkCommit);

    CHECK_GIT(gitFindMergeBase(&clientForkCommit, ctx.repo.get(), ctx.master.get(), clientRef));

    if (isSyncAction) {
      ScopeTimer t("prep-sync-action");
      // End is the client in standard sync operations...
      if (!isDry || !onActiveClient) {
        CHECK_GIT(git_reference_peel(
          reinterpret_cast<git_object **>(&endCommit), clientRef, GIT_OBJECT_COMMIT));
      } else {
        // But dry operations don't save the client so we use working changes.
        // Fetch the working tree and create a transient "save" commit.
        // When we run an actual sync operation we save the working changes so it's okay
        // to assume that dry and wet syncing produce the same merge result so long as changes
        // aren't made between the dry and the wet operation.
        GIT_PTR(index, workIndex);
        GIT_PTR(tree, backingTree);
        CHECK_GIT(git_commit_tree(&backingTree, clientForkCommit));
        CHECK_GIT(makeWorkIndex(ctx.repo.get(), &workIndex, backingTree));
        GIT_PTR(tree, workingTree);
        git_oid backingTreeOid;
        CHECK_GIT(git_index_write_tree(&backingTreeOid, workIndex));
        CHECK_GIT(git_tree_lookup(&workingTree, ctx.repo.get(), &backingTreeOid));

        GIT_PTR(signature, signature);
        CHECK_GIT(git_signature_default(&signature, ctx.repo.get()));

        git_oid transientSaveCommitOid;
        CHECK_GIT(git_commit_create(
          &transientSaveCommitOid,
          ctx.repo.get(),
          nullptr,
          signature,
          signature,
          nullptr,
          "Transient save commit for dry sync",
          workingTree,
          1,
          const_cast<const git_commit **>(&clientForkCommit)));

        CHECK_GIT(git_commit_lookup(&endCommit, ctx.repo.get(), &transientSaveCommitOid));
      }

      // Start is the client fork in all sync operations.
      CHECK_GIT(git_commit_dup(&startCommit, clientForkCommit));

      if (!req.getSyncParams().hasSyncCommitId()) {
        // When syncing to master, the destination is the latest master.
        CHECK_GIT(git_reference_peel(
          reinterpret_cast<git_object **>(&destCommit), ctx.master.get(), GIT_OBJECT_COMMIT));
      } else {
        // Do a rev-parse to expand shorthand commit id.
        {
          GIT_PTR(object, object);
          CHECK_GIT(git_revparse_single(
            &object, ctx.repo.get(), req.getSyncParams().getSyncCommitId().cStr()));
          CHECK_GIT(git_commit_lookup(&destCommit, ctx.repo.get(), git_object_id(object)));
        }

        // Ensure that this commit exists on the master line.
        // If the merge base is us that means we are on the master line.
        const git_oid *masterOid = git_reference_target(ctx.master.get());
        git_oid forkOid;
        CHECK_GIT(git_merge_base(&forkOid, ctx.repo.get(), masterOid, git_commit_id(destCommit)));

        if (!git_oid_equal(&forkOid, git_commit_id(destCommit))) {
          RESPOND_ERROR("Sync commit hash is not in the master branch");
        }

        // This function does not consider a commit to be a descendant of itself, which would
        // be the case if the specifed sync commit is the current fork. In this case no sync is
        // necessary.
        if (git_graph_descendant_of(ctx.repo.get(), git_commit_id(startCommit), &forkOid)) {
          needsSync = true;
        }
      }

      // In the case of syncing backwards, the fork commit and the master commit are the same.
      // In case of syncing forward the two are different.
      // In case of syncing to latest master or current fork point, the two are the same.
      if (!git_oid_equal(git_commit_id(startCommit), git_object_id(masterCommitObj))) {
        needsSync = true;
      }
    }

    if (isPatchAction) {
      ScopeTimer t("prep-patch-action");
      // The client is the destination.
      CHECK_GIT(git_reference_peel(
        reinterpret_cast<git_object **>(&destCommit), clientRef, GIT_OBJECT_COMMIT));

      const String sourceRef = extractRefFromParams(req.getPatchParams());

      CHECK_GIT(resolveRef(ctx, sourceRef, &endCommit));

      // TODO(pawel) What about the case when the client has no changes, i.e. the tip of
      // the branch is on the master line?

      GIT_PTR(commit, masterCommitObj);
      CHECK_GIT(git_reference_peel(
        reinterpret_cast<git_object **>(&masterCommitObj), ctx.master.get(), GIT_OBJECT_COMMIT));

      // Find the fork point from master, this is the starting point.
      CHECK_GIT(gitFindMergeBase(&startCommit, ctx.repo.get(), masterCommitObj, endCommit));

      // TODO(pawel) If the merge base between master and the sourceChangesRef is not startCommit,
      // create an amended commit for the end commit that has the merge base as parent.
      // This is a squash operation so that we are rebasing only one commit so at most
      // there is only one round of conflicts to deal with. This will make changeset branches
      // work neatly as well as any arbitrary commit.

      needsSync = true;
    }

    auto mergeId = response.builder().getMerge().getMergeId();
    mergeId.setOriginal(git_oid_tostr_s(git_commit_id(startCommit)));

    // For sync, your client changes are the source and the sync target is the destination.
    if (isSyncAction) {
      mergeId.setTheirs(git_oid_tostr_s(git_commit_id(destCommit)));
      mergeId.setYours(git_oid_tostr_s(git_commit_id(endCommit)));
    }

    // For patch, the end commit is the incoming changes and the destination is your client.
    if (isPatchAction) {
      mergeId.setTheirs(git_oid_tostr_s(git_commit_id(endCommit)));
      mergeId.setYours(git_oid_tostr_s(git_commit_id(destCommit)));
    }

    if (req.getSyncParams().hasExpectedMergeId()) {
      auto expected = req.getSyncParams().getExpectedMergeId();
      if (
        expected.getOriginal() != mergeId.getOriginal()
        || expected.getTheirs() != mergeId.getTheirs()
        || expected.getYours() != mergeId.getYours()) {
        RESPOND_ERROR("Expected mergeId mismatch.");
      }
    }

    if (!needsSync) {
      response.builder().getMerge().setStatus(G8MergeAnalysis::MergeStatus::COMPLETE);
    }

    if (needsSync) {
      // In order to ensure that there is only ever one commit to rebase, squash the endCommit.
      if (!git_oid_equal(git_commit_id(startCommit), git_commit_parent_id(endCommit, 0))) {
        GIT_PTR(tree, tree);
        CHECK_GIT(git_commit_tree(&tree, endCommit));
        Vector<const git_commit *> parents = {startCommit};
        git_oid squashedEndCommitOid;
        CHECK_GIT(git_commit_create(
          &squashedEndCommitOid,
          ctx.repo.get(),
          nullptr,
          git_commit_author(endCommit),
          git_commit_committer(endCommit),
          git_commit_message_encoding(endCommit),
          git_commit_message(endCommit),
          tree,
          1,
          parents.data()));
        GIT_PTR(commit, quashedEndCommit);
        CHECK_GIT(git_commit_lookup(&quashedEndCommit, ctx.repo.get(), &squashedEndCommitOid));
        std::swap(quashedEndCommit, endCommit);
      }

      GIT_PTR(annotated_commit, annotatedDestCommit);
      GIT_PTR(annotated_commit, annotatedEndCommit);
      GIT_PTR(annotated_commit, annotatedStartCommit);

      CHECK_GIT(git_annotated_commit_lookup(
        &annotatedDestCommit, ctx.repo.get(), git_commit_id(destCommit)));
      CHECK_GIT(
        git_annotated_commit_lookup(&annotatedEndCommit, ctx.repo.get(), git_commit_id(endCommit)));
      CHECK_GIT(git_annotated_commit_lookup(
        &annotatedStartCommit, ctx.repo.get(), git_commit_id(startCommit)));

      GIT_PTR(index, mergeIndex);
      GIT_PTR(rebase, rebase);

      git_rebase_options rebaseOpts = GIT_REBASE_OPTIONS_INIT;
      rebaseOpts.inmemory = 1;

      // By default, GIT_REBASE_OPTIONS_INIT sets merge_options to GIT_MERGE_OPTIONS_INIT,
      // which sets GIT_MERGE_FIND_RENAMES. Disable this to avoid problems with conflicts
      // involving renames.
      auto &mergeFlags = rebaseOpts.merge_options.flags;
      mergeFlags = static_cast<git_merge_flag_t>(mergeFlags & ~GIT_MERGE_FIND_RENAMES);

      git_rebase_init(
        &rebase,
        ctx.repo.get(),
        annotatedEndCommit,
        annotatedStartCommit,
        annotatedDestCommit,
        &rebaseOpts);

      git_rebase_operation *rebaseOperation;  // This has no 'free' method.
      int rebase_error;
      while (0 == (rebase_error = git_rebase_next(&rebaseOperation, rebase))) {
        // Perform all of the rebase operations. Clients should only ever have at most 1 commit
        // from fork.
        // NOTE(pawel) If we wanted to enable history for clients we can squash the tip of the
        // client onto the newest merge base from master. This would effectively make clients
        // appear to be a single commit from the perspective of the rebase operation while
        // maintaining history.
      }
      if (rebase_error != GIT_ITEROVER) {
        CHECK_GIT(rebase_error);
      }

      CHECK_GIT(git_rebase_inmemory_index(&mergeIndex, rebase));
      GIT_PTR(tree, endTree);
      GIT_PTR(tree, startTree);
      GIT_PTR(tree, destTree);

      // Figure out the files in the merge index.
      CHECK_GIT(git_commit_tree(&endTree, endCommit));
      CHECK_GIT(git_commit_tree(&startTree, startCommit));
      CHECK_GIT(git_commit_tree(&destTree, destCommit));

      // Figure out how this index differs from the client branch or patch branch.
      MutableRootMessage<G8FileInfoList> indexFiles;
      {
        // For syncs, the client is the end tree.
        // For patches, the client is the destination tree.
        auto tree = isSyncAction ? endTree : destTree;
        CHECK_GIT(gitDiffTreeToIndex(
          ctx.repo.get(), tree, mergeIndex, false, nullptr, indexFiles.builder()));
      }

      // Figure out the files in the client.
      MutableRootMessage<G8FileInfoList> clientFiles;
      CHECK_GIT(gitDiffToMergeBase(
        ctx.repo.get(), ctx.master.get(), clientRef, false, clientFiles.builder()));

      // Go through again and modify the index to add conflicted entries even when merge was
      // successful.
      GIT_PTR(index, startIndex);
      GIT_PTR(index, destIndex);
      GIT_PTR(index, endIndex);
      CHECK_GIT(git_index_new(&startIndex));
      CHECK_GIT(git_index_new(&destIndex));
      CHECK_GIT(git_index_new(&endIndex));
      CHECK_GIT(git_index_read_tree(startIndex, startTree));
      CHECK_GIT(git_index_read_tree(destIndex, destTree));
      CHECK_GIT(git_index_read_tree(endIndex, endTree));

      for (auto indexFile : indexFiles.reader().getInfo()) {
        if (indexFile.getStatus() != G8FileInfo::Status::CONFLICTED) {
          // Rather than exhaustively searching diffs and figuring out exactly which rename
          // happened in which ancestor, we just look up the rebase index path and previous path
          // in the ORIGINAL, YOURS, and THEIRS tree. If we find an entry, then we add it. There
          // are some potential cases we could miss, like if a different rename happens in both
          // the YOURS and THEIRS and were to not mark this as a conflict.

          auto addToIndex = [](
                              git_index *index,
                              const char *path,
                              const char *previousPath,
                              int stage,
                              git_index *rebaseIndex) -> int {
            // First look up by the previous path.
            const git_index_entry *entry =
              git_index_get_bypath(index, previousPath, GIT_INDEX_STAGE_NORMAL);
            if (!entry) {
              // Look up the new path if we didn't find the entry.
              entry = git_index_get_bypath(index, path, GIT_INDEX_STAGE_NORMAL);
            }
            if (entry) {
              // Add to the rebase index.
              git_index_entry entryRewrite = *entry;
              entryRewrite.path = path;
              GIT_INDEX_ENTRY_STAGE_SET(&entryRewrite, stage);
              return git_index_add(rebaseIndex, &entryRewrite);
            }
            return 0;
          };

          const char *path = indexFile.getPath().cStr();
          const char *previousPath = indexFile.hasPreviousPath()
            ? indexFile.getPreviousPath().cStr()
            : indexFile.getPath().cStr();

          bool found{false};
          for (auto clientFile : clientFiles.reader().getInfo()) {
            if (
              std::strcmp(clientFile.getPath().cStr(), path) == 0
              || std::strcmp(clientFile.getPath().cStr(), previousPath) == 0) {
              found = true;
              break;
            }
          }
          if (!found) {
            continue;
          }

          CHECK_GIT(
            addToIndex(startIndex, path, previousPath, GIT_INDEX_STAGE_ANCESTOR, mergeIndex));
          CHECK_GIT(addToIndex(endIndex, path, previousPath, GIT_INDEX_STAGE_THEIRS, mergeIndex));
          CHECK_GIT(addToIndex(destIndex, path, previousPath, GIT_INDEX_STAGE_OURS, mergeIndex));
        }
      }

      /*
      {  // Print the index.
        const git_index_entry *entry;
        GIT_PTR(index_iterator, iter);
        CHECK_GIT(git_index_iterator_new(&iter, mergeIndex));
        while (0 == git_index_iterator_next(&entry, iter)) {
          printf(
            "%s %s %d\n", git_oid_tostr_s(&entry->id), entry->path, git_index_entry_stage(entry));
        }
      }
      */

      // At this point we have a pre-decision mergeIndex, startIndex, endIndex and destIndex.
      // This is where we do the inspect flow. We look for files matching the inspectRegex pattern.
      // Entries that are marked as conflicts (both bona-fide and auto-mergeable changes) are
      // ignored.
      if (req.getSyncParams().hasInspectRegex()) {
        struct InspectRecord {
          String nextBlobId;
          String prevBlobId;
        };

        std::regex inspectRegex(req.getSyncParams().getInspectRegex().cStr());
        std::unordered_set<String> pathsNeedingDecision;
        std::unordered_map<String, InspectRecord> inspectRecords;

        int iteratorStatus{0};
        const git_index_entry *entry;

        // Search mergeIndex for conflicted/auto-merged files and paths that match the inspectRegex.
        GIT_PTR(index_iterator, mergeIndexIterator);
        CHECK_GIT(git_index_iterator_new(&mergeIndexIterator, mergeIndex));
        while (0 == (git_index_iterator_next(&entry, mergeIndexIterator))) {
          if (git_index_entry_is_conflict(entry)) {
            // Since auto-mergeable files have a normal entry, they will fall through so
            // this removes the just-made entries.
            inspectRecords.erase(entry->path);
            // We also need to keep track of paths needing
            // decisions so that we can exclude them when we search the startIndex.
            pathsNeedingDecision.insert(entry->path);
            continue;
          }

          if (std::regex_match(entry->path, inspectRegex)) {
            inspectRecords[entry->path].nextBlobId = git_oid_tostr_s(&entry->id);
          }
        }
        if (iteratorStatus != GIT_ITEROVER) {
          CHECK_GIT(iteratorStatus);
        }

        // Now we have a list of inspect files that are either modified, added, or unmodified.
        // We need to search the startIndex to find files that have been deleted.
        // Avoid adding paths that need decision.
        GIT_PTR(index_iterator, startIndexIterator);
        CHECK_GIT(git_index_iterator_new(&startIndexIterator, startIndex));
        while (0 == (git_index_iterator_next(&entry, startIndexIterator))) {
          if (
            std::regex_match(entry->path, inspectRegex)
            && !pathsNeedingDecision.count(entry->path)) {
            inspectRecords[entry->path].prevBlobId = git_oid_tostr_s(&entry->id);
          }
        }
        if (iteratorStatus != GIT_ITEROVER) {
          CHECK_GIT(iteratorStatus);
        }

        // Now that we know how many inspect records there are, we can allocate the space for them.
        response.builder().getMerge().initInspect(inspectRecords.size());
        size_t recordIdx = 0;

        // Now we have a list of inspect files that is mutually exclusive of the mergeList files.
        // Now figure out where the changes occurred.
        const git_index_entry *destEntry;
        const git_index_entry *endEntry;
        for (const auto &[path, inspectRecord] : inspectRecords) {
          auto recordBuilder = response.builder().getMerge().getInspect()[recordIdx++];
          recordBuilder.setPath(path);
          recordBuilder.setNextBlobId(inspectRecord.nextBlobId);
          recordBuilder.setPreviousBlobId(inspectRecord.prevBlobId);

          if (inspectRecord.nextBlobId == inspectRecord.prevBlobId) {
            recordBuilder.setSource(G8MergeAnalysisInspect::ChangeSource::NONE);
            recordBuilder.setStatus(G8MergeAnalysisInspect::Status::UNMODIFIED);
            continue;
          }

          if (inspectRecord.prevBlobId.empty()) {
            recordBuilder.setStatus(G8MergeAnalysisInspect::Status::ADDED);
          } else if (inspectRecord.nextBlobId.empty()) {
            recordBuilder.setStatus(G8MergeAnalysisInspect::Status::DELETED);
          } else {
            recordBuilder.setStatus(G8MergeAnalysisInspect::Status::MODIFIED);
          }

          destEntry = git_index_get_bypath(destIndex, path.c_str(), GIT_INDEX_STAGE_NORMAL);
          endEntry = git_index_get_bypath(endIndex, path.c_str(), GIT_INDEX_STAGE_NORMAL);
          String destBlobId = destEntry ? git_oid_tostr_s(&destEntry->id) : "";
          String endBlobId = endEntry ? git_oid_tostr_s(&endEntry->id) : "";

          if (destBlobId == endBlobId) {
            recordBuilder.setSource(G8MergeAnalysisInspect::ChangeSource::BOTH);
          } else if (endBlobId == inspectRecord.nextBlobId) {
            recordBuilder.setSource(G8MergeAnalysisInspect::ChangeSource::YOU);
          } else {
            recordBuilder.setSource(G8MergeAnalysisInspect::ChangeSource::THEY);
          }
        }
      }

      // Count the number of files that need to be merged.
      std::vector<std::pair<G8FileInfo::Reader, G8FileInfo::Reader>> mergeList;

      {
        std::map<String, G8FileInfo::Reader> indexFileMap;
        for (auto indexFile : indexFiles.reader().getInfo()) {
          switch (indexFile.getStatus()) {
            case G8FileInfo::Status::DELETED:
              // Nothing to do if the file is gone.
              break;
            case G8FileInfo::Status::MODIFIED:
            case G8FileInfo::Status::TYPE_CHANGED:
            case G8FileInfo::Status::CONFLICTED:
              // Add the index file to the map.
              indexFileMap[indexFile.getPath()] = indexFile;
              break;
            case G8FileInfo::Status::RENAMED:
              // Use the old path for renames.
              indexFileMap[indexFile.getPreviousPath()] = indexFile;
              break;
            default:
              // Do nothing for other status types, including ADDED, since they don't represent
              // files in the client that are being merged. Also ignore CONFLICTED files, since they
              // will be added below when we iterate over the conflicts.
              break;
          }
        }

        // NOTE(pawel) This only looks at files in the client.
        for (auto clientFile : clientFiles.reader().getInfo()) {
          auto iter = indexFileMap.find(clientFile.getPath());
          if (iter != indexFileMap.end()) {
            mergeList.push_back(std::make_pair(clientFile, iter->second));
          }
        }
      }

      // There could be conflicts that happened on master to files that are not in the client.
      // Ensure that all conflicted entries appear in the merge list.
      for (auto indexFile : indexFiles.reader().getInfo()) {
        if (indexFile.getStatus() == G8FileInfo::Status::CONFLICTED) {
          auto iter = std::find_if(begin(mergeList), end(mergeList), [&indexFile](auto &candidate) {
            return candidate.first.getPath() == indexFile.getPath();
          });
          // Client entry does not exist so add it in.
          if (iter == end(mergeList)) {
            mergeList.push_back(std::make_pair(indexFile, indexFile));
          }
        }
      }

      int neededDecisions = mergeList.size();
      response.builder().getMerge().initInfo(mergeList.size());

      int mergeIdx = 0;
      for (auto merge : mergeList) {
        G8MergeAnalysisInfo::Builder info = response.builder().getMerge().getInfo()[mergeIdx++];

        // NOTE(pawel) It is possible for there not to be a client entry. This happens when the
        // conflict occurs along the master line. In this case both clientInfo and IndexInfo
        // will be the same entry. Currently we only use clientInfo to set previous path.

        auto [clientInfo, indexInfo] = merge;
        info.setPath(indexInfo.getPath());

        // TODO(pawel) Make test for sync/patch to ensure previous path is correct.
        if (indexInfo.getPath() != clientInfo.getPath()) {
          info.setPreviousPath(clientInfo.getPath());
        }

        if (indexInfo.getStatus() == G8FileInfo::Status::CONFLICTED) {
          info.setStatus(G8MergeAnalysisInfo::Status::CONFLICTED);
        } else {
          const git_index_entry *mergeEntry =
            git_index_get_bypath(mergeIndex, indexInfo.getPath().cStr(), GIT_INDEX_STAGE_NORMAL);
          info.setMergeBlobId(git_oid_tostr_s(&mergeEntry->id));
          info.setStatus(G8MergeAnalysisInfo::Status::MERGEABLE);
        }

        const git_index_entry *ancestorEntry;
        const git_index_entry *ourEntry;
        const git_index_entry *theirEntry;

        CHECK_GIT(git_index_conflict_get(
          &ancestorEntry, &ourEntry, &theirEntry, mergeIndex, indexInfo.getPath().cStr()));

        // For sync: client is "theirs" changes being applied on "our" master onto.
        // For patch: "theirs" is the changes we are moving to "our" client
        if (isSyncAction) {
          std::swap(ourEntry, theirEntry);
        }

        if (ancestorEntry) {
          info.getFileId().setOriginal(git_oid_tostr_s(&ancestorEntry->id));
        }
        if (ourEntry) {
          info.getFileId().setYours(git_oid_tostr_s(&ourEntry->id));
        }
        if (theirEntry) {
          info.getFileId().setTheirs(git_oid_tostr_s(&theirEntry->id));
        }

        // Search for a decision on this merged file.
        // Directly modify mergeIndex, which is the working space for resolving conflicts.
        for (auto decision : req.getSyncParams().getDecision()) {
          auto dfi = decision.getFileId();
          auto ifi = info.getFileId();
          // NOTE(pawel) If there are two files with the exact same ancestor/yours/theirs
          // conflict, we do not make a distinction between them... perhaps also include
          // file path in decision searching?
          if (
            dfi.getOriginal() == ifi.getOriginal() && dfi.getTheirs() == ifi.getTheirs()
            && dfi.getYours() == ifi.getYours()) {
            info.setChoice(decision.getChoice());
            //            if (info.getStatus() == G8MergeAnalysisInfo::Status::CONFLICTED) {
            const git_index_entry *merge =
              git_index_get_bypath(mergeIndex, info.getPath().cStr(), GIT_INDEX_STAGE_NORMAL);

            switch (decision.getChoice()) {
              case G8MergeDecision::Choice::MANUAL_MERGE: {
                if (!decision.hasBlobId()) {
                  RESPOND_ERROR("Missing blobId for MANUAL_MERGE");
                }
                git_index_entry resolved = *(ourEntry != nullptr ? ourEntry : theirEntry);
                GIT_INDEX_ENTRY_STAGE_SET(&resolved, 0);
                CHECK_GIT(git_oid_fromstrn(
                  &resolved.id, decision.getBlobId().cStr(), decision.getBlobId().size()));
                CHECK_GIT(git_index_add(mergeIndex, &resolved));
                CHECK_GIT(git_index_conflict_remove(mergeIndex, info.getPath().cStr()));
                --neededDecisions;
                break;
              }
              // TODO(pawel) Remove this from the api as it should be handled by THEIR_CHANGE.
              case G8MergeDecision::Choice::THEIR_DELETE:
                CHECK_GIT(git_index_conflict_remove(mergeIndex, info.getPath().cStr()));
                --neededDecisions;
                break;
              case G8MergeDecision::Choice::THEIR_CHANGE: {
                if (ifi.getTheirs().size() > 0) {
                  git_index_entry resolved = *theirEntry;
                  GIT_INDEX_ENTRY_STAGE_SET(&resolved, 0);
                  CHECK_GIT(git_index_add(mergeIndex, &resolved));
                }
                CHECK_GIT(git_index_conflict_remove(mergeIndex, info.getPath().cStr()));
                --neededDecisions;
                break;
              }
              case G8MergeDecision::Choice::YOUR_CHANGE: {
                if (ifi.getYours().size() > 0) {
                  git_index_entry resolved = *ourEntry;
                  GIT_INDEX_ENTRY_STAGE_SET(&resolved, 0);
                  CHECK_GIT(git_index_add(mergeIndex, &resolved));
                }
                CHECK_GIT(git_index_conflict_remove(mergeIndex, info.getPath().cStr()));
                --neededDecisions;
                break;
              }
              case G8MergeDecision::Choice::AUTO_MERGE:
                if (!merge) {
                  // Merge is not valid for a conflicted entry and won't be in the index.
                  RESPOND_ERROR("Invalid merge choice for a conflicted entry.");
                }
                // Remove the conflicted entries.
                CHECK_GIT(git_index_conflict_remove(mergeIndex, info.getPath().cStr()));
                --neededDecisions;
                break;
              case G8MergeDecision::Choice::UNDECIDED:
                break;
            }
          }
        }
      }

      GIT_PTR(odb, odb);
      CHECK_GIT(git_repository_odb(&odb, ctx.repo.get()));
      for (auto additionalChange : req.getSyncParams().getAdditionalChanges()) {
        String path = additionalChange.getPath().cStr();
        String blobId = additionalChange.getBlobId().cStr();

        if (blobId.empty()) {
          CHECK_GIT(git_index_remove_bypath(mergeIndex, path.c_str()));
        } else {
          git_index_entry entry{};
          entry.path = path.c_str();
          entry.mode = GIT_FILEMODE_BLOB;
          CHECK_GIT(git_oid_fromstrn(&entry.id, blobId.c_str(), blobId.size()));
          if (!git_odb_exists(odb, &entry.id)) {
            RESPOND_ERROR(strCat("Blob not found: ", blobId));
          }
          CHECK_GIT(git_index_add(mergeIndex, &entry));
        }
      }

      bool hasInspectFiles = response.reader().getMerge().getInspect().size() > 0;
      bool inspectBlockingSync = hasInspectFiles && !req.getSyncParams().getInspectComplete();
      bool needsAction = neededDecisions > 0 || inspectBlockingSync;

      response.builder().getMerge().setStatus(
        needsAction ? G8MergeAnalysis::MergeStatus::NEEDS_ACTION
                    : G8MergeAnalysis::MergeStatus::COMPLETE);

      if (!needsAction && !isDry) {
        // Not more decisions needed, so commit the rebase patch.
        GIT_PTR(signature, signature);
        CHECK_GIT(git_signature_default(&signature, ctx.repo.get()));

        String message =
          strCat("[g8] sync ", client, " onto ", git_oid_tostr_s(git_commit_id(destCommit)));

        CHECK_GIT(git_commit_tree(&destTree, destCommit));
        const git_oid *destTreeOid = git_tree_id(destTree);

        git_oid rebaseTreeOid;

        CHECK_GIT(git_index_write_tree_to(&rebaseTreeOid, mergeIndex, ctx.repo.get()));

        git_oid rebaseCommitOid;
        if (git_oid_equal(&rebaseTreeOid, destTreeOid)) {
          // The rebase tree is the same as the commit we are rebasing.
          git_oid_cpy(&rebaseCommitOid, git_commit_id(destCommit));
        } else {
          // The rebase tree is different from the commit which we are rebasing, so we need a
          // commit. Commit the rebase to properly apply changes.
          CHECK_GIT(git_rebase_commit(
            &rebaseCommitOid, rebase, signature, signature, nullptr, message.c_str()));
        }

        // Checkout the rebase when we are on the active client.
        if (onActiveClient) {
          GIT_PTR(commit, rebaseCommit);
          CHECK_GIT(git_commit_lookup(&rebaseCommit, ctx.repo.get(), &rebaseCommitOid));
          GIT_PTR(tree, rebaseTree);
          git_object_peel(
            reinterpret_cast<git_object **>(&rebaseTree),
            reinterpret_cast<git_object *>(rebaseCommit),
            GIT_OBJECT_TREE);

          // Reset the working directory to the current commit.
          git_checkout_options checkoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
          checkoutOpts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_REMOVE_SPARSE_FILES;

          CHECK_GIT(git_checkout_tree(
            ctx.repo.get(), reinterpret_cast<git_object *>(rebaseCommit), &checkoutOpts));

          auto postMergeRes = postMergeHook(ctx.repo.get(), MergeType::STANDARD);
          if (postMergeRes && *postMergeRes) {
            // TODO(pawel) report non-0 return code?
            // post-merge hook cannot effect the outcome of the merge
          }

          // During sparse checkout, entries that are skipped are not updated in the index.
          // This leads git status to believe that they are staged.
          // By making the work index, we force the repository index to be updated.
          GIT_PTR(config, config);
          CHECK_GIT(git_repository_config(&config, ctx.repo.get()));
          int sparseEnabled = 0;
          if (
            GIT_OK == git_config_get_bool(&sparseEnabled, config, "core.sparseCheckout")
            && sparseEnabled) {
            GIT_PTR(index, syncedIndex);
            CHECK_GIT(makeWorkIndex(ctx.repo.get(), &syncedIndex, rebaseTree));
          }
        }

        // Move the client to the rebase commit. This is the new save commit. Only do it for sync.
        if (isSyncAction) {
          GIT_PTR(reference, newClientRef);
          CHECK_GIT(
            git_reference_set_target(&newClientRef, clientRef, &rebaseCommitOid, message.c_str()));
          std::swap(newClientRef, clientRef);
          // NOTE(pawel) It is expected that a caller who wants to skip the push calls saveClient
          // with the forceSave flag set right after syncing.
          if (!req.getSkipPushAfterSync()) {
            CHECK_GIT(g8GitForcePush(req.getAuth(), ctx.repo.get(), clientRef));
          }
        }
      }
    }
  }

  {
    auto responseClient = response.builder().initClient(1);

    responseClient[0].setName(client);
    responseClient[0].setActive(true);
  }

  return response;
}

}  // namespace c8
