// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":model-3d-io",
    "//c8/geometry:mesh",
    "//c8:string",
    "//c8/string:format",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x2aaac516);

#include <cstdio>
#include <cstdlib>

#include "c8/geometry/mesh.h"
#include "c8/io/model-3d-io.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "gtest/gtest.h"

namespace c8 {

class Model3dIOTest : public ::testing::Test {};

static constexpr bool toSavePLY = false;
static constexpr char MESH_PLY_OUTPUT_PATH[] = "/tmp/mesh.ply";

TEST_F(Model3dIOTest, writeToPLY) {
  if (!toSavePLY) {
    return;
  }
  // write a triangle to PLY file
  String fileName(MESH_PLY_OUTPUT_PATH);
  Vector<HPoint3> vertices = {
    {-0.5f, 0.0f, 0.0f},
    {0.5f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
  };
  Vector<MeshIndices> meshInd = {
    {0, 1, 2},
  };
  Vector<HVector3> normals;
  Vector<HPoint2> uvs;
  computeVertexNormals(vertices, meshInd, &normals);
  writeModelToPLY(fileName, vertices.size(), vertices, normals, uvs, meshInd.size(), meshInd);
}

}  // namespace c8
