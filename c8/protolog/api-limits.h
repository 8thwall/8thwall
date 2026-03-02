// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Shared constants for fixed length data types.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define C8_API_LIMITS_MAX_FEATURES 500

#define C8_API_LIMITS_MATRIX44 16

// 15 surfaces.
// 50 vertices per surface = 750 max vertices.
// Max faces per surface for n vertices is (2n - 5), or 95 per surface with 50 vertices.
#define C8_API_LIMITS_MAX_SURFACES 15
#define C8_API_LIMITS_MAX_SURFACE_FACES 1425
#define C8_API_LIMITS_MAX_SURFACE_VERTICES 750

#define C8_API_LIMITS_IMAGE_PROCESSING_WIDTH 480
#define C8_API_LIMITS_IMAGE_PROCESSING_HEIGHT 640

#define C8_API_LIMITS_MAX_TOUCHES 50
#define C8_API_LIMITS_MAX_SERVERS 50
#define C8_API_LIMITS_MAX_SERVERNAME_LENGTH 120

#ifdef __cplusplus
}  // extern "C"
#endif
