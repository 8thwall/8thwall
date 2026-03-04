#include "c8/xrapi/xrapi-addon.h"

#include <node.h>

#include <memory>
#include <optional>

#include "c8/c8-log.h"
#include "c8/map.h"
#include "c8/string.h"
#include "c8/xrapi/xrapi-manager.h"

using namespace c8;

XrApiManager &xrApiManagerSingleton() {
  static XrApiManager manager;
  return manager;
}

namespace {

double getNumberProperty(v8::Isolate *isolate, v8::Local<v8::Object> obj, const char *propName) {
  return obj
    ->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, propName).ToLocalChecked())
    .ToLocalChecked()
    ->NumberValue(isolate->GetCurrentContext())
    .FromJust();
};

}  // namespace

namespace c8 {

typedef std::shared_ptr<v8::Persistent<v8::Function>> PersistentFunction;
HashMap<String, PersistentFunction> persistentFunctions;

void updatePersistentFunctionMap(const String &key, PersistentFunction newPersistentFunction) {
  if (persistentFunctions.find(key) != persistentFunctions.end()) {
    persistentFunctions[key]->Reset();
  }

  persistentFunctions[key] = newPersistentFunction;
}

String SUPPORTED_MODES[] = {"inline", "immersive-vr", "immersive-ar"};

void isSessionSupported(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();

  // TODO(christoph): Implement proper string comparison
  // auto requestedMode = args[0].As<v8::String>().Utf8Value();

  bool found = true;
  // for (String mode : SUPPORTED_MODES) {
  //   if (mode == requestedMode) {
  //     found = true;
  //     break;
  //   }
  // }

  args.GetReturnValue().Set(v8::Boolean::New(isolate, found));
}

void requestSession(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();

  if (args.Length() != 3) {
    // TODO(christoph): Throw an error here
    return;
  }

  if (!xrApiManagerSingleton().canRequestSession()) {
    // TODO(christoph): Throw an error here
    return;
  }

  XrSessionRequest request;
  request.readyCallback = [&]() {
    v8::Local<v8::Function> readyCallback = args[1].As<v8::Function>();
    readyCallback->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 0, nullptr)
      .ToLocalChecked();
  };

  v8::Local<v8::Function> frameCallback = args[2].As<v8::Function>();
  PersistentFunction persistentFrameCallback =
    std::make_shared<v8::Persistent<v8::Function>>(isolate, frameCallback);
  updatePersistentFunctionMap("frameCallback", persistentFrameCallback);

  request.frameCallback = [persistentFrameCallback, isolate](const FrameCallbackData &data) {
    v8::HandleScope handleScope(isolate);
    auto buffer = v8::ArrayBuffer::New(isolate, data.size());
    memcpy(buffer->GetBackingStore()->Data(), data.data(), data.size());
    v8::Local<v8::Value> argv[1] = {buffer};
    v8::Local<v8::Function> localFunction = persistentFrameCallback->Get(isolate);

    localFunction->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 1, argv)
      .ToLocalChecked();
  };

  xrApiManagerSingleton().requestSession(request);
}

void tryGetReferenceSpacePose(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();

  v8::String::Utf8Value utf8Value(isolate, args[0].As<v8::String>());
  std::string spaceType(*utf8Value);

  const std::vector<uint8_t> result = xrApiManagerSingleton().tryGetReferenceSpacePose(spaceType);
  if (result.size() == 0) {
    args.GetReturnValue().Set(v8::Undefined(isolate));
    return;
  }

  const auto buffer = v8::ArrayBuffer::New(isolate, result.size());
  memcpy(buffer->GetBackingStore()->Data(), result.data(), result.size());
  args.GetReturnValue().Set(buffer);
}

void updateTargetFrameRate(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  float targetFrameRate = args[0].As<v8::Number>()->Value();
  xrApiManagerSingleton().trySetRefreshRate(targetFrameRate);
  args.GetReturnValue().Set(v8::Number::New(isolate, 0));
}

void getSupportedFrameRates(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  std::vector<float> refreshRates = xrApiManagerSingleton().getAvailableRefreshRates();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::ArrayBuffer> buffer =
    v8::ArrayBuffer::New(isolate, refreshRates.size() * sizeof(float));
  v8::Local<v8::Float32Array> result = v8::Float32Array::New(buffer, 0, refreshRates.size());

  memcpy(
    buffer->GetBackingStore()->Data(), refreshRates.data(), refreshRates.size() * sizeof(float));

  args.GetReturnValue().Set(result);
}

void initializeSwapchain(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  GLuint textureId = args[0].As<v8::Number>()->Value();
  bool spaceWarpRequested = args[1].As<v8::Boolean>()->Value();
  std::vector<uint8_t> data =
    xrApiManagerSingleton().initializeSwapchain(textureId, spaceWarpRequested);
  auto buffer = v8::ArrayBuffer::New(isolate, data.size());
  memcpy(buffer->GetBackingStore()->Data(), data.data(), data.size());
  args.GetReturnValue().Set(buffer);
}

