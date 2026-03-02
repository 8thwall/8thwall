// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// A thin wrapper for pixel buffers with subclasses for type safety.

#pragma once

#include <cstdint>

namespace c8 {

class Pixels {
public:
  inline int rows() const noexcept { return rows_; }
  inline int cols() const noexcept { return cols_; }
  inline int rowBytes() const noexcept { return rowBytes_; }
  inline uint8_t *pixels() const noexcept { return pixels_; }

  // Inline initialization.
  Pixels(int rows, int cols, int rowBytes, uint8_t *pixels) noexcept
      : rows_(rows), cols_(cols), rowBytes_(rowBytes), pixels_(pixels) {}

  // Define default constructors, destructors, copy and move operations.
  Pixels() noexcept = default;
  virtual ~Pixels() noexcept {}

private:
  int rows_ = 0;
  int cols_ = 0;
  int rowBytes_ = 0;
  uint8_t *pixels_ = nullptr;
};

class ConstPixels {
public:
  inline int rows() const noexcept { return rows_; }
  inline int cols() const noexcept { return cols_; }
  inline int rowBytes() const noexcept { return rowBytes_; }
  inline const uint8_t *pixels() const noexcept { return pixels_; }

  // Inline initialization.
  ConstPixels(int rows, int cols, int rowBytes, const uint8_t *pixels)
      : rows_(rows), cols_(cols), rowBytes_(rowBytes), pixels_(pixels) {}

  // Define default constructors, destructors, copy and move operations.
  ConstPixels() noexcept = default;
  virtual ~ConstPixels() noexcept {}

  // Allowed implicit conversions.
  ConstPixels(const Pixels &b) : ConstPixels(b.rows(), b.cols(), b.rowBytes(), b.pixels()){};
  ConstPixels &operator=(const Pixels &b) {
    *this = ConstPixels(b);
    return *this;
  }

private:
  int rows_ = 0;
  int cols_ = 0;
  int rowBytes_ = 0;
  const uint8_t *pixels_ = nullptr;
};

class Pixels32 {
public:
  inline int rows() const noexcept { return rows_; }
  inline int cols() const noexcept { return cols_; }
  inline int rowElements() const noexcept { return rowElements_; }
  inline uint32_t *pixels() const noexcept { return pixels_; }

  // Inline initialization.
  Pixels32(int rows, int cols, int rowElements, uint32_t *pixels) noexcept
      : rows_(rows), cols_(cols), rowElements_(rowElements), pixels_(pixels) {}

  // Define default constructors, destructors, copy and move operations.
  Pixels32() noexcept = default;
  virtual ~Pixels32() noexcept {}

private:
  int rows_ = 0;
  int cols_ = 0;
  int rowElements_ = 0;
  uint32_t *pixels_ = nullptr;
};

class ConstPixels32 {
public:
  inline int rows() const noexcept { return rows_; }
  inline int cols() const noexcept { return cols_; }
  inline int rowElements() const noexcept { return rowElements_; }
  inline const uint32_t *pixels() const noexcept { return pixels_; }

  // Inline initialization.
  ConstPixels32(int rows, int cols, int rowElements, const uint32_t *pixels)
      : rows_(rows), cols_(cols), rowElements_(rowElements), pixels_(pixels) {}

  // Define default constructors, destructors, copy and move operations.
  ConstPixels32() noexcept = default;
  virtual ~ConstPixels32() noexcept {}

  // Allowed implicit conversions.
  ConstPixels32(const Pixels32 &b) : ConstPixels32(b.rows(), b.cols(), b.rowElements(), b.pixels()){};
  ConstPixels32 &operator=(const Pixels32 &b) {
    *this = ConstPixels32(b);
    return *this;
  }

private:
  int rows_ = 0;
  int cols_ = 0;
  int rowElements_ = 0;
  const uint32_t *pixels_ = nullptr;
};

class FloatPixels {
public:
  inline int rows() const noexcept { return rows_; }
  inline int cols() const noexcept { return cols_; }
  inline int rowElements() const noexcept { return rowElements_; }
  inline float *pixels() const noexcept { return pixels_; }

  // Inline initialization.
  FloatPixels(int rows, int cols, int rowElements, float *pixels) noexcept
      : rows_(rows), cols_(cols), rowElements_(rowElements), pixels_(pixels) {}

