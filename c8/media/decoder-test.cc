// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":decoder",
    ":demuxer",
    ":media-status",
    "//c8/media/codec:codec-api",
    "@com_google_googletest//:gtest_main",
    "@json//:json",
  };
}
cc_end(0x99e5acaf);

#include "c8/media/codec/codec-api.h"
#include "c8/media/decoder.h"
#include "c8/media/demuxer.h"
#include "c8/media/media-status.h"

#include <nlohmann/json.hpp>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

using ::testing::Ref;
using ::testing::Return;

class Demuxer;

class MockDemuxer : public Demuxer {};

class MockDecoderApi : public DecoderApi {
public:
  MOCK_METHOD1(start, MediaStatus(Demuxer *demuxer));

  MOCK_METHOD4(
    decode,
    MediaStatus(
      const nlohmann::json &sampleConfig,
      const uint8_t **data,
      size_t *byteSize,
      nlohmann::json *sampleMetadata));

  MOCK_METHOD0(finish, MediaStatus());
};

class DecoderTest : public ::testing::Test {
public:
  MockDecoderApi *mockApi() {
    MockDecoderApi *api = new MockDecoderApi();
    decoder = Decoder(api);
    return api;
  }

protected:
  Decoder decoder;
};

TEST_F(DecoderTest, startTest) {
  MockDemuxer demuxer;

  // Start returns an error if no decoder api is registered.
  EXPECT_FALSE(decoder.isValid());
  auto result = decoder.start(&demuxer);
  EXPECT_EQ(1, result.code());

  MockDecoderApi *api = mockApi();
  EXPECT_TRUE(decoder.isValid());

  // Calling start will return the correct status.
  EXPECT_CALL(*api, start(&demuxer))
    .WillOnce(Return(MediaStatus{0, ""}))
    .WillOnce(Return(MediaStatus{3, "Error"}));

  result = decoder.start(&demuxer);
  EXPECT_EQ(0, result.code());

  result = decoder.start(&demuxer);
  EXPECT_EQ(3, result.code());
}

TEST_F(DecoderTest, decodeTest) {
  nlohmann::json sampleConfig;
  const uint8_t *data;
  size_t byteSize;
  nlohmann::json sampleMetadata;

  // Start returns an error if no decoder api is registered.
  EXPECT_FALSE(decoder.isValid());
  auto result = decoder.decode(sampleConfig, &data, &byteSize, &sampleMetadata);
  EXPECT_EQ(1, result.code());

  MockDecoderApi *api = mockApi();
  EXPECT_TRUE(decoder.isValid());

  // Calling start will return the correct status.
  EXPECT_CALL(*api, decode(Ref(sampleConfig), &data, &byteSize, &sampleMetadata))
    .WillOnce(Return(MediaStatus{0, ""}))
    .WillOnce(Return(MediaStatus{3, "Error"}));

  result = decoder.decode(sampleConfig, &data, &byteSize, &sampleMetadata);
  EXPECT_EQ(0, result.code());

  result = decoder.decode(sampleConfig, &data, &byteSize, &sampleMetadata);
  EXPECT_EQ(3, result.code());
}

TEST_F(DecoderTest, finishTest) {
  // Start returns an error if no decoder api is registered.
  EXPECT_FALSE(decoder.isValid());
  auto result = decoder.finish();
  EXPECT_EQ(1, result.code());

  MockDecoderApi *api = mockApi();
  EXPECT_TRUE(decoder.isValid());

  // Calling finish will return the correct status.
  EXPECT_CALL(*api, finish())
    .WillOnce(Return(MediaStatus{0, ""}))
    .WillOnce(Return(MediaStatus{3, "Error"}));

  result = decoder.finish();
  EXPECT_EQ(0, result.code());

  result = decoder.finish();
  EXPECT_EQ(3, result.code());
}

}  // namespace c8