void updateDeltaPose(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();

  v8::Local<v8::Object> position = args[0].As<v8::Object>();
  v8::Local<v8::Object> orientation = args[1].As<v8::Object>();

  float xPos = getNumberProperty(isolate, position, "x");
  float yPos = getNumberProperty(isolate, position, "y");
  float zPos = getNumberProperty(isolate, position, "z");

  float xOri = getNumberProperty(isolate, orientation, "x");
  float yOri = getNumberProperty(isolate, orientation, "y");
  float zOri = getNumberProperty(isolate, orientation, "z");
  float wOri = getNumberProperty(isolate, orientation, "w");

  xrApiManagerSingleton().updateDeltaPose({
    .orientation = {xOri, yOri, zOri, wOri},
    .position = {xPos, yPos, zPos},
  });
  args.GetReturnValue().Set(v8::Number::New(isolate, 0));
}

void updateFixedFoveation(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  float fixedFoveation = args[0].As<v8::Number>()->Value();
  xrApiManagerSingleton().updateFixedFoveation(fixedFoveation);
  args.GetReturnValue().Set(v8::Number::New(isolate, 0));
}

void updateRenderTexture(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  GLuint textureId = args[0].As<v8::Number>()->Value();
  xrApiManagerSingleton().updateRenderTexture(textureId);
  args.GetReturnValue().Set(v8::Number::New(isolate, 0));
}

void updateSpaceWarpTextures(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  GLuint motionVectorTextureId = args[0].As<v8::Number>()->Value();
  GLuint motionVectorDepthTextureId = args[1].As<v8::Number>()->Value();
  xrApiManagerSingleton().updateSpaceWarpTextures(
    motionVectorTextureId, motionVectorDepthTextureId);
  args.GetReturnValue().Set(v8::Number::New(isolate, 0));
}

void endSession(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  C8Log("[xrapi-addon] endSession not implemented");
  args.GetReturnValue().Set(v8::Number::New(isolate, 0));
}

void sendHapticFeedback(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  v8::String::Utf8Value utf8Value(isolate, args[0].As<v8::String>());
  std::string handedness(*utf8Value);
  float value = args[1].As<v8::Number>()->Value();
  float duration = args[2].As<v8::Number>()->Value();
  xrApiManagerSingleton().updateHapticState(handedness, value, duration);
  args.GetReturnValue().Set(v8::Number::New(isolate, 0));
}

void setIsSpaceWarpActiveCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();

  v8::Local<v8::Function> isSpaceWarpActiveCallback = args[0].As<v8::Function>();
  PersistentFunction persistentIsSpaceWarpActiveCallback =
    std::make_shared<v8::Persistent<v8::Function>>(isolate, isSpaceWarpActiveCallback);
  updatePersistentFunctionMap("isSpaceWarpActiveCallback", persistentIsSpaceWarpActiveCallback);

  auto nativeIsSpaceWarpActiveCallback = [persistentIsSpaceWarpActiveCallback, isolate]() -> bool {
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Function> localFunction = persistentIsSpaceWarpActiveCallback->Get(isolate);

    v8::Local<v8::Value> result =
      localFunction->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 0, nullptr)
        .ToLocalChecked();

    return result->BooleanValue(isolate);
  };

  xrApiManagerSingleton().setIsSpaceWarpActiveCallback(std::move(nativeIsSpaceWarpActiveCallback));
  args.GetReturnValue().Set(v8::Null(isolate));
}

void updateRenderState(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  float depthNear = args[0].As<v8::Number>()->Value();
  float depthFar = args[1].As<v8::Number>()->Value();
  xrApiManagerSingleton().updateRenderState(depthNear, depthFar);
  args.GetReturnValue().Set(v8::Number::New(isolate, 0));
}

void Cleanup(void *arg) {
  // Clean up all persistent functions
  for (auto &pair : persistentFunctions) {
    pair.second->Reset();
  }
  persistentFunctions.clear();
  xrApiManagerSingleton().endSession();
}

v8::Local<v8::Object> initXrapiAddon(
  android_app *android_app, v8::Isolate *isolate, v8::Local<v8::Context> context, uv_loop_t *loop) {

  xrApiManagerSingleton().performSetup(android_app, loop);

  v8::Local<v8::Object> exports = v8::Object::New(isolate);

  auto exportFunction = [&](const char *name, v8::FunctionCallback callback) {
    v8::Local<v8::Function> func = v8::Function::New(context, callback).ToLocalChecked();
    exports->Set(context, v8::String::NewFromUtf8(isolate, name).ToLocalChecked(), func).Check();
  };

  exportFunction("isSessionSupported", isSessionSupported);
  exportFunction("requestSession", requestSession);
  exportFunction("tryGetReferenceSpacePose", tryGetReferenceSpacePose);
  exportFunction("updateTargetFrameRate", updateTargetFrameRate);
  exportFunction("getSupportedFrameRates", getSupportedFrameRates);
  exportFunction("initializeSwapchain", initializeSwapchain);
  exportFunction("endSession", endSession);
  exportFunction("sendHapticFeedback", sendHapticFeedback);
  exportFunction("setIsSpaceWarpActiveCallback", setIsSpaceWarpActiveCallback);
  exportFunction("updateDeltaPose", updateDeltaPose);
  exportFunction("updateFixedFoveation", updateFixedFoveation);
  exportFunction("updateRenderState", updateRenderState);
  exportFunction("updateRenderTexture", updateRenderTexture);
  exportFunction("updateSpaceWarpTextures", updateSpaceWarpTextures);

  node::Environment *env = node::GetCurrentEnvironment(context);
  node::AtExit(env, Cleanup, nullptr);

  return exports;
}

}  // namespace c8
