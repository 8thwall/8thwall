// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// C-wrappers for structured data types.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "c8/protolog/api-limits.h"

// NOTE: These constants must be kept exactly in sync with its equivalent in XRExtern.
#define RENDERING_SYSTEM_UNSPECIFIED 0
#define RENDERING_SYSTEM_OPENGL 1
#define RENDERING_SYSTEM_METAL 2
#define RENDERING_SYSTEM_DIRECT3D11 3

// XR Response for bridging with experimental Swift apps, not for production.
struct c8_XRResponseLegacy {
  // Always present.
  int64_t eventIdTimeMicros;

  // Output when "maskLighting" in configured.
  float lightingGlobalExposure;

  // Output when "maskCamera" in configured.
  float cameraExtrinsicPositionX;
  float cameraExtrinsicPositionY;
  float cameraExtrinsicPositionZ;
  float cameraExtrinsicRotationW;
  float cameraExtrinsicRotationX;
  float cameraExtrinsicRotationY;
  float cameraExtrinsicRotationZ;
  float cameraIntrinsicMatrix44f[C8_API_LIMITS_MATRIX44];

  // Output when "maskSurfaces" in configured.
  int32_t surfacesSetSurfacesCount;
  int64_t surfacesSetSurfacesIdTimeMicros[C8_API_LIMITS_MAX_SURFACES];
  int32_t surfacesSetSurfacesFacesBeginIndex[C8_API_LIMITS_MAX_SURFACES];
  int32_t surfacesSetSurfacesFacesEndIndex[C8_API_LIMITS_MAX_SURFACES];
  int32_t surfacesSetSurfacesVerticesBeginIndex[C8_API_LIMITS_MAX_SURFACES];
  int32_t surfacesSetSurfacesVerticesEndIndex[C8_API_LIMITS_MAX_SURFACES];
  int32_t surfacesSetFacesCount;
  int32_t surfacesSetFaces[C8_API_LIMITS_MAX_SURFACE_FACES * 3];
  int32_t surfacesSetVerticesCount;
  float surfacesSetVertices[C8_API_LIMITS_MAX_SURFACE_VERTICES * 3];
  int64_t surfacesActiveSurfaceIdTimeMicros;
  float surfacesActiveSurfaceActivePointX;
  float surfacesActiveSurfaceActivePointY;
  float surfacesActiveSurfaceActivePointZ;
};

// XR Configuration for bridging with experimental Swift apps, not for production.
struct c8_XRConfigurationLegacy {
  // Specifies data requested from XR.
  bool maskLighting;
  bool maskCamera;
  bool maskSurfaces;
  bool maskVerticalSurfaces;
  bool autofocus;

  bool depthMapping;

  // Specifies the graphics context for displaying XR data.
  int32_t graphicsIntrinsicsTextureWidth;
  int32_t graphicsIntrinsicsTextureHeight;
  float graphicsIntrinsicsNearClip;
  float graphicsIntrinsicsFarClip;
  float graphicsIntrinsicsDigitalZoomHorizontal;
  float graphicsIntrinsicsDigitalZoomVertical;

  const char *mobileAppKey;
};

// Message for wrapping serialized capnp data without implied ownership.
struct c8_NativeByteArray {
  const void* bytes;
  int size;
};

#ifdef __cplusplus
}  // extern "C"
#endif
