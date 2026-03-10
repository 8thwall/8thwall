// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"video-writer.h"};
  deps = {
    "//c8:exceptions",
    "//c8:map",
    "//c8:string",
    "//c8:vector",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels:pixels",
    "//c8/string:contains",
    "//c8/string:format",
    "//c8/string:strcat",
    "//c8/media:media-recorder",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x6db2ce6b);

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/io/video-writer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/string/contains.h"
#include "c8/string/format.h"
#include "c8/string/strcat.h"

namespace c8 {

namespace {

bool isMkv(const String &filename) {
  if (
    endsWith(filename, ".mkv") || endsWith(filename, ".webm") || endsWith(filename, ".MKV")
    || endsWith(filename, ".WEBM")) {
    return true;
  }
  return false;
}

bool isMp4(const String &filename) {
  if (endsWith(filename, ".mp4") || endsWith(filename, ".MP4")) {
    return true;
  }
  return false;
}

String guessContainerFormat(const String &filename) {
  if (isMp4(filename)) {
    return "mp4";
  }
  if (isMkv(filename)) {
    return "mkv";
  }

  C8_THROW(strCat("Couldn't guess image format of file ", filename));
}

}  // namespace

MediaStatus VideoWriter::startFixedFramerate(const String &path, int rows, int cols, double fps) {
  return start(path, rows, cols, false, fps);
}

MediaStatus VideoWriter::startVariableFramerate(const String &path, int rows, int cols) {
  return start(path, rows, cols, true, 30.0);
}

MediaStatus VideoWriter::start(
  const String &path, int rows, int cols, bool variableFramerate, double fps) {
  variableFramerate_ = variableFramerate;
  auto status = recorder_.start(format(
    R"({
      "path": "%s",
      "container": "%s",
      "timescale": 90000,
      "optimize": true,
      "tracks": [{
        "name": "video",
        "codec": "h264",
        "format": "yuv420p",
        "profile": 66,
        "timescale": 90000,
        "qp": 23,
        "fps": %f,
        "width": %d,
        "height": %d,
        "rcMode": %d,
        "bitrate": 5000000
      }]
    })",
    path.c_str(),
    guessContainerFormat(path).c_str(),
    fps,
    cols,
    rows,
    variableFramerate_ ? 3 : 0));

  if (status.code()) {
    return status;
  }

  yuv_ = YUVA8888PlanePixelBuffer(rows, cols);
  y_ = YPlanePixelBuffer(rows, cols);
  u_ = UPlanePixelBuffer(rows, cols);
  v_ = VPlanePixelBuffer(rows, cols);
  us_ = UPlanePixelBuffer(rows >> 1, cols >> 1);
  vs_ = VPlanePixelBuffer(rows >> 1, cols >> 1);

  return {};
};  // namespace c8

MediaStatus VideoWriter::encode(ConstRGBA8888PlanePixels frame) { return encode(frame, 0.0); }

MediaStatus VideoWriter::encode(ConstRGBA8888PlanePixels frame, double frameTimeSeconds) {
  if (!hasFrames_) {
    startTimeSeconds_ = frameTimeSeconds;
    hasFrames_ = true;
  }
  auto yuv = yuv_.pixels();
  auto y = y_.pixels();
  auto u = u_.pixels();
  auto v = v_.pixels();
  auto us = us_.pixels();
  auto vs = vs_.pixels();

  applyColorMatrixKeepAlpha(ColorMat::rgbToYCbCrBt709Digital(), frame, &yuv);
  OneChannelPixels *channels[3] = {&y, &u, &v};
  splitPixels<3>(yuv, channels);
  downsize(u, &us);
  downsize(v, &vs);

  String opts = variableFramerate_ ? format(
                  R"({"name": "video", "timestamp": %d})",
                  static_cast<int>((frameTimeSeconds - startTimeSeconds_) * 90000))
                                   : R"({"name": "video"})";

  return recorder_.encodePlanarYUV(opts, y_.pixels(), us_.pixels(), vs_.pixels());
}

MediaStatus VideoWriter::finish() { return recorder_.finish(); }

MediaStatus VideoCollection::encode(const String &path, ConstRGBA8888PlanePixels frame) {
  return encode(path, frame, false, 0.0);
}

MediaStatus VideoCollection::encode(
  const String &path, ConstRGBA8888PlanePixels frame, double frameTimeSeconds) {
  return encode(path, frame, true, frameTimeSeconds);
}

MediaStatus VideoCollection::encode(
  const String &path, ConstRGBA8888PlanePixels frame, bool hasFrameTime, double frameTimeSeconds) {
  MediaStatus status;
  auto lookup = videos_.find(path);
  if (lookup == videos_.end()) {
    if (hasFrameTime) {
      status = videos_[path].startVariableFramerate(path, frame.rows(), frame.cols());
    } else {
      status = videos_[path].startFixedFramerate(path, frame.rows(), frame.cols(), fps_);
    }
    lookup = videos_.find(path);
  }

  if (status.code()) {
    return status;
  }

  lookup->second.encode(frame, frameTimeSeconds);
  return status;
}

Vector<String> VideoCollection::finish() {
  Vector<String> written;
  for (auto &[path, video] : videos_) {
    auto status = video.finish();
    if (status.code()) {
      C8Log("[video-writer] Couldn't finish video '%s': '%s'", path.c_str(), status.message());
      continue;
    }
    written.push_back(path);
  }
  videos_.clear();
  return written;
}

}  // namespace c8
