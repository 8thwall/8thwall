// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Contains a class for managing serial stage processing in a ring buffer with concurrency.

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/map.h"
#include "c8/vector.h"

namespace c8 {

// StagedRingBuffer is a thread-safe ring buffer containing N frames of type T, where a series of
// work stages will be run in sequence on each T, but different stages may run concurrently
// on different frames in the ring.
//
// For example, say you have a pipeline consisting of the following 4 stages:
//   (1) You capture a new image from a camera to a buffer
//   (2) Resize the image buffer from (1)
//   (3) Rotate the image buffer from (2)
//   (4) Render the image from (1) and the image from (3).
//
// Initially, only stage (1) can be be run, since (2), (3), and (4) depend on it directly or
// indirectly. Once (1) is complete, however, (2) can start running on the original frame, while (1)
// can now run concurrently on the next frame in the ring buffer. This ends up creating a
// dynamic chain, but it has a wrap-around requirement that (1) cannot capture to a frame until
// stage (4) is finished with that frame.
//
// Example Usage:
//   enum class MyStage { CAPTURE, RESIZE, ROTATE, RENDER };
//   class MyBuffer { MyBuffer(int x, int y); ... };
//
//   auto ring = StagedRingBuffer<MyBuffer, MyStage>(4, {CAPTURE, RESIZE, ROTATE, RENDER}, {x, y})
//
//   // Start a new thread for each stage, and process 10 frames.
//   std::thread t1([&ring]{
//     for (int i = 0; i < 10; ++i) {
//       auto stage = ring.getStage(MyStage::CAPTURE);
//       if (!stage.hasValue()) {
//         continue;
//       }
//       MyBuffer &buffer = stage.get();
//       ...
//      }
//   });
//   std::thread t2([&ring]{
//     for (int i = 0; i < 10; ++i) {
//       auto stage = ring.getStage(MyStage::RESIZE);
//       if (!stage.hasValue()) {
//         continue;
//       }
//       MyBuffer &buffer = stage.get();
//       ...
//      }
//   });
//   std::thread t3([&ring]{
//     for (int i = 0; i < 10; ++i) {
//       auto stage = ring.getStage(MyStage::ROTATE);
//       if (!stage.hasValue()) {
//         continue;
//       }
//       MyBuffer &buffer = stage.get();
//       ...
//      }
//   });
//   std::thread t4([&ring]{
//     for (int i = 0; i < 10; ++i) {
//       auto stage = ring.getStage(MyStage::RENDER);
//       if (!stage.hasValue()) {
//         continue;
//       }
//       MyBuffer &buffer = stage.get();
//       ...
//      }
//   });
//
//   // Wait for all work to complete.
//   t1.join();
//   t2.join();
//   t3.join();
//   t4.join();
template <typename T, typename StageEnum>
class StagedRingBuffer {
public:
  // Construct a new StagedRingBuffer.
  //
  // Template Parameters
  //   T - The data type for each frame in the buffer.
  //   StageName - An enum class naming the possible work stages
  //
  // Parameters
  //   size - The number of frames in the ring. Must be >= 2.
  //   stages - A braced-initializer list containing the stages in order from first to last.
  //   params - Any additional constructor arguments to forward to every constructed frame.
  template <typename... Params>
  StagedRingBuffer(size_t size, std::initializer_list<StageEnum> stages, Params &&... params);

  ~StagedRingBuffer() = default;

  // Default move constructors.
  StagedRingBuffer(StagedRingBuffer &&) = default;
  StagedRingBuffer &operator=(StagedRingBuffer &&) = default;

  // Delete copy constructors.
  StagedRingBuffer(const StagedRingBuffer &) = delete;
  StagedRingBuffer &operator=(const StagedRingBuffer &) = delete;

  // Scoped stage object returned on call to getStage.
  class Stage {
  public:
    // Empty constructor, used for creating an empty Stage object that can be moved into.
    Stage() : ring_(nullptr), data_(nullptr) {}

    T &get() { return *data_; };

    bool hasValue() { return data_ != nullptr; }

    ~Stage() {
      if (ring_) {
        ring_->endStage(stage_);
      }
    }

    // Move constructors.
    Stage(Stage &&rhs) : ring_(rhs.ring_), stage_(rhs.stage_), data_(rhs.data_) {
      rhs.data_ = nullptr;
      rhs.ring_ = nullptr;
    }
    Stage &operator=(Stage &&rhs) {
      if (ring_) {
        ring_->endStage(stage_);
      }

      ring_ = rhs.ring_;
      stage_ = rhs.stage_;
      data_ = rhs.data_;
      rhs.ring_ = nullptr;
      rhs.data_ = nullptr;
      return *this;
    }

    // Delete copy constructors.
    Stage(const Stage &) = delete;
    Stage &operator=(const Stage &) = delete;

  private:
    friend class StagedRingBuffer<T, StageEnum>;
    Stage(StagedRingBuffer *ring, StageEnum stage) : ring_(ring), stage_(stage) {
      data_ = ring_->beginStage(stage_);
      if (data_ == nullptr) {
        ring_ = nullptr;
      }
    }
    StagedRingBuffer *ring_ = nullptr;
    StageEnum stage_;
    T *data_ = nullptr;
  };

  // Get a stage to process. When the returned stage goes out of scope, you are indicating that any
  // work on the stage is complete.
  Stage getStage(StageEnum stage) { return Stage(this, stage); }

  // Make subsequent calls to getStage return immediately with no value, until resume is called.
  // When resume is called, all previously staged data will be cleared.
  void pauseAndClear();

