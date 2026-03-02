// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Singleton version of MediaRecorder with C-API for use in Javascript.

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/bind.h>

#include <nlohmann/json.hpp>

#include "c8/media/media-recorder.h"
#include "c8/media/media-transcoder.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/symbol-visibility.h"

namespace c8 {
namespace {

struct MediaTranscoderWasmGetInfoResult {
  int status = 0;
  std::string inputInfo;
  std::string outputInfo;
};

// Get access to the global singleton using C++11 magic statics.
c8::MediaRecorder &getMediaRecorder() {
  static c8::MediaRecorder instance;
  return instance;
}

// Get access to the global singleton using C++11 magic statics.
c8::MediaTranscoder &getMediaTranscoder() {
  static c8::MediaTranscoder instance;
  return instance;
}

}  // namespace

EM_JS(void, postMediaError, (int code, const char *msg, int msgBytes), {postMessage({
        type : 'error',
        code : code,
        message : UTF8ToString(msg, msgBytes),
      })});

// Run eval on code c of length b bytes and write the result to d up to a max
// allocation a.
EM_JS(void, jseval, (const char *c, int b, void *d, int a), {
  stringToUTF8(eval(UTF8ToString(c, b)), d, a);
});

int mediaRecorderWasmStart(const std::string &config) {
  c8::MediaRecorder &recorder = getMediaRecorder();
  auto rv = recorder.start(config);
  if (rv.code() != 0) {
    postMediaError(rv.code(), rv.message(), strlen(rv.message()));
  }
  return rv.code();
}

int mediaRecorderWasmEncode(const std::string &params, uintptr_t data, size_t byteSize) {
  c8::MediaRecorder &recorder = getMediaRecorder();
  // Embind can't pass pointers to raw primitives, so we send at a uintptr_t number.
  auto rv = recorder.encode(params, reinterpret_cast<const uint8_t *>(data), byteSize);
  if (rv.code() != 0) {
    postMediaError(rv.code(), rv.message(), strlen(rv.message()));
  }
  return rv.code();
}

int mediaRecorderWasmFinish() {
  c8::MediaRecorder &recorder = getMediaRecorder();
  auto rv = recorder.finish();
  if (rv.code() != 0) {
    postMediaError(rv.code(), rv.message(), strlen(rv.message()));
  }
  return rv.code();
}

int mediaTranscoderWasmStart(const std::string &inputConfig, const std::string &outputConfig) {
  c8::MediaTranscoder &transcoder = getMediaTranscoder();
  auto rv = transcoder.open(inputConfig, outputConfig);
  return rv.code();
}

MediaTranscoderWasmGetInfoResult mediaTranscoderWasmGetInfo() {
  MediaTranscoderWasmGetInfoResult result;
  c8::MediaTranscoder &transcoder = getMediaTranscoder();
  result.status = transcoder.getInfo(&result.inputInfo, &result.outputInfo).code();

  return result;
}

int mediaTranscoderWasmEncode() {
  c8::MediaTranscoder &transcoder = getMediaTranscoder();
  std::string metadata;
  auto rv = transcoder.transcode(&metadata);
  return rv.code();
}

int mediaTranscoderWasmFinish() {
  c8::MediaTranscoder &transcoder = getMediaTranscoder();
  auto rv = transcoder.close();
  return rv.code();
}

EMSCRIPTEN_BINDINGS(Mr8cc_Module) {
  emscripten::function("mediaRecorderStart", &mediaRecorderWasmStart);
  emscripten::function("mediaRecorderEncode", &mediaRecorderWasmEncode);
  emscripten::function("mediaRecorderFinish", &mediaRecorderWasmFinish);
  emscripten::function("mediaTranscoderStart", &mediaTranscoderWasmStart);
  emscripten::function("mediaTranscoderGetInfo", &mediaTranscoderWasmGetInfo);
  emscripten::function("mediaTranscoderEncode", &mediaTranscoderWasmEncode);
  emscripten::function("mediaTranscoderFinish", &mediaTranscoderWasmFinish);
  emscripten::value_object<MediaTranscoderWasmGetInfoResult>("MediaTranscoderWasmGetInfoResult")
    .field("status", &MediaTranscoderWasmGetInfoResult::status)
    .field("inputInfo", &MediaTranscoderWasmGetInfoResult::inputInfo)
    .field("outputInfo", &MediaTranscoderWasmGetInfoResult::outputInfo);
}

}  // namespace c8

#endif
