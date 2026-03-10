// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Read/write compressed images from the filesystem.

#pragma once

#include <nlohmann/json.hpp>

#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

bool fileExists(const String &filename);

// c_str() flavors
const Vector<uint8_t> readFile(const char *filename);
void writeFile(const char *filename, const uint8_t *data, size_t size);

const String readTextFile(const char *filename);
void writeTextFile(const char *filename, const char *data);

// String flavors
const Vector<uint8_t> readFile(const String &filename);
void writeFile(const String &filename, const uint8_t *data, size_t size);

const String readTextFile(const String &filename);
void writeTextFile(const String &filename, const String &data);

// Read/writes float values in bin files which is expected by np.fromfile when training
// the depth nets.
// c_str()
const Vector<float> readFloatFile(const char *filename);
void writeFloatFile(const char *filename, const float *data, size_t size);

// String
const Vector<float> readFloatFile(const String &filename);
void writeFloatFile(const String &filename, const float *data, size_t size);

const long int getFileSize(FILE *file, const char *filename);

// Json
// Reads JSON from a file.
//
// @param jsonFilePath the file path to read JSON from.
// @param keyName if specified returns json[keyName], else the full JSON object.
// @param fileNotFoundHelp help message to print if the specified file does not exist.
nlohmann::json readJsonFile(
  const String &jsonFilePath, const String &keyName = "", const String &fileNotFoundHelp = "");

}  // namespace c8
