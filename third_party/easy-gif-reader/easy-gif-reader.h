// Source:
// https://github.com/Chlumsky/EasyGifReader/blob/eaaa794a52d8385eea6efa49057dab98caa51d3f/EasyGifReader.h

#pragma once

#include <compare>
#include <cstddef>
#include <memory>

#include "c8/string.h"
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//
//  EASY GIF READER v1.1 by Viktor Chlumsky (c) 2021 - 2024
//  Modified by Xiaokai Li (xiaokaili@nianticlabs.com)
//  -------------------------------------------------------
//
//  This is a single-class C++ library that aims to simplify reading an animated GIF file.
//  It is built on top of and depends on giflib.
//
//  MIT license
//  https://github.com/Chlumsky/EasyGifReader
//

class EasyGifReader {
  struct Internal;

  struct FrameBounds {
    int x0_, y0_, x1_, y1_;
    const int width() const;
    const int height() const;
  };

public:
  using PixelComponent = unsigned char;
  using GifReadFunction = size_t (*)(void *outData, size_t size, void *userPtr);
  struct FrameDuration {
    int centiseconds_;
    const int milliseconds() const;
    const double seconds() const;
    FrameDuration &operator+=(FrameDuration other);
    FrameDuration &operator-=(FrameDuration other);
    FrameDuration operator+(FrameDuration other) const;
    FrameDuration operator-(FrameDuration other) const;
    auto operator<=>(const FrameDuration &) const = default;
  };

  class Frame {
  public:
    Frame();
    Frame(const Frame &orig);
    Frame(Frame &&orig);
    Frame &operator=(const Frame &orig);
    Frame &operator=(Frame &&orig);
    const PixelComponent *pixels() const;
    const int width() const;
    const int height() const;
    FrameDuration duration() const;
    FrameDuration rawDuration() const;

  protected:
    std::shared_ptr<Internal> parentData_;
    int index_;
    int w_, h_;
    void nextFrame();

  private:
    std::unique_ptr<PixelComponent[]> pixelBuffer_;
    int disposal_;
    int delay_;
    PixelComponent *row(const int y);
    PixelComponent *corner(const FrameBounds &bounds);
  };

  class FrameIterator : public Frame {
  public:
    enum Position { BEGIN, END, LOOP_END };
    explicit FrameIterator(const EasyGifReader *decoder = nullptr, Position position = BEGIN);
    FrameIterator &operator++();
    void operator++(int);
    bool operator==(const FrameIterator &other) const;
    bool operator!=(const FrameIterator &other) const;
    const Frame &operator*() const;
    const Frame *operator->() const;
    void rewind();
  };

  static EasyGifReader openFile(const char *filename);
  static EasyGifReader openMemory(const void *buffer, const std::size_t size);
  static EasyGifReader openCustom(GifReadFunction readFunc, void *userPtr);
  static const c8::String translateErrorCode(int error);

  EasyGifReader() = delete;
  EasyGifReader(const EasyGifReader &) = delete;
  EasyGifReader(EasyGifReader &&orig);
  EasyGifReader &operator=(const EasyGifReader &) = delete;
  EasyGifReader &operator=(EasyGifReader &&orig);

  const int width() const;
  const int height() const;
  const int frameCount() const;
  const int repeatCount() const;
  const bool repeatsInfinitely() const;
  FrameIterator begin() const;
  FrameIterator end() const;
  FrameIterator loopEnd() const;

private:
  std::shared_ptr<Internal> data_;

  explicit EasyGifReader(const std::shared_ptr<Internal> data);
};
