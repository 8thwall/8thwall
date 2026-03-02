// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "model-3d-io.h",
  };
  deps = {
    "//c8:hpoint",
    "//c8/geometry:mesh-types",
    "//c8:string",
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x70af5ad9);

#include <cstdio>
#include <fstream>
#include <iostream>

#include "c8/io/model-3d-io.h"

namespace c8 {

void writeModelToPLY(
  const String &fileName,
  const int vertexCount,
  const Vector<HPoint3> &vertexData,
  const Vector<HVector3> &vertexNormal,
  const Vector<HPoint2> &vertexUv,
  const int faceCount,
  const Vector<MeshIndices> &faceIndices) {
  // test if we have normal and uv data to write
  const bool hasNormal = (vertexNormal.size() == vertexData.size());
  const bool hasUv = (vertexUv.size() == vertexData.size());

  std::ofstream file(fileName);
  file << "ply\nformat ascii 1.0\nelement vertex " << std::to_string(vertexCount) << std::endl;
  file << "property float x\nproperty float y\nproperty float z\n";
  if (hasNormal) {
    file << "property float nx\nproperty float ny\nproperty float nz\n";
  }
  if (hasUv) {
    file << "property float s\nproperty float t\n";
  }
  file << "element face " << std::to_string(faceCount) << std::endl;
  file << "property list uchar int vertex_indices\nend_header\n";

  for (int i = 0; i < vertexCount; ++i) {
    file << std::to_string(vertexData[i].x()) << " " << std::to_string(vertexData[i].y()) << " "
         << std::to_string(vertexData[i].z());
    if (hasNormal) {
      file << " " << std::to_string(vertexNormal[i].x()) << " " << std::to_string(vertexNormal[i].y())
           << " " << std::to_string(vertexNormal[i].z());
    }
    if (hasUv) {
      file << " " << std::to_string(vertexUv[i].x()) << " " << std::to_string(vertexUv[i].y());
    }
    file << std::endl;
  }

  for (int i = 0; i < faceCount; ++i) {
    file << "3 " << faceIndices[i].a << " " << faceIndices[i].b << " " << faceIndices[i].c << "\n";
  }

  file.close();
}

}  // namespace c8
