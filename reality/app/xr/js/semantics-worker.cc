// Copyright (c) 2022 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_binary {
  name = "semantics-worker-cc";
  deps = {
    "//c8:c8-log",
    "//c8:symbol-visibility",
    "//c8:string",
    "//c8/string:format",
    "//c8:vector",
    "//c8/protolog:xr-requests",
    "//reality/engine/executor:xr-engine",
    "//reality/engine/semantics:semantics-classifier",
    "@json//:json",
  };
}
cc_end(0x0a11c6c2);

#ifdef JAVASCRIPT

#include <emscripten.h>
#include <emscripten/bind.h>

#include <nlohmann/json.hpp>

#include "c8/c8-log.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/symbol-visibility.h"
#include "reality/engine/semantics/semantics-classifier.h"

using namespace c8;

namespace {

struct SemanticsWorkerData {
  std::unique_ptr<SemanticsClassifier> semClassifier;

  int count = 0;
};

std::unique_ptr<SemanticsWorkerData> &data() {
  static std::unique_ptr<SemanticsWorkerData> d(nullptr);
  if (d == nullptr) {
    d.reset(new SemanticsWorkerData());
  }
  return d;
}

void semanticsWorkerStart() {
  EM_ASM_({
    self._c8 = {};
    self._c8.version = UTF8ToString($0, $1);
    self._c8.hasFetch = $2;
  });
}

void loadSemanticsModel(uint8_t *model, int modelSize) {
  data()->semClassifier = std::make_unique<SemanticsClassifier>(model, modelSize);
}

void semanticsWorkerCleanup() { data()->semClassifier.reset(nullptr); }

void processStagedSemanticsWorkerFrame(uint8_t *bytes, int rows, int cols) {
  int inputWidth = data()->semClassifier->getInputWidth();
  int inputHeight = data()->semClassifier->getInputHeight();
  // Only the first Sky class is needed.
  int dataSize = 4 * inputWidth * inputHeight;

  // Copy image to send to semantics classifier.
  auto semanticsResizeResult = RGBA8888PlanePixels(rows, cols, cols * 4, bytes);

  // Get semantics.
  auto semanticsFloatPixels = data()->semClassifier->obtainSemantics(semanticsResizeResult);

  data()->count += 1;

  EM_ASM_(
    {
      self._c8.response = $0;
      self._c8.responseSize = $1;
      self._c8.responseReceived = $2;
    },
    semanticsFloatPixels.pixels(),
    dataSize,
    data()->count);
}

}  // namespace

extern "C" {

C8_PUBLIC
void c8EmAsm_semanticsWorkerStart() { semanticsWorkerStart(); }

C8_PUBLIC
void c8EmAsm_loadSemanticsModel(uint8_t *model, int modelSize) {
  loadSemanticsModel(model, modelSize);
}

C8_PUBLIC
void c8EmAsm_semanticsWorkerCleanup() { semanticsWorkerCleanup(); }

// Tock for gpu processing, take a previously staged input, read its
// result and do processing.
C8_PUBLIC
void c8EmAsm_processStagedSemanticsWorkerFrame(uint8_t *bytes, int rows, int cols) {
  ScopeTimer t("c8EmAsm_processStagedSemanticsWorkerFrame");
  processStagedSemanticsWorkerFrame(bytes, rows, cols);
}

}  // EXTERN "C"
#else
#warning "semantics-worker-cc requires --cpu=js"
#endif
