#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:private",
  };
  deps = {
    "//c8/git:api-common",
    "//c8/git:g8-api.capnp-cc",
    "//c8/stats:scope-timer",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0x53e9cc23);

#include <git2.h>
#include <git2/oid.h>
#include <git2/refs.h>
#include <git2/types.h>

#include "c8/git/api-common.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

using namespace g8;

ConstRootMessage<G8ClientResponse> g8ClientCreate(ClientContext &ctx) {
  ScopeTimer t("api-client-create");
  auto req = ctx.request.reader();
  auto &response = ctx.response;

  for (const auto client : req.getClient()) {
    if (!clientNameIsValid(client.cStr())) {
      RESPOND_ERROR(
        strCat(
          "Invalid client name: ",
          client.cStr(), 
          "; only numbers, letters, un-bounding dashes (-), and un-bounding underscores (_) ([a-zA-Z0-9])"));
    }
  }

  {
    ScopeTimer t("create-client");
    if (req.getUser().size() == 0) {
      RESPOND_ERROR("Must set user for a CREATE client action");
    }

    GIT_PTR(remote, remote);
    CHECK_GIT(git_remote_lookup(&remote, ctx.repo.get(), ORIGIN));

    GIT_PTR(object, commit);
    GIT_PTR(reference, clientRef);
    for (const auto client : req.getClient()) {
      CHECK_GIT(git_reference_peel(&commit, ctx.master.get(), GIT_OBJECT_COMMIT));
      CHECK_GIT(git_branch_create(
        &clientRef,
        ctx.repo.get(),
        client.cStr(),
        reinterpret_cast<git_commit *>(commit),
        /* int force */ 0));

      String remoteClientRefName = strCat(HEADS_PREFIX, req.getUser().cStr(), "-", client.cStr());
      // Create the refspec for the remote client branch.
      String remoteClientRef = strCat("+", HEADS_PREFIX, client.cStr(), ":", remoteClientRefName);

      // Create the branch on the remote, with the appropriate prefix.
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

      char *refs[] = {const_cast<char *>(remoteClientRef.c_str())};
      git_strarray specs = {refs, 1};
      int remoteResult;
      {
        ScopeTimer t("git-remote-push");
        remoteResult = git_remote_push(remote, &specs, &pushOpts);
      }
      if (0 != remoteResult) {
        const git_error *err = git_error_last();
        int code = err->klass;
        String msg = err->message;
        // If the remote branch creation failed, delete the local branch we just created.
        git_branch_delete(clientRef);

        // Replay the error.
        git_error_set_str(code, msg.c_str());
        CHECK_GIT(remoteResult);
      }
      // Set the upstream branch to track, e.g. origin/mc-myclient.
      String upstream = strCat(ORIGIN, "/", req.getUser().cStr(), "-", client.cStr());
      CHECK_GIT(git_branch_set_upstream(clientRef, upstream.c_str()));
    }
  }

  writeResponseClients(ctx);

  return response;
}

}  // namespace c8
