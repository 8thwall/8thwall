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
cc_end(0x56f418ab);

// #include <capnp/pretty-print.h>
#include <git2.h>
#include <git2/oid.h>
#include <git2/refs.h>
#include <git2/types.h>

#include "c8/git/api-common.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

using namespace g8;
using Action = G8ClientRequest::Action;

ConstRootMessage<G8ClientResponse> g8ClientList(ClientContext &ctx) {
  ScopeTimer t("api-client-list");
  auto req = ctx.request.reader();
  auto &response = ctx.response;

  if (req.getIncludeCloudSaveInfo()) {
    if (!req.hasUser()) {
      RESPOND_ERROR("User is required for fetching cloud save info");
    }
  }

  writeResponseClients(ctx);

  return response;
}

}  // namespace c8
