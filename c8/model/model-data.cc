// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "model-data.h",
  };
  deps = {
    ":model-data-view",
    "//c8:c8-log",
    "//c8:vector",
    "//c8/geometry:mesh-types",
    "//c8/geometry:splat",
    "//c8/pixels:pixel-buffer",
  };
}
cc_end(0x5706dd51);

#include "c8/model/model-data.h"

namespace c8 {

namespace {

struct ReadPtr {
  const uint8_t *ptr = nullptr;
};

struct PixelBufferHeader {
  int32_t rows = 0;
  int32_t cols = 0;
  int32_t rowBytes = 0;
};

struct ModelDataHeader {
  int32_t bitMask = 0;
  bool hasMesh() const { return bitMask & (1 << 0); }
  bool hasSplatAttributes() const { return bitMask & (1 << 1); }
  bool hasSplatTexture() const { return bitMask & (1 << 2); }
  bool hasSplatMultiTexture() const { return bitMask & (1 << 3); }
  bool hasPointCloud() const { return bitMask & (1 << 4); }
};

////////////////////////////////  Serialization Helpers ////////////////////////////////

template <typename T>
int32_t serializeNumBytesStruct(const T &s) {
  return sizeof(T);
}

template <typename T>
uint8_t *serializeStruct(const T &s, uint8_t *bytesPtr) {
  memcpy(bytesPtr, &s, sizeof(T));
  return bytesPtr + sizeof(T);
}

template <typename T>
int32_t serializeNumBytesVector(const Vector<T> &v) {
  return sizeof(int32_t) + v.size() * sizeof(T);
}

template <typename T>
uint8_t *serializeVector(const Vector<T> &v, uint8_t *bytesPtr) {
  int32_t n = static_cast<int32_t>(v.size());
  memcpy(bytesPtr, &n, sizeof(n));
  bytesPtr += sizeof(n);
  memcpy(bytesPtr, v.data(), v.size() * sizeof(T));
  return bytesPtr + v.size() * sizeof(T);
}

int32_t serializeNumBytesPixels8(ConstRGBA8888PlanePixels pixels) {
  return sizeof(PixelBufferHeader) + pixels.rows() * pixels.rowBytes();
}

uint8_t *serializePixels8(ConstRGBA8888PlanePixels pixels, uint8_t *bytes) {
  bytes = serializeStruct<PixelBufferHeader>(
    {
      .rows = pixels.rows(),
      .cols = pixels.cols(),
      .rowBytes = pixels.rowBytes(),
    },
    bytes);
  memcpy(bytes, pixels.pixels(), pixels.rows() * pixels.rowBytes());
  return bytes + pixels.rows() * pixels.rowBytes();
}

template <typename T>
int32_t serializeNumBytesPixels32(T pixels) {
  return sizeof(PixelBufferHeader) + pixels.rows() * pixels.rowElements() * sizeof(uint32_t);
}

template <typename T>
uint8_t *serializePixels32(T pixels, uint8_t *bytes) {
  int32_t pixelRowBytes = pixels.rowElements() * sizeof(uint32_t);
  bytes = serializeStruct<PixelBufferHeader>(
    {
      .rows = pixels.rows(),
      .cols = pixels.cols(),
      .rowBytes = pixelRowBytes,
    },
    bytes);
  memcpy(bytes, pixels.pixels(), pixels.rows() * pixelRowBytes);
  return bytes + pixels.rows() * pixelRowBytes;
}

int32_t serializeNumBytesMeshData(const MeshData &mesh) {
  return serializeNumBytesVector<HPoint3>(mesh.geometry.points)
    + serializeNumBytesVector<Color>(mesh.geometry.colors)
    + serializeNumBytesVector<HVector3>(mesh.geometry.normals)
    + serializeNumBytesVector<HVector2>(mesh.geometry.uvs)
    + serializeNumBytesVector<MeshIndices>(mesh.geometry.triangles)
    + serializeNumBytesPixels8(mesh.texture.pixels());
}

uint8_t *serializeMeshData(const MeshData &mesh, uint8_t *bytesPtr) {
  bytesPtr = serializeVector<HPoint3>(mesh.geometry.points, bytesPtr);
  bytesPtr = serializeVector<Color>(mesh.geometry.colors, bytesPtr);
  bytesPtr = serializeVector<HVector3>(mesh.geometry.normals, bytesPtr);
  bytesPtr = serializeVector<HVector2>(mesh.geometry.uvs, bytesPtr);
  bytesPtr = serializeVector<MeshIndices>(mesh.geometry.triangles, bytesPtr);
  bytesPtr = serializePixels8(mesh.texture.pixels(), bytesPtr);
  return bytesPtr;
}

int32_t serializeNumBytesSplatAttributes(const SplatAttributes &splat) {
  return serializeNumBytesStruct<SplatMetadata>(splat.header)
    + serializeNumBytesVector<HPoint3>(splat.positions)
    + serializeNumBytesVector<Quaternion>(splat.rotations)
    + serializeNumBytesVector<HVector3>(splat.scales) + serializeNumBytesVector<Color>(splat.colors)
    + serializeNumBytesVector<std::array<uint32_t, 2>>(splat.shRed)
    + serializeNumBytesVector<std::array<uint32_t, 2>>(splat.shGreen)
    + serializeNumBytesVector<std::array<uint32_t, 2>>(splat.shBlue)
    + serializeNumBytesVector<uint8_t>(splat.skybox.data);
}

uint8_t *serializeSplatAttributes(const SplatAttributes &splat, uint8_t *bytesPtr) {
  bytesPtr = serializeStruct<SplatMetadata>(splat.header, bytesPtr);
  bytesPtr = serializeVector<HPoint3>(splat.positions, bytesPtr);
  bytesPtr = serializeVector<Quaternion>(splat.rotations, bytesPtr);
  bytesPtr = serializeVector<HVector3>(splat.scales, bytesPtr);
  bytesPtr = serializeVector<Color>(splat.colors, bytesPtr);
  bytesPtr = serializeVector<std::array<uint32_t, 2>>(splat.shRed, bytesPtr);
  bytesPtr = serializeVector<std::array<uint32_t, 2>>(splat.shGreen, bytesPtr);
  bytesPtr = serializeVector<std::array<uint32_t, 2>>(splat.shBlue, bytesPtr);
  bytesPtr = serializeVector<uint8_t>(splat.skybox.data, bytesPtr);
  return bytesPtr;
}

int32_t serializeNumBytesSplatTexture(const SplatTexture &splat) {
  return serializeNumBytesStruct<SplatMetadata>(splat.header)
    + serializeNumBytesVector<uint32_t>(splat.sortedIds)
    + serializeNumBytesPixels32<ConstRGBA32PlanePixels>(splat.texture.pixels())
    + serializeNumBytesVector<uint8_t>(splat.skybox.data);
}

uint8_t *serializeSplatTexture(const SplatTexture &splat, uint8_t *bytesPtr) {
  bytesPtr = serializeStruct<SplatMetadata>(splat.header, bytesPtr);
  bytesPtr = serializeVector<uint32_t>(splat.sortedIds, bytesPtr);
  bytesPtr = serializePixels32<ConstRGBA32PlanePixels>(splat.texture.pixels(), bytesPtr);
  bytesPtr = serializeVector<uint8_t>(splat.skybox.data, bytesPtr);
  return bytesPtr;
}

int32_t serializeNumBytesSplatMultiTexture(const SplatMultiTexture &splat) {
  return serializeNumBytesStruct<SplatMetadata>(splat.header)
    + serializeNumBytesPixels32<ConstR32PlanePixels>(splat.sortedIds.pixels())
    + serializeNumBytesPixels32<ConstRGBA32PlanePixels>(splat.positionColor.pixels())
    + serializeNumBytesPixels32<ConstRGBA32PlanePixels>(splat.rotationScale.pixels())
    + serializeNumBytesPixels32<ConstRGBA32PlanePixels>(splat.shRG.pixels())
    + serializeNumBytesPixels32<ConstRGBA32PlanePixels>(splat.shB.pixels())
    + serializeNumBytesVector<uint8_t>(splat.skybox.data);
}

uint8_t *serializeSplatMultiTexture(const SplatMultiTexture &splat, uint8_t *bytesPtr) {
  bytesPtr = serializeStruct<SplatMetadata>(splat.header, bytesPtr);
  bytesPtr = serializePixels32<ConstR32PlanePixels>(splat.sortedIds.pixels(), bytesPtr);
  bytesPtr = serializePixels32<ConstRGBA32PlanePixels>(splat.positionColor.pixels(), bytesPtr);
  bytesPtr = serializePixels32<ConstRGBA32PlanePixels>(splat.rotationScale.pixels(), bytesPtr);
  bytesPtr = serializePixels32<ConstRGBA32PlanePixels>(splat.shRG.pixels(), bytesPtr);
  bytesPtr = serializePixels32<ConstRGBA32PlanePixels>(splat.shB.pixels(), bytesPtr);
  bytesPtr = serializeVector<uint8_t>(splat.skybox.data, bytesPtr);
  return bytesPtr;
}

int32_t serializeNumBytesPointCloudGeometry(const PointCloudGeometry &pc) {
  return serializeNumBytesVector<HPoint3>(pc.points) + serializeNumBytesVector<Color>(pc.colors)
    + serializeNumBytesVector<HVector3>(pc.normals);
}

uint8_t *serializePointCloudGeometry(const PointCloudGeometry &pc, uint8_t *bytesPtr) {
  bytesPtr = serializeVector<HPoint3>(pc.points, bytesPtr);
  bytesPtr = serializeVector<Color>(pc.colors, bytesPtr);
  bytesPtr = serializeVector<HVector3>(pc.normals, bytesPtr);
  return bytesPtr;
}

int32_t serializeNumBytesModelData(const ModelData &model) {
  int32_t meshBytes = model.mesh != nullptr ? serializeNumBytesMeshData(*model.mesh) : 0;
  int32_t splatAttributesBytes =
    model.splatAttributes != nullptr ? serializeNumBytesSplatAttributes(*model.splatAttributes) : 0;
  int32_t splatTextureBytes =
    model.splatTexture != nullptr ? serializeNumBytesSplatTexture(*model.splatTexture) : 0;
  int32_t splatMultiTextureBytes = model.splatMultiTexture != nullptr
    ? serializeNumBytesSplatMultiTexture(*model.splatMultiTexture)
    : 0;
  int32_t pointCloudBytes =
    model.pointCloud != nullptr ? serializeNumBytesPointCloudGeometry(*model.pointCloud) : 0;

  return sizeof(ModelDataHeader) + meshBytes + splatAttributesBytes + splatTextureBytes
    + splatMultiTextureBytes + pointCloudBytes;
}

uint8_t *serializeModelData(const ModelData &model, uint8_t *bytesPtr) {
  bytesPtr = serializeStruct<ModelDataHeader>(
    {.bitMask = ((model.mesh != nullptr) << 0)        // hashMesh
       | ((model.splatAttributes != nullptr) << 1)    // hasSplatAttributes
       | ((model.splatTexture != nullptr) << 2)       // hasSplatTexture
       | ((model.splatMultiTexture != nullptr) << 3)  // hasSplatMultiTexture
       | ((model.pointCloud != nullptr) << 4)},       // hasPointCloud
    bytesPtr);

  if (model.mesh != nullptr) {
    bytesPtr = serializeMeshData(*(model.mesh), bytesPtr);
  }

  if (model.splatAttributes != nullptr) {
    bytesPtr = serializeSplatAttributes(*(model.splatAttributes), bytesPtr);
  }

  if (model.splatTexture != nullptr) {
    bytesPtr = serializeSplatTexture(*(model.splatTexture), bytesPtr);
  }

  if (model.splatMultiTexture != nullptr) {
    bytesPtr = serializeSplatMultiTexture(*(model.splatMultiTexture), bytesPtr);
  }

  if (model.pointCloud != nullptr) {
    bytesPtr = serializePointCloudGeometry(*(model.pointCloud), bytesPtr);
  }

  return bytesPtr;
}

////////////////////////////////  Deserialization Helpers ////////////////////////////////

template <typename T>
T readStruct(ReadPtr *readPtr) {
  T s;
  memcpy(&s, readPtr->ptr, sizeof(T));
  readPtr->ptr += sizeof(T);
  return s;
}

template <typename T>
VectorView<T> vectorView(ReadPtr *readPtr) {
  VectorView<T> view;
  memcpy(&view.size, readPtr->ptr, sizeof(int32_t));
  readPtr->ptr += sizeof(int32_t);
  view.data = view.size == 0 ? nullptr : reinterpret_cast<const T *>(readPtr->ptr);
  readPtr->ptr += view.size * sizeof(T);
  return view;
}

ConstRGBA8888PlanePixels constPixelsRGBA8(ReadPtr *readPtr) {
  auto header = readStruct<PixelBufferHeader>(readPtr);
  if (header.rows == 0 || header.rowBytes == 0) {
    return {};
  }
  ConstRGBA8888PlanePixels pix = {
    header.rows,
    header.cols,
    header.rowBytes,
    readPtr->ptr,
  };
  readPtr->ptr += header.rows * header.rowBytes;
  return pix;
}

template <typename T>
T pixels32(ReadPtr *readPtr) {
  auto header = readStruct<PixelBufferHeader>(readPtr);
  if (header.rows == 0 || header.rowBytes == 0) {
    return {};
  }
  T pix = {
    header.rows,
    header.cols,
    header.rowBytes / 4,
    reinterpret_cast<const uint32_t *>(readPtr->ptr),
  };
  readPtr->ptr += header.rows * header.rowBytes;
  return pix;
}

// This is a convenience method to read the model data header from the beginning of the serialized
// model data, which we do multiple times during the deserialization process.
ModelDataHeader modelDataHeader(const uint8_t *serializedModelDataPtr) {
  ReadPtr readPtr = {.ptr = serializedModelDataPtr};
  return readStruct<ModelDataHeader>(&readPtr);
}

const uint8_t *meshPtr(const uint8_t *serializedModelDataPtr) {
  if (!hasMesh(serializedModelDataPtr)) {
    return nullptr;
  }
  return serializedModelDataPtr + sizeof(ModelDataHeader);
}

MeshDataView meshView(const uint8_t *serializedModelDataPtr, ReadPtr *readPtr) {
  if (!hasMesh(serializedModelDataPtr)) {
    return {};
  }
  return {
    .geometry =
      {
        .points = vectorView<HPoint3>(readPtr),
        .colors = vectorView<Color>(readPtr),
        .normals = vectorView<HVector3>(readPtr),
        .uvs = vectorView<HVector2>(readPtr),
        .triangles = vectorView<MeshIndices>(readPtr),
      },
    .texture = constPixelsRGBA8(readPtr),
  };
}

const uint8_t *splatAttributesPtr(const uint8_t *serializedModelDataPtr) {
  if (!hasSplatAttributes(serializedModelDataPtr)) {
    return nullptr;
  }
  ReadPtr readPtr = {.ptr = serializedModelDataPtr + sizeof(ModelDataHeader)};
  meshView(serializedModelDataPtr, &readPtr);
  return readPtr.ptr;
}

SplatSkybox skyboxView(ReadPtr *data) {
  auto v = vectorView<uint8_t>(data);
  return skybox(v.data, v.size);
}

SplatAttributesView splatAttributesView(const uint8_t *serializedModelDataPtr, ReadPtr *readPtr) {
  if (!hasSplatAttributes(serializedModelDataPtr)) {
    return {};
  }
  return {
    .header = readStruct<SplatMetadata>(readPtr),
    .positions = vectorView<HPoint3>(readPtr),
    .rotations = vectorView<Quaternion>(readPtr),
    .scales = vectorView<HVector3>(readPtr),
    .colors = vectorView<Color>(readPtr),
    .shRed = vectorView<std::array<uint32_t, 2>>(readPtr),
    .shGreen = vectorView<std::array<uint32_t, 2>>(readPtr),
    .shBlue = vectorView<std::array<uint32_t, 2>>(readPtr),
    .skybox = skyboxView(readPtr),
  };
}

const uint8_t *splatTexturePtr(const uint8_t *serializedModelDataPtr) {
  if (!hasSplatTexture(serializedModelDataPtr)) {
    return nullptr;
  }

  ReadPtr readPtr = {.ptr = serializedModelDataPtr + sizeof(ModelDataHeader)};
  meshView(serializedModelDataPtr, &readPtr);
  splatAttributesView(serializedModelDataPtr, &readPtr);
  return readPtr.ptr;
}

SplatTextureView splatTextureView(const uint8_t *serializedModelDataPtr, ReadPtr *readPtr) {
  if (!hasSplatTexture(serializedModelDataPtr)) {
    return {};
  }
  SplatTextureView v = {
    .header = readStruct<SplatMetadata>(readPtr),
    .sortedIds = vectorView<uint32_t>(readPtr),
    .texture = pixels32<ConstRGBA32PlanePixels>(readPtr),
    .skybox = skyboxView(readPtr),
  };
  if (v.texture.pixels() != nullptr) {  // Add interleaved attribute data as a view on the texture.
    v.interleavedAttributeData = {
      .data = reinterpret_cast<const uint8_t *>(v.texture.pixels()),
      .size = static_cast<size_t>(64 * v.header.numPoints),  // 4 RGBA32 pixels = 64 bytes.
    };
  }
  return v;
}

const uint8_t *splatMultiTexturePtr(const uint8_t *serializedModelDataPtr) {
  if (!hasSplatMultiTexture(serializedModelDataPtr)) {
    return nullptr;
  }
  ReadPtr readPtr = {.ptr = serializedModelDataPtr + sizeof(ModelDataHeader)};
  meshView(serializedModelDataPtr, &readPtr);
  splatAttributesView(serializedModelDataPtr, &readPtr);
  splatTextureView(serializedModelDataPtr, &readPtr);
  return readPtr.ptr;
}

SplatMultiTextureView splatMultiTextureView(
  const uint8_t *serializedModelDataPtr, ReadPtr *readPtr) {
  if (!hasSplatMultiTexture(serializedModelDataPtr)) {
    return {};
  }
  return {
    .header = readStruct<SplatMetadata>(readPtr),
    .sortedIds = pixels32<ConstR32PlanePixels>(readPtr),
    .positionColor = pixels32<ConstRGBA32PlanePixels>(readPtr),
    .rotationScale = pixels32<ConstRGBA32PlanePixels>(readPtr),
    .shRG = pixels32<ConstRGBA32PlanePixels>(readPtr),
    .shB = pixels32<ConstRGBA32PlanePixels>(readPtr),
    .skybox = skyboxView(readPtr),
  };
}

const uint8_t *pointCloudPtr(const uint8_t *serializedModelDataPtr) {
  if (!hasPointCloud(serializedModelDataPtr)) {
    return nullptr;
  }
  ReadPtr readPtr = {.ptr = serializedModelDataPtr + sizeof(ModelDataHeader)};
  meshView(serializedModelDataPtr, &readPtr);
  splatAttributesView(serializedModelDataPtr, &readPtr);
  splatTextureView(serializedModelDataPtr, &readPtr);
  splatMultiTextureView(serializedModelDataPtr, &readPtr);
  return readPtr.ptr;
}

PointCloudGeometryView pointCloudView(const uint8_t *serializedModelDataPtr, ReadPtr *readPtr) {
  if (!hasPointCloud(serializedModelDataPtr)) {
    return {};
  }
  return {
    .points = vectorView<HPoint3>(readPtr),
    .colors = vectorView<Color>(readPtr),
    .normals = vectorView<HVector3>(readPtr),
  };
}

}  // namespace

////////////////////////////////  Public Function Implementations ////////////////////////////////

int32_t serialize(const ModelData &model, Vector<uint8_t> *result) {
  int32_t numBytes = serializeNumBytesModelData(model);
  if (result->size() < numBytes) {
    result->resize(numBytes);
  }
  serializeModelData(model, result->data());
  return numBytes;
}

bool hasMesh(const uint8_t *serializedModelDataPtr) {
  return modelDataHeader(serializedModelDataPtr).hasMesh();
}

MeshDataView meshView(const uint8_t *serializedModelDataPtr) {
  if (!hasMesh(serializedModelDataPtr)) {
    return {};
  }
  ReadPtr readPtr = {.ptr = meshPtr(serializedModelDataPtr)};
  return meshView(serializedModelDataPtr, &readPtr);
}

bool hasSplatAttributes(const uint8_t *serializedModelDataPtr) {
  return modelDataHeader(serializedModelDataPtr).hasSplatAttributes();
}

SplatAttributesView splatAttributesView(const uint8_t *serializedModelDataPtr) {
  if (!hasSplatAttributes(serializedModelDataPtr)) {
    return {};
  }
  ReadPtr readPtr = {.ptr = splatAttributesPtr(serializedModelDataPtr)};
  return splatAttributesView(serializedModelDataPtr, &readPtr);
}

bool hasSplatTexture(const uint8_t *serializedModelDataPtr) {
  return modelDataHeader(serializedModelDataPtr).hasSplatTexture();
}

SplatTextureView splatTexture(const uint8_t *serializedModelDataPtr) {
  if (!hasSplatTexture(serializedModelDataPtr)) {
    return {};
  }
  ReadPtr readPtr = {.ptr = splatTexturePtr(serializedModelDataPtr)};
  return splatTextureView(serializedModelDataPtr, &readPtr);
}

bool hasSplatMultiTexture(const uint8_t *serializedModelDataPtr) {
  return modelDataHeader(serializedModelDataPtr).hasSplatMultiTexture();
}

SplatMultiTextureView splatMultiTexture(const uint8_t *serializedModelDataPtr) {
  if (!hasSplatMultiTexture(serializedModelDataPtr)) {
    return {};
  }
  ReadPtr readPtr = {.ptr = splatMultiTexturePtr(serializedModelDataPtr)};
  return splatMultiTextureView(serializedModelDataPtr, &readPtr);
}

bool hasPointCloud(const uint8_t *serializedModelDataPtr) {
  return modelDataHeader(serializedModelDataPtr).hasPointCloud();
}

PointCloudGeometryView pointCloudView(const uint8_t *serializedModelDataPtr) {
  if (!hasPointCloud(serializedModelDataPtr)) {
    return {};
  }
  ReadPtr readPtr = {.ptr = pointCloudPtr(serializedModelDataPtr)};
  return pointCloudView(serializedModelDataPtr, &readPtr);
}

}  // namespace c8
