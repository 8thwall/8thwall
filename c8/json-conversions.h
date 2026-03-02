// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Akul Gupta (akulgupta@nianticlabs.com)

#pragma once

#include <nlohmann/json.hpp>

#include "c8/hmatrix.h"
#include "c8/quaternion.h"
#include "c8/string.h"

namespace c8 {

nlohmann::json matToJson(const HMatrix &mat);

HMatrix jsonStringToMat(const String &data);

HMatrix jsonToMat(const nlohmann::json &json);

bool isAnyNull(const nlohmann::json &json);

}  // namespace c8
