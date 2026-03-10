// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "load-splat.h",
  };
  deps = {
    "//c8/spz:load-spz",
    "//c8:c8-log",
    "//c8:map",
    "//c8:scope-exit",
    "//c8/geometry:splat",
    "//c8/stats:scope-timer",
    "@draco//:draco",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xfba235ae);

#include <draco/compression/decode.h>

#include <fstream>
#include <sstream>

#include "c8/c8-log.h"
#include "c8/geometry/load-splat.h"
#include "c8/geometry/splat-io.h"
#include "c8/map.h"
#include "c8/scope-exit.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

namespace {

#define SPLAT_SCALES_ID 10001
#define SPLAT_ROTATIONS_ID 10002
#define SPLAT_ALPHAS_ID 10003
#define SPLAT_COLORS_ID 10004
#define SPLAT_SH_ID 10005

struct MemoryStreamBuf : public std::streambuf {
  MemoryStreamBuf(const char *data, size_t size) {
    char *non_const_data = const_cast<char *>(data);
    this->setg(non_const_data, non_const_data, non_const_data + size);
  }
};

int degreeForDim(int dim) {
  if (dim < 3)
    return 0;
  if (dim < 8)
    return 1;
  if (dim < 15)
    return 2;
  return 3;
}

bool readGenericFloatData(
  const std::unique_ptr<draco::PointCloud> &dracoPointCloud, int type, std::vector<float> &data) {
  // Decode Data
  const draco::PointAttribute *labelAttribute =
    dracoPointCloud->GetNamedAttributeByUniqueId(draco::GeometryAttribute::GENERIC, type);

  if (labelAttribute) {

    draco::DataBuffer *dataBuffer = labelAttribute->buffer();

    data.resize(labelAttribute->size() * labelAttribute->num_components());
    memcpy(
      (void *)data.data(),
      (void *)dataBuffer->data(),
      sizeof(float) * labelAttribute->size() * labelAttribute->num_components());

    return true;

  } else {
    C8Log("ReadGenericFloatData(): ERROR, labelAttribute %d is NULL.\n", type);
    return false;
  }
}

SplatRawData loadSplatDrcStream(std::istream &in) {
  if (!in.good()) {
    C8Log("Unable to open data stream");
    return {};
  }

  std::stringstream bufferString;
  bufferString << in.rdbuf();

  std::string data = bufferString.str();
  int size = data.size();

  draco::DecoderBuffer buffer;
  buffer.Init(data.c_str(), size);

  const draco::EncodedGeometryType geomType =
    draco::Decoder::GetEncodedGeometryType(&buffer).value();

  if (geomType != draco::POINT_CLOUD) {
    return {};
  }

  draco::Decoder decoder;

  SplatRawData splat;

  std::unique_ptr<draco::PointCloud> dracoPointCloud =
    decoder.DecodePointCloudFromBuffer(&buffer).value();

  const draco::PointAttribute *attribute =
    dracoPointCloud->GetNamedAttribute(draco::GeometryAttribute::POSITION);
  draco::DataBuffer *attributeDataBuffer = attribute->buffer();

  splat.header.numPoints = attribute->size();
  splat.header.maxNumPoints = attribute->size();
  splat.positions.resize(attribute->size() * attribute->num_components());
  memcpy(
    (void *)splat.positions.data(),
    (void *)attributeDataBuffer->data(),
    sizeof(float) * attribute->size() * attribute->num_components());

  // Decode Colors
  if (!readGenericFloatData(dracoPointCloud, SPLAT_COLORS_ID, splat.colors)) {
    C8Log("ERROR: Failed reading Colors attribute\n");
  }

  // Decode Scales
  if (!readGenericFloatData(dracoPointCloud, SPLAT_SCALES_ID, splat.scales)) {
    C8Log("ERROR: Failed reading Scales attribute\n");
  }

  // Decode Rotations
  if (!readGenericFloatData(dracoPointCloud, SPLAT_ROTATIONS_ID, splat.rotations)) {
    C8Log("ERROR: Failed reading Rotations attribute\n");
  }

  // Decode alphas
  if (!readGenericFloatData(dracoPointCloud, SPLAT_ALPHAS_ID, splat.alphas)) {
    C8Log("ERROR: Failed reading Alphas attribute\n");
  }

  // Decode sh
  if (!readGenericFloatData(dracoPointCloud, SPLAT_SH_ID, splat.sh)) {
    C8Log("ERROR: Failed reading Sh attribute\n");
  }

  splat.header.shDegree = degreeForDim(splat.sh.size() / (3 * splat.header.numPoints));

  return splat;
}

SplatRawData loadSplatPlyStream(std::istream &in) {
  if (!in.good()) {
    C8Log("Unable to open data stream");
    return {};
  }
  std::string line;
  std::getline(in, line);
  if (line != "ply") {
    C8Log("Not a .ply file: %s", line.c_str());
    return {};
  }

  std::getline(in, line);
  if (line != "format binary_little_endian 1.0") {
    C8Log("Unsupported .ply format");
    return {};
  }

  std::getline(in, line);
  if (line.find("element vertex ") != 0) {
    C8Log("Expected `element vertex`");
    return {};
  }

  int numPoints = std::stoi(line.substr(std::strlen("element vertex ")));
  if (numPoints <= 0 || numPoints > 10 * 1024 * 1024) {
    C8Log("Invalid number of points: %d", numPoints);
    return {};
  }

  TreeMap<String, int> fields;  // name -> index
  for (int i = 0;; i++) {
    std::getline(in, line);
    if (line == "end_header") {
      break;
    }

    if (line.find("property float ") != 0) {
      C8Log("Expected `property float`");
      return {};
    }

    std::string name = line.substr(std::strlen("property float "));
    fields[name] = i;
  }

  // Find field indices
  const auto index = [&fields](const std::string &name) {
    const auto &itr = fields.find(name);
    return itr->second;
  };

  const std::vector<int> positionIdx = {index("x"), index("y"), index("z")};
  const std::vector<int> scaleIdx = {index("scale_0"), index("scale_1"), index("scale_2")};
  const std::vector<int> rotIdx = {index("rot_0"), index("rot_1"), index("rot_2"), index("rot_3")};
  const std::vector<int> alphaIdx = {index("opacity")};
  const std::vector<int> colorIdx = {index("f_dc_0"), index("f_dc_1"), index("f_dc_2")};
  // Spherical harmonics are optional and variable in size (depending on degree)
  std::vector<int> shIdx;
  for (int i = 0; i < 45; i++) {
    const auto &itr = fields.find("f_rest_" + std::to_string(i));
    if (itr == fields.end())
      break;
    shIdx.push_back(itr->second);
  }
  const int shDim = shIdx.size() / 3;

  // Initialize result with preallocated vectors
  SplatRawData result;
  result.header.numPoints = numPoints;
  result.header.maxNumPoints = numPoints;
  result.header.shDegree = degreeForDim(shDim);

  // Pre-allocate all vectors to their final size
  result.positions.resize(numPoints * 3);
  result.scales.resize(numPoints * 3);
  result.rotations.resize(numPoints * 4);
  result.alphas.resize(numPoints);
  result.colors.resize(numPoints * 3);
  if (shDim > 0) {
    result.sh.resize(numPoints * 3 * shDim);
  }

  // SH coefficient inverter for coordinate system conversion
  std::array<int, 8> shYZIndices = {0, 1, 3, 6, 8, 10, 11, 13};
  std::array<float, 15> shInverter;
  std::fill(shInverter.begin(), shInverter.end(), 1.0f);
  for (auto idx : shYZIndices) {
    if (idx < shInverter.size()) {  // prevents array access violations if the SH degree is higher
                                    // than expected
      shInverter[idx] = -1.0f;
    }
  }

  // Process data in chunks to reduce memory usage
  const size_t fieldsSize = fields.size();
  constexpr int chunkSize = 1024;  // Process 1024 points at a time
  std::vector<float> chunk(chunkSize * fieldsSize);

  for (int pointOffset = 0; pointOffset < numPoints; pointOffset += chunkSize) {
    // Calculate actual chunk size (might be smaller for last chunk)
    const int currentChunkSize = std::min(chunkSize, numPoints - pointOffset);

    // Read chunk of data
    in.read(reinterpret_cast<char *>(chunk.data()), currentChunkSize * fieldsSize * sizeof(float));

    if (!in.good() && pointOffset + currentChunkSize < numPoints) {
      C8Log("[load-splat] Error reading data from stream at point %d", pointOffset);
      return {};
    }

    // Process chunk
    // We have to convert from rdf ro rub. The swizzling/mirroring below should implement this.
    for (int i = 0; i < currentChunkSize; i++) {
      const int pointIdx = pointOffset + i;
      const int baseChunkIdx = i * fieldsSize;

      // Position (with coordinate conversion)
      result.positions[pointIdx * 3] = chunk[baseChunkIdx + positionIdx[0]];
      result.positions[pointIdx * 3 + 1] = -chunk[baseChunkIdx + positionIdx[1]];
      result.positions[pointIdx * 3 + 2] = -chunk[baseChunkIdx + positionIdx[2]];

      // Scales
      for (int j = 0; j < 3; j++) {
        result.scales[pointIdx * 3 + j] = chunk[baseChunkIdx + scaleIdx[j]];
      }

      // Rotation (with coordinate conversion)
      const float w = chunk[baseChunkIdx + rotIdx[0]];
      const float x = chunk[baseChunkIdx + rotIdx[1]];
      const float y = chunk[baseChunkIdx + rotIdx[2]];
      const float z = chunk[baseChunkIdx + rotIdx[3]];

      result.rotations[pointIdx * 4] = -x;
      result.rotations[pointIdx * 4 + 1] = w;
      result.rotations[pointIdx * 4 + 2] = -z;
      result.rotations[pointIdx * 4 + 3] = y;

      // Alpha
      result.alphas[pointIdx] = chunk[baseChunkIdx + alphaIdx[0]];

      // Colors
      for (int j = 0; j < 3; j++) {
        result.colors[pointIdx * 3 + j] = chunk[baseChunkIdx + colorIdx[j]];
      }

      // Spherical harmonics (with coordinate conversion)
      for (int j = 0; j < shDim; j++) {
        const float inverter = (j < shInverter.size()) ? shInverter[j] : 1.0f;
        result.sh[pointIdx * 3 * shDim + j * 3] = inverter * chunk[baseChunkIdx + shIdx[j]];
        result.sh[pointIdx * 3 * shDim + j * 3 + 1] =
          inverter * chunk[baseChunkIdx + shIdx[j + shDim]];
        result.sh[pointIdx * 3 * shDim + j * 3 + 2] =
          inverter * chunk[baseChunkIdx + shIdx[j + 2 * shDim]];
      }
    }
  }
  return result;
}

SplatRawData loadSplatSpz(const Vector<uint8_t> &data) {
  auto gCloud = loadGaussianCloud(data);
  SplatRawData result = {
    .header =
      {
        .numPoints = gCloud.numPoints,
        .maxNumPoints = gCloud.numPoints,
        .shDegree = gCloud.shDegree,
        .antialiased = gCloud.antialiased,
      },
    .positions = std::move(gCloud.positions),
    .scales = std::move(gCloud.scales),
    .rotations = std::move(gCloud.rotations),
    .alphas = std::move(gCloud.alphas),
    .colors = std::move(gCloud.colors),
    .sh = std::move(gCloud.sh),
  };
  // Change rotation from xyzw to wxyz
  for (size_t i = 0; i < result.rotations.size(); i += 4) {
    auto x = result.rotations[i];
    auto y = result.rotations[i + 1];
    auto z = result.rotations[i + 2];
    auto w = result.rotations[i + 3];

    result.rotations[i] = w;
    result.rotations[i + 1] = x;
    result.rotations[i + 2] = y;
    result.rotations[i + 3] = z;
  }
  return result;
}

Vector<uint8_t> readFileBytes(const String &filename) {
  std::ifstream file(filename.c_str(), std::ios::binary);
  if (!file) {
    return {};
  }

  // Determine the size of the file
  file.seekg(0, std::ios::end);
  std::streampos fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  // Create a vector to store the file content
  Vector<uint8_t> fileContent(fileSize);

  // Read the file content into the vector
  file.read(reinterpret_cast<char *>(fileContent.data()), fileSize);
  return fileContent;
}

}  // namespace

