// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Definitions and functions for renderers

#pragma once

namespace c8 {

enum class GpuReadPixelsOptions {
  UNSPECIFIED_OPTIONS,
  // Delay readback of the rendered texture until the next call to draw or readPixels.
  DEFER_READ,
  // Set a new texture and read it back immediately.
  READ_IMMEDIATELY,
  // Like DEFER_READ but also query and restore opengl state (slow but safe).
  DEFER_READ_RESTORE_STATE,
  // Like READ_IMMEDIATELY but also query and restore opengl state (slow but safe).
  READ_IMMEDIATELY_RESTORE_STATE
};

bool isDeferredGpuReadPixels(const GpuReadPixelsOptions o);

bool isRestoreStateGpuReadPixels(const GpuReadPixelsOptions o);

}  // namespace c8
