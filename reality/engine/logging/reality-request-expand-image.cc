// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "reality-request-expand-image.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8/io:capnp-messages",
    "//c8/io:image-io",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//reality/engine/api:reality.capnp-cc",
    "//c8/protolog/api:remote-request.capnp-cc",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x8117d5f7);

#include "c8/c8-log.h"
#include "c8/io/capnp-messages.h"
#include "c8/io/image-io.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/protolog/api/remote-request.capnp.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/logging/reality-request-expand-image.h"

namespace c8 {

void expandImagePtrsToImages(
  const RealityRequest::Reader &request, RealityRequest::Builder requestOut) {
  if (!request.getSensors().getCamera().hasCurrentFrame()) {
    return;
  }

  auto origFrame = request.getSensors().getCamera().getCurrentFrame();

  MutableRootMessage<CameraFrame> imgmsg;
  CameraFrame::Builder frame = imgmsg.builder();

  frame.setVideoTimestampNanos(origFrame.getVideoTimestampNanos());
  frame.setFrameTimestampNanos(origFrame.getFrameTimestampNanos());
  frame.setTimestampNanos(origFrame.getTimestampNanos());

  if (origFrame.getImage().getOneOf().getGrayImagePointer().getUInt8PixelDataPointer() != 0) {
    GrayImagePointer::Reader imagePointer = origFrame.getImage().getOneOf().getGrayImagePointer();

    int rows = imagePointer.getRows();
    int cols = imagePointer.getCols();
    int bytesPerRow = imagePointer.getBytesPerRow();

    // Cast a long to a size_t, possibly downcasting, before reinterpreting as a
    // pointer.
    size_t imageAddr = static_cast<size_t>(imagePointer.getUInt8PixelDataPointer());
    uint8_t *rowStart = reinterpret_cast<uint8_t *>(imageAddr);

    GrayImageData::Builder imageBuilder = frame.getImage().getOneOf().initGrayImageData();
    imageBuilder.setRows(rows);
    imageBuilder.setCols(cols);
    imageBuilder.setBytesPerRow(bytesPerRow);
    imageBuilder.initUInt8PixelData(rows * bytesPerRow);
    std::memcpy(imageBuilder.getUInt8PixelData().begin(), rowStart, rows * bytesPerRow);
  }

  if (origFrame.getUvImage().getOneOf().getGrayImagePointer().getUInt8PixelDataPointer() != 0) {
    GrayImagePointer::Reader imagePointer = origFrame.getUvImage().getOneOf().getGrayImagePointer();

    int rows = imagePointer.getRows();
    int cols = imagePointer.getCols();
    int bytesPerRow = imagePointer.getBytesPerRow();

    // Cast a long to a size_t, possibly downcasting, before reinterpreting as a pointer.
    size_t imageAddr = static_cast<size_t>(imagePointer.getUInt8PixelDataPointer());
    uint8_t *rowStart = reinterpret_cast<uint8_t *>(imageAddr);

    GrayImageData::Builder imageBuilder = frame.getUvImage().getOneOf().initGrayImageData();
    imageBuilder.setRows(rows);
    imageBuilder.setCols(cols);
    imageBuilder.setBytesPerRow(bytesPerRow);
    imageBuilder.initUInt8PixelData(rows * bytesPerRow);
    std::memcpy(imageBuilder.getUInt8PixelData().begin(), rowStart, rows * bytesPerRow);
  }

  requestOut.getSensors().getCamera().initCurrentFrame();
  requestOut.getSensors().getCamera().setCurrentFrame(frame);
}

void expandImagePtrsToJpg(
  const RealityRequest::Reader &request, RealityRequest::Builder requestOut) {
  if (!request.getSensors().getCamera().hasCurrentFrame()) {
    return;
  }

  // Static to avoid reallocation across repeated calls.
  static UPlanePixelBuffer uBuf;
  static VPlanePixelBuffer vBuf;

  auto origFrame = request.getSensors().getCamera().getCurrentFrame();

  MutableRootMessage<CameraFrame> imgmsg;
  CameraFrame::Builder frame = imgmsg.builder();

  frame.setVideoTimestampNanos(origFrame.getVideoTimestampNanos());
  frame.setFrameTimestampNanos(origFrame.getFrameTimestampNanos());
  frame.setTimestampNanos(origFrame.getTimestampNanos());

  auto yMsg = origFrame.getImage().getOneOf().getGrayImagePointer();
  auto yAddr = static_cast<size_t>(yMsg.getUInt8PixelDataPointer());
  ConstYPlanePixels y{
    yMsg.getRows(), yMsg.getCols(), yMsg.getBytesPerRow(), reinterpret_cast<uint8_t *>(yAddr)};

  auto uvMsg = origFrame.getUvImage().getOneOf().getGrayImagePointer();
  auto uvAddr = static_cast<size_t>(uvMsg.getUInt8PixelDataPointer());
  ConstUVPlanePixels uv{
    uvMsg.getRows(), uvMsg.getCols(), uvMsg.getBytesPerRow(), reinterpret_cast<uint8_t *>(uvAddr)};

  // Reallocate scratch space if needed.
  if (uBuf.pixels().rows() != uv.rows() || uBuf.pixels().cols() != uv.cols()) {
    uBuf = UPlanePixelBuffer{uv.rows(), uv.cols()};
    vBuf = VPlanePixelBuffer{uv.rows(), uv.cols()};
  }
  splitPixels(uv, uBuf.pixels(), vBuf.pixels());

  // Encode our the image prior to storing the blob. Since JPG is internally in YUV, this
  // skips extra conversions.
  auto imageBuilder = frame.getRGBAImage().getOneOf().initCompressedImageData();
  imageBuilder.setHeight(y.rows());
  imageBuilder.setWidth(y.cols());
  imageBuilder.setEncoding(CompressedImageData::Encoding::JPG_RGBA);

  Vector<uint8_t> encodedBuf = writePixelsToJpg(y, uBuf.pixels(), vBuf.pixels(), 90);
  imageBuilder.initData(encodedBuf.size());
  std::memcpy(imageBuilder.getData().begin(), encodedBuf.data(), encodedBuf.size());

  requestOut.getSensors().getCamera().initCurrentFrame();
  requestOut.getSensors().getCamera().setCurrentFrame(frame);

  // TODO(nb): Convert depth buffers to sqrt millimeter uint8_t images and compress.
  // Note: per google, arcore depth estimation quality decays quadratically, so this should be
  // optimal.
}

}  // namespace c8
