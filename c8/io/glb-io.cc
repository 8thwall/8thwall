// Copyright (c) 2025 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "glb-io.h",
  };
  deps = {
    ":file-io",
    "//c8:c8-log",
    "//c8:string",
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xb21595cd);

#include "c8/c8-log.h"
#include "c8/io/file-io.h"
#include "c8/io/glb-io.h"

namespace c8 {

namespace {

struct GlbHeader {
  uint32_t magic;
  uint32_t version;
  uint32_t length;
};

struct GlbChunkHeader{
  uint32_t length;
  uint32_t type;
};

struct ReadPtr {
  const uint8_t *ptr = nullptr;
};

template <typename T>
T readStruct(ReadPtr *readPtr) {
  T s;
  memcpy(&s, readPtr->ptr, sizeof(T));
  readPtr->ptr += sizeof(T);
  return s;
}

}

GlbRawData readGlbRaw(const String &filename) {
  auto data = readFile(filename);
  return readGlbRaw(data.data(), data.size());
}

GlbRawData readGlbRaw(const Vector<uint8_t> &data) {
  return readGlbRaw(data.data(), data.size());
}

GlbRawData readGlbRaw(const uint8_t *data, size_t size) {
  if (size == 0) {
    C8Log("[glb-io] ERROR: GLB file is empty.");
    return {};
  }

  ReadPtr curr = {.ptr = data};
  ReadPtr end = {.ptr = data + size};

  auto header = readStruct<GlbHeader>(&curr);
  if (header.magic != 0x46546c67) {
    C8Log("[glb-io] ERROR: GLB file has invalid magic number. (0x%x)", header.magic);
    return {};
  }
  if (header.version != 2) {
    C8Log("[glb-io] ERROR: GLB file has invalid version number. (%d)", header.version);
    return {};
  }
  if (header.length != size) {
    C8Log("[glb-io] ERROR: GLB file has invalid size. (%d != %d)", header.length, size);
    return {};
  }

  auto jsonHeader = readStruct<GlbChunkHeader>(&curr);
  if (jsonHeader.type != 0x4E4F534A) {
    C8Log("[glb-io] ERROR: GLB file has invalid JSON chunk type. (0x%x)", jsonHeader.type);
    return {};
  }

  if (jsonHeader.length > end.ptr - curr.ptr) {
    C8Log("[glb-io] ERROR: GLB file has invalid JSON chunk length. (%d)", jsonHeader.length);
    return {};
  }

  String json(reinterpret_cast<const char *>(curr.ptr), jsonHeader.length);
  curr.ptr += jsonHeader.length;

  if (curr.ptr == end.ptr) {
    return {.json = json};
  }

  auto binHeader = readStruct<GlbChunkHeader>(&curr);
  if (binHeader.type != 0x004E4942) {
    C8Log("[glb-io] ERROR: GLB file has invalid BIN chunk type. (0x%x)", binHeader.type);
    return {};
  }

  if (binHeader.length > end.ptr - curr.ptr) {
    C8Log("[glb-io] ERROR: GLB file has invalid BIN chunk length. (%d)", binHeader.length);
    return {};
  }

  Vector<uint8_t> bin(curr.ptr, curr.ptr + binHeader.length); 
  curr.ptr += binHeader.length;

  if (curr.ptr != end.ptr) {
    C8Log("[glb-io] ERROR: GLB file has extra data at the end.");
    return {};
  }

  return {.json = json, .bin = bin};
}

}  // namespace c8
