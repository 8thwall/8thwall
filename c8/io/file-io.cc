// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "file-io.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:scope-exit",
    "//c8:string",
    "//c8:vector",
    "//c8/string:strcat",
    "@json//:json",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x23f1aaaf);

#include <cstdio>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/io/file-io.h"
#include "c8/scope-exit.h"
#include "c8/string/strcat.h"

namespace c8 {

bool fileExists(const String &filename) {
  std::ifstream ifs(filename);
  return !ifs.fail();
}

const Vector<uint8_t> readFile(const String &filename) { return readFile(filename.c_str()); }

const Vector<uint8_t> readFile(const char *filename) {
  FILE *file;

  file = fopen(filename, "rb");
  auto fileSize = getFileSize(file, filename);
  SCOPE_EXIT([file] { fclose(file); });
  std::vector<uint8_t> inputBuffer(fileSize);

  rewind(file);

  // Read file file into memory.
  size_t result = fread(inputBuffer.data(), 1, fileSize, file);
  if (result != fileSize) {
    C8Log("[file-io] %s", "Failed to read file");
    C8_THROW("Failed to read file");
  }
  return inputBuffer;
}

void writeFile(const String &filename, const uint8_t *data, size_t size) {
  writeFile(filename.c_str(), data, size);
}

void writeFile(const char *filename, const uint8_t *data, size_t size) {
  FILE *file = fopen(filename, "wb");

  if (!file) {
    C8_THROW(strCat("Failed to open file ", filename));
  }
  SCOPE_EXIT([file] { fclose(file); });

  // Write file to disk.
  size_t result = fwrite(data, size, 1, file);
  if (result != 1) {
    C8_THROW(strCat("Failed to write file ", filename));
  }
}

const String readTextFile(const String &filename) { return readTextFile(filename.c_str()); }

const String readTextFile(const char *filename) {
  FILE *file;

  file = fopen(filename, "r");
  auto fileSize = getFileSize(file, filename);
  SCOPE_EXIT([file] { fclose(file); });
  String inputBuffer(fileSize, 0);

  rewind(file);

  // Read whole file into memory.
  size_t result = fread(inputBuffer.data(), 1, fileSize, file);
  if (result != fileSize) {
    C8Log("[file-io] %s", "Failed to read file");
    C8_THROW("Failed to read file");
  }
  return inputBuffer;
}

void writeTextFile(const String &filename, const String &data) {
  FILE *file = fopen(filename.c_str(), "w");

  if (!file) {
    C8_THROW("Failed to open file " + filename);
  }
  SCOPE_EXIT([file] { fclose(file); });

  // Write text to disk.
  auto result = fputs(data.c_str(), file);
  if (result < 0) {
    C8_THROW("Failed to write file " + filename);
  }
}

const Vector<float> readFloatFile(const String &filename) {
  return readFloatFile(filename.c_str());
}

const Vector<float> readFloatFile(const char *filename) {
  FILE *file;

  file = fopen(filename, "rb");
  auto fileSize = getFileSize(file, filename);
  SCOPE_EXIT([file] { fclose(file); });
  std::vector<float> inputBuffer(fileSize);

  rewind(file);

  // Read file file into memory.
  size_t result = fread(inputBuffer.data(), 4, fileSize / 4, file);
  if (result != fileSize / 4) {
    C8Log("[file-io] %s", "Failed to read file");
    C8_THROW("Failed to read file");
  }
  return inputBuffer;
}

void writeFloatFile(const String &filename, const float *data, size_t size) {
  writeFloatFile(filename.c_str(), data, size);
}

void writeFloatFile(const char *filename, const float *data, size_t size) {
  FILE *file = fopen(filename, "wb");

  if (!file) {
    C8_THROW(strCat("Failed to open file ", filename));
  }
  SCOPE_EXIT([file] { fclose(file); });

  // Write file to disk.
  size_t result = fwrite(data, 4, size, file);
  if (result != size) {
    C8_THROW(strCat("Failed to write file ", filename));
  }
}

const long int getFileSize(FILE *file, const char *filename) {
  if (!file) {
    C8Log("[file-io] Failed to open file %s", filename);
    C8_THROW(strCat("Failed to open file ", filename));
  }

  fseek(file, 0L, SEEK_END);
  auto fileSize = ftell(file);
  rewind(file);

  // Max size 100MB file.
  constexpr int MAX_SIZE = 104857600;

  if (fileSize <= 0) {
    C8Log("[file-io] %s", "Filesize must be greater than zero bytes");
    C8_THROW("Filesize must be greater than zero bytes");
  }

  if (fileSize > MAX_SIZE) {
    C8Log("[file-io] %s", "Filesize must be smaller than 100MB");
    C8_THROW("Filesize must be smaller than 100MB");
  }

  return fileSize;
}

nlohmann::json readJsonFile(
  const String &jsonFilePath, const String &keyName, const String &fileNotFoundHelp) {
  std::ifstream ifs(jsonFilePath);
  if (ifs.fail()) {
    C8_THROW("Could not find/open file %s. %s", jsonFilePath.c_str(), fileNotFoundHelp.c_str());
  }
  auto json = nlohmann::json::parse(ifs);
  if (json.is_discarded()) {
    C8_THROW("Error opening JSON file.");
  }
  if (!keyName.empty() && !json.contains(keyName)) {
    C8_THROW("Key \"%s\" not found in JSON input.", keyName.c_str());
  }
  return keyName.empty() ? json : json[keyName];
}

}  // namespace c8
