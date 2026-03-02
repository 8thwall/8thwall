#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "multiclass-operations.h",
  };
  deps = {
    "//c8:color",
    "//c8:exceptions",
    "//c8/pixels:pixels",
    "//c8:string",
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xa1137840);

#include "c8/color.h"
#include "c8/exceptions.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
#include "reality/engine/deepnets/multiclass-operations.h"

namespace c8 {

void maxMultiClassMap(
  const Vector<FloatPixels> semanticsRes, int numClasses, RGBA8888PlanePixels &outPix) {
  // Check input size and output dimensions.
  if (semanticsRes.empty() || semanticsRes.size() < numClasses) {
    C8_THROW(
      "We expect the number of semantics result layers match the number of semantics classes.");
    return;
  }

  int inW = semanticsRes[0].cols();
  int inH = semanticsRes[0].rows();
  int outW = outPix.cols();
  int outH = outPix.rows();
  if (inW * inH != outW * outH) {
    C8_THROW("We expect the number of pixels match between semanticsRes layers and outPix.");
    return;
  }

  int offset = 0;
  auto pix = outPix.pixels();
  for (int c = 0; c < inW * inH; ++c) {
    float maxC = -1000.0f;
    float maxI = 0;
    for (int i = 0; i < numClasses; ++i) {
      float semVal = semanticsRes[i].pixels()[offset];
      if (semVal > maxC) {
        maxC = semVal;
        maxI = i;
      }
    }
    auto color =
      (maxC > 0.4f) ? Color::turbo(static_cast<float>(maxI + 1) / numClasses) : Color::BLACK;
    pix[0] = color.r();
    pix[1] = color.g();
    pix[2] = color.b();
    pix[3] = 255;

    offset += 1;
    pix += 4;
  }
}

// Compute the mask for certain semantics class.
// If the semantics possibility of 'classId' is greater than 'threshold',
// assign the mask color & alpha to this pixel;
// otherwise, the pixel is assigned to background color and alpha.
//
// @param semanticsRes the semantics results
// @param outPix result mask pixels
// @param classId desired semantics class ID
// @param threshold the semantics possibility threshold
// @param maskColor pixel color if it is valid
// @param maskAlpha pixel alpha if it is valid
// @param backgroundColor pixel color if it is background
// @param backgroundAlpha pixel alpha if it is background
// @return number of mask pixels
int multiClassBinaryMap(
  const Vector<FloatPixels> semanticsRes,
  RGBA8888PlanePixels &outPix,
  int classId,
  float threshold,
  Color maskColor,
  uint8_t maskAlpha,
  Color backgroundColor,
  uint8_t backgroundAlpha) {
  int pixCount = 0;
  // Check input size and output dimensions.
  if (
    semanticsRes.empty() || classId < 0 || (outPix.rows() != semanticsRes[0].rows())
    || (outPix.cols() != semanticsRes[0].cols())) {
    return pixCount;
  }

  int rows = semanticsRes[0].rows();
  int cols = semanticsRes[0].cols();
  int offset = 0;
  auto pix = outPix.pixels();
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      float semVal = semanticsRes[classId].pixels()[offset];
      bool isValid = (semVal > threshold);
      auto color = isValid ? maskColor : backgroundColor;
      auto alpha = isValid ? maskAlpha : backgroundAlpha;
      pix[0] = color.r();
      pix[1] = color.g();
      pix[2] = color.b();
      pix[3] = alpha;
      if (isValid) {
        pixCount++;
      }

      offset += 1;
      pix += 4;
    }
  }
  return pixCount;
}

}  // namespace c8