  // Define default constructors, destructors, copy and move operations.
  FloatPixels() noexcept = default;
  virtual ~FloatPixels() noexcept {}

private:
  int rows_ = 0;
  int cols_ = 0;
  int rowElements_ = 0;
  float *pixels_ = nullptr;
};

class ConstFloatPixels {
public:
  inline int rows() const noexcept { return rows_; }
  inline int cols() const noexcept { return cols_; }
  inline int rowElements() const noexcept { return rowElements_; }
  inline const float *pixels() const noexcept { return pixels_; }

  // Inline initialization.
  ConstFloatPixels(int rows, int cols, int rowElements, const float *pixels)
      : rows_(rows), cols_(cols), rowElements_(rowElements), pixels_(pixels) {}

  // Define default constructors, destructors, copy and move operations.
  ConstFloatPixels() noexcept = default;
  virtual ~ConstFloatPixels() noexcept {}

  // Allowed implicit conversions.
  ConstFloatPixels(const FloatPixels &b)
      : ConstFloatPixels(b.rows(), b.cols(), b.rowElements(), b.pixels()){};
  ConstFloatPixels &operator=(const FloatPixels &b) {
    *this = ConstFloatPixels(b);
    return *this;
  }

private:
  int rows_ = 0;
  int cols_ = 0;
  int rowElements_ = 0;
  const float *pixels_ = nullptr;
};

#define C8_PIXELS_DEFINE_SUBCLASS(subclass, superclass)                  \
  class subclass : public superclass {                                   \
  public:                                                                \
    subclass(int rows, int cols, int rowBytes, uint8_t *pixels) noexcept \
        : superclass(rows, cols, rowBytes, pixels) {}                    \
    subclass() noexcept = default;                                       \
    virtual ~subclass() noexcept {}                                      \
  };

#define C8_PIXELS_DEFINE_CONST_CLASS(t, constt, superconstt)                     \
  class constt : public superconstt {                                            \
  public:                                                                        \
    constt(int rows, int cols, int rowBytes, const uint8_t *pixels) noexcept     \
        : superconstt(rows, cols, rowBytes, pixels) {}                           \
    constt() = default;                                                          \
    virtual ~constt() noexcept {}                                                \
    constt(const t &b) : constt(b.rows(), b.cols(), b.rowBytes(), b.pixels()){}; \
    constt &operator=(const t &b) {                                              \
      *this = constt(b);                                                         \
      return *this;                                                              \
    }                                                                            \
  };

#define C8_PIXELS32_DEFINE_SUBCLASS(subclass, superclass)                    \
  class subclass : public superclass {                                       \
  public:                                                                    \
    subclass(int rows, int cols, int rowElements, uint32_t *pixels) noexcept \
        : superclass(rows, cols, rowElements, pixels) {}                     \
    subclass() noexcept = default;                                           \
    virtual ~subclass() noexcept {}                                          \
  };

#define C8_PIXELS32_DEFINE_CONST_CLASS(t, constt, superconstt)                      \
  class constt : public superconstt {                                               \
  public:                                                                           \
    constt(int rows, int cols, int rowElements, const uint32_t *pixels) noexcept    \
        : superconstt(rows, cols, rowElements, pixels) {}                           \
    constt() = default;                                                             \
    virtual ~constt() noexcept {}                                                   \
    constt(const t &b) : constt(b.rows(), b.cols(), b.rowElements(), b.pixels()){}; \
    constt &operator=(const t &b) {                                                 \
      *this = constt(b);                                                            \
      return *this;                                                                 \
    }                                                                               \
  };

#define C8_FLOAT_PIXELS_DEFINE_SUBCLASS(subclass, superclass)             \
  class subclass : public superclass {                                    \
  public:                                                                 \
    subclass(int rows, int cols, int rowElements, float *pixels) noexcept \
        : superclass(rows, cols, rowElements, pixels) {}                  \
    subclass() noexcept = default;                                        \
    virtual ~subclass() noexcept {}                                       \
  };

#define C8_FLOAT_PIXELS_DEFINE_CONST_CLASS(t, constt, superconstt)                  \
  class constt : public superconstt {                                               \
  public:                                                                           \
    constt(int rows, int cols, int rowElements, const float *pixels) noexcept       \
        : superconstt(rows, cols, rowElements, pixels) {}                           \
    constt() = default;                                                             \
    virtual ~constt() noexcept {}                                                   \
    constt(const t &b) : constt(b.rows(), b.cols(), b.rowElements(), b.pixels()){}; \
    constt &operator=(const t &b) {                                                 \
      *this = constt(b);                                                            \
      return *this;                                                                 \
    }                                                                               \
  };

C8_PIXELS_DEFINE_SUBCLASS(OneChannelPixels, Pixels);
C8_PIXELS_DEFINE_CONST_CLASS(OneChannelPixels, ConstOneChannelPixels, ConstPixels);

C8_PIXELS_DEFINE_SUBCLASS(TwoChannelPixels, Pixels);
C8_PIXELS_DEFINE_CONST_CLASS(TwoChannelPixels, ConstTwoChannelPixels, ConstPixels);

C8_PIXELS_DEFINE_SUBCLASS(ThreeChannelPixels, Pixels);
C8_PIXELS_DEFINE_CONST_CLASS(ThreeChannelPixels, ConstThreeChannelPixels, ConstPixels);

C8_PIXELS_DEFINE_SUBCLASS(FourChannelPixels, Pixels);
C8_PIXELS_DEFINE_CONST_CLASS(FourChannelPixels, ConstFourChannelPixels, ConstPixels);

C8_PIXELS_DEFINE_SUBCLASS(SixteenChannelPixels, Pixels);
C8_PIXELS_DEFINE_CONST_CLASS(SixteenChannelPixels, ConstSixteenChannelPixels, ConstPixels);

C8_PIXELS_DEFINE_SUBCLASS(YPlanePixels, OneChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(YPlanePixels, ConstYPlanePixels, ConstOneChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(UPlanePixels, OneChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(UPlanePixels, ConstUPlanePixels, ConstOneChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(VPlanePixels, OneChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(VPlanePixels, ConstVPlanePixels, ConstOneChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(UVPlanePixels, TwoChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(UVPlanePixels, ConstUVPlanePixels, ConstTwoChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(USkipPlanePixels, TwoChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(USkipPlanePixels, ConstUSkipPlanePixels, ConstTwoChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(VSkipPlanePixels, TwoChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(VSkipPlanePixels, ConstVSkipPlanePixels, ConstTwoChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(BGR888PlanePixels, ThreeChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(BGR888PlanePixels, ConstBGR888PlanePixels, ConstThreeChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(RGB888PlanePixels, ThreeChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(RGB888PlanePixels, ConstRGB888PlanePixels, ConstThreeChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(RGBA8888PlanePixels, FourChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(RGBA8888PlanePixels, ConstRGBA8888PlanePixels, ConstFourChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(BGRA8888PlanePixels, FourChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(BGRA8888PlanePixels, ConstBGRA8888PlanePixels, ConstFourChannelPixels);

C8_PIXELS_DEFINE_SUBCLASS(YUVA8888PlanePixels, FourChannelPixels);
C8_PIXELS_DEFINE_CONST_CLASS(YUVA8888PlanePixels, ConstYUVA8888PlanePixels, ConstFourChannelPixels);

C8_PIXELS32_DEFINE_SUBCLASS(OneChannelPixels32, Pixels32);
C8_PIXELS32_DEFINE_CONST_CLASS(OneChannelPixels32, ConstOneChannelPixels32, ConstPixels32);

C8_PIXELS32_DEFINE_SUBCLASS(FourChannelPixels32, Pixels32);
C8_PIXELS32_DEFINE_CONST_CLASS(FourChannelPixels32, ConstFourChannelPixels32, ConstPixels32);

C8_PIXELS32_DEFINE_SUBCLASS(R32PlanePixels, OneChannelPixels32);
C8_PIXELS32_DEFINE_CONST_CLASS(R32PlanePixels, ConstR32PlanePixels, ConstOneChannelPixels32);

C8_PIXELS32_DEFINE_SUBCLASS(RGBA32PlanePixels, FourChannelPixels32);
C8_PIXELS32_DEFINE_CONST_CLASS(RGBA32PlanePixels, ConstRGBA32PlanePixels, ConstFourChannelPixels32);

C8_FLOAT_PIXELS_DEFINE_SUBCLASS(OneChannelFloatPixels, FloatPixels);
C8_FLOAT_PIXELS_DEFINE_CONST_CLASS(
  OneChannelFloatPixels, ConstOneChannelFloatPixels, ConstFloatPixels);

C8_FLOAT_PIXELS_DEFINE_SUBCLASS(DepthFloatPixels, OneChannelFloatPixels);
C8_FLOAT_PIXELS_DEFINE_CONST_CLASS(
  DepthFloatPixels, ConstDepthFloatPixels, ConstOneChannelFloatPixels)

}  // namespace c8
