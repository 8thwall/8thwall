// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/quaternion.h"
#include "reality/engine/api/base/camera-intrinsics.capnp.h"
#include "reality/engine/api/face.capnp.h"
#include "reality/engine/api/hand.capnp.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/api/semantics.capnp.h"

namespace c8 {

// Adjust the coordinate system in the request based on the orientation specified in the request.
void deviceToPortraitOrientationPreprocess(
  AppContext::DeviceOrientation orientation, RealityRequest::Builder request);

// Adjust the coordinate system in the response based on the orientation specified in the request.
void portraitToDeviceOrientationPostprocess(
  AppContext::DeviceOrientation orientation, RealityResponse::Builder response);

// Adjust the coordinate system in the response based on the coordinate system specified in the
// request.
void rewriteCoordinateSystemPostprocess(
  CoordinateSystemConfiguration::Reader system, RealityResponse::Builder response);

// Adjust the coordinate system in the response based on the coordinate system specified in the
// request.
void rewriteCoordinateSystemPostprocess(
  CoordinateSystemConfiguration::Reader system, XrQueryResponse::Builder response);

// Adjust the coordinate system in the response based on the coordinate system specified in the
// request.  This is performed on a per-frame basis for the FaceResponse for variable data like
// vertices and normals.
void rewriteCoordinateSystemPostprocess(
  CameraCoordinates::Reader system, FaceResponse::Builder response);

// Adjust the coordinate system in the response based on the coordinate system specified in the
// request.  This is performed on a per-frame basis for the SemanticsResponse for variable data like
// vertices and normals.
void rewriteCoordinateSystemPostprocess(
  CameraCoordinates::Reader system, SemanticsResponse::Builder response);

// Adjust the coordinate system in the response based on the coordinate system specified in the
// request.  This is performed only once for the FrameworkResponse for static face data such as
// indices and uvs.
void rewriteCoordinateSystemFrameworkPostprocess(
  CameraCoordinates::Reader system, FrameworkResponse::Builder response);

void makeRightHanded(Position32f::Builder p);

void makeRightHanded(Quaternion32f::Builder q);

void recenterAndScale(Quaternion32f::Builder q, Quaternion o);

void recenterAndScale(Position32f::Builder p, const HMatrix &o, float s);

void mirror(Position32f::Builder p);

void mirror(Quaternion32f::Builder q);

}  // namespace c8
