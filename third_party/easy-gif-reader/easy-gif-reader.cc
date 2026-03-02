// Source:
// https://github.com/Chlumsky/EasyGifReader/blob/eaaa794a52d8385eea6efa49057dab98caa51d3f/EasyGifReader.cpp

#include "easy-gif-reader.h"

#include <gif_lib.h>

#include <algorithm>
#include <cstring>

#include "c8/exceptions.h"
#include "c8/string.h"

using std::size_t;

void closeGifFile(GifFileType *gif) {
  int error = D_GIF_SUCCEEDED;
  DGifCloseFile(gif, &error);
  if (error != D_GIF_SUCCEEDED) {
    c8::C8Log("[gif-reader] %s", EasyGifReader::translateErrorCode(error).c_str());
  }
}

using GifFile = std::unique_ptr<GifFileType, decltype(&closeGifFile)>;

static const size_t pxSize = 4;

struct EasyGifReader::Internal {
  GifFile gif{nullptr, &closeGifFile};
  union {
    struct {
      const void *dataPtr;
      size_t remainingSize;
    };
    struct {
      size_t (*readFunction)(void *outData, size_t size, void *userPtr);
      void *userPtr;
    };
  };
  int loopCount;
  size_t pixelBufferSize;

  static FrameBounds frameBounds(GifFile &gif, const int imageIndex);
  static void clearRows(PixelComponent *dst, int width, int height, size_t dstStride);
  static void copyRows(
    PixelComponent *dst,
    const PixelComponent *src,
    int width,
    int height,
    size_t dstStride,
    size_t srcStride);
  static int memoryRead(GifFileType *gif, GifByteType *outData, int size);
  static int customRead(GifFileType *gif, GifByteType *outData, int size);
  static bool readLoopExtension(
    int &loopCount, const ExtensionBlock *extensionBlocks, int extensionBlockCount);
  static GraphicsControlBlock readGCBExtension(
    const ExtensionBlock *extensionBlocks, int extensionBlockCount);
};

const c8::String EasyGifReader::translateErrorCode(int error) {
  switch (error) {
    case D_GIF_ERR_OPEN_FAILED:
      return "Failed to open file";
    case D_GIF_ERR_READ_FAILED:
      return "Failed to read file";
    case D_GIF_ERR_NOT_GIF_FILE:
      return "Not a GIF file";
    case D_GIF_ERR_NO_SCRN_DSCR:
    case D_GIF_ERR_NO_IMAG_DSCR:
    case D_GIF_ERR_NO_COLOR_MAP:
    case D_GIF_ERR_WRONG_RECORD:
    case D_GIF_ERR_DATA_TOO_BIG:
      return "Invalid GIF file";
    case D_GIF_ERR_NOT_ENOUGH_MEM:
      return "Out of memory";
    case D_GIF_ERR_CLOSE_FAILED:
      return "Failed to close file";
    case D_GIF_ERR_NOT_READABLE:
      return "File not readable";
    case D_GIF_ERR_IMAGE_DEFECT:
      return "Image defect";
    case D_GIF_ERR_EOF_TOO_SOON:
      return "Unexpected end of file";
    default:
      return "Unknown error";
  }
}

EasyGifReader::FrameBounds EasyGifReader::Internal::frameBounds(
  GifFile &gif, const int imageIndex) {
  const GifImageDesc &imageDesc = gif->SavedImages[imageIndex].ImageDesc;
  return FrameBounds{
    std::max(imageDesc.Left, 0),
    std::max(imageDesc.Top, 0),
    std::min(imageDesc.Left + imageDesc.Width, gif->SWidth),
    std::min(imageDesc.Top + imageDesc.Height, gif->SHeight)};
}

void EasyGifReader::Internal::clearRows(
  PixelComponent *dst, int width, int height, size_t dstStride) {
  for (int y = 0; y < height; ++y) {
    std::fill_n(dst, pxSize * width, 0);
    dst += dstStride;
  }
}

void EasyGifReader::Internal::copyRows(
  PixelComponent *dst,
  const PixelComponent *src,
  int width,
  int height,
  size_t dstStride,
  size_t srcStride) {
  for (int y = 0; y < height; ++y) {
    std::copy_n(src, pxSize * width, dst);
    dst += dstStride;
    src += srcStride;
  }
}

