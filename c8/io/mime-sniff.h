// Copyright (c) 2024 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#pragma once

#include <span>

#include "c8/string-view.h"

namespace c8 {

enum class MimeType {
  UNDEFINED,
  IMAGE_X_ICON,
  IMAGE_BMP,
  IMAGE_GIF,
  IMAGE_WEBP,
  IMAGE_PNG,
  IMAGE_JPEG,
};

// To determine which image MIME type byte pattern a byte sequence input matches, if any, use the
// following image type pattern matching algorithm:
// See: https://mimesniff.spec.whatwg.org/#matching-an-image-type-pattern
MimeType matchImageMimeType(std::span<const uint8_t> input);

// Return a string view for the given image MIME type. Throws an exception (or terminates) if the
// MIME type is MimeType::UNDEFINED.
StringView mimeTypeString(MimeType mimeType);

}  // namespace c8
