// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_binary {
  name = "g8-git-js";
  deps = {
    ":g8-git",
    "//c8:c8-log",
    "//c8:string",
    "//c8:c8-log-proto",
    "//c8:symbol-visibility",
    "//c8/stats:scope-timer",
  };
}
cc_end(0xf6487735);

#ifdef JAVASCRIPT

#include <emscripten.h>

#include <optional>
#include <variant>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/git/g8-git.h"
#include "c8/symbol-visibility.h"
#include "c8/stats/scope-timer.h"

namespace {

using c8::ScopeTimer;

struct GitData {
  c8::ConstRootMessage<c8::G8RepositoryResponse> repositoryResponse;
  c8::ConstRootMessage<c8::G8ClientResponse> clientResponse;
  c8::ConstRootMessage<c8::G8ChangesetResponse> changesetResponse;
  c8::ConstRootMessage<c8::G8FileResponse> fileResponse;
  c8::ConstRootMessage<c8::G8DiffBlobsResponse> diffBlobsResponse;
  c8::ConstRootMessage<c8::G8BlobResponse> getBlobResponse;
  c8::ConstRootMessage<c8::G8MergeFileResponse> mergeFileResponse;
  c8::ConstRootMessage<c8::G8CreateBlobResponse> createBlobResponse;
  c8::ConstRootMessage<c8::G8DiffResponse> diffResponse;
  c8::ConstRootMessage<c8::G8InspectResponse> inspectResponse;
};

GitData &data() {
  static GitData d;
  return d;
}

// NOTE(pawel) output/errors from other workers/main event loop can become hidden in here
struct ConsoleGroup {
  ConsoleGroup(const char *title) {
    if (!c8::releaseConfigIsConsoleLoggingEnabled()) {
      return;
    }
    EM_ASM_({ console.groupCollapsed(UTF8ToString($0)); }, title);
  }
  ~ConsoleGroup() {
    if (!c8::releaseConfigIsConsoleLoggingEnabled()) {
      return;
    }
    EM_ASM_({ console.groupEnd(); });
  }
};

}  // namespace

extern "C" {

C8_PUBLIC
void c8EmAsm_g8Repository(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8Repository");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8Repository");
  auto request = c8::ConstRootMessage<c8::G8RepositoryRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().repositoryResponse = c8::g8Repository(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().repositoryResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().repositoryResponse.bytes().begin(),
    data().repositoryResponse.bytes().size());
  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8Repository");
}

C8_PUBLIC
void c8EmAsm_g8Client(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8CLient");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8Client");
  auto request = c8::ConstRootMessage<c8::G8ClientRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().clientResponse = c8::g8Client(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().clientResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().clientResponse.bytes().begin(),
    data().clientResponse.bytes().size());
  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8Client");
}

C8_PUBLIC
void c8EmAsm_g8Changeset(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8Changeset");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8Changeset");
  auto request = c8::ConstRootMessage<c8::G8ChangesetRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().changesetResponse = c8::g8Changeset(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().changesetResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().changesetResponse.bytes().begin(),
    data().changesetResponse.bytes().size());
  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8Changeset");
}

C8_PUBLIC
void c8EmAsm_g8File(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8File");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8File");
  auto request = c8::ConstRootMessage<c8::G8FileRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().fileResponse = c8::g8FileRequest(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().fileResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().fileResponse.bytes().begin(),
    data().fileResponse.bytes().size());
  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8File");
}

C8_PUBLIC
void c8EmAsm_g8DiffBlobs(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8DiffBlobs");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8DiffBlobs");
  auto request = c8::ConstRootMessage<c8::G8DiffBlobsRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().diffBlobsResponse = c8::g8DiffBlobs(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().diffBlobsResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().diffBlobsResponse.bytes().begin(),
    data().diffBlobsResponse.bytes().size());
  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8DiffBlobs");
}

C8_PUBLIC
void c8EmAsm_g8GetBlob(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8GetBlob");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8GetBlob");
  auto request = c8::ConstRootMessage<c8::G8BlobRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().getBlobResponse = c8::g8GetBlob(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().getBlobResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().getBlobResponse.bytes().begin(),
    data().getBlobResponse.bytes().size());
  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8GetBlob");
}

C8_PUBLIC
void c8EmAsm_g8MergeFile(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8MergeFile");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8MergeFile");
  auto request = c8::ConstRootMessage<c8::G8MergeFileRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().mergeFileResponse = c8::g8MergeFile(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().mergeFileResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().mergeFileResponse.bytes().begin(),
    data().mergeFileResponse.bytes().size());
  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8MergeFile");
}

C8_PUBLIC
void c8EmAsm_g8CreateBlob(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8CreateBlob");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8CreateBlob");
  auto request = c8::ConstRootMessage<c8::G8CreateBlobRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().createBlobResponse = c8::g8CreateBlob(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().createBlobResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().createBlobResponse.bytes().begin(),
    data().createBlobResponse.bytes().size());
  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8CreateBlob");
}

C8_PUBLIC
void c8EmAsm_g8Diff(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8Diff");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8Diff");
  auto request = c8::ConstRootMessage<c8::G8DiffRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().diffResponse = c8::g8Diff(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().diffResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().diffResponse.bytes().begin(),
    data().diffResponse.bytes().size());

  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8Diff");
}

C8_PUBLIC
void c8EmAsm_g8Inspect(uint8_t *ptr, int size) {
  ScopeTimer t("g8-git-js");
  ConsoleGroup cg("g8Inspect");
  c8::C8Log("[g8-git-js] vvvvvvvvvvvvvvvvvvvv %s vvvvvvvvvvvvvvvvvvvv", "c8EmAsm_g8Inspect");
  auto request = c8::ConstRootMessage<c8::G8InspectRequest>(ptr, size);

  c8::C8Log("[g8-git-js] %s", "Sent request:\n------------------------");
  c8::C8LogCapnpMessage(request.reader());

  {
    ScopeTimer t1("api-call");
    data().inspectResponse = c8::g8Inspect(request.reader());
  }

  c8::C8Log("[g8-git-js] %s", "Got response:\n------------------------");
  c8::C8LogCapnpMessage(data().inspectResponse.reader());

  EM_ASM_(
    {
      _c8.g8Response = $0;
      _c8.g8ResponseSize = $1;
    },
    data().inspectResponse.bytes().begin(),
    data().inspectResponse.bytes().size());


  c8::C8Log("[g8-git-js] ^^^^^^^^^^^^^^^^^^^^ %s ^^^^^^^^^^^^^^^^^^^^", "c8EmAsm_g8Inspect");
}

C8_PUBLIC
void c8EmAsm_printG8Stats() {
  ScopeTimer::logBriefSummary();
  ScopeTimer::reset();
}

}  // extern "C"

#endif  // JAVASCRIPT
