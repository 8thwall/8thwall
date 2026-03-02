// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "png-buffer-writer.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:vector",
    "//c8/pixels:pixels",
    "@png//:png",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x6f688b54);

#include <png.h>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/io/png-buffer-writer.h"

namespace c8 {

namespace {
Vector<uint8_t> writeInternal(ConstPixels pix, png_uint_32 format) {
  png_image img{
    nullptr,  // opaque, free with png_image_free
    PNG_IMAGE_VERSION,
    static_cast<png_uint_32>(pix.cols()),
    static_cast<png_uint_32>(pix.rows()),
    format,
    0,   // flags
    0,   // colormap entries
    0,   // warning or error
    {},  // message
  };

  size_t requiredBytes = 0;

  // Call to find number of required bytes.
  png_image_write_to_memory(
    &img, nullptr, &requiredBytes, false, pix.pixels(), pix.rowBytes(), nullptr);

  // Call again to encode the image.
  Vector<uint8_t> encoded(requiredBytes);
  png_image_write_to_memory(
    &img, encoded.data(), &requiredBytes, false, pix.pixels(), pix.rowBytes(), nullptr);

  // In case the actually written size differs from the estimated size.
  encoded.resize(requiredBytes);

  // Free anything allocated in the "opaque" pointer.
  png_image_free(&img);

  return encoded;
}
}  // namespace

Vector<uint8_t> PngBufferWriter::write(ConstYPlanePixels pix) {
  return writeInternal(pix, PNG_FORMAT_GRAY);
}

Vector<uint8_t> PngBufferWriter::write(ConstRGB888PlanePixels pix) {
  return writeInternal(pix, PNG_FORMAT_RGB);
}

Vector<uint8_t> PngBufferWriter::write(ConstBGR888PlanePixels pix) {
  return writeInternal(pix, PNG_FORMAT_BGR);
}

Vector<uint8_t> PngBufferWriter::write(ConstBGRA8888PlanePixels pix) {
  return writeInternal(pix, PNG_FORMAT_BGRA);
}

Vector<uint8_t> PngBufferWriter::write(ConstRGBA8888PlanePixels pix) {
  return writeInternal(pix, PNG_FORMAT_RGBA);
}

}  // namespace c8
