// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Akul Gupta (akulgupta@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":decode-mesh",
    "//c8/io:file-io",
    "@com_google_googletest//:gtest_main",
    "@json",
  };
  data = {
    "//c8/geometry/testdata:mock-mesh-data",
  };
}
cc_end(0x48fd5c74);

#include <nlohmann/json.hpp>

#include "c8/geometry/decode-mesh.h"
#include "c8/io/file-io.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class DecodeMeshTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    // Use synchronous version of mesh decoding for this test.
    forceDecodeMeshBlockingCpp();
  }
};

TEST_F(DecodeMeshTest, DecodeMeshAndRunCallback) {
  auto getMeshResponsePath = "c8/geometry/testdata/mock-mesh-data.json";
  auto getMeshResponse = readFile(getMeshResponsePath);

  auto data = reinterpret_cast<char *>(getMeshResponse.data());
  auto size = getMeshResponse.size();
  auto parsedResponse = nlohmann::json::parse(data, data + size, nullptr, false);

  auto success = false;
  decodeMesh(
    "meshId",
    parsedResponse["meshData"].get<String>(),
    [&success](const String &nodeId, const MeshGeometry &mesh) { success = true; });

  EXPECT_TRUE(success);
}

}  // namespace c8