SplatRawData loadSplatPlyFile(const String &filename) {
  std::ifstream in(filename, std::ios::binary);
  auto ex = ScopeExit([&in] { in.close(); });
  return loadSplatPlyStream(in);
}

SplatRawData loadSplatDrcFile(const String &filename) {
  std::ifstream in(filename, std::ios::binary);
  auto ex = ScopeExit([&in] { in.close(); });
  return loadSplatDrcStream(in);
}

SplatRawData loadSplatDrcData(const Vector<uint8_t> &data) {
  return loadSplatDrcData(data.data(), data.size());
}

SplatRawData loadSplatDrcData(const uint8_t *data, size_t size) {
  std::stringstream in(String{reinterpret_cast<const char *>(data), size});
  return loadSplatDrcStream(in);
}

SplatRawData loadSplatPlyData(const Vector<uint8_t> &data) {
  return loadSplatPlyData(data.data(), data.size());
}

SplatRawData loadSplatPlyData(const uint8_t *data, size_t size) {
  // Create a memory buffer that doesn't copy the data
  MemoryStreamBuf buffer(reinterpret_cast<const char *>(data), size);
  std::istream in(&buffer);
  return loadSplatPlyStream(in);
}

SplatRawData loadSplatSpzFile(const String &filename) {
  auto fileContent = readFileBytes(filename);
  return loadSplatSpz(fileContent);
}

