// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "c8/pixels/pixels.h"
#include "c8/vector.h"

namespace c8 {
class PngBufferWriter {
public:
  static Vector<uint8_t> write(ConstYPlanePixels pix);
  static Vector<uint8_t> write(ConstRGB888PlanePixels pix);
  static Vector<uint8_t> write(ConstBGR888PlanePixels pix);
  static Vector<uint8_t> write(ConstBGRA8888PlanePixels pix);
  static Vector<uint8_t> write(ConstRGBA8888PlanePixels pix);
};
}  // namespace c8
