// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include <cstdint>
#include <memory>
#include "external/png/png.h"
#include "c8/pixels/pixel-buffer.h"

namespace c8 {
class PngBufferReader {
public:
  /**
   * \param buffer contains the PNG binary data
   * \param bufferSize length of the buffer
   */
  PngBufferReader(const uint8_t *buffer, long bufferSize);

  PngBufferReader(const PngBufferReader &) = delete;
  PngBufferReader operator=(const PngBufferReader &) = delete;

  /** Read the header data in the PNG
   * Once read, information like height, width, bitDepth and colorSpace will be available
   * You have to call this before you call read()
   */
  void readHeader();

  int colorType() const { return colorType_; }
  int bitDepth() const { return bitDepth_; }

  // These methods set input transformations to be applied. Once read() is called, the data
  // is transformed before a PixelBuffer subclass is returned.
  void setRgbToGray();
  void setScaleDownTo8();
  void setExpandTo8();
  void setBgr();
  void setStripAlpha();
  void setAddAlpha(int filler);
  void setPaletteToRgb();
  void setGrayToRgb();

  template <class PixelsType, int bytesPerPix>
  PixelBuffer<PixelsType, bytesPerPix> read() {
    const int widthStep = width_ * bytesPerPix;
    PixelBuffer<PixelsType, bytesPerPix> output(height_, width_, widthStep);

    std::unique_ptr<png_bytep[]> rowPointers(new png_bytep[height_]);
    for (int i = 0; i < height_; ++i) {
      rowPointers[i] = output.pixels().pixels() + i * widthStep;
    }

    // Read images
    png_read_image(pngPtr_, rowPointers.get());
    png_read_end(pngPtr_, (png_infop)NULL);
    return output;
  }

  uint8_t *readPtr() { return readPtr_; }
  void incrementReadPtr(size_t length);
  ~PngBufferReader();

private:
  uint8_t *readPtr_;
  long bytesRemaining_;

  int colorType_;
  int bitDepth_;
  uint32_t width_;
  uint32_t height_;

  png_structp pngPtr_;
  png_infop infoPtr_;
};
}  // namespace c8