const int EasyGifReader::FrameDuration::milliseconds() const { return 10 * centiseconds_; }

const double EasyGifReader::FrameDuration::seconds() const { return .01 * centiseconds_; }

EasyGifReader::FrameDuration &EasyGifReader::FrameDuration::operator+=(FrameDuration other) {
  centiseconds_ += other.centiseconds_;
  return *this;
}

EasyGifReader::FrameDuration &EasyGifReader::FrameDuration::operator-=(FrameDuration other) {
  centiseconds_ -= other.centiseconds_;
  return *this;
}

EasyGifReader::FrameDuration EasyGifReader::FrameDuration::operator+(FrameDuration other) const {
  return FrameDuration{centiseconds_ + other.centiseconds_};
}

EasyGifReader::FrameDuration EasyGifReader::FrameDuration::operator-(FrameDuration other) const {
  return FrameDuration{centiseconds_ - other.centiseconds_};
}

EasyGifReader::Frame::Frame()
    : parentData_(nullptr), index_(-1), w_(0), h_(0), disposal_(DISPOSAL_UNSPECIFIED), delay_(0) {}

EasyGifReader::Frame::Frame(const Frame &orig)
    : parentData_(orig.parentData_),
      index_(orig.index_),
      w_(orig.w_),
      h_(orig.h_),
      disposal_(orig.disposal_),
      delay_(orig.delay_) {
  if (orig.pixelBuffer_) {
    pixelBuffer_ = std::make_unique<PixelComponent[]>(parentData_->pixelBufferSize);
    std::copy_n(orig.pixelBuffer_.get(), parentData_->pixelBufferSize, pixelBuffer_.get());
  }
}

EasyGifReader::Frame::Frame(Frame &&orig)
    : parentData_(orig.parentData_),
      index_(orig.index_),
      w_(orig.w_),
      h_(orig.h_),
      pixelBuffer_(std::move(orig.pixelBuffer_)),
      disposal_(orig.disposal_),
      delay_(orig.delay_) {}

EasyGifReader::Frame &EasyGifReader::Frame::operator=(const Frame &orig) {
  if (this != &orig) {
    bool keepPixelBuffer = pixelBuffer_ && orig.pixelBuffer_
      && parentData_->pixelBufferSize == orig.parentData_->pixelBufferSize;
    if (!keepPixelBuffer) {
      pixelBuffer_ = nullptr;
    }

    parentData_ = orig.parentData_;
    index_ = orig.index_;
    w_ = orig.w_;
    h_ = orig.h_;
    disposal_ = orig.disposal_;
    delay_ = orig.delay_;
    if (orig.pixelBuffer_) {
      pixelBuffer_ = std::make_unique<PixelComponent[]>(parentData_->pixelBufferSize);
      std::copy_n(orig.pixelBuffer_.get(), parentData_->pixelBufferSize, pixelBuffer_.get());
    } else {
      pixelBuffer_ = nullptr;
    }
  }
  return *this;
}

EasyGifReader::Frame &EasyGifReader::Frame::operator=(Frame &&orig) {
  if (this != &orig) {
    parentData_ = orig.parentData_;
    index_ = orig.index_;
    w_ = orig.w_;
    h_ = orig.h_;
    pixelBuffer_ = std::move(orig.pixelBuffer_);
    disposal_ = orig.disposal_;
    delay_ = orig.delay_;
  }
  return *this;
}

const EasyGifReader::PixelComponent *EasyGifReader::Frame::pixels() const {
  return pixelBuffer_.get();
}

const int EasyGifReader::Frame::width() const { return w_; }

const int EasyGifReader::Frame::height() const { return h_; }

EasyGifReader::FrameDuration EasyGifReader::Frame::duration() const {
  return FrameDuration{delay_ > 1 ? delay_ : 10};
}

EasyGifReader::FrameDuration EasyGifReader::Frame::rawDuration() const {
  return FrameDuration{delay_};
}

