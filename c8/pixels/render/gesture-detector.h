// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include <memory>

#include "c8/hvector.h"
#include "c8/map.h"
#include "c8/pixels/render/object8.h"
#include "c8/set.h"
#include "c8/string.h"

#pragma once

namespace c8 {

struct TouchPosition {
  float x;
  float y;
  float rawX;
  float rawY;
};

struct TouchEvent {
  double timeMillis = 0.0;
  int count;
  Vector<TouchPosition> pos;
  int viewWidth = 0;
  int viewHeight = 0;
  int screenWidth = 0;
  int screenHeight = 0;
};

struct TouchState {
  int touchCount = 0;
  // Position of the touch in screen coordinates (pixels).
  HVector2 positionRaw;
  // Position of the touch in averaged view coordinates:
  // x in [0 : width/(.5*(width+height))]
  // y in [0 : height/(.5*(width+height))]
  HVector2 position;
  // Velocity of the touch in averaged view coordinates per millisecond:
  // x in [0 : width/(.5*(width+height))]
  // y in [0 : height/(.5*(width+height))]
  HVector2 velocity;
  // Position of the touch in normalized device coordinates (clip space):
  // x in [-1 : 1] where -1 is left and 1 is right.
  // y in [-1 : 1] where -1 is bottom and 1 is top.
  HVector2 positionClip;
  // Size of the view in normalized coordinates:
  // x = width / (0.5 * (width + height))
  // y = height / (0.5 * (width + height))
  HVector2 normalizedViewSize;
  float spread = 0.0f;
  double touchMillis = 0.0f;
  double startMillis = 0.0;
  HVector2 startPosition;
  float startSpread = 0.0f;
  HVector2 positionChange;
  float spreadChange = 0.0f;

  // calculate two pointers rotation
  float startRotationRadian = 0.0f;
  float rotationRadianChange = 0.0f;

  // Position history for velocity calculation.
  std::array<HVector2, 5> previousPositions = {};
  std::array<double, 5> previousTouchMillis = {};
  size_t historySize = 0;

  String toString() const;
};

class GestureDetector {
public:
  typedef std::function<void(const String &, const TouchState &)> GestureHandler;

  // Default constructor.
  GestureDetector() = default;

  // Default move constructors.
  GestureDetector(GestureDetector &&) = default;
  GestureDetector &operator=(GestureDetector &&) = default;

  // Disallow copying.
  GestureDetector(const GestureDetector &) = delete;
  GestureDetector &operator=(const GestureDetector &) = delete;

  void observe(const TouchEvent &t);

  void addListener(const String &event, GestureHandler *handler);
  void removeListener(const String &event, GestureHandler *handler);

private:
  void emit(const String &event, const TouchState &s) const;
  TouchState prev_;
  TouchState lastEnd_;
  TouchState lastTap_;
  TreeMap<String, TreeSet<GestureHandler *>> listeners_;
};

// A gesture handler that prints out all events from the gesture detector.
class DebugGestureHandler {
public:
  static std::unique_ptr<DebugGestureHandler> create(GestureDetector &gestureDetector);

  // Destructor removes this handler from the Gesture detector. The gesture detector should outlive
  // the handler.
  ~DebugGestureHandler();

  // Default move constructors.
  DebugGestureHandler(DebugGestureHandler &&) = default;
  DebugGestureHandler &operator=(DebugGestureHandler &&) = default;

  // Disallow copying.
  DebugGestureHandler(const DebugGestureHandler &) = delete;
  DebugGestureHandler &operator=(const DebugGestureHandler &) = delete;

private:
  GestureDetector *detector_;
  GestureDetector::GestureHandler handleGesture_;
  DebugGestureHandler(GestureDetector &gestureDetector);
};

// Rotates an object about the y-axis when one fingers is used.
class OneFingerRotateHandler {
public:
  static std::unique_ptr<OneFingerRotateHandler> create(
    GestureDetector &gestureDetector, Object8 &object);

  // Destructor removes this handler from the Gesture detector. The gesture detector should outlive
  // the handler.
  ~OneFingerRotateHandler();

  // Default move constructors.
  OneFingerRotateHandler(OneFingerRotateHandler &&) = default;
  OneFingerRotateHandler &operator=(OneFingerRotateHandler &&) = default;

  // Disallow copying.
  OneFingerRotateHandler(const OneFingerRotateHandler &) = delete;
  OneFingerRotateHandler &operator=(const OneFingerRotateHandler &) = delete;

private:
  GestureDetector *detector_;
  Object8 *object_;
  GestureDetector::GestureHandler handleGesture_;
  OneFingerRotateHandler(GestureDetector &gestureDetector, Object8 &object);
};

// Rotates an object about the y-axis when two fingers are used.
class TwoFingerRotateHandler {
public:
  static std::unique_ptr<TwoFingerRotateHandler> create(
    GestureDetector &gestureDetector, Object8 &object);

  // Destructor removes this handler from the Gesture detector. The gesture detector should outlive
  // the handler.
  ~TwoFingerRotateHandler();

  // Default move constructors.
  TwoFingerRotateHandler(TwoFingerRotateHandler &&) = default;
  TwoFingerRotateHandler &operator=(TwoFingerRotateHandler &&) = default;

  // Disallow copying.
  TwoFingerRotateHandler(const TwoFingerRotateHandler &) = delete;
  TwoFingerRotateHandler &operator=(const TwoFingerRotateHandler &) = delete;

private:
  GestureDetector *detector_;
  Object8 *object_;
  GestureDetector::GestureHandler handleGesture_;
  TwoFingerRotateHandler(GestureDetector &gestureDetector, Object8 &object);
};

// Uses a pinch gesture to scale an object up or down.
class PinchScaleHandler {
public:
  static std::unique_ptr<PinchScaleHandler> create(
    GestureDetector &gestureDetector, Object8 &object);

  // Destructor removes this handler from the Gesture detector. The gesture detector should outlive
  // the handler.
  ~PinchScaleHandler();

  // Default move constructors.
  PinchScaleHandler(PinchScaleHandler &&) = default;
  PinchScaleHandler &operator=(PinchScaleHandler &&) = default;

  // Disallow copying.
  PinchScaleHandler(const PinchScaleHandler &) = delete;
  PinchScaleHandler &operator=(const PinchScaleHandler &) = delete;

private:
  GestureDetector *detector_;
  Object8 *object_;
  GestureDetector::GestureHandler handleGesture_;
  HVector3 initScale_;
  float scaleFactor_ = 1.0f;
  PinchScaleHandler(GestureDetector &gestureDetector, Object8 &object);
};

// Moves an object when one fingers is used.
class ModelMoveHandler {
public:
  static std::unique_ptr<ModelMoveHandler> create(
    GestureDetector &gestureDetector, Camera &camera, Object8 &object);

  // Destructor removes this handler from the Gesture detector. The gesture detector should outlive
  // the handler.
  ~ModelMoveHandler();

  // Default move constructors.
  ModelMoveHandler(ModelMoveHandler &&) = default;
  ModelMoveHandler &operator=(ModelMoveHandler &&) = default;

  // Disallow copying.
  ModelMoveHandler(const ModelMoveHandler &) = delete;
  ModelMoveHandler &operator=(const ModelMoveHandler &) = delete;

private:
  GestureDetector *detector_;
  Camera *camera_;
  Object8 *object_;
  GestureDetector::GestureHandler handleGesture_;
  ModelMoveHandler(GestureDetector &gestureDetector, Camera &camera, Object8 &object);
};

}  // namespace c8
