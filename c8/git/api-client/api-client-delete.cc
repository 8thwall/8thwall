#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:private",
  };
  deps = {
    "//c8/git:api-common",
    "//c8/git:g8-api.capnp-cc",
    "//c8:scope-exit",
    "//c8/stats:scope-timer",
    "//c8:vector",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0x6aa9b8bf);

#include <git2.h>
#include <git2/oid.h>
#include <git2/refs.h>
#include <git2/types.h>

#include "c8/git/api-common.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"

namespace c8 {

using namespace g8;

ConstRootMessage<G8ClientResponse> g8ClientDelete(ClientContext &ctx) {
  ScopeTimer t("api-client-delete");
  auto req = ctx.request.reader();
  auto &response = ctx.response;

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
    ScopeTimer t("delete-client");
    for (const auto client : req.getClient()) {
      auto [branchName, foundBranch] = findBranchByName(ctx.repo.get(), client.cStr());
      if (!foundBranch) {
        RESPOND_ERROR("Could not find branch associated with client");
      }

      const auto [lsGitRes, changesetList] = g8GitLsChangesets(ctx.repo.get(), branchName);
      CHECK_GIT(lsGitRes);

      GIT_PTR(remote, remote);
      CHECK_GIT(git_remote_lookup(&remote, ctx.repo.get(), ORIGIN));
      Vector<String> changesetsToDelete;
      Vector<String> deleteBranchRefspecs;

      for (const auto &changeset : changesetList.changesets) {
        String changesetFullName =
          strCat(CHANGESETS_PREFIX, branchName, "-", CHANGESET_CODE, changeset.name);

        changesetsToDelete.push_back(changesetFullName);

        GIT_PTR(reference, csRef);
        if (0 == git_reference_lookup(&csRef, ctx.repo.get(), changesetFullName.c_str())) {
          GIT_PTR(reference, remoteRef);
          if (0 == git_branch_upstream(&remoteRef, csRef)) {
            // If the changeset has an upstream branch, add a refspec to delete it.
            git_buf remoteRefName = {0};
            SCOPE_EXIT([&remoteRefName] { git_buf_dispose(&remoteRefName); });
            CHECK_GIT(git_refspec_rtransform(
              &remoteRefName, git_remote_get_refspec(remote, 0), git_reference_name(remoteRef)));
            deleteBranchRefspecs.push_back(String(":") + remoteRefName.ptr);
          }
        }
      }

      GIT_PTR(reference, clientRef);

      // TODO(mc): Maybe add some feedback on success.
      if (0 == git_branch_lookup(&clientRef, ctx.repo.get(), branchName, GIT_BRANCH_LOCAL)) {
        const bool clientIsHead = git_branch_is_head(clientRef);
        if (clientIsHead) {
          // Save changes in the active client before deleting to remove untracked files.
          // TODO(pawel) Add option to omit pushing remote even if there are changes saved
          // since this branch is about to be deleted remotely anyways.
          CHECK_GIT(g8GitSave(req.getAuth(), ctx.repo.get(), false));

          // Update the clientRef after saving.
          GIT_PTR(reference, newClientRef);
          CHECK_GIT(git_branch_lookup(&newClientRef, ctx.repo.get(), branchName, GIT_BRANCH_LOCAL));
          std::swap(newClientRef, clientRef);

          // We checkout out the latest master.
          GIT_PTR(object, masterTree);
          CHECK_GIT(git_reference_peel(&masterTree, ctx.master.get(), GIT_OBJECT_TREE));
          git_checkout_options checkoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
          checkoutOpts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_REMOVE_SPARSE_FILES;
          git_checkout_tree(ctx.repo.get(), masterTree, &checkoutOpts);
          git_repository_set_head(ctx.repo.get(), git_reference_name(ctx.master.get()));
        }

        // Get the upstream remote ref.
        GIT_PTR(reference, remoteRef);
        if (0 == git_branch_upstream(&remoteRef, clientRef)) {
          git_buf remoteRefName = {0};
          SCOPE_EXIT([&remoteRefName] { git_buf_dispose(&remoteRefName); });
          git_refspec_rtransform(
            &remoteRefName, git_remote_get_refspec(remote, 0), git_reference_name(remoteRef));
          deleteBranchRefspecs.push_back(String(":") + remoteRefName.ptr);
        }

        // Do all of the remote deletes.
        std::unique_ptr<char *[]> refs(new char *[deleteBranchRefspecs.size()]);
        std::transform(
          deleteBranchRefspecs.begin(),
          deleteBranchRefspecs.end(),
          refs.get(),
          [](String &spec) -> char * {
            // Pre C++17 is missing non-const String::data() member. This won't be mutated
            // anyway.
            return const_cast<char *>(spec.data());
          });
        git_strarray specs = {refs.get(), deleteBranchRefspecs.size()};
        git_push_options pushOpts = GIT_PUSH_OPTIONS_INIT;
        auto authReader = req.getAuth();
        pushOpts.callbacks = gitRemoteCallbacks(authReader);
        pushOpts.callbacks.payload = reinterpret_cast<void *>(&authReader);

        String cred = authReader.hasCred() ? authReader.getCred() : "";
        bool useCreds = !cred.empty();
        auto authStr = format("Authorization: %s", cred.c_str());
        char *authCStr = const_cast<char *>(authStr.c_str());
        if (useCreds) {
          pushOpts.custom_headers.count = 1;
          pushOpts.custom_headers.strings = &authCStr;
        }

        {
          ScopeTimer t("git-remote-push");
          CHECK_GIT(git_remote_push(remote, &specs, &pushOpts));
        }

        for (auto csName : changesetsToDelete) {
          GIT_PTR(reference, csRef);
          if (0 == git_reference_lookup(&csRef, ctx.repo.get(), csName.c_str())) {
            CHECK_GIT(git_branch_set_upstream(csRef, nullptr));
            git_reference_delete(csRef);
          }
        }

        CHECK_GIT(git_branch_set_upstream(clientRef, nullptr));
        git_branch_delete(clientRef);
      }
    }
  }

  writeResponseClients(ctx);

  return response;
}

}  // namespace c8
