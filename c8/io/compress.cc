// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "compress.h",
  };
  deps = {
    "//c8:string",
    "//c8:vector",
    "@zlib//:zlib",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xe2549a7c);

#include <zlib.h>

#include "c8/io/compress.h"

namespace c8 {

namespace {
bool decompressGzippedImpl(
  const uint8_t *compressed, size_t size, int windowSize, Vector<uint8_t> *out) {
  std::vector<uint8_t> buffer(8192);
  z_stream stream = {};
  stream.next_in = const_cast<Bytef *>(compressed);
  stream.avail_in = size;
  if (inflateInit2(&stream, windowSize) != Z_OK) {
    return false;
  }
  out->clear();
  bool success = false;
  while (true) {
    stream.next_out = buffer.data();
    stream.avail_out = buffer.size();
    int res = inflate(&stream, Z_NO_FLUSH);
    if (res != Z_OK && res != Z_STREAM_END) {
      break;
    }
    out->insert(out->end(), buffer.data(), buffer.data() + buffer.size() - stream.avail_out);
    if (res == Z_STREAM_END) {
      success = true;
      break;
    }
  }
  inflateEnd(&stream);
  return success;
}

}  // namespace

bool compressGzipped(const String &data, Vector<uint8_t> *out) {
  return compressGzipped(reinterpret_cast<const uint8_t *>(data.data()), data.size(), out);
}

bool compressGzipped(const Vector<uint8_t> &data, Vector<uint8_t> *out) {
  return compressGzipped(data.data(), data.size(), out);
}

bool compressGzipped(const uint8_t *data, size_t size, Vector<uint8_t> *out) {
  std::vector<uint8_t> buffer(8192);
  z_stream stream = {};
  if (
    deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 9, Z_DEFAULT_STRATEGY)
    != Z_OK) {
    return false;
  }
  out->clear();
  out->reserve(size / 4);
  stream.next_in = const_cast<Bytef *>(reinterpret_cast<const Bytef *>(data));
  stream.avail_in = size;
  bool success = false;
  while (true) {
    stream.next_out = buffer.data();
    stream.avail_out = buffer.size();
    int res = deflate(&stream, Z_FINISH);
    if (res != Z_OK && res != Z_STREAM_END) {
      break;
    }
    out->insert(out->end(), buffer.data(), buffer.data() + buffer.size() - stream.avail_out);
    if (res == Z_STREAM_END) {
      success = true;
      break;
    }
  }
  deflateEnd(&stream);
  return success;
}

bool decompressGzipped(const Vector<uint8_t> &compressed, String *out) {
  return decompressGzipped(compressed.data(), compressed.size(), out);
}

bool decompressGzipped(const Vector<uint8_t> &compressed, Vector<uint8_t> *out) {
  return decompressGzipped(compressed.data(), compressed.size(), out);
}

bool decompressGzipped(const uint8_t *compressed, size_t size, String *out) {
  Vector<uint8_t> buffer;
  if (!decompressGzipped(compressed, size, &buffer)) {
    return false;
  }
  out->assign(reinterpret_cast<const char *>(buffer.data()), buffer.size());
  return true;
}

bool decompressGzipped(const uint8_t *compressed, size_t size, Vector<uint8_t> *out) {
  // Here 16 means enable automatic gzip header detection; consider switching this to 32 to enable
  // both automated gzip and zlib header detection.
  return decompressGzippedImpl(compressed, size, 16 | MAX_WBITS, out);
}

bool decompressGzippedRaw(const Vector<uint8_t> &compressed, Vector<uint8_t> *out) {
  return decompressGzippedRaw(compressed.data(), compressed.size(), out);
}

bool decompressGzippedRaw(const uint8_t *compressed, size_t size, Vector<uint8_t> *out) {
  // Here -MAX_WBITS means there is no gzip header.
  return decompressGzippedImpl(compressed, size, -MAX_WBITS, out);
}

}  // namespace c8