  // Allow data to be added and retrieved from the staged ring buffer again after a call to
  // pauseAndClear.
  void resume();

  // Access the underlying frame data to reconfigure them if needed. This is only safe to do when
  // the ring buffer is paused, so if the buffer is not paused this returns null.
  Vector<T> *getRawFramesIfPaused() {
    if (!isPaused_) {
      return nullptr;
    }
    return &frames_;
  }

private:
  // Get the T for a particular stage. As a caller of this method, you are expected to
  // call endStage when you are done with the processing stage, otherwise you will deadlock the
  // ring buffer. If T is not available when beginStage is called, this will thread-block until it
  // becomes available. These can be called exactly one time per stage, per frame.
  T *beginStage(StageEnum stage);

  // Release the frame after you are done with the stage. This must be called exactly one time per
  // stage, per frame.
  void endStage(StageEnum stage);

  struct FrameStep {
    FrameStep(Vector<T> &frames) : current(frames.begin()), ready(frames.begin()) {}

    FrameStep(FrameStep &&) = default;
    FrameStep &operator=(FrameStep &&) = default;

    FrameStep(const FrameStep &) = delete;
    FrameStep &operator=(const FrameStep &) = delete;

    std::mutex mtx;
    std::condition_variable cv;
    typename Vector<T>::iterator current;
    typename Vector<T>::iterator ready;
    StageEnum next;
    bool isPaused = false;
    std::atomic<bool> running{false};
  };

  // Increment a frames_ iterator and wrap-around when the end is reached.
  void ringIncrement(typename Vector<T>::iterator &iter);

  // Ring buffer of frames.
  Vector<T> frames_;

  // Map of stages.
  TreeMap<StageEnum, FrameStep> stages_;
  StageEnum firstStage_;
  bool isPaused_ = false;
};

template <typename T, typename StageEnum>
template <typename... Params>
StagedRingBuffer<T, StageEnum>::StagedRingBuffer(
  size_t size, std::initializer_list<StageEnum> stages, Params &&... params) {
  if (size < 2) {
    C8_THROW_INVALID_ARGUMENT("StagedRingBuffer size must be >= 2");
  }

  if (stages.size() < 2) {
    C8_THROW_INVALID_ARGUMENT("StagedRingBuffer number of stages must be >= 2");
  }

  frames_.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    frames_.emplace_back(std::forward<Params>(params)...);
  }

  firstStage_ = *stages.begin();
  FrameStep *lastStage = nullptr;
  for (auto &stage : stages) {
    auto inserted = stages_.emplace(
      std::piecewise_construct, std::forward_as_tuple(stage), std::forward_as_tuple(frames_));
    if (!inserted.second) {
      C8_THROW_INVALID_ARGUMENT("Duplicate stage in StagedRingBuffer construction");
    }
    if (lastStage) {
      lastStage->next = stage;
    }
    lastStage = &inserted.first->second;
  }
  lastStage->next = firstStage_;

  // Set the ready pointer for the first stage to the end of the ring.
  stages_.at(firstStage_).ready = std::prev(frames_.end());
}

template <typename T, typename StageEnum>
T *StagedRingBuffer<T, StageEnum>::beginStage(StageEnum stage) {
  auto &fs = stages_.at(stage);
  std::unique_lock<std::mutex> lock(fs.mtx);

  // Wait until the ready iterator has advanced past current.
  fs.cv.wait(lock, [&fs] { return fs.isPaused || fs.current != fs.ready; });

  if (fs.isPaused) {
    return nullptr;
  }

  fs.running.store(true, std::memory_order_relaxed);

  // Increment the 'current' iterator.
  ringIncrement(fs.current);

  return &(*fs.current);
}

template <typename T, typename StageEnum>
void StagedRingBuffer<T, StageEnum>::endStage(StageEnum stage) {
  auto &current = stages_.at(stage);
  current.running.store(false, std::memory_order_relaxed);
  auto &fs = stages_.at(current.next);
  {
    std::lock_guard<std::mutex> lock(fs.mtx);
    if (!fs.isPaused) {
      ringIncrement(fs.ready);
    }
  }
  // Notify that work on the frame was completed.
  fs.cv.notify_one();
}

template <typename T, typename StageEnum>
void StagedRingBuffer<T, StageEnum>::ringIncrement(typename Vector<T>::iterator &iter) {
  iter++;
  if (iter == frames_.end()) {
    iter = frames_.begin();
  }
}

template <typename T, typename StageEnum>
void StagedRingBuffer<T, StageEnum>::pauseAndClear() {
  for (auto &item : stages_) {
    auto stage = item.first;
    auto &step = item.second;
    {
      std::lock_guard<std::mutex> lock(step.mtx);

      step.isPaused = true;
      step.current = frames_.begin();

      if (stage == firstStage_) {
        step.ready = std::prev(frames_.end());
      } else {
        step.ready = frames_.begin();
      }
    }
    step.cv.notify_one();
  }

  // Spin until all began stages have ended.
  for (auto &item : stages_) {
    auto &step = item.second;
    while (true) {
      std::lock_guard<std::mutex> lock(step.mtx);
      if (!step.running.load(std::memory_order_relaxed)) {
        break;
      }
      // return
    }
  }
  isPaused_ = true;
}

template <typename T, typename StageEnum>
void StagedRingBuffer<T, StageEnum>::resume() {
  for (auto &item : stages_) {
    auto &step = item.second;
    {
      std::lock_guard<std::mutex> lock(step.mtx);
      step.isPaused = false;
    }
  }
  isPaused_ = false;
}

}  // namespace c8
