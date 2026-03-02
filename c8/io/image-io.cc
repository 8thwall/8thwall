// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "image-io.h",
  };
  deps = {
    ":file-io",
    ":mime-sniff",
    ":png-buffer-reader",
    ":png-buffer-writer",
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:scope-exit",
    "//c8:string",
    "//c8/string:contains",
    "//c8/string:strcat",
    "//c8:vector",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels:pixels",
    "//third_party/easy-gif-reader",
    "@libjpegturbo//:libjpeg-turbo",
    "@png//:png",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xfe753c31);

#include <cstdio>
#include <functional>
#include <iostream>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/io/mime-sniff.h"
#include "c8/io/png-buffer-reader.h"
#include "c8/io/png-buffer-writer.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/scope-exit.h"
#include "c8/string.h"
#include "c8/string/contains.h"
#include "c8/string/strcat.h"
#include "external/libjpegturbo/turbojpeg.h"
#include "external/png/png.h"
#include "image-io.h"
#include "third_party/easy-gif-reader/easy-gif-reader.h"

namespace c8 {

namespace {

template <class PixelsType, int bytesPerPix>
PixelBuffer<PixelsType, bytesPerPix> readJpgToPixelBuffer(
  const uint8_t *buffer, long bufferSize, TJPF format);

template <class PixelsType, int bytesPerPix>
PixelBuffer<PixelsType, bytesPerPix> readJpgToPixelBuffer(const String &filename, TJPF format);

template <class PixelsType, int bytesPerPix>
PixelBuffer<PixelsType, bytesPerPix> readPngToPixelBuffer(
  const uint8_t *buffer, long bufferSize, int format);

template <class ConstPixelsType>
void writePixelsToJpgInternal(
  ConstPixelsType pixels, const String &filename, TJPF format, TJSAMP subSamp, int quality);

template <class ConstPixelsType>
Vector<uint8_t> writePixelsToJpgInternal(
  ConstPixelsType pixels, TJPF format, TJSAMP subSamp, int quality);

bool isJpg(const String &filename) {
  if (
    endsWith(filename, ".jpg") || endsWith(filename, ".jpeg") || endsWith(filename, ".JPG")
    || endsWith(filename, ".JPEG")) {
    return true;
  }
  return false;
}

bool isPng(const String &filename) {
  if (endsWith(filename, ".png") || endsWith(filename, ".PNG")) {
    return true;
  }
  return false;
}

MimeType inferImageType(const String &filename) {
  FILE *file = std::fopen(filename.c_str(), "rb");
  SCOPE_EXIT([file] { std::fclose(file); });

  // Read the first 32 bytes of the file.
  std::array<uint8_t, 32> buffer = {};
  fread(buffer.data(), 1, buffer.size(), file);

  if (std::ferror(file)) {
    C8Log("[file-io] %s", "Failed to read file");
    C8_THROW("Failed to read file");
  }

  return matchImageMimeType(buffer);
}

template <typename T>
T readImageToPixels(
  const String &filename,
  T (*readJpgT)(const String &),
  T (*readPngT)(const String &),
  T (*readGifT)(const String &) = nullptr) {
  MimeType imageType = inferImageType(filename);
  switch (imageType) {
    case MimeType::IMAGE_JPEG:
      return readJpgT(filename);
    case MimeType::IMAGE_PNG:
      return readPngT(filename);
    case MimeType::IMAGE_GIF:
      if (readGifT) {
        return readGifT(filename);
      }
      C8_THROW("GIF decoding not supported for that format: %c", typeid(T).name());
    case MimeType::UNDEFINED:
      C8_THROW(strCat("Couldn't guess image format of file ", filename));
    default:
      C8_THROW(strCat("Unsupported image MIME type: ", mimeTypeString(imageType)));
  }
}

}  // namespace

// Read an image file on disk into a pixel buffer, guessing the encoding based on filename.
YPlanePixelBuffer readImageToY(const String &filename) {
  return readImageToPixels(filename, readJpgToY, readPngToY);
}

RGB888PlanePixelBuffer readImageToRGB(const String &filename) {
  return readImageToPixels(filename, readJpgToRGB, readPngToRGB);
}
BGR888PlanePixelBuffer readImageToBGR(const String &filename) {
  return readImageToPixels(filename, readJpgToBGR, readPngToBGR);
}
BGRA8888PlanePixelBuffer readImageToBGRA(const String &filename) {
  return readImageToPixels(filename, readJpgToBGRA, readPngToBGRA);
}
RGBA8888PlanePixelBuffer readImageToRGBA(const String &filename) {
  return readImageToPixels(filename, readJpgToRGBA, readPngToRGBA, readGifToRGBA);
}

void writeImage(ConstYPlanePixels image, const String &filename, int quality) {
  if (isJpg(filename)) {
    return writePixelsToJpg(image, filename, quality);
  }
  if (isPng(filename)) {
    return writePixelsToPng(image, filename);
  }
  C8_THROW(strCat("Couldn't guess image format of file ", filename));
}
void writeImage(ConstRGB888PlanePixels image, const String &filename, int quality) {
  if (isJpg(filename)) {
    return writePixelsToJpg(image, filename, quality);
  }
  if (isPng(filename)) {
    return writePixelsToPng(image, filename);
  }
  C8_THROW(strCat("Couldn't guess image format of file ", filename));
}
void writeImage(ConstBGR888PlanePixels image, const String &filename, int quality) {
  if (isJpg(filename)) {
    return writePixelsToJpg(image, filename, quality);
  }
  if (isPng(filename)) {
    return writePixelsToPng(image, filename);
  }
  C8_THROW(strCat("Couldn't guess image format of file ", filename));
}
void writeImage(ConstBGRA8888PlanePixels image, const String &filename, int quality) {
  if (isJpg(filename)) {
    return writePixelsToJpg(image, filename, quality);
  }
  if (isPng(filename)) {
    return writePixelsToPng(image, filename);
  }
  C8_THROW(strCat("Couldn't guess image format of file ", filename));
}
void writeImage(ConstRGBA8888PlanePixels image, const String &filename, int quality) {
  if (isJpg(filename)) {
    return writePixelsToJpg(image, filename, quality);
  }
  if (isPng(filename)) {
    return writePixelsToPng(image, filename);
  }
  C8_THROW(strCat("Couldn't guess image format of file ", filename));
}

YPlanePixelBuffer readJpgToY(const String &filename) {
  return readJpgToPixelBuffer<YPlanePixels, 1>(filename, TJPF_GRAY);
}

RGB888PlanePixelBuffer readJpgToRGB(const String &filename) {
  return readJpgToPixelBuffer<RGB888PlanePixels, 3>(filename, TJPF_RGB);
}

BGR888PlanePixelBuffer readJpgToBGR(const String &filename) {
  return readJpgToPixelBuffer<BGR888PlanePixels, 3>(filename, TJPF_BGR);
}

BGRA8888PlanePixelBuffer readJpgToBGRA(const String &filename) {
  return readJpgToPixelBuffer<BGRA8888PlanePixels, 4>(filename, TJPF_BGRA);
}

RGBA8888PlanePixelBuffer readJpgToRGBA(const String &filename) {
  return readJpgToPixelBuffer<RGBA8888PlanePixels, 4>(filename, TJPF_RGBA);
}

YPlanePixelBuffer readJpgToY(const uint8_t *buffer, long bufferSize) {
  return readJpgToPixelBuffer<YPlanePixels, 1>(buffer, bufferSize, TJPF_GRAY);
}

RGB888PlanePixelBuffer readJpgToRGB(const uint8_t *buffer, long bufferSize) {
  return readJpgToPixelBuffer<RGB888PlanePixels, 3>(buffer, bufferSize, TJPF_RGB);
}

BGR888PlanePixelBuffer readJpgToBGR(const uint8_t *buffer, long bufferSize) {
  return readJpgToPixelBuffer<BGR888PlanePixels, 3>(buffer, bufferSize, TJPF_BGR);
}

BGRA8888PlanePixelBuffer readJpgToBGRA(const uint8_t *buffer, long bufferSize) {
  return readJpgToPixelBuffer<BGRA8888PlanePixels, 4>(buffer, bufferSize, TJPF_BGRA);
}

RGBA8888PlanePixelBuffer readJpgToRGBA(const uint8_t *buffer, long bufferSize) {
  return readJpgToPixelBuffer<RGBA8888PlanePixels, 4>(buffer, bufferSize, TJPF_RGBA);
}

void readJpgToPixelsAllocate(
  const uint8_t *buffer,
  long bufferSize,
  YPlanePixelBuffer *yBuf,
  UPlanePixelBuffer *uBuf,
  VPlanePixelBuffer *vBuf) {
  auto decompressor = tjInitDecompress();
  if (!decompressor) {
    C8_THROW("Failed to initialize JPEG decompressor");
  }
  SCOPE_EXIT([&decompressor] { tjDestroy(decompressor); });

  int width, height, subSamp;
  // Logically const.
  if (tjDecompressHeader2(
        decompressor, const_cast<uint8_t *>(buffer), bufferSize, &width, &height, &subSamp)) {
    C8_THROW(String("JPEG error - ") + tjGetErrorStr());
  }
  *yBuf = {height, width};
  *uBuf = {height / 2, width / 2};
  *vBuf = {height / 2, width / 2};
}

void readJpgToPixels(
  const uint8_t *buffer,
  long bufferSize,
  YPlanePixels yImage,
  UPlanePixels uImage,
  VPlanePixels vImage) {
  auto compressor = tjInitDecompress();
  if (!compressor) {
    C8_THROW("Failed to initialize JPEG decompressor");
  }
  SCOPE_EXIT([&compressor] { tjDestroy(compressor); });

  uint8_t *dstPlanes[3] = {yImage.pixels(), uImage.pixels(), vImage.pixels()};
  int strides[3] = {yImage.rowBytes(), uImage.rowBytes(), vImage.rowBytes()};

  if (tjDecompressToYUVPlanes(
        compressor, buffer, bufferSize, dstPlanes, yImage.cols(), strides, yImage.rows(), 0)) {
    C8_THROW(String("JPEG error - ") + tjGetErrorStr());
  }
}

YPlanePixelBuffer readPngToY(const String &filename) {
  auto inputBuffer = readFile(filename);
  return readPngToY(inputBuffer.data(), inputBuffer.size());
}

RGB888PlanePixelBuffer readPngToRGB(const String &filename) {
  auto inputBuffer = readFile(filename);
  return readPngToRGB(inputBuffer.data(), inputBuffer.size());
}

BGR888PlanePixelBuffer readPngToBGR(const String &filename) {
  auto inputBuffer = readFile(filename);
  return readPngToBGR(inputBuffer.data(), inputBuffer.size());
}

BGRA8888PlanePixelBuffer readPngToBGRA(const String &filename) {
  auto inputBuffer = readFile(filename);
  return readPngToBGRA(inputBuffer.data(), inputBuffer.size());
}

RGBA8888PlanePixelBuffer readPngToRGBA(const String &filename) {
  auto inputBuffer = readFile(filename);
  return readPngToRGBA(inputBuffer.data(), inputBuffer.size());
}

YPlanePixelBuffer readPngToY(const uint8_t *buffer, long bufferSize) {
  PngBufferReader pngReader(buffer, bufferSize);
  pngReader.readHeader();
  if (pngReader.colorType() != PNG_COLOR_TYPE_GRAY) {
    pngReader.setRgbToGray();
  }
  if (pngReader.bitDepth() > 8) {
    pngReader.setScaleDownTo8();
  } else if (pngReader.bitDepth() < 8) {
    pngReader.setExpandTo8();
  }
  return pngReader.read<YPlanePixels, 1>();
}

RGB888PlanePixelBuffer readPngToRGB(const uint8_t *buffer, long bufferSize) {
  PngBufferReader pngReader(buffer, bufferSize);
  pngReader.readHeader();
  if (pngReader.bitDepth() > 8) {
    pngReader.setScaleDownTo8();
  } else if (pngReader.bitDepth() < 8) {
    pngReader.setExpandTo8();
  }
  if (pngReader.colorType() & PNG_COLOR_MASK_ALPHA) {
    pngReader.setStripAlpha();
  }
  return pngReader.read<RGB888PlanePixels, 3>();
}

BGR888PlanePixelBuffer readPngToBGR(const uint8_t *buffer, long bufferSize) {
  PngBufferReader pngReader(buffer, bufferSize);
  pngReader.readHeader();
  if (pngReader.bitDepth() > 8) {
    pngReader.setScaleDownTo8();
  } else if (pngReader.bitDepth() < 8) {
    pngReader.setExpandTo8();
  }
  if (pngReader.colorType() == PNG_COLOR_TYPE_RGB) {
    pngReader.setBgr();
  }
  if (pngReader.colorType() & PNG_COLOR_MASK_ALPHA) {
    pngReader.setStripAlpha();
  }
  return pngReader.read<BGR888PlanePixels, 3>();
}

BGRA8888PlanePixelBuffer readPngToBGRA(const uint8_t *buffer, long bufferSize) {
  PngBufferReader pngReader(buffer, bufferSize);
  pngReader.readHeader();
  if (pngReader.bitDepth() > 8) {
    pngReader.setScaleDownTo8();
  } else if (pngReader.bitDepth() < 8) {
    pngReader.setExpandTo8();
  }
  if (pngReader.colorType() == PNG_COLOR_TYPE_RGB) {
    pngReader.setBgr();
    pngReader.setAddAlpha(0xff);
  }
  if (pngReader.colorType() == PNG_COLOR_TYPE_RGB_ALPHA) {
    pngReader.setBgr();
  }
  if (pngReader.colorType() == PNG_COLOR_TYPE_PALETTE) {
    pngReader.setPaletteToRgb();
    pngReader.setBgr();
    pngReader.setAddAlpha(0xff);
  }
  if (
    pngReader.colorType() == PNG_COLOR_TYPE_GRAY
    || pngReader.colorType() == PNG_COLOR_TYPE_GRAY_ALPHA) {
    pngReader.setGrayToRgb();
    pngReader.setBgr();
    pngReader.setAddAlpha(0xff);
  }
  return pngReader.read<BGRA8888PlanePixels, 4>();
}

RGBA8888PlanePixelBuffer readPngToRGBA(const uint8_t *buffer, long bufferSize) {
  PngBufferReader pngReader(buffer, bufferSize);
  pngReader.readHeader();
  if (pngReader.bitDepth() > 8) {
    pngReader.setScaleDownTo8();
  } else if (pngReader.bitDepth() < 8) {
    pngReader.setExpandTo8();
  }
  if (pngReader.colorType() == PNG_COLOR_TYPE_PALETTE) {
    pngReader.setPaletteToRgb();
  }
  if (
    pngReader.colorType() == PNG_COLOR_TYPE_GRAY
    || pngReader.colorType() == PNG_COLOR_TYPE_GRAY_ALPHA) {
    pngReader.setGrayToRgb();
  }

  // This transformation doesn't affect images that already have an alpha channel.
  pngReader.setAddAlpha(0xffff);
  return pngReader.read<RGBA8888PlanePixels, 4>();
}

RGBA8888PlanePixelBuffer readGifToRGBA(const String &filename) {
  auto inputBuffer = readFile(filename);
  return readGifToRGBA(inputBuffer.data(), inputBuffer.size());
}

RGBA8888PlanePixelBuffer readGifToRGBA(const uint8_t *buffer, long bufferSize) {
  RGBA8888PlanePixelBuffer output{};
  readGifToPixelsAllocate(buffer, bufferSize, &output);
  readGifToPixels(buffer, bufferSize, output.pixels());
  return output;
}

void readGifToPixelsAllocate(
  const uint8_t *buffer, long bufferSize, RGBA8888PlanePixelBuffer *rgbaBuf) {
  auto gifReader = EasyGifReader::openMemory(buffer, bufferSize);
  int gifWidth = gifReader.width();
  int gifHeight = gifReader.height();
  if (gifWidth == 0 || gifHeight == 0) {
    C8_THROW("[gif-buffer-reader] %s", "GIF has invalid width or height");
  }
  *rgbaBuf = {gifHeight, gifWidth};
}

void readGifToPixels(const uint8_t *buffer, long bufferSize, RGBA8888PlanePixels rgbaImage) {
  auto gifReader = EasyGifReader::openMemory(buffer, bufferSize);
  int gifWidth = gifReader.width();
  int gifHeight = gifReader.height();
  if (gifWidth == 0 || gifHeight == 0) {
    C8_THROW("[gif-buffer-reader] %s", "GIF has invalid width or height");
  }
  if (gifWidth != rgbaImage.cols() || gifHeight != rgbaImage.rows()) {
    C8_THROW("[gif-buffer-reader] %s", "GIF size doesn't match the provided buffer");
  }
  if (gifReader.frameCount() > 1) {
    C8Log("[gif-buffer-reader] GIF has %s frames, using the first frame", gifReader.frameCount());
  }
  // Only get the first frame.
  auto &&frame = gifReader.begin();
  auto &&pixels = frame.pixels();
  // The library is default to read RGBA format.
  std::copy(pixels, pixels + (gifHeight * gifWidth * 4), rgbaImage.pixels());
}

void writePixelsToJpg(ConstYPlanePixels pixels, const String &filename, int quality) {
  writePixelsToJpgInternal(pixels, filename, TJPF_GRAY, TJSAMP_GRAY, quality);
}

// Only apply to multi-channel
TJSAMP getSubSampForQuality(int quality) { return (quality >= 88) ? TJSAMP_444 : TJSAMP_420; }

void writePixelsToJpg(ConstRGB888PlanePixels pixels, const String &filename, int quality) {
  writePixelsToJpgInternal(pixels, filename, TJPF_RGB, getSubSampForQuality(quality), quality);
}

void writePixelsToJpg(ConstBGR888PlanePixels pixels, const String &filename, int quality) {
  writePixelsToJpgInternal(pixels, filename, TJPF_BGR, getSubSampForQuality(quality), quality);
}

void writePixelsToJpg(ConstBGRA8888PlanePixels pixels, const String &filename, int quality) {
  writePixelsToJpgInternal(pixels, filename, TJPF_BGRA, getSubSampForQuality(quality), quality);
}

void writePixelsToJpg(ConstRGBA8888PlanePixels pixels, const String &filename, int quality) {
  writePixelsToJpgInternal(pixels, filename, TJPF_RGBA, getSubSampForQuality(quality), quality);
}

Vector<uint8_t> writePixelsToJpg(ConstYPlanePixels pixels, int quality) {
  return writePixelsToJpgInternal(pixels, TJPF_GRAY, TJSAMP_GRAY, quality);
}

Vector<uint8_t> writePixelsToJpg(ConstRGB888PlanePixels pixels, int quality) {
  return writePixelsToJpgInternal(pixels, TJPF_RGB, getSubSampForQuality(quality), quality);
}

Vector<uint8_t> writePixelsToJpg(ConstBGR888PlanePixels pixels, int quality) {
  return writePixelsToJpgInternal(pixels, TJPF_BGR, getSubSampForQuality(quality), quality);
}

Vector<uint8_t> writePixelsToJpg(ConstBGRA8888PlanePixels pixels, int quality) {
  return writePixelsToJpgInternal(pixels, TJPF_BGRA, getSubSampForQuality(quality), quality);
}

Vector<uint8_t> writePixelsToJpg(ConstRGBA8888PlanePixels pixels, int quality) {
  return writePixelsToJpgInternal(pixels, TJPF_RGBA, getSubSampForQuality(quality), quality);
}

Vector<uint8_t> writePixelsToJpg(
  ConstYPlanePixels yImage, ConstUPlanePixels uImage, ConstVPlanePixels vImage, int quality) {
  auto compressor = tjInitCompress();
  if (!compressor) {
    C8_THROW("Failed to initialize JPEG compressor");
  }
  SCOPE_EXIT([&compressor] { tjDestroy(compressor); });

  uint8_t *jpgBuffer = nullptr;
  SCOPE_EXIT([&jpgBuffer] {
    if (jpgBuffer) {
      tjFree(jpgBuffer);
    }
  });
  const uint8_t *srcPlanes[3] = {yImage.pixels(), uImage.pixels(), vImage.pixels()};
  int strides[3] = {yImage.rowBytes(), uImage.rowBytes(), vImage.rowBytes()};

  unsigned long jpegSize;
  if (tjCompressFromYUVPlanes(
        compressor,
        srcPlanes,
        yImage.cols(),
        strides,
        yImage.rows(),
        TJSAMP_420,
        &jpgBuffer,
        &jpegSize,
        quality,
        0)) {
    C8_THROW(String("JPEG error - ") + tjGetErrorStr());
  }

  Vector<uint8_t> retval(jpegSize);
  std::memcpy(&retval[0], jpgBuffer, jpegSize);
  return retval;
}

Vector<uint8_t> writePixelsToPng(ConstYPlanePixels pixels) {
  return PngBufferWriter::write(pixels);
}

Vector<uint8_t> writePixelsToPng(ConstRGB888PlanePixels pixels) {
  return PngBufferWriter::write(pixels);
}

Vector<uint8_t> writePixelsToPng(ConstBGR888PlanePixels pixels) {
  return PngBufferWriter::write(pixels);
}

Vector<uint8_t> writePixelsToPng(ConstBGRA8888PlanePixels pixels) {
  return PngBufferWriter::write(pixels);
}

Vector<uint8_t> writePixelsToPng(ConstRGBA8888PlanePixels pixels) {
  return PngBufferWriter::write(pixels);
}

void writePixelsToPng(ConstYPlanePixels pixels, const String &filename) {
  auto buf = writePixelsToPng(pixels);
  writeFile(filename, buf.data(), buf.size());
}

void writePixelsToPng(ConstRGB888PlanePixels pixels, const String &filename) {
  auto buf = writePixelsToPng(pixels);
  writeFile(filename, buf.data(), buf.size());
}

void writePixelsToPng(ConstBGR888PlanePixels pixels, const String &filename) {
  auto buf = writePixelsToPng(pixels);
  writeFile(filename, buf.data(), buf.size());
}

void writePixelsToPng(ConstBGRA8888PlanePixels pixels, const String &filename) {
  auto buf = writePixelsToPng(pixels);
  writeFile(filename, buf.data(), buf.size());
}

void writePixelsToPng(ConstRGBA8888PlanePixels pixels, const String &filename) {
  auto buf = writePixelsToPng(pixels);
  writeFile(filename, buf.data(), buf.size());
}

namespace {

template <class PixelsType, int bytesPerPix>
PixelBuffer<PixelsType, bytesPerPix> readJpgToPixelBuffer(
  const uint8_t *buffer, long bufferSize, TJPF format) {
  auto decompressor = tjInitDecompress();
  if (!decompressor) {
    C8_THROW("Failed to initialize JPEG decompressor");
  }
  SCOPE_EXIT([&decompressor] { tjDestroy(decompressor); });

  int width, height, subSamp;
  // Logically const.
  if (tjDecompressHeader2(
        decompressor, const_cast<uint8_t *>(buffer), bufferSize, &width, &height, &subSamp)) {
    C8_THROW(String("JPEG error - ") + tjGetErrorStr());
  }

  const int widthStep = TJPAD(width * tjPixelSize[format]);
  PixelBuffer<PixelsType, bytesPerPix> output(height, width, widthStep);

  if (tjDecompress2(
        decompressor,
        buffer,
        bufferSize,
        output.pixels().pixels(),
        width,
        widthStep,
        height,
        format,
        0)) {
    C8_THROW(String("JPEG error - ") + tjGetErrorStr());
  }

  return output;
}

template <class PixelsType, int bytesPerPix>
PixelBuffer<PixelsType, bytesPerPix> readJpgToPixelBuffer(const String &filename, TJPF format) {
  auto inputBuffer = readFile(filename);
  return readJpgToPixelBuffer<PixelsType, bytesPerPix>(
    inputBuffer.data(), inputBuffer.size(), format);
}

template <class ConstPixelsType>
void writePixelsToJpgInternal(
  ConstPixelsType pix, const String &filename, TJPF format, TJSAMP subSamp, int quality) {

  if (filename.size() < 5 || filename.compare(filename.size() - 4, 4, ".jpg") != 0) {
    C8_THROW("JPEG filename must end in '.jpg'");
  }

  // Call the code to write to memory, and then write that memory block to disk; Technically this
  // does an extra copy, but this should be fast compared to writing to disk and makes the code
  // more maintainable.
  auto buffer = writePixelsToJpgInternal(pix, format, subSamp, quality);
  const auto *jpgBuffer = &buffer[0];
  const auto jpgSize = buffer.size();
  writeFile(filename, jpgBuffer, jpgSize);
}

template <class ConstPixelsType>
Vector<uint8_t> writePixelsToJpgInternal(
  ConstPixelsType pix, TJPF format, TJSAMP subSamp, int quality) {

  auto compressor = tjInitCompress();
  if (!compressor) {
    C8_THROW("Failed to initialize JPEG compressor");
  }
  SCOPE_EXIT([&compressor] { tjDestroy(compressor); });

  uint8_t *jpgBuffer = nullptr;
  SCOPE_EXIT([&jpgBuffer] {
    if (jpgBuffer) {
      tjFree(jpgBuffer);
    }
  });

  unsigned long jpegSize;
  if (tjCompress2(
        compressor,
        pix.pixels(),
        pix.cols(),
        pix.rowBytes(),
        pix.rows(),
        format,
        &jpgBuffer,
        &jpegSize,
        subSamp,
        quality,
        0)) {
    C8_THROW(String("JPEG error - ") + tjGetErrorStr());
  }

  Vector<uint8_t> retval(jpegSize);
  std::memcpy(&retval[0], jpgBuffer, jpegSize);
  return retval;
}

}  // namespace

}  // namespace c8
