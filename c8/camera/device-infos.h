// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Pooja Bansal (pooja@8thwall.com)
//
// This is a class that contains a map of mobile device make and models.
// It contains an enum for different cellphones and returns the enum value when provided
// with a manufacturer and model. This class only contains static methods and cannot be
// instantiated.

#pragma once

#include "c8/string.h"
#include "reality/engine/api/device/info.capnp.h"

namespace c8 {

// Gets the device model. Strips off appended data for Samsung and Huawei devices.
// @param manufacturerUpperCase the manufacturer in upper case
// @param model the model as found in DeviceInfo
String getModel(const String &manufacturerUpperCase, const String &model);

class DeviceInfos {
public:
  // Disallow instantiation
  DeviceInfos() = delete;

  // Disallow move constructors
  DeviceInfos(DeviceInfos &&) = delete;
  DeviceInfos &operator=(DeviceInfos &&) = default;

  // Disallow copying.
  DeviceInfos(const DeviceInfos &) = delete;
  DeviceInfo &operator=(const DeviceInfos &) = delete;

  // Member attributes.
  enum DeviceModel {
    NOT_SPECIFIED,
    APPLE_IPAD_2,
    APPLE_IPAD_G3,                // Calibrated
    APPLE_IPAD_G4,                //
    APPLE_IPAD_G5,                // Calibrated
    APPLE_IPAD_G6,                // Calibrated
    APPLE_IPAD_AIR,               //
    APPLE_IPAD_AIR2,              //
    APPLE_IPAD_PRO10,             //
    APPLE_IPAD_PRO12,             //
    APPLE_IPAD_PRO12_G2,          // Calibrated
    APPLE_IPAD_PRO9,              //
    APPLE_IPHONE_5,               //
    APPLE_IPHONE_5C,              // Calibrated
    APPLE_IPHONE_5S,              // Calibrated
    APPLE_IPHONE_6,               // Calibrated
    APPLE_IPHONE_6PLUS,           // Calibrated
    APPLE_IPHONE_6S,              // Calibrated
    APPLE_IPHONE_6SPLUS,          //
    APPLE_IPHONE_SE,              // Calibrated
    APPLE_IPHONE_7,               // Calibrated
    APPLE_IPHONE_7PLUS,           // Calibrated
    APPLE_IPHONE_8,               // Calibrated
    APPLE_IPHONE_8PLUS,           // Calibrated
    APPLE_IPHONE_X,               // Calibrated
    APPLE_IPHONE_XR,              //
    APPLE_IPHONE_XS,              // Calibrated
    APPLE_IPHONE_XS_MAX,          // Calibrated
    APPLE_IPHONE_11,              // Calibrated
    APPLE_IPHONE_11_PRO,          // Calibrated, TODO(nb): add uncalibrated models.
    APPLE_IPHONE_12_MINI,         // Calibrated
    APPLE_IPHONE_12,              // Calibrated
    APPLE_IPHONE_12_PRO,          // Calibrated
    APPLE_IPHONE_12_PRO_MAX,      // Calibrated
    APPLE_IPHONE_13,              // Calibrated
    APPLE_IPHONE_13_PRO,          // Calibrated
    APPLE_IPHONE_13_PRO_MAX,      // Calibrated
    APPLE_IPHONE_13_MINI,         // Calibrated
    APPLE_IPHONE_14,              // Uncalibrated
    APPLE_IPHONE_14_PLUS,         // Calibrated
    APPLE_IPHONE_14_PRO,          // Calibrated
    APPLE_IPHONE_14_PRO_MAX,      // Calibrated
    APPLE_IPHONE_15,              // Uncalibrated
    APPLE_IPHONE_15_PLUS,         // Uncalibrated
    APPLE_IPHONE_15_PRO,          // Uncalibrated
    APPLE_IPHONE_15_PRO_MAX,      // Uncalibrated
    ASUS_ZENPHONE2,               // Calibrated
    EPSON_MOVERIO_BT300,          // Calibrated
    ESSENTIAL_PH_1,               // Calibrated
    GOOGLE_PIXEL,                 // Calibrated
    GOOGLE_PIXEL_XL,              // Calibrated
    GOOGLE_PIXEL2,                // Calibrated
    GOOGLE_PIXEL2_XL,             // Calibrated
    GOOGLE_PIXEL3,                // Calibrated
    GOOGLE_PIXEL3_XL,             // Calibrated
    GOOGLE_PIXEL4,                // Calibrated
    GOOGLE_PIXEL4_XL,             // Calibrated
    GOOGLE_PIXEL5,                // Calibrated
    GOOGLE_PIXEL5A,               // Calibrated
    GOOGLE_PIXEL5_XL,             // Uncalibrated
    GOOGLE_PIXEL6,                // Calibrated
    GOOGLE_PIXEL6_PRO,            // Calibrated
    GOOGLE_PIXEL7,                // Calibrated
    GOOGLE_PIXEL7_PRO,            // Calibrated
    HIP_H450R,                    // Calibrated
    HTC_ONE_M8,                   // Calibrated
    HUAWEI_HONOR_8,               // Calibrated
    HUAWEI_HONOR_9,               // Calibrated
    HUAWEI_MATE_9,                // Calibrated
    HUAWEI_MATE_10,               // Calibrated
    HUAWEI_NEXUS_6P,              // Calibrated
    HUAWEI_P20,                   // Calibrated
    HUAWEI_P20_LITE,              // Calibrated
    HUAWEI_P30,                   // Calibrated
    LENOVO_K6_NOTE,               // Calibrated
    LENOVO_PB2_690Y,              // Calibrated
    LGE_NEXUS_4,                  // Calibrated
    LGE_NEXUS_5,                  // Calibrated
    LGE_NEXUS_5X,                 // Calibrated
    MOTOROLA_EDGE_PLUS,           // Calibrated
    MOTOROLA_NEXUS_6,             // Calibrated
    MOTOROLA_MOTOG3,              // Calibrated
    MOTOROLA_RAZR_PLUS,           // Calibrated
    ODG_R7,                       // Calibrated
    ONEPLUS_8T,                   // Calibrated
    ONEPLUS_9_5G,                 // Calibrated
    ONEPLUS_9_PRO,                // Calibrated
    ONEPLUS_NORD10_5G,            // Calibrated
    SAMSUNG_GALAXY_CORE_PRIME,    // Calibrated
    SAMSUNG_GALAXY_J7,            // Calibrated
    SAMSUNG_GALAXY_S5,            // Calibrated
    SAMSUNG_GALAXY_S6,            // Calibrated
    SAMSUNG_GALAXY_S6_EDGE,       // Calibrated
    SAMSUNG_GALAXY_S7,            // Calibrated
    SAMSUNG_GALAXY_S7_EDGE,       // Calibrated
    SAMSUNG_GALAXY_S8,            // Calibrated
    SAMSUNG_GALAXY_S8_PLUS,       // Calibrated
    SAMSUNG_GALAXY_S9,            // Calibrated
    SAMSUNG_GALAXY_S9_PLUS,       // Calibrated
    SAMSUNG_GALAXY_S10,           // Calibrated
    SAMSUNG_GALAXY_S10e,          // Calibrated
    SAMSUNG_GALAXY_S10_PLUS,      // Calibrated
    SAMSUNG_GALAXY_S21_5G,        // Calibrated
    SAMSUNG_GALAXY_S21_ULTRA_5G,  // Calibrated
    SAMSUNG_GALAXY_S22_PLUS,      // Calibrated
    SAMSUNG_ZFOLD_3,              // Calibrated
    SAMSUNG_ZFLIP_3,              // Calibrated
    SAMSUNG_GALAXY_NOTE8,         // Calibrated
    SAMSUNG_GALAXY_NOTE9,         // Calibrated
    SAMSUNG_GALAXY_NOTE10_PLUS,   // Calibrated
    SAMSUNG_GALAXY_TAB_A,         // Calibrated
    XIAOMI_MIX_2,                 // Calibrated
    XIAOMI_REDMI_4X               // Calibrated
  };

  // Member functions.
  static DeviceInfos::DeviceModel getDeviceModel(const DeviceInfo::Reader &deviceInfoReader);

private:
};

struct ModelInfo {
  String name;
  uint32_t singleCore;
  uint32_t multiCore;
};

ModelInfo getModelInfo(const DeviceInfos::DeviceModel &model);

}  // namespace c8
