// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Pooja Bansal (pooja@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":device-infos",
    "//c8/io:capnp-messages",
    "//reality/engine/api/device:info.capnp-cc",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x2f8f9459);

#include <gtest/gtest.h>

#include "c8/io/capnp-messages.h"
#include "reality/engine/api/device/info.capnp.h"
#include "c8/camera/device-infos.h"

using MutableDeviceInfo = c8::MutableRootMessage<c8::DeviceInfo>;

namespace c8 {

class DeviceInfoTest : public ::testing::Test {};

TEST_F(DeviceInfoTest, DeviceModel) {
  MutableDeviceInfo deviceInfoMessage;

  auto deviceInfoBuilder = deviceInfoMessage.builder();
  deviceInfoBuilder.setManufacturer("APPLE");
  deviceInfoBuilder.setModel("IPHONE5,3");
  EXPECT_EQ(DeviceInfos::APPLE_IPHONE_5C, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setManufacturer("Samsung");
  deviceInfoBuilder.setModel("SM-G935F");
  EXPECT_EQ(DeviceInfos::SAMSUNG_GALAXY_S7_EDGE, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setManufacturer("");
  deviceInfoBuilder.setModel("");
  EXPECT_EQ(DeviceInfos::NOT_SPECIFIED, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setManufacturer("Samsung");
  deviceInfoBuilder.setModel("");
  EXPECT_EQ(DeviceInfos::NOT_SPECIFIED, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setManufacturer("HUAWEI");
  deviceInfoBuilder.setModel("STF-AL10");
  EXPECT_EQ(DeviceInfos::HUAWEI_HONOR_9, DeviceInfos::getDeviceModel(deviceInfoBuilder));
}

TEST_F(DeviceInfoTest, GetSamsungModelBase) {
  MutableDeviceInfo deviceInfoMessage;

  auto deviceInfoBuilder = deviceInfoMessage.builder();
  deviceInfoBuilder.setManufacturer("Samsung");
  deviceInfoBuilder.setModel("SM-G991U");
  EXPECT_EQ(DeviceInfos::SAMSUNG_GALAXY_S21_5G, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setManufacturer("Samsung");
  deviceInfoBuilder.setModel("sm-G360123456789");
  EXPECT_EQ(DeviceInfos::SAMSUNG_GALAXY_CORE_PRIME, DeviceInfos::getDeviceModel(deviceInfoBuilder));
}

TEST_F(DeviceInfoTest, GetHuaweiModelBase) {
  MutableDeviceInfo deviceInfoMessage;

  auto deviceInfoBuilder = deviceInfoMessage.builder();
  deviceInfoBuilder.setManufacturer("Huawei");
  deviceInfoBuilder.setModel("frd-l09");
  EXPECT_EQ(DeviceInfos::HUAWEI_HONOR_8, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setManufacturer("Huawei");
  deviceInfoBuilder.setModel("ALP-123456789");
  EXPECT_EQ(DeviceInfos::HUAWEI_MATE_10, DeviceInfos::getDeviceModel(deviceInfoBuilder));
}

TEST_F(DeviceInfoTest, GetNoManufacturer) {
  MutableDeviceInfo deviceInfoMessage;
  auto deviceInfoBuilder = deviceInfoMessage.builder();

  deviceInfoBuilder.setModel("Pixel 6 Pro");
  EXPECT_EQ(DeviceInfos::GOOGLE_PIXEL6_PRO, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setModel("BE2026");
  EXPECT_EQ(DeviceInfos::ONEPLUS_NORD10_5G, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setModel("ANE-LX1");
  EXPECT_EQ(DeviceInfos::HUAWEI_P20_LITE, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setModel("ANELX1");
  EXPECT_EQ(DeviceInfos::NOT_SPECIFIED, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setModel("EML-L29");
  EXPECT_EQ(DeviceInfos::HUAWEI_P20, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setModel("IPHONE5,3");
  EXPECT_EQ(DeviceInfos::APPLE_IPHONE_5C, DeviceInfos::getDeviceModel(deviceInfoBuilder));

  deviceInfoBuilder.setModel("PB2-690Y");
  EXPECT_EQ(DeviceInfos::LENOVO_PB2_690Y, DeviceInfos::getDeviceModel(deviceInfoBuilder));
}

TEST_F(DeviceInfoTest, GetSamsungNoManufacturer) {
  MutableDeviceInfo deviceInfoMessage;
  EXPECT_EQ("SM-S906", getModel("", "SM-S906U1"));

  auto deviceInfoBuilder = deviceInfoMessage.builder();
  deviceInfoBuilder.setModel("SM-S906U1");
  EXPECT_EQ(DeviceInfos::SAMSUNG_GALAXY_S22_PLUS, DeviceInfos::getDeviceModel(deviceInfoBuilder));
}

}  // namespace c8
