#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//c8/git:internal",
  };
  hdrs = {
    "api-client.h",
  };
  deps = {
    "//c8/git:api-common",
    "//c8/git:g8-api.capnp-cc",
    "//c8/git:hooks",
    "//c8:scope-exit",
    "//c8/stats:scope-timer",
    "//c8:c8-log",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0x3dc789f5);

#include <git2.h>
#include <git2/oid.h>
#include <git2/refs.h>
#include <git2/types.h>

#include "c8/c8-log.h"
#include "c8/git/api-client/api-client.h"
#include "c8/git/api-common.h"
#include "c8/git/hooks.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

using namespace g8;

ConstRootMessage<G8ClientResponse> g8ClientSwitch(ClientContext &ctx) {
  ScopeTimer t("api-client-switch");
  auto req = ctx.request.reader();
  auto &response = ctx.response;

  const bool forceSave = req.getForceSave();

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

  GIT_PTR(reference, activeClient);
  {
    GIT_PTR(reference, head);
    CHECK_GIT(git_reference_lookup(&head, ctx.repo.get(), "HEAD"));
    CHECK_GIT(git_reference_resolve(&activeClient, head));
  }

  {
    ScopeTimer t("switch-client");
    if (req.getClient().size() != 1) {
      RESPOND_ERROR("Must specify exactly 1 client for SWITCH action.");
    }

    auto [gitStatus, validClient] = branchIsValidClient(ctx.repo.get(), req.getClient()[0].cStr());
    CHECK_GIT(gitStatus);

    if (!validClient) {
      RESPOND_ERROR("Target is not a valid g8 client.");
    }

    // Save changes in the active client before switching.
    if (clientNameIsValid(ctx.activeClientName.c_str())) {
      std::tie(gitStatus, validClient) =
        branchIsValidClient(ctx.repo.get(), ctx.activeClientName.c_str());
      CHECK_GIT(gitStatus);
      if (validClient) {
        CHECK_GIT(g8GitSave(req.getAuth(), ctx.repo.get(), forceSave));
      } else {
        C8Log(
          "[g8-git] Branch (%s) is not a valid client, not saving before switching.",
          ctx.activeClientName.c_str());
      }
    }

    const char *inputClient = req.getClient()[0].cStr();

    auto [branchName, foundBranch] = findBranchByName(ctx.repo.get(), inputClient);

    if (!foundBranch) {
      RESPOND_ERROR("Could not find branch associated with target client");
    }

    GIT_PTR(reference, newActiveClient);
    CHECK_GIT(git_branch_lookup(&newActiveClient, ctx.repo.get(), branchName, GIT_BRANCH_LOCAL));

    git_checkout_options checkoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
    checkoutOpts.checkout_strategy = GIT_CHECKOUT_SAFE | GIT_CHECKOUT_REMOVE_SPARSE_FILES;

    GIT_PTR(object, treeish);
    CHECK_GIT(git_reference_peel(&treeish, newActiveClient, GIT_OBJECT_TREE));
    CHECK_GIT(git_checkout_tree(ctx.repo.get(), treeish, &checkoutOpts));

    {
      auto postCheckoutRes = postCheckoutHook(
        ctx.repo.get(),
        git_reference_name(activeClient),     // previous HEAD
        git_reference_name(newActiveClient),  // new HEAD
        CheckoutType::BRANCH);
      if (postCheckoutRes && *postCheckoutRes != 0) {
        // TODO(pawel) notify hook returned non-zero value?
      }
    }

    CHECK_GIT(git_repository_set_head(ctx.repo.get(), git_reference_name(newActiveClient)));
    std::swap(activeClient, newActiveClient);
    ctx.activeClientName = git_reference_shorthand(activeClient);
  }

  writeResponseClients(ctx);

  return response;
}

}  // namespace c8
