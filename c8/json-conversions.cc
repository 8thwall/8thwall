// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Akul Gupta (akulgupta@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"json-conversions.h"};
  deps = {
    "@json//:json",
    "//c8:c8-log",
    "//c8:hmatrix",
    "//c8:quaternion",
    "//c8:string",
    "//c8/geometry:egomotion",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xc403347d);

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/json-conversions.h"

namespace c8 {

nlohmann::json matToJson(const HMatrix &mat) {
  const auto matT = translation(mat);
  const auto matR = rotation(mat);
  nlohmann::json res;
  res["translation"]["x"] = matT.x();
  res["translation"]["y"] = matT.y();
  res["translation"]["z"] = matT.z();
  res["rotation"]["x"] = matR.x();
  res["rotation"]["y"] = matR.y();
  res["rotation"]["z"] = matR.z();
  res["rotation"]["w"] = matR.w();
  return res;
}

HMatrix jsonStringToMat(const String &data) {
  auto json = nlohmann::json::parse(data);
  return jsonToMat(json);
}

HMatrix jsonToMat(const nlohmann::json &json) {
  if (json.is_discarded()) {
    C8_THROW("[json-conversions] Bad input json");
  }
  const auto &t = json["translation"];
  const auto &r = json["rotation"];
  auto pos = HVector3{
    t["x"].get<float>(),
    t["y"].get<float>(),
    t["z"].get<float>(),
  };
  auto rot = Quaternion{
    r["w"].get<float>(),
    r["x"].get<float>(),
    r["y"].get<float>(),
    r["z"].get<float>(),
  };
  return cameraMotion(pos, rot);
}

bool isAnyNull(const nlohmann::json &json) {
  if (json.is_discarded()) {
    C8_THROW("[json-conversions] Bad input json");
  }
  if (json.is_null()) {
    return true;
  }
  if (!json.is_object() && !json.is_array() && json.size() <= 1) {
    return false;
  }

  for (const auto &v : json) {
    if (isAnyNull(v)) {
      return true;
    }
  }
  return false;
}

}  // namespace c8
