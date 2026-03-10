// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#ifdef JAVASCRIPT

#include <emscripten.h>

#include <cstdint>

#include "c8/geometry/intrinsics.h"
#include "c8/pixels/pipeline/gl-texture-pipeline.h"
#include "c8/stats/scope-timer.h"
#include "c8/string-view.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/symbol-visibility.h"
#include "reality/app/xr/js/xrweb.h"

using namespace c8;

namespace {

struct Data {
  GlTexturePipeline pipeline;
  Vector<std::unique_ptr<ScopeTimer>> traceStack;
};

Data &data() {
  static Data d;
  return d;
}

}  // namespace

namespace c8 {

void pushTrace(const String &tag) { data().traceStack.emplace_back(new ScopeTimer{tag}); }

void popTrace() {
  if (data().traceStack.empty()) {
    C8Log("[xr-js] %s", "popTrace called with empty stack");
    return;
  }
  data().traceStack.pop_back();
}

}  // namespace c8

namespace {

void flushTrace() {
  if (data().traceStack.empty()) {
    return;  // noop
  }
  C8Log(
    "[xr-js] flushTrace called with non-empty root '%s'",
    data().traceStack.back()->loggingContext()->tag().c_str());
  // Get rid of the current trace root without exporting it. Normally this shouldn't happen if push
  // and pop are reliably paired; but this can help us recover from exceptional conditions.
  while (!data().traceStack.empty()) {
    data().traceStack.pop_back();
  }
}

}  // namespace

extern "C" {

C8_PUBLIC
void c8EmAsm_initDevice() {
  bool hasFetch = false;
  EM_ASM_(
    {
      window._c8 = {};
      window._c8.version = UTF8ToString($0, $1);
      window._c8.hasFetch = $2;
    },
    releaseConfigBuildId().c_str(),
    releaseConfigBuildId().size(),
    hasFetch);
}

// Call only after initDevice
C8_PUBLIC
void c8EmAsm_getDeviceModelInfo(uint8_t *deviceInfo, size_t deviceInfoSize) {
  ConstRootMessage<DeviceInfo> deviceInfoMsg{deviceInfo, deviceInfoSize};
  auto deviceInfoReader = deviceInfoMsg.reader();
  auto m = DeviceInfos::getDeviceModel(deviceInfoReader);
  auto modelInfo = getModelInfo(m);
  auto K = Intrinsics::getCameraIntrinsics(m);
  EM_ASM_(
    {
      window._c8.deviceModel = UTF8ToString($0, $1);
      window._c8.focalLength = $2;
      window._c8.performanceIndex = $3;
    },
    modelInfo.name.c_str(),
    modelInfo.name.size(),
    K.focalLengthHorizontal,
    0.75 * modelInfo.singleCore + 0.25 * modelInfo.multiCore);
}

C8_PUBLIC
void c8EmAsm_initCameraRuntime(
  int captureWidth, int captureHeight, int displayWidth, int displayHeight, bool isWebGl2) {
  xrwebEnvironment().displayWidth = displayWidth;
  xrwebEnvironment().displayHeight = displayHeight;
  xrwebEnvironment().isWebGl2 = isWebGl2;

  EM_ASM_(
    {
      window._c8.displayWidth = $0;
      window._c8.displayHeight = $1;
      window._c8.useWebGl2 = $2;
    },
    displayWidth,
    displayHeight,
    isWebGl2);

  data().pipeline.initialize(captureWidth, captureHeight, 1, 9);
}

C8_PUBLIC
void c8EmAsm_logSummary() { ScopeTimer::logBriefSummary(); }

C8_PUBLIC
void c8EmAsm_getReadyInPipeline(int captureWidth, int captureHeight) {
  EM_ASM_(
    { window._c8.readyTexture = GL.textures[$0]; },
    data().pipeline.getReady(captureWidth, captureHeight));
}

C8_PUBLIC
void c8EmAsm_markTextureFilledInPipeline() { data().pipeline.markTextureFilled(); }

C8_PUBLIC
void c8EmAsm_markStageFinishedInPipeline() { data().pipeline.markStageFinished(); }

C8_PUBLIC
void c8EmAsm_releaseInPipeline(GLuint textureId) { data().pipeline.release(textureId); }

C8_PUBLIC
void c8EmAsm_pushTrace(char *tag) { pushTrace(tag); }

C8_PUBLIC
void c8EmAsm_popTrace() { popTrace(); }

C8_PUBLIC
void c8EmAsm_flushTrace() { flushTrace(); }

C8_PUBLIC void c8EmAsm_onCanvasSizeChange(int displayWidth, int displayHeight) {
  xrwebEnvironment().displayWidth = displayWidth;
  xrwebEnvironment().displayHeight = displayHeight;
}

C8_PUBLIC
void c8EmAsm_setCheckAllZeroes(bool checkAllZeroes) {
  EM_ASM_({ window._c8.checkAllZeroes = $0; }, checkAllZeroes);
}

C8_PUBLIC
void c8EmAsm_resetOnPipelineStop() { data().pipeline = GlTexturePipeline(); }

}  // EXTERN "C"
#else
#warning "xr-js-cc requires --cpu=js"
#endif
