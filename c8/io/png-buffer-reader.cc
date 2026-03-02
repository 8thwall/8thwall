// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "png-buffer-reader.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixels",
    "@png//:png",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xc792bb12);

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/io/png-buffer-reader.h"

namespace c8 {
namespace {
void readFromBuffer(png_structp pngPtr, png_bytep data, size_t length) {
  PngBufferReader *bufReader = static_cast<PngBufferReader *>(png_get_io_ptr(pngPtr));
  memcpy(data, bufReader->readPtr(), length);
  bufReader->incrementReadPtr(length);
}

}  // namespace
PngBufferReader::PngBufferReader(const uint8_t *buffer, long bufferSize)
    : readPtr_(const_cast<uint8_t*>(buffer)), bytesRemaining_(bufferSize) {  // Logically const.
  pngPtr_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) nullptr, nullptr, nullptr);

  if (!pngPtr_) {
    C8Log("[png-buffer-reader] %s", "PNG cannot create read struct");
    C8_THROW("PNG cannot create read struct");
  }

  infoPtr_ = png_create_info_struct(pngPtr_);
  if (!infoPtr_) {
    png_destroy_read_struct(&pngPtr_, (png_infopp) nullptr, (png_infopp) nullptr);
    C8Log("[png-buffer-reader] %s", "PNG cannot destroy read struct");
    C8_THROW("PNG cannot destroy read struct");
  }

  png_set_read_fn(pngPtr_, (void *)this, readFromBuffer);
}

void PngBufferReader::readHeader() {
  png_read_info(pngPtr_, infoPtr_);
  png_get_IHDR(
    pngPtr_, infoPtr_, &width_, &height_, &bitDepth_, &colorType_, nullptr, nullptr, nullptr);
}

void PngBufferReader::setRgbToGray() {
  png_set_rgb_to_gray(pngPtr_, 1, 0.2126, 0.7152);
}
void PngBufferReader::setScaleDownTo8() {
  png_set_scale_16(pngPtr_);
}
void PngBufferReader::setExpandTo8() {
  png_set_expand_gray_1_2_4_to_8(pngPtr_);
}
void PngBufferReader::setBgr() {
  png_set_bgr(pngPtr_);
}
void PngBufferReader::setStripAlpha() {
  png_set_strip_alpha(pngPtr_);
}
void PngBufferReader::setAddAlpha(int filler) {
  png_set_add_alpha(pngPtr_, filler, PNG_FILLER_AFTER);
}

void PngBufferReader::setPaletteToRgb() {
  png_set_palette_to_rgb(pngPtr_);
}

void PngBufferReader::setGrayToRgb() {
  png_set_gray_to_rgb(pngPtr_);
}

void PngBufferReader::incrementReadPtr(size_t length) {
  if (length > bytesRemaining_) {
    C8_THROW("Trying read past buffer");
  }
  readPtr_ += length;
  bytesRemaining_ -= length;
}

PngBufferReader::~PngBufferReader() {
  png_destroy_read_struct(&pngPtr_, &infoPtr_, (png_infopp)NULL);
}
}  // namespace c8
