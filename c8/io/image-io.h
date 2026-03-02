// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Read/write compressed images from the filesystem.

#pragma once

#include "c8/exceptions.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {
// Read a PNG or JPEG image file on disk into a pixel buffer, using magic bytes to guess the format.
YPlanePixelBuffer readImageToY(const String &filename);
RGB888PlanePixelBuffer readImageToRGB(const String &filename);
BGR888PlanePixelBuffer readImageToBGR(const String &filename);
BGRA8888PlanePixelBuffer readImageToBGRA(const String &filename);
RGBA8888PlanePixelBuffer readImageToRGBA(const String &filename);

// Read a JPEG file on disk into a pixel buffer.
YPlanePixelBuffer readJpgToY(const String &filename);
RGB888PlanePixelBuffer readJpgToRGB(const String &filename);
BGR888PlanePixelBuffer readJpgToBGR(const String &filename);
BGRA8888PlanePixelBuffer readJpgToBGRA(const String &filename);
RGBA8888PlanePixelBuffer readJpgToRGBA(const String &filename);

// Read a PNG file on disk into a pixel buffer.
YPlanePixelBuffer readPngToY(const String &filename);
RGB888PlanePixelBuffer readPngToRGB(const String &filename);
BGR888PlanePixelBuffer readPngToBGR(const String &filename);
BGRA8888PlanePixelBuffer readPngToBGRA(const String &filename);
RGBA8888PlanePixelBuffer readPngToRGBA(const String &filename);

// Read a GIF file on disk into a pixel buffer.
RGBA8888PlanePixelBuffer readGifToRGBA(const String &filename);

// Read a JPEG buffer in memory into a pixel buffer
YPlanePixelBuffer readJpgToY(const uint8_t *buffer, long bufferSize);
RGB888PlanePixelBuffer readJpgToRGB(const uint8_t *buffer, long bufferSize);
BGR888PlanePixelBuffer readJpgToBGR(const uint8_t *buffer, long bufferSize);
BGRA8888PlanePixelBuffer readJpgToBGRA(const uint8_t *buffer, long bufferSize);
RGBA8888PlanePixelBuffer readJpgToRGBA(const uint8_t *buffer, long bufferSize);
// Allocate buffer by reading the jpg header. Buffers can then be used with readJpgToPixels.
void readJpgToPixelsAllocate(
  const uint8_t *buffer,
  long bufferSize,
  YPlanePixelBuffer *yBuf,
  UPlanePixelBuffer *uBuf,
  VPlanePixelBuffer *vBuf);
// Read data into memory. This is the invert of writePixelsToJpg(ConstY,ConstU,ConstV).
void readJpgToPixels(
  const uint8_t *buffer,
  long bufferSize,
  YPlanePixels yImage,
  UPlanePixels uImage,
  VPlanePixels vImage);

// Read a PNG buffer in memory into a pixel buffer
YPlanePixelBuffer readPngToY(const uint8_t *buffer, long bufferSize);
RGB888PlanePixelBuffer readPngToRGB(const uint8_t *buffer, long bufferSize);
BGR888PlanePixelBuffer readPngToBGR(const uint8_t *buffer, long bufferSize);
BGRA8888PlanePixelBuffer readPngToBGRA(const uint8_t *buffer, long bufferSize);
RGBA8888PlanePixelBuffer readPngToRGBA(const uint8_t *buffer, long bufferSize);

// Read a GIF buffer in memory into a pixel buffer
RGBA8888PlanePixelBuffer readGifToRGBA(const uint8_t *buffer, long bufferSize);

// Allocate buffer by reading the gif header. Buffers can then be used with readGifToPixels.
void readGifToPixelsAllocate(
  const uint8_t *buffer, long bufferSize, RGBA8888PlanePixelBuffer *rgbaBuf);

// Read data into memory.
void readGifToPixels(const uint8_t *buffer, long bufferSize, RGBA8888PlanePixels rgbaImage);

// Write an image file on disk into a pixel buffer, guessing the encoding based on filename.
void writeImage(ConstYPlanePixels image, const String &filename, int quality = 85);
void writeImage(ConstRGB888PlanePixels image, const String &filename, int quality = 85);
void writeImage(ConstBGR888PlanePixels image, const String &filename, int quality = 85);
void writeImage(ConstBGRA8888PlanePixels image, const String &filename, int quality = 85);
void writeImage(ConstRGBA8888PlanePixels image, const String &filename, int quality = 85);

// Write pixels to a JPEG file on disk.
void writePixelsToJpg(ConstYPlanePixels image, const String &filename, int quality = 85);
void writePixelsToJpg(ConstRGB888PlanePixels image, const String &filename, int quality = 85);
void writePixelsToJpg(ConstBGR888PlanePixels image, const String &filename, int quality = 85);
void writePixelsToJpg(ConstBGRA8888PlanePixels image, const String &filename, int quality = 85);
void writePixelsToJpg(ConstRGBA8888PlanePixels image, const String &filename, int quality = 85);

// Write pixels to a JPEG a buffer in memory.
Vector<uint8_t> writePixelsToJpg(ConstYPlanePixels image, int quality = 85);
Vector<uint8_t> writePixelsToJpg(ConstRGB888PlanePixels image, int quality = 85);
Vector<uint8_t> writePixelsToJpg(ConstBGR888PlanePixels image, int quality = 85);
Vector<uint8_t> writePixelsToJpg(ConstBGRA8888PlanePixels pixels, int quality = 85);
Vector<uint8_t> writePixelsToJpg(ConstRGBA8888PlanePixels image, int quality = 85);
// NOTE(dat): libjpeg-turbo uses a different RGB <-> YUV conversion formula than what we have in
// pixels-transform. If you used our formula in pixels-transform to generate these YUV planes,
// make sure to use readJpgToPixels that read the data back into YUV planes. Then perform our
// yuv-to-rgb conversion so the color will look correct. Not doing this will result in an image
// in rgb looking more green and purple.
Vector<uint8_t> writePixelsToJpg(
  ConstYPlanePixels yImage, ConstUPlanePixels uImage, ConstVPlanePixels vImage, int quality = 85);

// Write pixels to a PNG file on disk.
void writePixelsToPng(ConstYPlanePixels image, const String &filename);
void writePixelsToPng(ConstRGB888PlanePixels image, const String &filename);
void writePixelsToPng(ConstBGR888PlanePixels image, const String &filename);
void writePixelsToPng(ConstBGRA8888PlanePixels image, const String &filename);
void writePixelsToPng(ConstRGBA8888PlanePixels image, const String &filename);

// Write pixels to a PNG a buffer in memory.
Vector<uint8_t> writePixelsToPng(ConstYPlanePixels image);
Vector<uint8_t> writePixelsToPng(ConstRGB888PlanePixels image);
Vector<uint8_t> writePixelsToPng(ConstBGR888PlanePixels image);
Vector<uint8_t> writePixelsToPng(ConstBGRA8888PlanePixels image);
Vector<uint8_t> writePixelsToPng(ConstRGBA8888PlanePixels image);

}  // namespace c8
