// Copyright (c) 2024 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)
//
// Transfer buffer for use with the CommandBuffer class. The TransferBuffer is thread-compatible but
// not thread-safe, meaning that it can be used in a multi-threaded environment where there is
// existing synchronization. Specifically, one thread calls store and another thread calls release,
// there should be an acquire-release synchronization point between the store and release calls.

#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>

namespace c8 {

class TransferBuffer {
public:
  // Create a transfer buffer with a given size in bytes.
  TransferBuffer(std::size_t sizeBytes) noexcept
      : buffer_(std::make_unique<char[]>(sizeBytes)),
        bufferSize_(sizeBytes),
        writeMarker_(buffer_.get()),
        readMarker_(buffer_.get()) {}
  ~TransferBuffer() noexcept = default;

  // Disallow copy.
  TransferBuffer(const TransferBuffer &) = delete;
  TransferBuffer &operator=(const TransferBuffer &) = delete;

  // Default move.
  TransferBuffer(TransferBuffer &&) noexcept = default;
  TransferBuffer &operator=(TransferBuffer &&) noexcept = default;

  // Try to store a block of count elements of T data to the buffer. Returns the location of the
  // written data in the buffer or nullptr if there isn't enough space. Data will be in a contiguous
  // and properly aligned block if the write is successful.
  template <typename T>
  const T *store(const T *data, std::size_t count) noexcept;

  // Release a block of data from the buffer. The data must be released in the same order it was
  // written, although it is fine to skip block(s) and release the last data written. The marker
  // should be the location returned by store plus the size of the data. Don't call release if
  // the store failed.
  void release(const void *marker) noexcept;

private:
  // Advance a char* pointer forward to the next memory alignment for the requested type.
  static char *alignPointer(char *ptr, std::size_t align) {
    std::uintptr_t ptrUint = reinterpret_cast<std::uintptr_t>(ptr);
    std::uintptr_t ptrAlignedUint = (ptrUint + align - 1) & ~(align - 1);
    return reinterpret_cast<char *>(ptrAlignedUint);
  };

  std::unique_ptr<char[]> buffer_;
  std::size_t bufferSize_;
  char *writeMarker_;
  const char *readMarker_;
};
;

template <typename T>
const T *TransferBuffer::store(const T *data, std::size_t count) noexcept {
  const char *readMarker = readMarker_;

  std::size_t align;
  int sizeBytes;
  if constexpr (std::is_void_v<T>) {
    // Size in bytes of the data to write.
    sizeBytes = static_cast<int>(count);

    // If T is a void, then align to the largest power of two less than or equal to max_align_t that
    // is seen in the source data pointer value.
    constexpr std::size_t maxAlign = alignof(std::max_align_t);
    // Count trailing zeros in the data pointer.
    std::size_t trailingZeros =
      data ? __builtin_ctz(reinterpret_cast<uintptr_t>(data)) : sizeof(uintptr_t) * 8;

    // Return the largest power of two, clamped to max_alignment
    align = std::min(std::size_t(1) << trailingZeros, maxAlign);
  } else {
    // Align to the alignment of T.
    align = alignof(T);
    sizeBytes = static_cast<int>(count * sizeof(T));
  }

  // Start by placing the writeMarkerAligned at the first properly aligned memory location for type
  // T, since the consumer will reinterpret the data as T.
  char *writeMarkerAligned = alignPointer(writeMarker_, align);

  char *bufferData = buffer_.get();
  char *bufferDataAligned = alignPointer(bufferData, align);
  const char *bufferEnd = bufferData + bufferSize_;

  // Figure out how much space is left until the read pointer (or buffer end) minus 1.
  const std::ptrdiff_t spaceRemainingForward = (readMarker <= writeMarker_)
    ? bufferEnd - writeMarkerAligned - 1
    : (readMarker - writeMarkerAligned - 1);

  // Check if there is enough space ahead of the writeMarker.
  if (spaceRemainingForward < sizeBytes) {
    if (readMarker > writeMarker_) {
      // The readptr is ahead, so there is no space left.
      return nullptr;
    }

    // Compute space remaining at the start of the buffer and subtract one to ensure we don't end up
    // with readMarker == writeMarker.
    const std::ptrdiff_t spaceRemainingAtStart = readMarker - bufferDataAligned - 1;

    // Check if there is enough space at the start of the buffer, taking care not to implicitly
    // convert a negative spaceRemainingAtStart to a large positive number.
    if (spaceRemainingAtStart < sizeBytes) {
      // There isn't enough space in the buffer.
      return nullptr;
    }

    // Wrap around to the beginning of the buffer.
    writeMarkerAligned = bufferDataAligned;
  }

  // Write the data to the buffer.
  std::memcpy(writeMarkerAligned, data, sizeBytes);

  // Advance the write pointer.
  writeMarker_ = writeMarkerAligned + sizeBytes;

  return reinterpret_cast<const T *>(writeMarkerAligned);
}

}  // namespace c8
