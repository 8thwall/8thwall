// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Pooja Bansal (pooja@8thwall.com)
#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "device-infos.h",
  };
  deps = {
    "//c8:exceptions",
    "//c8:map",
    "//c8:set",
    "//c8:string",
    "//c8/string:format",
    "//reality/engine/api/device:info.capnp-cc",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x08ff163a);

#include <algorithm>
#include <cctype>

#include "c8/exceptions.h"
#include "c8/map.h"
#include "c8/set.h"
#include "c8/string/format.h"
#include "c8/camera/device-infos.h"

namespace c8 {

namespace {

String getSamsungModelBase(const String &modelNumber) { return modelNumber.substr(0, 7); }

String getHuaweiModelBase(const String &modelNumber) {
  auto p = modelNumber.find('-');
  if (p == std::string::npos) {
    return modelNumber;
  }
  return modelNumber.substr(0, p);
}

TreeSet<String>& getHuaweiModelPrefixes() {
  static TreeSet<String> huaweiModelPrefixes;
  return huaweiModelPrefixes;
}

}  // namespace

String getModel(const String &manufacturerUpperCase, const String &model) {
  // Samsung phones have the same base number for handsets of a particular model. Strip off the
  // appended data and use only the base number to identify the model.
  if (manufacturerUpperCase == "SAMSUNG") {
    return getSamsungModelBase(model);
  } else if (manufacturerUpperCase == "HUAWEI") {
    return getHuaweiModelBase(model);
  } else if (manufacturerUpperCase.empty()) {
    if (model.starts_with("SM-")) {
      return getSamsungModelBase(model);
    }

    const auto &huaweiModelPrefixes = getHuaweiModelPrefixes();
    if (
      (huaweiModelPrefixes.find(model.substr(0, 3)) != huaweiModelPrefixes.end()) &&
      (model.length() > 3 && model[3] == '-')
    ) {
      return getHuaweiModelBase(model);
    }
  }
  return model;
}

TreeMap<String, DeviceInfos::DeviceModel> buildModelMap(
  const TreeMap<String, TreeMap<String, DeviceInfos::DeviceModel>> &deviceMap) {
  TreeMap<String, DeviceInfos::DeviceModel> modelMap;
  for (const auto &[manufacturerName, manufacturerModelMap] : deviceMap) {
    for (const auto &[modelName, deviceEnum] : manufacturerModelMap) {
      // NOTE(dat): Can't static_assert unfortunately. This should fail on Omniscope though.
      if (manufacturerModelMap.find(modelName) == manufacturerModelMap.end()) {
        C8_THROW("[device-infos] Phone model collision");
      }
      modelMap.insert({modelName, deviceEnum});
    }
  }
  return modelMap;
}

DeviceInfos::DeviceModel DeviceInfos::getDeviceModel(const DeviceInfo::Reader &deviceInfoReader) {

  // Map of the device manufacturer and model to the enum.
  const static TreeMap<String, TreeMap<String, DeviceInfos::DeviceModel>> deviceMap{
    {"APPLE",
     {
       {"IPAD2,1", APPLE_IPAD_2},
       {"IPAD2,2", APPLE_IPAD_2},
       {"IPAD2,3", APPLE_IPAD_2},
       {"IPAD2,4", APPLE_IPAD_2},
       {"IPAD3,1", APPLE_IPAD_G3},
       {"IPAD3,2", APPLE_IPAD_G3},
       {"IPAD3,3", APPLE_IPAD_G3},
       {"IPAD3,4", APPLE_IPAD_G4},
       {"IPAD3,5", APPLE_IPAD_G4},
       {"IPAD3,6", APPLE_IPAD_G4},
       {"IPAD4,1", APPLE_IPAD_AIR},
       {"IPAD4,2", APPLE_IPAD_AIR},
       {"IPAD4,3", APPLE_IPAD_AIR},
       {"IPAD5,3", APPLE_IPAD_AIR2},
       {"IPAD5,4", APPLE_IPAD_AIR2},
       {"IPAD6,3", APPLE_IPAD_PRO9},
       {"IPAD6,4", APPLE_IPAD_PRO9},
       {"IPAD6,7", APPLE_IPAD_PRO12},
       {"IPAD6,8", APPLE_IPAD_PRO12},
       {"IPAD6,11", APPLE_IPAD_G5},
       {"IPAD6,12", APPLE_IPAD_G5},
       {"IPAD7,1", APPLE_IPAD_PRO12_G2},
       {"IPAD7,2", APPLE_IPAD_PRO12_G2},
       {"IPAD7,3", APPLE_IPAD_PRO10},
       {"IPAD7,4", APPLE_IPAD_PRO10},
       {"IPAD7,5", APPLE_IPAD_G6},
       {"IPHONE5,1", APPLE_IPHONE_5},
       {"IPHONE5,2", APPLE_IPHONE_5},
       {"IPHONE5,3", APPLE_IPHONE_5C},
       {"IPHONE5,4", APPLE_IPHONE_5C},
       {"IPHONE6,1", APPLE_IPHONE_5S},
       {"IPHONE6,2", APPLE_IPHONE_5S},
       {"IPHONE7,2", APPLE_IPHONE_6},
       {"IPHONE7,1", APPLE_IPHONE_6PLUS},
       {"IPHONE8,1", APPLE_IPHONE_6S},
       {"IPHONE8,2", APPLE_IPHONE_6SPLUS},
       {"IPHONE8,4", APPLE_IPHONE_SE},
       {"IPHONE9,1", APPLE_IPHONE_7},
       {"IPHONE9,3", APPLE_IPHONE_7},
       {"IPHONE9,2", APPLE_IPHONE_7PLUS},
       {"IPHONE9,4", APPLE_IPHONE_7PLUS},
       {"IPHONE10,1", APPLE_IPHONE_8},
       {"IPHONE10,4", APPLE_IPHONE_8},
       {"IPHONE10,2", APPLE_IPHONE_8PLUS},
       {"IPHONE10,5", APPLE_IPHONE_8PLUS},
       {"IPHONE10,3", APPLE_IPHONE_X},
       {"IPHONE10,6", APPLE_IPHONE_X},
       {"IPHONE11,2", APPLE_IPHONE_XS},
       {"IPHONE11,4", APPLE_IPHONE_XS_MAX},
       {"IPHONE11,6", APPLE_IPHONE_XS_MAX},
       {"IPHONE11,8", APPLE_IPHONE_XR},
       {"IPHONE12,2", APPLE_IPHONE_11},
       {"IPHONE12,3", APPLE_IPHONE_11_PRO},
       {"IPHONE13,1", APPLE_IPHONE_12_MINI},
       {"IPHONE13,2", APPLE_IPHONE_12},
       {"IPHONE13,3", APPLE_IPHONE_12_PRO},
       {"IPHONE13,4", APPLE_IPHONE_12_PRO_MAX},
       {"IPHONE14,1", APPLE_IPHONE_13},
       {"IPHONE14,2", APPLE_IPHONE_13_PRO},
       {"IPHONE14,3", APPLE_IPHONE_13_PRO_MAX},
       {"IPHONE14,4", APPLE_IPHONE_13_MINI},
       {"IPHONE15,1", APPLE_IPHONE_14},
       {"IPHONE15,2", APPLE_IPHONE_14_PLUS},
       {"IPHONE15,3", APPLE_IPHONE_14_PRO},
       {"IPHONE15,4", APPLE_IPHONE_14_PRO_MAX},
       {"IPHONE16,1", APPLE_IPHONE_15},
       {"IPHONE16,2", APPLE_IPHONE_15_PLUS},
       {"IPHONE16,3", APPLE_IPHONE_15_PRO},
       {"IPHONE16,4", APPLE_IPHONE_15_PRO_MAX},
       // Web fallbacks for inexact matches:
       {"IPHONE 12/12PRO", APPLE_IPHONE_12},
       {"IPHONE 13/13PRO", APPLE_IPHONE_13},
       {"IPHONE 14/14PRO", APPLE_IPHONE_14},
       {"IPHONE XR/11", APPLE_IPHONE_XR},
       {"IPHONE XSMAX/11PROMAX", APPLE_IPHONE_XS_MAX},
       {"IPHONE X/XS/11PRO", APPLE_IPHONE_11_PRO},
       {"IPHONE 6+/6S+/7+/8+", APPLE_IPHONE_7PLUS},
       {"IPHONE 6/6S/7/8", APPLE_IPHONE_7},
       {"IPHONE 5/5S/5C/SE", APPLE_IPHONE_SE},
       {"IPHONE 4/4S", APPLE_IPHONE_5C},
       {"IPHONE 2G/3G/3G", APPLE_IPHONE_5C},
       {"IPHONE", APPLE_IPHONE_X},
       {"IPAD/IPAD2/IPAD MINI", APPLE_IPAD_G3},
       {"IPAD3/4/5/6/MINI2/MINI3/MINI4/AIR/AIR2", APPLE_IPAD_G5},
       {"IPADPRO12/IPADPRO12G2", APPLE_IPAD_PRO12_G2},
       {"IPAD", APPLE_IPAD_G6},
     }},
    {"ASUS", {{"ASUS_Z00AD", ASUS_ZENPHONE2}}},
    {"EPSON", {{"EMBT3C", EPSON_MOVERIO_BT300}}},
    {"ESSENTIAL PRODUCTS", {{"PH-1", ESSENTIAL_PH_1}}},
    {"GOOGLE",
     {
       {"PIXEL", GOOGLE_PIXEL},
       {"PIXEL XL", GOOGLE_PIXEL_XL},
       {"PIXEL 2", GOOGLE_PIXEL2},
       {"PIXEL 2 XL", GOOGLE_PIXEL2_XL},
       {"PIXEL 3", GOOGLE_PIXEL3},
       {"PIXEL 3 XL", GOOGLE_PIXEL3_XL},
       {"PIXEL 4", GOOGLE_PIXEL4},
       {"PIXEL 4 XL", GOOGLE_PIXEL4_XL},
       {"PIXEL 5", GOOGLE_PIXEL5},
       {"PIXEL 5A", GOOGLE_PIXEL5A},
       {"PIXEL 5 XL", GOOGLE_PIXEL5_XL},
       {"PIXEL 6", GOOGLE_PIXEL6},
       {"PIXEL 6 PRO", GOOGLE_PIXEL6_PRO},
       {"PIXEL 7", GOOGLE_PIXEL7},
       {"PIXEL 7 PRO", GOOGLE_PIXEL7_PRO},
     }},
    {"HIP", {{"H450R", HIP_H450R}}},
    {"HTC", {{"HTC ONE_M8", HTC_ONE_M8}}},
    {"HUAWEI",
     {{"FRD", HUAWEI_HONOR_8},
      {"STF", HUAWEI_HONOR_9},
      {"MHA", HUAWEI_MATE_9},
      {"ALP", HUAWEI_MATE_10},
      {"NEXUS 6P", HUAWEI_NEXUS_6P},
      {"EML", HUAWEI_P20},
      {"ANE", HUAWEI_P20_LITE},
      {"VOG", HUAWEI_P30}}},
    {"LENOVO",
     {
       {"LENOVO K53A48", LENOVO_K6_NOTE},
       {"PB2-690Y", LENOVO_PB2_690Y},
     }},
    {"LGE", {{"NEXUS 4", LGE_NEXUS_4}, {"NEXUS 5", LGE_NEXUS_5}, {"NEXUS 5X", LGE_NEXUS_5X}}},
    {"MOTOROLA",
     {{"NEXUS 6", MOTOROLA_NEXUS_6}, {"MOTOG3", MOTOROLA_MOTOG3}, {"EDGE", MOTOROLA_EDGE_PLUS},
     {"RAZR+", MOTOROLA_RAZR_PLUS}}},
    {"OSTERHOUT_DESIGN_GROUP", {{"R7-W", ODG_R7}}},
    {"ONEPLUS",
     {
       {"KB2005", ONEPLUS_8T},
     }},
    {"",
     {
       // OnePlus manufacturers are not being picked up by ua-parser-js
       {"BE2026", ONEPLUS_NORD10_5G},
       {"LE2115", ONEPLUS_9_5G},
       {"LE2125", ONEPLUS_9_PRO},
     }},
    {"SAMSUNG",
     {
       {"SM-G360", SAMSUNG_GALAXY_CORE_PRIME},  {"SM-J710", SAMSUNG_GALAXY_J7},
       {"SM-G900", SAMSUNG_GALAXY_S5},          {"SM-G920", SAMSUNG_GALAXY_S6},
       {"SM-G925", SAMSUNG_GALAXY_S6_EDGE},     {"SM-G930", SAMSUNG_GALAXY_S7},
       {"SM-G935", SAMSUNG_GALAXY_S7_EDGE},     {"SM-G950", SAMSUNG_GALAXY_S8},
       {"SM-G955", SAMSUNG_GALAXY_S8_PLUS},     {"SM-G960", SAMSUNG_GALAXY_S9},
       {"SM-G965", SAMSUNG_GALAXY_S9_PLUS},     {"SM-G975", SAMSUNG_GALAXY_S10_PLUS},
       {"SM-G973", SAMSUNG_GALAXY_S10},         {"SM-G970", SAMSUNG_GALAXY_S10e},
       {"SM-N950", SAMSUNG_GALAXY_NOTE8},       {"SM-N960", SAMSUNG_GALAXY_NOTE9},
       {"SM-N975", SAMSUNG_GALAXY_NOTE10_PLUS}, {"SM-T580", SAMSUNG_GALAXY_TAB_A},
       {"SM-G991", SAMSUNG_GALAXY_S21_5G},      {"SM-G998", SAMSUNG_GALAXY_S21_ULTRA_5G},
       {"SM-S906", SAMSUNG_GALAXY_S22_PLUS},    {"SM-F926", SAMSUNG_ZFOLD_3},
       {"SM-F711", SAMSUNG_ZFLIP_3},
     }},
    {"XIAOMI",
     {
       {"MIX 2", XIAOMI_MIX_2},
       {"REDMI 4X", XIAOMI_REDMI_4X},
     }},
  };

  // Built from deviceMap for looking up only by model
  static TreeMap<String, DeviceInfos::DeviceModel> modelMap = buildModelMap(deviceMap);

  // Build the list of Huawei prefixes from deviceMap
  auto &huaweiModelPrefixes = getHuaweiModelPrefixes();
  if (huaweiModelPrefixes.size() == 0) {
    for (const auto& [prefix, model] : deviceMap.at("HUAWEI")) {
      huaweiModelPrefixes.insert(prefix);
    }
  }

  String manufacturer = toUpperCase(deviceInfoReader.getManufacturer());
  String model = toUpperCase(getModel(manufacturer, deviceInfoReader.getModel()));

  if (manufacturer.empty()) {
    auto itModel = modelMap.find(model);
    if (itModel != modelMap.end()) {
      return itModel->second;
    }
  } else {
    // If make and model exist, return the corresponding enum, else return not specified.
    auto it_manufacturer = deviceMap.find(manufacturer);
    if (
      it_manufacturer != deviceMap.end()
      && it_manufacturer->second.find(model) != it_manufacturer->second.end()) {
      return it_manufacturer->second.find(model)->second;
    }
  }
  return NOT_SPECIFIED;
}

#define MODEL_INFO(model, name, single, multi) case model: return {name, single, multi};

ModelInfo getModelInfo(const DeviceInfos::DeviceModel &model) {
  // Name should be close to https://browser.geekbench.com/mobile-benchmarks.json
  switch (model) {
    MODEL_INFO(DeviceInfos::DeviceModel::NOT_SPECIFIED, "Not Specified", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_2, "iPad 2", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_G3, "iPad 3rd generation", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_G4, "iPad 4th generation", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_G5, "iPad 5th generation", 628, 946)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_G6, "iPad 6th generation", 842, 1203)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_AIR, "iPad Air", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_AIR2, "iPad Air 2", 436, 915)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_PRO10, "iPad Pro 10.5-inch", 941, 2201)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_PRO12, "iPad Pro 12.9-inch", 770, 1311)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_PRO12_G2, "iPad Pro 12.9-inch 2nd generation", 884, 2162)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPAD_PRO9, "iPad Pro 9.7-inch", 727, 1059)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_5, "iPhone 5", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_5C, "iPhone 5C", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_5S, "iPhone 5S", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_6, "iPhone 6", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_6PLUS, "iPhone 6 Plus", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_6S, "iPhone 6s", 600, 850)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_6SPLUS, "iPhone 6s Plus", 600, 832)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_SE, "iPhone SE", 612, 901)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_7, "iPhone 7", 777, 924)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_7PLUS, "iPhone 7 Plus", 801, 1103)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_8, "iPhone 8", 1018, 1575)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_8PLUS, "iPhone 8 Plus", 1049, 2131)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_X, "iPhone X", 1042, 1910)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_XR, "iPhone XR", 1231, 2225)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_XS, "iPhone XS", 1271, 2540)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_XS_MAX, "iPhone XS Max", 1270, 2555)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_11, "iPhone 11", 1666, 3423)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_11_PRO, "iPhone 11 Pro", 1699, 3670)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_12_MINI, "iPhone 12 Mini", 1987, 4351)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_12, "iPhone 12", 1986, 4332)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_12_PRO, "iPhone 12 Pro", 1978, 4344)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_12_PRO_MAX, "iPhone 12 Pro Max", 2044, 4648)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_13, "iPhone 13", 2179, 5125)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_13_PRO, "iPhone 13 Pro", 2253, 5404)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_13_PRO_MAX, "iPhone 13 Pro Max", 2263, 5443)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_13_MINI, "iPhone 13 Mini", 2195, 5146)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_14, "iPhone 14", 2220, 5367)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_14_PLUS, "iPhone 14 Plus", 2230, 5395)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_14_PRO, "iPhone 14 Pro", 2515, 6363)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_14_PRO_MAX, "iPhone 14 Pro Max", 2514, 6341)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_15, "iPhone 15", 2547, 6320)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_15_PLUS, "iPhone 15 Plus", 2556, 6375)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_15_PRO, "iPhone 15 Pro", 2899, 7214)
    MODEL_INFO(DeviceInfos::DeviceModel::APPLE_IPHONE_15_PRO_MAX, "iPhone 15 Pro Max", 2891, 7181)
    MODEL_INFO(DeviceInfos::DeviceModel::ASUS_ZENPHONE2, "Asus Zenfone 2", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::EPSON_MOVERIO_BT300, "Epson Moverio BT300", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::ESSENTIAL_PH_1, "Essential Phone 1", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL, "Pixel", 326, 755)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL_XL, "Pixel XL", 333, 766)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL2, "Pixel 2", 384, 1324)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL2_XL, "Pixel 2 XL", 380, 1319)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL3, "Pixel 3", 491, 1526)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL3_XL, "Pixel 3 XL", 510, 1648)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL4, "Pixel 4", 882, 2479)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL4_XL, "Pixel 4 XL", 869, 2488)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL5, "Pixel 5", 776, 1827)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL5A, "Pixel 5a", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL5_XL, "Pixel 5 XL", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL6, "Pixel 6", 1362, 4375)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL6_PRO, "Pixel 6 Pro", 1332, 3075)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL7, "Pixel 7", 1367, 3187)
    MODEL_INFO(DeviceInfos::DeviceModel::GOOGLE_PIXEL7_PRO, "Pixel 7 Pro", 1404, 3362)
    MODEL_INFO(DeviceInfos::DeviceModel::HIP_H450R, "", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::HTC_ONE_M8, "HTC One (M8)", 154, 574)
    MODEL_INFO(DeviceInfos::DeviceModel::HUAWEI_HONOR_8, "Huawei Honor 8", 357, 1020)
    MODEL_INFO(DeviceInfos::DeviceModel::HUAWEI_HONOR_9, "Huawei Honor 9", 418, 1323)
    MODEL_INFO(DeviceInfos::DeviceModel::HUAWEI_MATE_9, "Huawei Mate 9", 393, 1274)
    MODEL_INFO(DeviceInfos::DeviceModel::HUAWEI_MATE_10, "Huawei Mate 10", 373, 1294)
    MODEL_INFO(DeviceInfos::DeviceModel::HUAWEI_NEXUS_6P, "Huawei Nexus 6P", 275, 656)
    MODEL_INFO(DeviceInfos::DeviceModel::HUAWEI_P20, "Huawei P20", 365, 1257)
    MODEL_INFO(DeviceInfos::DeviceModel::HUAWEI_P20_LITE, "Huawei P20 Lite", 209, 775)
    MODEL_INFO(DeviceInfos::DeviceModel::HUAWEI_P30, "Huawei P30", 800, 2329)
    MODEL_INFO(DeviceInfos::DeviceModel::LENOVO_K6_NOTE, "Lenovo K6 Note", 141, 578)
    MODEL_INFO(DeviceInfos::DeviceModel::LENOVO_PB2_690Y, "", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::LGE_NEXUS_4, "LG Nexus 4", 80, 224)
    MODEL_INFO(DeviceInfos::DeviceModel::LGE_NEXUS_5, "", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::LGE_NEXUS_5X, "LG Nexus 5X", 168, 496)
    MODEL_INFO(DeviceInfos::DeviceModel::MOTOROLA_EDGE_PLUS, "Motorola Edge+", 1658, 4097)
    MODEL_INFO(DeviceInfos::DeviceModel::MOTOROLA_NEXUS_6, "Motorola Nexus 6", 130, 353)
    MODEL_INFO(DeviceInfos::DeviceModel::MOTOROLA_MOTOG3, "Motorola MotoG3", 94, 181)
    MODEL_INFO(DeviceInfos::DeviceModel::MOTOROLA_RAZR_PLUS, "Motorola Razr+", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::ODG_R7, "", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::ONEPLUS_8T, "OnePlus 8T", 1159, 3173)
    MODEL_INFO(DeviceInfos::DeviceModel::ONEPLUS_9_5G, "OnePlus 9", 1184, 3308)
    MODEL_INFO(DeviceInfos::DeviceModel::ONEPLUS_9_PRO, "OnePlus 9 Pro", 1208, 3273)
    MODEL_INFO(DeviceInfos::DeviceModel::ONEPLUS_NORD10_5G, "OnePlus Nord N10 5G", 779, 1835)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_CORE_PRIME, "", 0, 0)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_J7, "Samsung Galaxy J7", 122, 453)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S5, "Samsung Galaxy S5", 155, 401)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S6, "Samsung Galaxy S6", 185, 750)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S6_EDGE, "Samsung Galaxy S6 edge", 264, 904)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S7, "Samsung Galaxy S7", 287, 627)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S7_EDGE, "Samsung Galaxy S7 edge", 322, 1005)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S8, "Samsung Galaxy S8", 361, 1282)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S8_PLUS, "Samsung Galaxy S8+", 395, 1211)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S9, "Samsung Galaxy S9", 509, 1723)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S9_PLUS, "Samsung Galaxy S9+", 511, 1782)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S10, "Samsung Galaxy S10", 895, 2604)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S10e, "Samsung Galaxy S10e", 883, 2092)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S10_PLUS, "Samsung Galaxy S10+", 882, 2159)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S21_5G, "Samsung Galaxy S21 5G", 1139, 3048)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S21_ULTRA_5G, "Samsung Galaxy S21 Ultra 5G", 1165, 3097)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_S22_PLUS, "Samsung Galaxy S22+", 1478, 3473)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_ZFOLD_3, "Samsung Galaxy Z Fold3", 2153, 3331)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_ZFLIP_3, "Samsung Galaxy Z Flip3", 1153, 3248)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_NOTE8, "Samsung Galaxy Note 8", 367, 1357)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_NOTE9, "Samsung Galaxy Note 9", 520, 1788)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_NOTE10_PLUS, "Samsung Galaxy Note10+", 613, 1864)
    MODEL_INFO(DeviceInfos::DeviceModel::SAMSUNG_GALAXY_TAB_A, "Samsung Galaxy Tab A 10.1", 121, 440)
    MODEL_INFO(DeviceInfos::DeviceModel::XIAOMI_MIX_2, "Xiaomi Mi Mix 2", 397, 1234)
    MODEL_INFO(DeviceInfos::DeviceModel::XIAOMI_REDMI_4X, "Xiaomi Redmi 4X", 144, 655)
    default: return {"Unknown", 0, 0};
  }
}

}  // namespace c8
