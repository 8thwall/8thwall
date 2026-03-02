// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/map.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/media-recorder.h"

namespace c8 {

// A conveninece wrapper for MediaRecorder that enables easy writing of RGBA pixel buffers to a
// video file without sound.
//
// Videos can either be 30FPS (by using the api without timestamps) or variable bitrate (by using
// the api with timestamps).
//
// A typical usage pattern would be:
//
// VideoWriter video;
// video.startFixedFramerate("/tmp/video.mp4", 480, 640, 30.0);
// for (ConstRGBA8888PlanePixels frame : frames) {
//   video.encode(frame);
// }
// video.finish();
class VideoWriter {
public:
  VideoWriter() = default;

  // Fixed FPS variants.
  MediaStatus startFixedFramerate(const String &path, int rows, int cols, double fps);
  MediaStatus encode(ConstRGBA8888PlanePixels frame);

  // Variable framerate variants. The first frame will begin the video, and subsequent frames are
  // expected to have increasing times offset from the first time.
  MediaStatus startVariableFramerate(const String &path, int rows, int cols);
  MediaStatus encode(ConstRGBA8888PlanePixels frame, double frameTimeSeconds);

  // Complete writing the file. No encode operations should be called after finish.
  MediaStatus finish();

  // Default move constructors.
  VideoWriter(VideoWriter &&) = default;
  VideoWriter &operator=(VideoWriter &&) = default;

  // Disallow copying.
  VideoWriter(const VideoWriter &) = delete;
  VideoWriter &operator=(const VideoWriter &) = delete;

private:
  MediaStatus start(const String &Path, int rows, int cols, bool variableFramerate, double fps);
  MediaRecorder recorder_;
  YUVA8888PlanePixelBuffer yuv_;
  YPlanePixelBuffer y_;
  UPlanePixelBuffer u_;
  VPlanePixelBuffer v_;
  UPlanePixelBuffer us_;
  VPlanePixelBuffer vs_;
  bool variableFramerate_ = false;
  bool hasFrames_ = false;
  double startTimeSeconds_ = 0.0;
};

// A video collection manages a set of videos keyed by file path. A file is initialized when the
// first frame is written to a given path, and all videos are finished on the call to finish.
//
// Videos can either be fixed framerate (by using the api without timestamps) or variable bitrate
// (by using the api with timestamps). The default framerate is 30, but this can be changed with
// setFpsForFixedFramerate().
//
// A typical usage pattern would be:
//
// VideoCollection videos;
// for (auto &frameAndSensorData : sensorSequence) {
//   double frameTimeSeconds = frameAndSensorData.frameTimeNanos() / 1e9;
//   ...  // do some processing on the frame.
//   videos.encode("/tmp/viz1.mp4", viz1, frameTimeSeconds);
//   ...  // do some processing on the frame.
//   videos.encode("/tmp/viz2.mp4", viz2, frameTimeSeconds);
//   ...  // do some processing on the frame.
//   videos.encode("/tmp/viz3.mp4", viz3, frameTimeSeconds);
// }
// auto filenames = videos.finish();
// for (const auto &file : filenames) {
//   C8Log("Wrote file %s", file.c_str());  // prints /tmp/viz1.mp4, /tmp/viz2.mp4, /tmp/viz3.mp4
// }
class VideoCollection {
public:
  VideoCollection() = default;

  // Disallow move; std::map can't be moved, so these would be deleted anyway. This just makes it
  // more clear.
  VideoCollection(VideoCollection &&) = delete;
  VideoCollection &operator=(VideoCollection &&) = delete;

  // Disallow copying.
  VideoCollection(const VideoCollection &) = delete;
  VideoCollection &operator=(const VideoCollection &) = delete;

  // Fixed framerate variant. The default 30FPS can be changed for new videos using
  // setFpsForFixedFramerate.
  MediaStatus encode(const String &path, ConstRGBA8888PlanePixels frame);

  // Variable framerate variant.
  MediaStatus encode(
    const String &path,
    ConstRGBA8888PlanePixels frame,
    double frameTimeSeconds);

  // Finish all videos that this VideoCollection is managing, and return the names of the files that
  // were written.
  Vector<String> finish();

  // Override the default framerate for fixed framerate encoding.
  void setFpsForFixedFramerate(double fps) {fps_ = fps;}

private:
  MediaStatus encode(
    const String &path,
    ConstRGBA8888PlanePixels frame,
    bool hasFrameTime,
    double frameTimeSeconds);
  TreeMap<String, VideoWriter> videos_;
  double fps_ = 30.0;
};

}  // namespace c8