void EasyGifReader::Frame::nextFrame() {
  if (!(parentData_ && parentData_->gif))
    C8_THROW("Invalid operation");
  ++index_;
  if (
    !parentData_->gif->ImageCount
    || (parentData_->loopCount && index_ >= parentData_->loopCount * parentData_->gif->ImageCount))
    return;
  int imageIndex = index_ % parentData_->gif->ImageCount;
  if (!pixelBuffer_) {
    if (!imageIndex)
      pixelBuffer_ = std::make_unique<PixelComponent[]>(parentData_->pixelBufferSize);
    else
      C8_THROW("Invalid operation");
  }
  GraphicsControlBlock gcb = Internal::readGCBExtension(
    parentData_->gif->SavedImages[imageIndex].ExtensionBlocks,
    parentData_->gif->SavedImages[imageIndex].ExtensionBlockCount);
  delay_ = gcb.DelayTime;
  const ColorMapObject *colorMap = parentData_->gif->SavedImages[imageIndex].ImageDesc.ColorMap;
  if (!colorMap)
    colorMap = parentData_->gif->SColorMap;
  const GifImageDesc &imageDesc = parentData_->gif->SavedImages[imageIndex].ImageDesc;
  const GifByteType *raster = parentData_->gif->SavedImages[imageIndex].RasterBits;
  if (!colorMap || (!raster && imageDesc.Width > 0 && imageDesc.Height > 0))
    C8_THROW("Invalid GIF frame");
  FrameBounds frameBounds = Internal::frameBounds(parentData_->gif, imageIndex);
  if (
    gcb.TransparentColor >= 0 || frameBounds.x0_ > 0 || frameBounds.x1_ < w_ || frameBounds.y0_ > 0
    || frameBounds.y1_ < h_ || gcb.DisposalMode == DISPOSE_PREVIOUS) {
    if (!imageIndex)
      std::fill_n(pixelBuffer_.get(), pxSize * w_ * h_, 0);
    else if (disposal_ == DISPOSE_BACKGROUND || disposal_ == DISPOSE_PREVIOUS) {
      FrameBounds prevBounds = Internal::frameBounds(parentData_->gif, imageIndex - 1);
      if (disposal_ == DISPOSE_PREVIOUS)
        Internal::copyRows(
          corner(prevBounds),
          pixelBuffer_.get() + pxSize * w_ * h_,
          prevBounds.width(),
          prevBounds.height(),
          pxSize * w_,
          pxSize * prevBounds.width());
      else
        Internal::clearRows(
          corner(prevBounds), prevBounds.width(), prevBounds.height(), pxSize * w_);
    }
  }
  disposal_ = gcb.DisposalMode;
  if (disposal_ == DISPOSE_PREVIOUS) {
    if (imageIndex && imageIndex < parentData_->gif->ImageCount - 1)
      Internal::copyRows(
        pixelBuffer_.get() + pxSize * w_ * h_,
        corner(frameBounds),
        frameBounds.width(),
        frameBounds.height(),
        pxSize * frameBounds.width(),
        pxSize * w_);
    else
      disposal_ = DISPOSE_BACKGROUND;
  }
  for (int y = frameBounds.y0_; y < frameBounds.y1_; ++y) {
    PixelComponent *dst = row(y) + pxSize * frameBounds.x0_;
    const GifByteType *src =
      raster + (imageDesc.Width * (y - imageDesc.Top) + (frameBounds.x0_ - imageDesc.Left));
    for (int x = frameBounds.x0_; x < frameBounds.x1_; ++x) {
      int i = int(*src++);
      i *= (unsigned)i < (unsigned)colorMap->ColorCount;
      GifColorType c = colorMap->Colors[i];
      if (i != gcb.TransparentColor) {
        dst[0] = c.Red;
        dst[1] = c.Green;
        dst[2] = c.Blue;
        dst[3] = PixelComponent(0xff);
      }
      dst += pxSize;
    }
  }
}

EasyGifReader::PixelComponent *EasyGifReader::Frame::row(const int y) {
  if (y < 0 || y >= h_) {
    C8_THROW("Row index out of bounds");
  }
#ifdef EASY_GIF_DECODER_BOTTOM_TO_TOP_ROWS
  return pixelBuffer_.get() + pxSize * w_ * (h_ - y - 1);
#else
  return pixelBuffer_.get() + pxSize * w_ * y;
#endif
}

EasyGifReader::PixelComponent *EasyGifReader::Frame::corner(const FrameBounds &bounds) {
  if (bounds.x0_ < 0 || bounds.x0_ >= w_ || bounds.y0_ < 0 || bounds.y0_ >= h_) {
    C8_THROW("Corner bounds out of bounds");
  }
#ifdef EASY_GIF_DECODER_BOTTOM_TO_TOP_ROWS
  return pixelBuffer_.get() + pxSize * (w_ * (h - bounds.y1_) + bounds.x0_);
#else
  return pixelBuffer_.get() + pxSize * (w_ * bounds.y0_ + bounds.x0_);
#endif
}