SplatRawData loadSplatSpzData(const Vector<uint8_t> &data) { return loadSplatSpz(data); }

SplatRawData loadSplatSpzData(const uint8_t *data, size_t size) {
  // This makes a copy :-(. Would be nice to not have to do this if loadSplat in ScanKit works
  // on const uint8_t* and size_t.
  Vector<uint8_t> dataVec(data, data + size);
  return loadSplatSpz(dataVec);
}

SplatRawData loadSplat(const String &filename) {
  auto fileBytes = readFileBytes(filename);
  return loadSplat(fileBytes.data(), fileBytes.size());
}

SplatRawData loadSplat(const uint8_t *data, size_t size) {
  // Spz files are zipped, so we can't just check the first 4 bytes for MAGIC header
  // We can check for ply file header though
  if (data[0] == 'p' && data[1] == 'l' && data[2] == 'y') {
    return loadSplatPlyData(data, size);
  } else if (
    data[0] == 'D' && data[1] == 'R' && data[2] == 'A' && data[3] == 'C' && data[4] == 'O') {
    return loadSplatDrcData(data, size);
  } else {
    return loadSplatSpzData(data, size);
  }
}

PackedGaussians loadSplatSpzFilePacked(const String &filename) {
  ScopeTimer t("load-spz-file");
  return loadSplatPacked(filename);
}

PackedGaussians loadSplatSpzDataPacked(const uint8_t *data, size_t size) {
  ScopeTimer t("decode-spz");
  return loadSplatPacked(data, size);
}

PackedGaussians loadSplatSpzDataPacked(const Vector<uint8_t> &data) {
  return loadSplatSpzDataPacked(data.data(), data.size());
}

}  // namespace c8
