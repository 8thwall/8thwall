#include <emscripten.h>

#include <memory>
#include <nlohmann/json.hpp>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/model/model-view.h"
#include "c8/model/web/constants.h"
#include "c8/stats/scope-timer.h"
#include "c8/symbol-visibility.h"
#include "c8/time/now.h"
#include "c8/time/rolling-framerate.h"

using namespace c8;

namespace {

struct StaticData {
  std::unique_ptr<ModelView> modelView = std::make_unique<ModelView>();
  String renderState = "";
  String editState = "{}";
  RollingFramerate framerate;
  TimeReset timer;
  std::unique_ptr<LoggingContext> loopLoggingContext;
};

StaticData &data() {
  static StaticData data;
  return data;
}

}  // namespace

extern "C" {

C8_PUBLIC
void c8EmAsm_setModel(const uint8_t *modelData, int modelSize) {
  {
    ScopeTimer t("set-model");
    data().modelView->setModel(modelData);
  }
}

C8_PUBLIC
void c8EmAsm_updateModel(const uint8_t *modelData, int modelSize) {
  {
    ScopeTimer t("update-model");
    data().modelView->updateModel(modelData);
  }
}

C8_PUBLIC
void c8EmAsm_setResolution(int width, int height) {
  data().modelView->setRenderResolution(width, height);
}

C8_PUBLIC
void c8EmAsm_render(double frameTimeMillis) {
  if (data().loopLoggingContext.get() != nullptr) {
    data().loopLoggingContext->markCompletionTimepoint();
    ScopeTimer::summarize(*data().loopLoggingContext);
  }
  data().loopLoggingContext = LoggingContext::createRootLoggingTreePtr("model-render-loop");

  if (C8_MODEL_LOG_PERFORMANCE) {
    data().framerate.push();
    if (data().timer.shift(nowMicros()) > 10e6) {  // Timer has been running for 10 seconds.
      C8Log("Render framerate: %f", data().framerate.fps());
      data().timer = {};  // Reset timer.
      ScopeTimer::logBriefSummary();
      ScopeTimer::reset();
    }
  }

  ScopeTimer t("render");
  auto renderState = data().modelView->render(frameTimeMillis);
  nlohmann::json renderStateJson;
  renderStateJson["cameraPos"] = {
    {"x", renderState.cameraPos.x()},
    {"y", renderState.cameraPos.y()},
    {"z", renderState.cameraPos.z()},
  };
  renderStateJson["cameraRot"] = {
    {"w", renderState.cameraRot.w()},
    {"x", renderState.cameraRot.x()},
    {"y", renderState.cameraRot.y()},
    {"z", renderState.cameraRot.z()},
  };
  renderStateJson["modelPos"] = {
    {"x", renderState.modelPos.x()},
    {"y", renderState.modelPos.y()},
    {"z", renderState.modelPos.z()},
  };
  renderStateJson["modelRot"] = {
    {"w", renderState.modelRot.w()},
    {"x", renderState.modelRot.x()},
    {"y", renderState.modelRot.y()},
    {"z", renderState.modelRot.z()},
  };
  renderStateJson["updatedCamera"] = renderState.updatedCamera;
  renderStateJson["rendered"] = renderState.rendered;
  data().renderState = renderStateJson.dump();
  EM_ASM_(
    {
      window._modelView = {};
      self._modelView.renderState = UTF8ToString($0, $1);
    },
    data().renderState.c_str(),
    data().renderState.size());
}

C8_PUBLIC
void c8EmAsm_startWebXr() { data().modelView->startXrMode(); }

C8_PUBLIC
void c8EmAsm_handleWebXrFrame(const char *frameJson) {
  auto json = nlohmann::json::parse(frameJson);
  MultiViewSpec viewSpec = {
    .framebufferWidth = json["framebuffer"]["width"].get<int>(),
    .framebufferHeight = json["framebuffer"]["height"].get<int>(),
    .framebufferId = json["framebuffer"]["id"].get<int>(),
  };

  for (const auto &view : json["views"]) {
    // Infer perspective paramters from the projection matrix, including near and far clip.
    std::array<float, 16> p;
    for (int i = 0; i < 16; ++i) {
      p[i] = view["projectionMatrix"][i].get<float>();
    }
    HMatrix projectionMatrix(
      {p[0], p[4], p[8], p[12]},
      {p[1], p[5], p[9], p[13]},
      {p[2], p[6], p[10], p[14]},
      {p[3], p[7], p[11], p[15]});

    auto flipZ = HMatrixGen::scaleZ(-1.0f);

    auto low = flipZ * projectionMatrix.inv() * HPoint3{-1.0f, -1.0f, -1.0f};
    auto center = flipZ * projectionMatrix.inv() * HPoint3{0.0f, 0.0f, 0.0f};
    auto high = flipZ * projectionMatrix.inv() * HPoint3{1.0f, 1.0f, 1.0f};

    auto low2 = low.flatten();
    auto center2 = center.flatten();
    auto high2 = high.flatten();

    if (low.z() < 0) {
      C8Log("[model-web-view] WARNING: Unexpected projection matrix sign flip");
    }

    auto viewWidth = view["viewport"]["width"].get<int>();
    auto viewHeight = view["viewport"]["height"].get<int>();
    auto near = low.z();
    auto far = high.z();
    auto flx = viewWidth / (high2.x() - low2.x());
    auto fly = viewHeight / (high2.y() - low2.y());
    auto cx = center2.x() * flx + 0.5f * (viewWidth - 1);
    auto cy = -center2.y() * fly + 0.5f * (viewHeight - 1);

    // Fill view metadata.
    viewSpec.views.push_back({
      .viewportX = view["viewport"]["x"].get<int>(),
      .viewportY = view["viewport"]["y"].get<int>(),
      .viewportWidth = viewWidth,
      .viewportHeight = viewHeight,
      .position =
        {
          view["position"]["x"].get<float>(),
          view["position"]["y"].get<float>(),
          -view["position"]["z"].get<float>(),
        },
      .orientation =
        {
          view["orientation"]["w"].get<float>(),
          -view["orientation"]["x"].get<float>(),
          -view["orientation"]["y"].get<float>(),
          view["orientation"]["z"].get<float>(),
        },
      .intrinsics =
        {
          .pixelsWidth = view["viewport"]["width"].get<int>(),
          .pixelsHeight = view["viewport"]["height"].get<int>(),
          .centerPointX = cx,
          .centerPointY = cy,
          .focalLengthHorizontal = flx,
          .focalLengthVertical = fly,
        },
      .near = near,
      .far = far,
    });
  }

  if (!viewSpec.views.empty()) {
    auto position = HPoint3{
      json["pose"]["position"]["x"].get<float>(),
      json["pose"]["position"]["y"].get<float>(),
      -json["pose"]["position"]["z"].get<float>(),
    };

    auto orientation = Quaternion{
      json["pose"]["orientation"]["w"].get<float>(),
      -json["pose"]["orientation"]["x"].get<float>(),
      -json["pose"]["orientation"]["y"].get<float>(),
      json["pose"]["orientation"]["z"].get<float>(),
    };

    auto extrinsic = cameraMotion(position, orientation);

    // Set the overall camera position to communicate back to the model manager etc. This is also
    // used for centering the model at the view start.
    data().modelView->setCameraPosition(extrinsic, viewSpec.views[0].intrinsics);

    // Set the specific cameras to draw the eyes.
    data().modelView->setMultiCameraPosition(viewSpec);
  }
}

C8_PUBLIC
void c8EmAsm_finishWebXr() { data().modelView->finishXrMode(); }

C8_PUBLIC
void c8EmAsm_handleTouchEvent(const char *touchJson) {
  auto json = nlohmann::json::parse(touchJson);
  TouchEvent e;
  e.timeMillis = json["timeMillis"].get<double>();
  e.count = json["count"].get<int>();
  for (int i = 0; i < e.count; ++i) {
    auto &je = json["pos"][i];
    e.pos.push_back({
      static_cast<float>(je["x"].get<float>()),
      static_cast<float>(je["y"].get<float>()),
      static_cast<float>(je["rawX"].get<float>()),
      static_cast<float>(je["rawY"].get<float>()),
    });
  }
  e.viewWidth = json["viewWidth"].get<int>();
  e.viewHeight = json["viewHeight"].get<int>();
  e.screenWidth = json["screenWidth"].get<int>();
  e.screenHeight = json["screenHeight"].get<int>();
  data().modelView->gotTouches(e);
}

C8_PUBLIC
void c8EmAsm_configure(const char *configJson) {
  auto json = nlohmann::json::parse(configJson);
  ViewParams params;
  if (json.contains("orbit")) {
    auto &orbit = json["orbit"];
    if (orbit.contains("yaw")) {
      auto &yaw = orbit["yaw"];
      params.orbit.yawRadians.min = yaw["min"].get<float>();
      params.orbit.yawRadians.start = yaw["start"].get<float>();
      params.orbit.yawRadians.max = yaw["max"].get<float>();
    }
    if (orbit.contains("pitch")) {
      auto &pitch = orbit["pitch"];
      params.orbit.pitchRadians.min = pitch["min"].get<float>();
      params.orbit.pitchRadians.start = pitch["start"].get<float>();
      params.orbit.pitchRadians.max = pitch["max"].get<float>();
    }
    if (orbit.contains("radius")) {
      auto &distance = orbit["radius"];
      params.orbit.distance.min = distance["min"].get<float>();
      params.orbit.distance.start = distance["start"].get<float>();
      params.orbit.distance.max = distance["max"].get<float>();
    }
    if (orbit.contains("center")) {
      auto &center = orbit["center"];
      params.orbit.center = {
        center[0].get<float>(),
        center[1].get<float>(),
        center[2].get<float>(),
      };
    }
  }
  if (json.contains("space")) {
    params.space = static_cast<ModelConfig::CoordinateSpace>(json["space"].get<int>());
  }
  if (json.contains("sortTexture")) {
    params.sortTexture = json["sortTexture"].get<bool>();
  }
  if (json.contains("multiTexture")) {
    params.multiTexture = json["multiTexture"].get<bool>();
  }
  if (json.contains("useOctree")) {
    params.useOctree = json["useOctree"].get<bool>();
  }
  data().modelView->configure(params);
}

C8_PUBLIC
void c8EmAsm_startCropMode(const char *editJson) {
  auto json = nlohmann::json::parse(data().editState);
  json.merge_patch(nlohmann::json::parse(editJson));
  Box3 cropBox;
  float rotationDegrees = 0;
  auto shape = CropParams::Shape::BOX;
  if (json.contains("crop")) {
    const auto &b = json["crop"];
    cropBox.min = {b["min"][0].get<float>(), b["min"][1].get<float>(), b["min"][2].get<float>()};
    cropBox.max = {b["max"][0].get<float>(), b["max"][1].get<float>(), b["max"][2].get<float>()};
  }
  if (json.contains("rotationDegrees")) {
    rotationDegrees = json["rotationDegrees"].get<float>();
  }
  if (json.contains("shape")) {
    shape = static_cast<CropParams::Shape>(json["shape"].get<int>());
  }

  data().modelView->startCropMode({cropBox, rotationDegrees, shape});
}

C8_PUBLIC
void c8EmAsm_finishCropMode() {
  nlohmann::json editStateJson = nlohmann::json::parse(data().editState);
  auto cropResult = data().modelView->finishCropMode();

  if (cropResult.cropBox) {
    auto cropBox = *cropResult.cropBox;
    editStateJson["crop"] = {
      {"min", {cropBox.min.x(), cropBox.min.y(), cropBox.min.z()}},
      {"max", {cropBox.max.x(), cropBox.max.y(), cropBox.max.z()}},
    };
  }

  if (cropResult.rotationDegrees) {
    editStateJson["rotationDegrees"] = *cropResult.rotationDegrees;
  }

  if (cropResult.shape) {
    editStateJson["shape"] = static_cast<int>(*cropResult.shape);
  }

  data().editState = editStateJson.dump();
  EM_ASM_(
    {
      window._modelView = {};
      self._modelView.editState = UTF8ToString($0, $1);
    },
    data().editState.c_str(),
    data().editState.size());
}

C8_PUBLIC
void c8EmAsm_updateCropViewpoint(const char *viewpointJson) {
  auto json = nlohmann::json::parse(viewpointJson);
  CropBox::Side side = CropBox::Side::FRONT;
  if (json.contains("side")) {
    side = static_cast<CropBox::Side>(json["side"].get<int>());
    if (side < CropBox::Side::TOP || side > CropBox::Side::FRONT) {
      side = CropBox::Side::FRONT;
    }
  }
  data().modelView->updateCropViewpoint(side);
}

C8_PUBLIC
void c8EmAsm_updateCropMode(const char *adjustmentsJson) {
  auto json = nlohmann::json::parse(adjustmentsJson);
  CropParams params;
  if (json.contains("crop")) {
    const auto &b = json["crop"];
    params.cropBox = Box3{
      {b["min"][0].get<float>(), b["min"][1].get<float>(), b["min"][2].get<float>()},
      {b["max"][0].get<float>(), b["max"][1].get<float>(), b["max"][2].get<float>()}};
  }
  if (json.contains("rotationDegrees")) {
    params.rotationDegrees = json["rotationDegrees"].get<float>();
  }
  if (json.contains("shape")) {
    params.shape = static_cast<CropParams::Shape>(json["shape"].get<int>());
  }
  data().modelView->updateCropMode(params);
}

}  // EXTERN "C"