EasyGifReader::FrameIterator::FrameIterator(const EasyGifReader *decoder, Position position) {
  if (decoder) {
    if (!((parentData_ = decoder->data_) && parentData_->gif))
      C8_THROW("Invalid operation");
    w_ = parentData_->gif->SWidth;
    h_ = parentData_->gif->SHeight;
    switch (position) {
      case BEGIN:
        rewind();
        break;
      case END:
        index_ = parentData_->gif->ImageCount;
        break;
      case LOOP_END:
        if (parentData_->loopCount)
          index_ = parentData_->loopCount * parentData_->gif->ImageCount;
        else
          index_ = -1;
        break;
    }
  }
}

EasyGifReader::FrameIterator &EasyGifReader::FrameIterator::operator++() {
  nextFrame();
  return *this;
}

void EasyGifReader::FrameIterator::operator++(int) { nextFrame(); }

bool EasyGifReader::FrameIterator::operator==(const FrameIterator &other) const {
  return parentData_ == other.parentData_ && index_ == other.index_;
}

bool EasyGifReader::FrameIterator::operator!=(const FrameIterator &other) const {
  return parentData_ != other.parentData_ || index_ != other.index_;
}

const EasyGifReader::Frame &EasyGifReader::FrameIterator::operator*() const { return *this; }

const EasyGifReader::Frame *EasyGifReader::FrameIterator::operator->() const { return this; }

void EasyGifReader::FrameIterator::rewind() {
  index_ = -1;
  nextFrame();
}

EasyGifReader EasyGifReader::openFile(const char *filename) {
  int error = D_GIF_SUCCEEDED;
  if (GifFileType *gif = DGifOpenFileName(filename, &error)) {
    const auto data = std::make_shared<Internal>();
    data->gif.reset(gif);
    return EasyGifReader(data);
  } else
    C8_THROW(translateErrorCode(error));
}

int EasyGifReader::Internal::memoryRead(GifFileType *gif, GifByteType *outData, int size) {
  Internal *data = reinterpret_cast<Internal *>(gif->UserData);
  size_t realSize = data->remainingSize < (size_t)size ? data->remainingSize : (size_t)size;
  std::memcpy(outData, data->dataPtr, realSize);
  data->remainingSize -= realSize;
  data->dataPtr = reinterpret_cast<const GifByteType *>(data->dataPtr) + realSize;
  return (int)realSize;
}

EasyGifReader EasyGifReader::openMemory(const void *buffer, const size_t size) {
  const auto data = std::make_shared<Internal>();
  int error = D_GIF_SUCCEEDED;
  data->dataPtr = buffer;
  data->remainingSize = size;
  data->gif.reset(DGifOpen(data.get(), &Internal::memoryRead, &error));
  if (data->gif) {
    return EasyGifReader(data);
  } else {
    C8_THROW(translateErrorCode(error));
  }
}

int EasyGifReader::Internal::customRead(GifFileType *gif, GifByteType *outData, int size) {
  Internal *data = reinterpret_cast<Internal *>(gif->UserData);
  return (int)data->readFunction(outData, (size_t)size, data->userPtr);
}

EasyGifReader EasyGifReader::openCustom(GifReadFunction readFunction, void *userPtr) {
  const auto data = std::make_shared<Internal>();
  int error = D_GIF_SUCCEEDED;
  data->readFunction = readFunction;
  data->userPtr = userPtr;
  data->gif.reset(DGifOpen(data.get(), &Internal::customRead, &error));
  if (data->gif) {
    return EasyGifReader(data);
  } else {
    C8_THROW(translateErrorCode(error));
  }
}

const int EasyGifReader::FrameBounds::width() const { return x1_ - x0_; }

const int EasyGifReader::FrameBounds::height() const { return y1_ - y0_; }

