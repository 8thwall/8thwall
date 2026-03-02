// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"parameter-data.h"};
  deps = {
    "//c8:string",
    "@json//:json",
  };
}
cc_end(0xdf86c281);

#include <nlohmann/json.hpp>

#include "c8/parameter-data.h"
namespace c8 {

using Json = nlohmann::json;

struct ParameterData::Parameters {
  nlohmann::json json_;
};

ParameterData::ParameterData() : parameters_(new Parameters()) {}

ParameterData::~ParameterData() { delete parameters_; }

template <typename T>
T ParameterData::get(const char *key) const {
  return parameters_->json_[key].get<T>();
}

template <typename T>
T ParameterData::getOrSet(const char *key, T fallback) {
  if (parameters_->json_.find(key) != parameters_->json_.end()) {
    return parameters_->json_[key].get<T>();
  }
  parameters_->json_[key] = fallback;
  return fallback;
}

template <typename T>
void ParameterData::set(const char *key, T val) {
  ++version_;
  parameters_->json_[key] = val;
}

bool ParameterData::contains(const char *key) const {
  return parameters_->json_.find(key) != parameters_->json_.end();
}

void ParameterData::deleteKey(const char *key) {
  ++version_;
  parameters_->json_.erase(key);
}

void ParameterData::mergeJsonString(const String &jsonString) {
  parameters_->json_.update(Json::parse(jsonString));
}

String ParameterData::toJsonString() const { return parameters_->json_.dump(); }

void ScopedParameterUpdates::mergeJsonString(const String &jsonString) {
  auto json = Json::parse(jsonString);
  for (const auto &[key, value] : json.items()) {
    if (value.is_boolean()) {
      set(key.c_str(), value.get<bool>());
    } else if (value.is_number_integer()) {
      set(key.c_str(), value.get<int>());
    } else if (value.is_number_unsigned()) {
      set(key.c_str(), value.get<uint32_t>());
    } else if (value.is_number_float()) {
      set(key.c_str(), value.get<float>());
    } else if (value.is_string()) {
      set(key.c_str(), value.get<String>());
    }
  }
}

template bool ParameterData::get<bool>(const char *key) const;
template int ParameterData::get<int>(const char *key) const;
template int64_t ParameterData::get<int64_t>(const char *key) const;
template uint32_t ParameterData::get<uint32_t>(const char *key) const;
template uint64_t ParameterData::get<uint64_t>(const char *key) const;
template float ParameterData::get<float>(const char *key) const;
template double ParameterData::get<double>(const char *key) const;
template String ParameterData::get<String>(const char *key) const;

template bool ParameterData::getOrSet<bool>(const char *key, bool fallback);
template int ParameterData::getOrSet<int>(const char *key, int fallback);
template int64_t ParameterData::getOrSet<int64_t>(const char *key, int64_t fallback);
template uint32_t ParameterData::getOrSet<uint32_t>(const char *key, uint32_t fallback);
template uint64_t ParameterData::getOrSet<uint64_t>(const char *key, uint64_t fallback);
template float ParameterData::getOrSet<float>(const char *key, float fallback);
template double ParameterData::getOrSet<double>(const char *key, double fallback);
template String ParameterData::getOrSet<String>(const char *key, String fallback);

template void ParameterData::set<bool>(const char *key, bool val);
template void ParameterData::set<int>(const char *key, int val);
template void ParameterData::set<int64_t>(const char *key, int64_t val);
template void ParameterData::set<uint32_t>(const char *key, uint32_t val);
template void ParameterData::set<uint64_t>(const char *key, uint64_t val);
template void ParameterData::set<float>(const char *key, float val);
template void ParameterData::set<double>(const char *key, double val);
template void ParameterData::set<String>(const char *key, String val);

// Global static used to hold parameter data.
ParameterData &globalParams() {
  static ParameterData d;
  return d;
}

}  // namespace c8
