// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"gesture-detector.h"};
  deps = {
    ":object8",
    "//c8:c8-log",
    "//c8:hvector",
    "//c8:map",
    "//c8:set",
    "//c8:string",
    "//c8/geometry:egomotion",
    "//c8/string:format",
    "//c8/geometry:vectors",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xfd1ec306);

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/pixels/render/gesture-detector.h"
#include "c8/quaternion.h"
#include "c8/string/format.h"
#include "c8/time/now.h"

namespace c8 {

namespace {

// These parameters are recommended by copilot. 200ms feels a little long (150ms might be better)
// but going with the recommendation for now.
double TAP_MILLIS = 200.0;
double DOUBLE_TAP_MILLIS = 300.0;
// It's not needed right now, but if we need a long press / hold gesture in the future, copilot
// recommends 500ms.

void debugHandleGesture(const String &event, const TouchState &s) {
  C8Log("%s: %s", event.c_str(), s.toString().c_str());
}

TouchState getTouchState(const TouchEvent &t) {
  TouchState s;
  s.touchCount = t.count;

  if (!s.touchCount) {
    return s;
  }

  s.touchMillis = t.timeMillis;

  float cx = 0.0f;
  float cy = 0.0f;
  for (int i = 0; i < s.touchCount; ++i) {
    cx += t.pos[i].x;  // TODO: rawX
    cy += t.pos[i].y;  // TODO: rawX
  }
  s.positionRaw = {cx / s.touchCount, cy / s.touchCount};

  auto screenScale = 2.0f / (t.viewWidth + t.viewHeight);  // TODO: screenW + screenH
  s.normalizedViewSize = {t.viewWidth * screenScale, t.viewHeight * screenScale};
  s.position = {s.positionRaw.x() * screenScale, s.positionRaw.y() * screenScale};

  s.positionClip = {
    2.0f * s.positionRaw.x() / t.viewWidth - 1.0f, 1.0f - 2.0f * s.positionRaw.y() / t.viewHeight};

  if (s.touchCount >= 2) {
    float sum = 0.0f;
    for (int i = 0; i < s.touchCount; ++i) {
      float xd = s.positionRaw.x() - t.pos[i].x;
      float yd = s.positionRaw.y() - t.pos[i].y;
      sum += std::sqrt(xd * xd + yd * yd);
    }
    s.spread = sum * screenScale / s.touchCount;
  }

  return s;
}

String fingerString(int f) {
  switch (f) {
    case 1:
      return "one";
    case 2:
      return "two";
    case 3:
      return "three";
    default:
      return "many";
  }
}

}  // namespace

String TouchState::toString() const {
  return format(
    "{count: %d, raw: (%0.1f, %0.1f), pos: (%0.3f, %0.3f), vel: (%0.5f, %0.5f), "
    "posClip: (%0.3f, %0.3f), normViewSz(%0.3f, %0.3f), spread: %0.3f, touchMs: %0.1f,"
    "startMs: %0.1f, startPos: (%0.3f, %0.3f), startSpread: %0.3f, posChange: (%0.3f, %0.3f), "
    "spreadChange: %0.3f}",
    touchCount,
    positionRaw.x(),
    positionRaw.y(),
    position.x(),
    position.y(),
    velocity.x(),
    velocity.y(),
    positionClip.x(),
    positionClip.y(),
    normalizedViewSize.x(),
    normalizedViewSize.y(),
    spread,
    touchMillis,
    startMillis,
    startPosition.x(),
    startPosition.y(),
    startSpread,
    startRotationRadian,
    rotationRadianChange,
    positionChange.x(),
    positionChange.y(),
    spreadChange);
}

void GestureDetector::addListener(const String &event, GestureDetector::GestureHandler *handler) {
  listeners_[event].insert(handler);
}

void GestureDetector::removeListener(
  const String &event, GestureDetector::GestureHandler *handler) {
  if (listeners_.contains(event)) {
    listeners_[event].erase(handler);
  }
}

void GestureDetector::emit(const String &event, const TouchState &s) const {
  if (!listeners_.contains(event)) {
    return;
  }
  for (const auto *handler : (*listeners_.find(event)).second) {
    (*handler)(event, s);
  }
}

void GestureDetector::observe(const TouchEvent &t) {
  auto s = getTouchState(t);

  auto gestureContinues = s.touchCount == prev_.touchCount;
  auto gestureEnded = prev_.touchCount && !gestureContinues;
  auto gestureStarted = s.touchCount && !gestureContinues;

  if (gestureEnded) {
    emit(format("%sfingerend", fingerString(prev_.touchCount).c_str()), prev_);
    bool isTap = (t.timeMillis - prev_.startMillis) <= TAP_MILLIS;
    if (isTap) {
      bool touchesDecreasing = prev_.touchCount < lastEnd_.touchCount;
      bool touchesIncreasing = prev_.touchCount < s.touchCount;
      bool isDoubleTap = (t.timeMillis - lastTap_.startMillis) <= DOUBLE_TAP_MILLIS;
      bool isMatchedTap = lastTap_.touchCount == prev_.touchCount;
      if (touchesDecreasing || touchesIncreasing) {
        // If we are shifting finger numbers, suppress the tap.
      } else if (isDoubleTap) {
        if (isMatchedTap) {
          emit(format("%sfingerdoubletap", fingerString(prev_.touchCount).c_str()), prev_);
        }
        // If the last tap was very recent and a different number, suppress the second tap.
      } else {
        // Normal tap.
        emit(format("%sfingertap", fingerString(prev_.touchCount).c_str()), prev_);
        lastTap_ = prev_;
        lastTap_.startMillis = t.timeMillis;
      }
    }

    lastEnd_ = prev_;
    lastEnd_.startMillis = t.timeMillis;
    prev_ = {};
  }

  s.startMillis = prev_.startMillis;
  s.startPosition = prev_.startPosition;
  s.startSpread = prev_.startSpread;
  s.startRotationRadian = prev_.startRotationRadian;

  // Compute velocity over the last several frames.
  double deltaMillis = 1.0;
  auto oldestPosition = s.position;
  if (prev_.historySize > 0) {
    deltaMillis = std::max(s.touchMillis - prev_.previousTouchMillis[prev_.historySize - 1], 1.0);
    oldestPosition = prev_.previousPositions[prev_.historySize - 1];
  }
  s.velocity = (s.position - oldestPosition) * (1.0 / deltaMillis);

  // Shift the previous history by one, and add the most recent frame at the front.
  s.previousPositions[0] = prev_.position;
  std::memcpy(
    s.previousPositions.data() + 1,
    prev_.previousPositions.data(),
    (s.previousPositions.size() - 1) * sizeof(HVector2));

  s.previousTouchMillis[0] = prev_.touchMillis;
  std::memcpy(
    s.previousTouchMillis.data() + 1,
    prev_.previousTouchMillis.data(),
    (s.previousTouchMillis.size() - 1) * sizeof(double));

  // Update history size, up to max history.
  s.historySize = std::min(prev_.historySize + 1, s.previousPositions.size());

  if (gestureStarted) {
    s.startMillis = t.timeMillis;
    s.startPosition = s.position;
    s.startSpread = s.spread;
    if (s.touchCount >= 2) {
      // only take the positions of the first two touches.
      s.startRotationRadian = std::atan2(t.pos[1].y - t.pos[0].y, t.pos[1].x - t.pos[0].x);
    }
    
    emit(format("%sfingerstart", fingerString(s.touchCount).c_str()), s);
    prev_ = s;
  }

  if (gestureContinues) {
    s.positionChange = s.position - prev_.position;
    if (s.spread) {
      s.spreadChange = s.spread - prev_.spread;
    }
    if (s.touchCount >= 2) {
      float twoFingerLineRadian = std::atan2(t.pos[1].y - t.pos[0].y, t.pos[1].x - t.pos[0].x);
      s.rotationRadianChange = twoFingerLineRadian - s.startRotationRadian;

      // Normalize the rotation change to be within the range [-PI, PI]
      if (s.rotationRadianChange > M_PI) {
        s.rotationRadianChange -= 2 * M_PI;
      } else if (s.rotationRadianChange < -M_PI) {
        s.rotationRadianChange += 2 * M_PI;
      }
    }
    emit(format("%sfingermove", fingerString(s.touchCount).c_str()), s);
    prev_ = s;
  }
}

std::unique_ptr<DebugGestureHandler> DebugGestureHandler::create(GestureDetector &gestureDetector) {
  return std::unique_ptr<DebugGestureHandler>{new DebugGestureHandler(gestureDetector)};
}

// Destructor removes this handler from the Gesture detector. The gesture detector should outlive
// the handler.
DebugGestureHandler::~DebugGestureHandler() {
  detector_->removeListener("onefingerstart", &(this->handleGesture_));
  detector_->removeListener("onefingermove", &(this->handleGesture_));
  detector_->removeListener("onefingerend", &(this->handleGesture_));
  detector_->removeListener("onefingertap", &(this->handleGesture_));
  detector_->removeListener("onefingerdoubletap", &(this->handleGesture_));
  detector_->removeListener("twofingerstart", &(this->handleGesture_));
  detector_->removeListener("twofingermove", &(this->handleGesture_));
  detector_->removeListener("twofingerend", &(this->handleGesture_));
  detector_->removeListener("twofingertap", &(this->handleGesture_));
  detector_->removeListener("twofingerdoubletap", &(this->handleGesture_));
  detector_->removeListener("threefingerstart", &(this->handleGesture_));
  detector_->removeListener("threefingermove", &(this->handleGesture_));
  detector_->removeListener("threefingerend", &(this->handleGesture_));
  detector_->removeListener("threefingertap", &(this->handleGesture_));
  detector_->removeListener("threefingerdoubletap", &(this->handleGesture_));
  detector_->removeListener("manyfingerstart", &(this->handleGesture_));
  detector_->removeListener("manyfingermove", &(this->handleGesture_));
  detector_->removeListener("manyfingerend", &(this->handleGesture_));
  detector_->removeListener("manyfingertap", &(this->handleGesture_));
  detector_->removeListener("manyfingerdoubletap", &(this->handleGesture_));
}

DebugGestureHandler::DebugGestureHandler(GestureDetector &gestureDetector) {
  handleGesture_ = &debugHandleGesture;
  detector_ = &gestureDetector;
  detector_->addListener("onefingerstart", &(this->handleGesture_));
  detector_->addListener("onefingermove", &(this->handleGesture_));
  detector_->addListener("onefingerend", &(this->handleGesture_));
  detector_->addListener("onefingertap", &(this->handleGesture_));
  detector_->addListener("onefingerdoubletap", &(this->handleGesture_));
  detector_->addListener("twofingerstart", &(this->handleGesture_));
  detector_->addListener("twofingermove", &(this->handleGesture_));
  detector_->addListener("twofingerend", &(this->handleGesture_));
  detector_->addListener("twofingertap", &(this->handleGesture_));
  detector_->addListener("twofingerdoubletap", &(this->handleGesture_));
  detector_->addListener("threefingerstart", &(this->handleGesture_));
  detector_->addListener("threefingermove", &(this->handleGesture_));
  detector_->addListener("threefingerend", &(this->handleGesture_));
  detector_->addListener("threefingertap", &(this->handleGesture_));
  detector_->addListener("threefingerdoubletap", &(this->handleGesture_));
  detector_->addListener("manyfingerstart", &(this->handleGesture_));
  detector_->addListener("manyfingermove", &(this->handleGesture_));
  detector_->addListener("manyfingerend", &(this->handleGesture_));
  detector_->addListener("manyfingertap", &(this->handleGesture_));
  detector_->addListener("manyfingerdoubletap", &(this->handleGesture_));
}

std::unique_ptr<OneFingerRotateHandler> OneFingerRotateHandler::create(
  GestureDetector &gestureDetector, Object8 &object) {
  return std::unique_ptr<OneFingerRotateHandler>{
    new OneFingerRotateHandler(gestureDetector, object)};
}

// Destructor removes this handler from the Gesture detector. The gesture detector should outlive
// the handler.
OneFingerRotateHandler::~OneFingerRotateHandler() {
  detector_->removeListener("onefingermove", &(this->handleGesture_));
}

OneFingerRotateHandler::OneFingerRotateHandler(GestureDetector &gestureDetector, Object8 &object) {
  object_ = &object;
  detector_ = &gestureDetector;
  handleGesture_ = [this](const String &e, const TouchState &t) {
    this->object_->setLocal(
      updateWorldPosition(this->object_->local(), HMatrixGen::yRadians(-t.positionChange.x() * 6)));
    this->object_->setLocal(
      updateWorldPosition(this->object_->local(), HMatrixGen::xRadians(t.positionChange.y())));
  };
  detector_->addListener("onefingermove", &(this->handleGesture_));
}

std::unique_ptr<TwoFingerRotateHandler> TwoFingerRotateHandler::create(
  GestureDetector &gestureDetector, Object8 &object) {
  return std::unique_ptr<TwoFingerRotateHandler>{
    new TwoFingerRotateHandler(gestureDetector, object)};
}

// Destructor removes this handler from the Gesture detector. The gesture detector should outlive
// the handler.
TwoFingerRotateHandler::~TwoFingerRotateHandler() {
  detector_->removeListener("twofingermove", &(this->handleGesture_));
}

TwoFingerRotateHandler::TwoFingerRotateHandler(GestureDetector &gestureDetector, Object8 &object) {
  object_ = &object;
  detector_ = &gestureDetector;
  handleGesture_ = [this](const String &e, const TouchState &t) {
    this->object_->setLocal(
      updateWorldPosition(this->object_->local(), HMatrixGen::yRadians(-t.positionChange.x() * 6)));
  };
  detector_->addListener("twofingermove", &(this->handleGesture_));
}

std::unique_ptr<PinchScaleHandler> PinchScaleHandler::create(
  GestureDetector &gestureDetector, Object8 &object) {
  return std::unique_ptr<PinchScaleHandler>{new PinchScaleHandler(gestureDetector, object)};
}

// Destructor removes this handler from the Gesture detector. The gesture detector should outlive
// the handler.
PinchScaleHandler::~PinchScaleHandler() {
  detector_->removeListener("twofingermove", &(this->handleGesture_));
}

PinchScaleHandler::PinchScaleHandler(GestureDetector &gestureDetector, Object8 &object) {
  object_ = &object;
  detector_ = &gestureDetector;
  initScale_ = trsScale(object_->local());
  handleGesture_ = [this](const String &e, const TouchState &t) {
    this->scaleFactor_ *= 1 + t.spreadChange / t.startSpread;
    this->scaleFactor_ = std::min(std::max(this->scaleFactor_, 0.33f), 3.0f);
    auto scaleTarget = this->initScale_ * this->scaleFactor_;
    auto currScale = trsScale(this->object_->local());
    auto scaleDelta = HMatrixGen::scale(
      scaleTarget.x() / currScale.x(),
      scaleTarget.y() / currScale.y(),
      scaleTarget.z() / currScale.z());
    this->object_->setLocal(updateWorldPosition(this->object_->local(), scaleDelta));
  };
  detector_->addListener("twofingermove", &(this->handleGesture_));
}

std::unique_ptr<ModelMoveHandler> ModelMoveHandler::create(
  GestureDetector &gestureDetector, Camera &camera, Object8 &object) {
  return std::unique_ptr<ModelMoveHandler>{new ModelMoveHandler(gestureDetector, camera, object)};
}

// Destructor removes this handler from the Gesture detector. The gesture detector should outlive
// the handler.
ModelMoveHandler::~ModelMoveHandler() {
  detector_->removeListener("onefingermove", &(this->handleGesture_));
}

ModelMoveHandler::ModelMoveHandler(
  GestureDetector &gestureDetector, Camera &camera, Object8 &object) {
  object_ = &object;
  camera_ = &camera;
  detector_ = &gestureDetector;
  handleGesture_ = [this](const String &e, const TouchState &t) {
    // We want to move the object left / right, forward / back (aligned with gravity) in point of
    // view of the camera, so we need to find the camera's y-axis rotation and apply that to the
    // directional update.
    auto rotationRads = groundPlaneRotationRads(rotation(camera_->local()));
    auto rotatedPositionChange = HMatrixGen::yRadians(rotationRads)
      * HPoint3(t.positionChange.x(), 0.0f, -t.positionChange.y());
    auto rotatedUpdate = cameraMotion(rotatedPositionChange, {});
    this->object_->setLocal(updateWorldPosition(rotatedUpdate, this->object_->local()));
  };
  detector_->addListener("onefingermove", &(this->handleGesture_));
}

}  // namespace c8
