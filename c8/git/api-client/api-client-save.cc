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
    "//c8/stats:scope-timer",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0xd7a074f0);

#include "c8/git/api-common.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

using namespace g8;

ConstRootMessage<G8ClientResponse> g8ClientSave(ClientContext &ctx) {
  ScopeTimer t("api-client-save");
  auto req = ctx.request.reader();
  auto &response = ctx.response;

  for (const auto client : req.getClient()) {
    if (!clientNameIsValid(client.cStr())) {
      RESPOND_ERROR(format("Invalid client name %s", client.cStr()));
    }
  }

  if (req.getClient().size() > 1) {
    RESPOND_ERROR("SAVE action requires exactly one client");
  }
  if (req.getClient().size() == 1 && req.getClient()[0] != ctx.activeClientName) {
    RESPOND_ERROR("SAVE action only valid on the active client.");
  }

  CHECK_GIT(g8GitSave(req.getAuth(), ctx.repo.get(), req.getForceSave()));

  auto responseClient = response.builder().initClient(1);
  responseClient[0].setName(ctx.activeClientName);
  responseClient[0].setActive(true);

  return response;
}

}  // namespace c8
