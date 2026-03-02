#include "bzl/inliner/rules2.h"

cc_library { hdrs = {"circular-memcpy.h"}; }
cc_end(0x137f19ca);

#include <cstring>

#include "c8/command/circular-memcpy.h"

namespace c8 {

char *circularMemcpyStore(
  char *dst, const char *src, std::size_t size, char *bufferStart, const char *bufferEnd) {
  const std::size_t bytesUntilEnd = bufferEnd - dst;
  if (bytesUntilEnd < size) [[unlikely]] {
    std::memcpy(dst, src, bytesUntilEnd);
    std::memcpy(bufferStart, src + bytesUntilEnd, size - bytesUntilEnd);
    return bufferStart + size - bytesUntilEnd;
  } else [[likely]] {
    std::memcpy(dst, src, size);
    return dst + size;
  }
}

const char *circularMemcpyLoad(
  char *dst, const char *src, std::size_t size, const char *bufferStart, const char *bufferEnd) {
  const std::size_t bytesUntilEnd = bufferEnd - src;
  if (bytesUntilEnd < size) [[unlikely]] {
    std::memcpy(dst, src, bytesUntilEnd);
    std::memcpy(dst + bytesUntilEnd, bufferStart, size - bytesUntilEnd);
    return bufferStart + size - bytesUntilEnd;
  } else [[likely]] {
    std::memcpy(dst, src, size);
    return src + size;
  }
}

}  // namespace c8
