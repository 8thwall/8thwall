// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Akul Gupta (akulgupta@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8/io:file-io",
    "//c8:c8-log",
    "//c8:json-conversions",
    "@com_google_googletest//:gtest_main",
    "@json",
  };
}
cc_end(0x5526a5bb);

#include <nlohmann/json.hpp>

#include "c8/c8-log.h"
#include "c8/io/file-io.h"
#include "c8/json-conversions.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

using testing::Eq;
using testing::Pointwise;

namespace {
const HMatrix TR_MAT =
  HMatrixGen::translation(1.f, 2.f, 3.f) * HMatrixGen::rotationD(20.f, 30.f, 40.f);

const auto TR_MAT_STR = R""""(
  {
  "rotation": {
    "w": 0.8915935158729553,
    "x": 0.16817930340766907,
    "y": 0.2522689700126648,
    "z": 0.33635860681533813
  },
  "translation": {
    "x": 1.0,
    "y": 2.0,
    "z": 3.0
  }
})"""";

const auto NULL_JSON_STR = R""""(
  {
  "data": "{\"requestIdentifier\":\"a2582707ccec3dd00d9639c9417a74a2\", \"statusCode\":\"STATUS_CODE_FAIL\", \"clientTrackingToNodeTransform\":{\"translation\":{\"x\":0, \"y\":0, \"z\":0}, \"rotation\":{\"x\":0, \"y\":0, \"z\":0, \"w\":1}, \"scale\":1}, \"nodeIdentifier\":\"\", \"confidence\":0, \"sessionState\":null}",
  "debugExtrinsicThatGeneratedResponse": "{\"rotation\":{\"w\":null,\"x\":null,\"y\":null,\"z\":null},\"translation\":{\"x\":null,\"y\":null,\"z\":null}}",
  "debugRequest": "{\"coordinateSpaces\":[],\"frames\":[{\"cameraImage\":{\"cX\":359.5,\"cY\":269.5,\"fX\":550.3494262695313,\"fY\":550.3494262695313,\"skew\":0},\"locationData\":{\"horizontalAccuracy\":3.535533905029297,\"latitude\":37.426151275634766,\"longitude\":-122.14118957519531},\"timestamp\":\"576444192\"}],\"imageBytesHash\":2071704654}",
  "nodeInCameraInNar": "{\"rotation\":{\"w\":null,\"x\":null,\"y\":null,\"z\":null},\"translation\":{\"x\":null,\"y\":null,\"z\":null}}",
  "status": 200,
  "timestamp": "2023-01-06-16-45-09"
})"""";

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return Pointwise(AreWithin(0.0001), matrix.data());
}

}  // namespace

class JsonConversionsTest : public ::testing::Test {};

TEST_F(JsonConversionsTest, JsonStringToMat) {
  EXPECT_THAT(TR_MAT.data(), equalsMatrix(jsonStringToMat(TR_MAT_STR)));
}

TEST_F(JsonConversionsTest, JsonToMat) {
  auto json = nlohmann::json::parse(TR_MAT_STR);
  EXPECT_THAT(TR_MAT.data(), equalsMatrix(jsonToMat(json)));
}

TEST_F(JsonConversionsTest, MatToJsonToMat) {
  auto json = matToJson(TR_MAT);
  auto mat = jsonStringToMat(json.dump());
  EXPECT_THAT(TR_MAT.data(), equalsMatrix(mat));
}

TEST_F(JsonConversionsTest, IsAnyNull) {
  EXPECT_FALSE(isAnyNull({{1, 2, 3}}));
  EXPECT_FALSE(isAnyNull({{"1", 2}, {"3", 5}}));

  EXPECT_TRUE(isAnyNull({{}}));
  EXPECT_TRUE(isAnyNull({nullptr}));
  EXPECT_TRUE(isAnyNull({{"1", nullptr}, {"3", 5}}));
  EXPECT_TRUE(isAnyNull({{1, nullptr}, {3, 5}}));
  EXPECT_TRUE(isAnyNull({{1, 2, 3, nullptr}}));
  auto json = nlohmann::json::parse(NULL_JSON_STR);
  auto jsonData = nlohmann::json::parse(json["data"].get<String>());
  EXPECT_TRUE(isAnyNull(jsonData));
}

}  // namespace c8
