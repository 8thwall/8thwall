// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Nathan Waters (nathanwaters@nianticlabs.com)
// Testing draco integration.

#include "bzl/inliner/rules2.h"

cc_binary {
  deps = {
    "//c8:c8-log",
    "//c8/io:file-io",
    "//c8/pixels:base64",
    "@draco//:draco",
  };
  data = {
    "//bzl/examples/draco/testdata:draco-asset",
  };
}
cc_end(0xb7bd2455);

#include <draco/compression/decode.h>
#include <draco/core/decoder_buffer.h>
#include <draco/io/file_utils.h>

#include "c8/c8-log.h"
#include "c8/io/file-io.h"
#include "c8/pixels/base64.h"

using namespace c8;

namespace {}  // namespace

int readDraco(const String &assetPath, bool isBase64Encoded) {
  // I couldn't get this working but this is the typical approach.  draco FileReaderFactory
  // GetFileReaderOpenFunctions() returns an empty vector in OpenReader().
  // std::vector<char> dataTmp;
  // if (!draco::ReadFileToBuffer(bunny, &dataTmp)) {
  //   C8Log("Failed opening file %s", bunny.c_str());
  // }

  std::vector<uint8_t> data;

  if (isBase64Encoded) {
    const auto &strData = readTextFile(assetPath);
    data = decode(strData);
  } else {
    data = readFile(assetPath);
  }

  draco::DecoderBuffer buffer;
  buffer.Init(reinterpret_cast<char *>(data.data()), data.size());

  // buffer.Init(data.data(), data.size());
  // Typically you would do something like this to determine if it's a mesh or point cloud but in
  // this case we know it's a mesh.
  // const draco::EncodedGeometryType geom_type = draco::GetEncodedGeometryType(&buffer);

  draco::Decoder decoder;
  auto statusor = decoder.DecodeMeshFromBuffer(&buffer);
  if (!statusor.ok()) {
    C8Log("[hello-draco] Error when decoding mesh from buffer");
    return false;
  }

  std::unique_ptr<draco::Mesh> mesh = std::move(statusor).value();

  if (!mesh) {
    C8Log("[hello-draco] Failed to decode the input file.");
    return false;
  }

  C8Log("[hello-draco] Able to decode %s!", assetPath.c_str());
  C8Log("[hello-draco] Number of faces: %d", mesh->num_faces());
  C8Log("[hello-draco] Number of attributes: %d", mesh->num_attributes());
  C8Log("[hello-draco] Number of points: %d", mesh->num_points());

  // Test 1 - Getting points for a face.
  draco::GeometryAttribute *positionAttr = mesh->attribute(0);
  for (draco::FaceIndex fi(0); fi < 3; ++fi) {
    const auto &face = mesh->face(fi);
    C8Log("[hello-draco] Face %d is <%d, %d, %d>", fi, face[0], face[1], face[2]);

    float pts[3][3];
    positionAttr->GetValue(draco::AttributeValueIndex(face[0].value()), &pts[0]);
    positionAttr->GetValue(draco::AttributeValueIndex(face[1].value()), &pts[1]);
    positionAttr->GetValue(draco::AttributeValueIndex(face[2].value()), &pts[2]);
    C8Log(
      "[hello-draco] Corresponding points: <%.6fx, %.6fy, %.6fz>, <%.6fx, %.6fy, %.6fz>, <%.6fx, "
      "%.6fy, %.6fz>",
      pts[0][0],
      pts[0][1],
      pts[0][2],
      pts[1][0],
      pts[1][1],
      pts[1][2],
      pts[2][0],
      pts[2][1],
      pts[2][2]);
  }

  // Test 2 - Get the first through fifth point using this approach.
  float pts[15];
  positionAttr->GetValue(draco::AttributeValueIndex(0), &pts[0]);
  positionAttr->GetValue(draco::AttributeValueIndex(1), &pts[3]);
  positionAttr->GetValue(draco::AttributeValueIndex(2), &pts[6]);
  positionAttr->GetValue(draco::AttributeValueIndex(3), &pts[9]);
  positionAttr->GetValue(draco::AttributeValueIndex(4), &pts[12]);
  C8Log(
    "[hello-draco] Points 0 through 4: <%.6fx, %.6fy, %.6fz>, <%.6fx, %.6fy, %.6fz>, <%.6fx, "
    "%.6fy, %.6fz>, <%.6fx, %.6fy, %.6fz>, <%.6fx, %.6fy, %.6fz>",
    pts[0],
    pts[1],
    pts[2],
    pts[3],
    pts[4],
    pts[5],
    pts[6],
    pts[7],
    pts[8],
    pts[9],
    pts[10],
    pts[11],
    pts[12],
    pts[13],
    pts[14]);

  // Test 3 - This is likely an even faster approach for getting all the points since it's using
  // memcpy.
  const draco::DataBuffer *posBuffer = positionAttr->buffer();
  float ptsRead[15];
  posBuffer->Read(0, ptsRead, 15 * sizeof(float));
  C8Log(
    "[hello-draco] Points 0 through 4: <%.6fx, %.6fy, %.6fz>, <%.6fx, %.6fy, %.6fz>, <%.6fx, "
    "%.6fy, %.6fz>, <%.6fx, %.6fy, %.6fz>, <%.6fx, %.6fy, %.6fz>",
    ptsRead[0],
    ptsRead[1],
    ptsRead[2],
    ptsRead[3],
    ptsRead[4],
    ptsRead[5],
    ptsRead[6],
    ptsRead[7],
    ptsRead[8],
    ptsRead[9],
    ptsRead[10],
    ptsRead[11],
    ptsRead[12],
    ptsRead[13],
    ptsRead[14]);
  C8Log("[hello-draco] Finished successfully processing %s\n", assetPath.c_str());
  return true;
}

int main(int argc, char *argv[]) {
  if (!readDraco("bzl/examples/draco/testdata/bunny.drc", false)) {
    return -1;
  }
}
