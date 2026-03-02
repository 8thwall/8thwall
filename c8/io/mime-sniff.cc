// Copyright (c) 2024 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "mime-sniff.h",
  };
  deps = {
    "//c8:exceptions",
    "//c8:string-view",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x05b10c28);

#include <cassert>
#include <optional>
#include <set>
#include <span>
#include <tuple>

#include "c8/exceptions.h"
#include "c8/io/mime-sniff.h"

using c8::MimeType;

namespace {

// To determine whether a byte sequence matches a particular byte pattern, use the following pattern
// matching algorithm. It is given a byte sequence input, a byte pattern pattern, a pattern mask
// mask, and a set of bytes to be ignored ignored, and returns true or false.
// See: https://mimesniff.spec.whatwg.org/#pattern-matching-algorithm
bool patternMatchingAlgorithm(
  std::span<const uint8_t> input,
  std::span<const uint8_t> pattern,
  std::span<const uint8_t> mask,
  const std::set<uint8_t> &ignored) {
  // 1. Assert: pattern’s length is equal to mask’s length.
  assert((pattern.size() == mask.size()));

  // 2. If input’s length is less than pattern’s length, return false.
  if (input.size() < pattern.size()) {
    return false;
  }

  // 3. Let s be 0.
  size_t s = 0;

  // 4. While s < input’s length:
  while (s < input.size()) {
    // 1. If ignored does not contain input[s], break.
    if (ignored.find(input[s]) == ignored.end()) {
      break;
    }

    // 2. Set s to s + 1.
    s++;
  }

  // 5. Let p be 0.
  size_t p = 0;

  // 6. While p < pattern’s length:
  while (p < pattern.size()) {
    // 1. Let maskedData be the result of applying the bitwise AND operator to input[s] and mask[p].
    uint8_t maskedData = input[s] & mask[p];

    // 2. If maskedData is not equal to pattern[p], return false.
    if (maskedData != pattern[p]) {
      return false;
    }

    // 3. Set s to s + 1.
    s++;

    // 4. Set p to p + 1.
    p++;
  }

  // 7. Return true.
  return true;
}

struct ImageTypePattern {
  const std::vector<uint8_t> pattern;
  const std::vector<uint8_t> mask;
  const std::set<uint8_t> ignored;
  const MimeType mimeType;
};

const auto rows = std::vector<ImageTypePattern>{
  // Byte Pattern	Pattern Mask	Leading Bytes to Be Ignored	Image MIME Type	Note
  // 00 00 01 00	FF FF FF FF	None.	image/x-icon	A Windows Icon signature.
  {
    // Byte Pattern
    {0x00, 0x00, 0x01, 0x00},
    // Pattern Mask
    {0xFF, 0xFF, 0xFF, 0xFF},
    // Leading Bytes to Be Ignored
    {},
    // Image MIME Type
    MimeType::IMAGE_X_ICON,
  },
  // Byte Pattern	Pattern Mask	Leading Bytes to Be Ignored	Image MIME Type	Note
  // 00 00 02 00	FF FF FF FF	None.	image/x-icon	A Windows Cursor signature.
  {
    // Byte Pattern
    {0x00, 0x00, 0x02, 0x00},
    // Pattern Mask
    {0xFF, 0xFF, 0xFF, 0xFF},
    // Leading Bytes to Be Ignored
    {},
    // Image MIME Type
    MimeType::IMAGE_X_ICON,
  },
  // Byte Pattern	Pattern Mask	Leading Bytes to Be Ignored	Image MIME Type	Note
  // 42 4D	FF FF	None.	image/bmp	The string "BM", a BMP signature.
  {
    // Byte Pattern
    {0x42, 0x4D},
    // Pattern Mask
    {0xFF, 0xFF},
    // Leading Bytes to Be Ignored
    {},
    // Image MIME Type
    MimeType::IMAGE_BMP,
  },
  // Byte Pattern	Pattern Mask	Leading Bytes to Be Ignored	Image MIME Type	Note
  // 47 49 46 38 37 61	FF FF FF FF FF FF	None.	image/gif	The string "GIF87a", a GIF
  // signature.
  {
    // Byte Pattern
    {0x47, 0x49, 0x46, 0x38, 0x37, 0x61},
    // Pattern Mask
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    // Leading Bytes to Be Ignored
    {},
    // Image MIME Type
    MimeType::IMAGE_GIF,
  },
  // Byte Pattern	Pattern Mask	Leading Bytes to Be Ignored	Image MIME Type	Note
  // 47 49 46 38 39 61	FF FF FF FF FF FF	None.	image/gif	The string "GIF89a", a GIF
  // signature.
  {
    // Byte Pattern
    {0x47, 0x49, 0x46, 0x38, 0x39, 0x61},
    // Pattern Mask
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    // Leading Bytes to Be Ignored
    {},
    // Image MIME Type
    MimeType::IMAGE_GIF,
  },
  // Byte Pattern	Pattern Mask	Leading Bytes to Be Ignored	Image MIME Type	Note
  // 52 49 46 46 00 00 00 00 57 45 42 50 56 50	FF FF FF FF 00 00 00 00 FF FF FF FF FF FF
  //   None.	image/webp	The string "RIFF" followed by four bytes followed by the string
  //   "WEBPVP".
  {
    // Byte Pattern
    {0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x57, 0x45, 0x42, 0x50, 0x56, 0x50},
    // Pattern Mask
    {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    // Leading Bytes to Be Ignored
    {},
    // Image MIME Type
    MimeType::IMAGE_WEBP,
  },
  // Byte Pattern	Pattern Mask	Leading Bytes to Be Ignored	Image MIME Type	Note
  // 89 50 4E 47 0D 0A 1A 0A	FF FF FF FF FF FF FF FF
  // None.	image/png	An error-checking byte followed by the string "PNG" followed by
  // CR LF SUB LF, the PNG signature.
  {
    // Byte Pattern
    {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A},
    // Pattern Mask
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    // Leading Bytes to Be Ignored
    {},
    // Image MIME Type
    MimeType::IMAGE_PNG,
  },
  // Byte Pattern	Pattern Mask	Leading Bytes to Be Ignored	Image MIME Type	Note
  // FF D8 FF	FF FF FF	None.	image/jpeg	The JPEG Start of Image marker followed by
  // the
  // indicator
  // byte of another marker.
  {
    // Byte Pattern
    {0xFF, 0xD8, 0xFF},
    // Pattern Mask
    {0xFF, 0xFF, 0xFF},
    // Leading Bytes to Be Ignored
    {},
    // Image MIME Type
    MimeType::IMAGE_JPEG,
  }};
}  // namespace

namespace c8 {

// To determine which image MIME type byte pattern a byte sequence input matches, if any, use the
// following image type pattern matching algorithm:
// See: https://mimesniff.spec.whatwg.org/#matching-an-image-type-pattern
MimeType matchImageMimeType(std::span<const uint8_t> input) {
  // 1. Execute the following steps for each row row in the following table:
  for (const auto &row : rows) {
    // 1. Let patternMatched be the result of the pattern matching algorithm given input, the value
    // in the first column of row, the value in the second column of row, and the value in the third
    // column of row.
    bool patternMatched = patternMatchingAlgorithm(input, row.pattern, row.mask, row.ignored);

    // 2. If patternMatched is true, return the value in the fourth column of row.
    if (patternMatched) {
      return row.mimeType;
    }
  }

  // 2. Return undefined.
  return MimeType::UNDEFINED;
}

// Return a string view for the given image MIME type. Throws an exception (or terminates) if the
// MIME type is MimeType::UNDEFINED.
StringView mimeTypeString(MimeType mimeType) {
  switch (mimeType) {
    case MimeType::UNDEFINED:
      C8_THROW_INVALID_ARGUMENT("unknown image MIME type");
    case MimeType::IMAGE_X_ICON:
      return "image/x-icon";
    case MimeType::IMAGE_BMP:
      return "image/bmp";
    case MimeType::IMAGE_GIF:
      return "image/gif";
    case MimeType::IMAGE_WEBP:
      return "image/webp";
    case MimeType::IMAGE_PNG:
      return "image/png";
    case MimeType::IMAGE_JPEG:
      return "image/jpeg";
  }
}

}  // namespace c8
