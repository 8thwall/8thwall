// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Akul Gupta (akulgupta@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"decode-mesh.h"};
  deps = {
    "//c8:map",
    "//c8:c8-log",
    "//c8/pixels:base64",
    "//c8/geometry:mesh-types",
    "//c8/geometry:load-mesh"};
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x09d4e375);

#include <thread>

#include "c8/c8-log.h"
#include "c8/geometry/decode-mesh.h"
#include "c8/geometry/load-mesh.h"
#include "c8/pixels/base64.h"

#ifdef JAVASCRIPT
#include <emscripten.h>
#endif

namespace c8 {

namespace {

// Note(akul): We always want to use block mesh decoding for deterministic imgui/benchmark results.
// Consider removing native async decode code.
static bool useDecodeMeshBlocking = true;

#if defined(JAVASCRIPT) && !defined(JSRUN)
EM_JS(void, decodeMeshJs, (const char *meshId, const char *encodedBase64Mesh), {
  meshId = UTF8ToString(meshId);
  encodedBase64Mesh = UTF8ToString(encodedBase64Mesh);
  window.postMessage({type : "loadMesh", meshId, encodedBase64Mesh}, window.location.origin);
});
#endif

void decodeMeshBlockingCpp(
  const String &meshId, const String &encodedBase64Mesh, const MeshDecodedCb &callback) {
  auto base64EncodedDracoMesh = decode(encodedBase64Mesh);
  auto maybeDecodedMesh = dracoDataToMesh(&base64EncodedDracoMesh, true);
  if (maybeDecodedMesh.has_value()) {
    // Sets the decoded mesh on vps-mesh-manager.
    callback(meshId, std::move(*maybeDecodedMesh));
  } else {
    // TODO(akul): handle failure case.
    C8Log("[decode-mesh] Failed to decode meshId: %s", meshId.c_str());
  }
}

void decodeMeshCpp(
  const String &meshId, const String &encodedBase64Mesh, const MeshDecodedCb &callback) {
  // NOTE(akul): There shouldn't be a case where we have to handle a terminate of the thread
  // with std::set_terminate.
  std::thread{decodeMeshBlockingCpp, meshId, encodedBase64Mesh, callback}.detach();
}
}  // namespace

void forceDecodeMeshBlockingCpp() { useDecodeMeshBlocking = true; }

void decodeMesh(
  const String &meshId, const String &encodedBase64Mesh, const MeshDecodedCb &callback) {
#if defined(JAVASCRIPT) && !defined(JSRUN)
  decodeMeshJs(meshId.c_str(), encodedBase64Mesh.c_str());
#else
  if (useDecodeMeshBlocking) {
    decodeMeshBlockingCpp(meshId, encodedBase64Mesh, callback);
  } else {
    decodeMeshCpp(meshId, encodedBase64Mesh, callback);
  }
#endif
}
}  // namespace c8