bool EasyGifReader::Internal::readLoopExtension(
  int &loopCount, const ExtensionBlock *extensionBlocks, int extensionBlockCount) {
  for (; --extensionBlockCount > 0; ++extensionBlocks) {
    if (
            extensionBlocks[0].Function == 0xff &&
            extensionBlocks[0].ByteCount == 11 &&
            (!std::memcmp(extensionBlocks[0].Bytes, "NETSCAPE2.0", 11) || !std::memcmp(extensionBlocks[0].Bytes, "ANIMEXTS1.0", 11)) &&
            extensionBlocks[1].Function == 0x00 &&
            extensionBlocks[1].ByteCount == 3 &&
            extensionBlocks[1].Bytes[0] == 0x01
        ) {
      loopCount =
        (unsigned)extensionBlocks[1].Bytes[1] | (unsigned)extensionBlocks[1].Bytes[2] << 8;
      return true;
    }
  }
  return false;
}

GraphicsControlBlock EasyGifReader::Internal::readGCBExtension(
  const ExtensionBlock *extensionBlocks, int extensionBlockCount) {
  GraphicsControlBlock gcb;
  gcb.DisposalMode = DISPOSAL_UNSPECIFIED;
  gcb.UserInputFlag = 0;
  gcb.DelayTime = 0;
  gcb.TransparentColor = NO_TRANSPARENT_COLOR;
  for (const ExtensionBlock *extensionBlock = extensionBlocks + extensionBlockCount;
       extensionBlock-- != extensionBlocks;) {
    if (
      extensionBlock->Function == GRAPHICS_EXT_FUNC_CODE
      && DGifExtensionToGCB(extensionBlock->ByteCount, extensionBlock->Bytes, &gcb) == GIF_OK)
      break;
  }
  return gcb;
}

EasyGifReader::EasyGifReader(const std::shared_ptr<Internal> data) : data_(data) {
  if (!data_) {
    C8_THROW("Invalid operation, data is null");
  }
  if (!data_->gif) {
    C8_THROW("Invalid operation, no GIF file");
  }
  if (DGifSlurp(data_->gif.get()) != GIF_OK) {
    c8::String errorMsg = translateErrorCode(data_->gif->Error);
    C8_THROW(errorMsg);
  }
  data_->loopCount = 1;
  if (
    !Internal::readLoopExtension(
      data_->loopCount, data_->gif->ExtensionBlocks, data_->gif->ExtensionBlockCount)
    && data_->gif->ImageCount > 0)
    Internal::readLoopExtension(
      data_->loopCount,
      data_->gif->SavedImages[0].ExtensionBlocks,
      data_->gif->SavedImages[0].ExtensionBlockCount);
  size_t frameArea = (size_t)data_->gif->SWidth * (size_t)data_->gif->SHeight;
  size_t prevFrameArea = 0;
  for (int i = 0; i < data_->gif->ImageCount - 1; ++i) {
    GraphicsControlBlock gcb = Internal::readGCBExtension(
      data_->gif->SavedImages[i].ExtensionBlocks, data_->gif->SavedImages[i].ExtensionBlockCount);
    if (gcb.DisposalMode == DISPOSE_PREVIOUS) {
      size_t area = (size_t)data_->gif->SavedImages[i].ImageDesc.Width
        * (size_t)data_->gif->SavedImages[i].ImageDesc.Height;
      if (area > prevFrameArea) {
        prevFrameArea = area;
        if (area == frameArea)
          break;
      }
    }
  }
  data_->pixelBufferSize = pxSize * (frameArea + prevFrameArea);
}

EasyGifReader::EasyGifReader(EasyGifReader &&orig) : data_(std::move(orig.data_)) {}

EasyGifReader &EasyGifReader::operator=(EasyGifReader &&orig) {
  if (this != &orig) {
    data_ = std::move(orig.data_);
  }
  return *this;
}

const int EasyGifReader::width() const { return data_->gif->SWidth; }

const int EasyGifReader::height() const { return data_->gif->SHeight; }

const int EasyGifReader::frameCount() const { return data_->gif->ImageCount; }

const int EasyGifReader::repeatCount() const { return data_->loopCount; }

const bool EasyGifReader::repeatsInfinitely() const { return !data_->loopCount; }

EasyGifReader::FrameIterator EasyGifReader::begin() const {
  return FrameIterator(this, FrameIterator::BEGIN);
}

EasyGifReader::FrameIterator EasyGifReader::end() const {
  return FrameIterator(this, FrameIterator::END);
}

EasyGifReader::FrameIterator EasyGifReader::loopEnd() const {
  return FrameIterator(this, FrameIterator::LOOP_END);
}
