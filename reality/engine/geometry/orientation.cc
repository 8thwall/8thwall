// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"orientation.h"};
  deps = {
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:hmatrix",
    "//c8:hvector",
    "//c8:quaternion",
    "//c8/geometry:egomotion",
    "//c8/geometry:homography",
    "//c8/protolog:xr-requests",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/api:face.capnp-cc",
    "//reality/engine/api:hand.capnp-cc",
    "//reality/engine/api:semantics.capnp-cc",
    "//reality/engine/api/base:camera-intrinsics.capnp-cc",
  };
}
cc_end(0x5de95216);

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/hmatrix.h"
#include "c8/hvector.h"
#include "c8/protolog/xr-requests.h"
#include "reality/engine/geometry/orientation.h"

namespace c8 {

namespace {

void updateCameraIntrinsics(
  AppContext::DeviceOrientation orientation, GraphicsCalibration::Builder b) {
  auto m = b.getMatrix44f();
  if (m.size() < 16) {
    return;
  }

  auto r0x = m[0 + 0 * 4];
  auto r0z = m[0 + 2 * 4];
  auto r1y = m[1 + 1 * 4];
  auto r1z = m[1 + 2 * 4];

  auto nr0z = -r0z;
  auto nr1z = -r1z;

  switch (orientation) {
    case AppContext::DeviceOrientation::LANDSCAPE_RIGHT:
      m.set(0 + 0 * 4, r1y);
      m.set(0 + 2 * 4, r1z);
      m.set(1 + 1 * 4, r0x);
      m.set(1 + 2 * 4, nr0z);
      break;
    case AppContext::DeviceOrientation::PORTRAIT_UPSIDE_DOWN:
      m.set(0 + 2 * 4, nr0z);
      m.set(1 + 2 * 4, nr1z);
      break;
    case AppContext::DeviceOrientation::LANDSCAPE_LEFT:
      m.set(0 + 0 * 4, r1y);
      m.set(0 + 2 * 4, nr1z);
      m.set(1 + 1 * 4, r0x);
      m.set(1 + 2 * 4, r0z);
      break;
    default:
      // Leave as specified.
      break;
  }
}

void updateCameraExtrinsics(AppContext::DeviceOrientation orientation, RealityResponse::Builder r) {
  auto rot = HMatrixGen::i();
  switch (orientation) {
    case AppContext::DeviceOrientation::LANDSCAPE_RIGHT:
      rot = HMatrixGen::z90();
      break;
    case AppContext::DeviceOrientation::PORTRAIT_UPSIDE_DOWN:
      rot = HMatrixGen::z180();
      break;
    case AppContext::DeviceOrientation::LANDSCAPE_LEFT:
      rot = HMatrixGen::z270();
      break;
    default:
      // No correction needed.
      return;
  }

  {
    auto c = r.getXRResponse().getCamera().getExtrinsic();

    auto cp = c.getPosition();
    auto cr = c.getRotation();

    auto cam = cameraMotion(
      HVector3{cp.getX(), cp.getY(), cp.getZ()},
      Quaternion{cr.getW(), cr.getX(), cr.getY(), cr.getZ()});

    auto newCam = cam * rot;
    auto newR = rotation(newCam);
    auto newT = translation(newCam);

    setPosition32f(newT.x(), newT.y(), newT.z(), &cp);
    setQuaternion32f(newR.w(), newR.x(), newR.y(), newR.z(), &cr);
  }

  {
    auto c = r.getPose().getExperimental().getVioTrackerTransform();

    auto cp = c.getPosition();
    auto cr = c.getRotation();

    auto cam = cameraMotion(
      HVector3{cp.getX(), cp.getY(), cp.getZ()},
      Quaternion{cr.getW(), cr.getX(), cr.getY(), cr.getZ()});

    auto newCam = cam * rot;
    auto newR = rotation(newCam);
    auto newT = translation(newCam);

    setPosition32f(newT.x(), newT.y(), newT.z(), &cp);
    setQuaternion32f(newR.w(), newR.x(), newR.y(), newR.z(), &cr);
  }
}

void makeRightHanded(SurfaceVertex::Builder v) { v.setZ(-v.getZ()); }

void makeRightHanded(RealityResponse::Builder r) {
  // Camera position.
  makeRightHanded(r.getXRResponse().getCamera().getExtrinsic().getRotation());
  makeRightHanded(r.getXRResponse().getCamera().getExtrinsic().getPosition());

  // VIO camera position.
  makeRightHanded(r.getPose().getExperimental().getVioTrackerTransform().getRotation());
  makeRightHanded(r.getPose().getExperimental().getVioTrackerTransform().getPosition());

  // World points.
  for (auto p : r.getFeatureSet().getPoints()) {
    makeRightHanded(p.getPosition());
  }

  // Image targets.
  for (auto t : r.getXRResponse().getDetection().getImages()) {
    makeRightHanded(t.getPlace().getPosition());
    makeRightHanded(t.getPlace().getRotation());
  }

  // VPS Wayspot Anchor.
  for (auto t : r.getXRResponse().getDetection().getWayspotAnchors()) {
    makeRightHanded(t.getPlace().getPosition());
    makeRightHanded(t.getPlace().getRotation());
  }

  // VPS Mesh.
  auto meshPlace = r.getXRResponse().getDetection().getMesh().getPlace();
  makeRightHanded(meshPlace.getPosition());
  makeRightHanded(meshPlace.getRotation());

  // Surfaces.
  auto surfaceSet = r.getXRResponse().getSurfaces().getSet();
  for (auto s : surfaceSet.getSurfaces()) {
    makeRightHanded(s.getNormal().getPosition());
    makeRightHanded(s.getNormal().getRotation());
  }
  for (auto v : surfaceSet.getVertices()) {
    makeRightHanded(v);
  }
}

void makeRightHanded(XrQueryResponse::Builder r) {
  for (auto h : r.getHitTest().getHits()) {
    makeRightHanded(h.getPlace().getPosition());
    makeRightHanded(h.getPlace().getRotation());
  }
}

void makeRightHanded(FaceResponse::Builder r) {
  // camera position
  makeRightHanded(r.getCamera().getExtrinsic().getRotation());
  makeRightHanded(r.getCamera().getExtrinsic().getPosition());

  for (auto face : r.getFaces()) {
    // update transform
    makeRightHanded(face.getTransform().getPosition());
    makeRightHanded(face.getTransform().getRotation());

    for (auto localPoint : face.getVertices()) {
      makeRightHanded(localPoint);
    }

    for (auto normal : face.getNormals()) {
      makeRightHanded(normal);
    }

    for (auto attachmentPoint : face.getAttachmentPoints()) {
      makeRightHanded(attachmentPoint.getPosition());
      makeRightHanded(attachmentPoint.getRotation());
    }

    // make ear data right handed
    for (auto localPoint : face.getEarVertices()) {
      makeRightHanded(localPoint);
    }
    for (auto attachmentPoint : face.getEarAttachmentPoints()) {
      makeRightHanded(attachmentPoint.getPosition());
      makeRightHanded(attachmentPoint.getRotation());
    }
  }
}

void makeRightHanded(SemanticsResponse::Builder r) {
  // Camera position.
  makeRightHanded(r.getCamera().getExtrinsic().getRotation());
  makeRightHanded(r.getCamera().getExtrinsic().getPosition());
}

// If the user requests a right handed coordinate system, then the indices should be counter
// clockwise
void makeRightHanded(FrameworkResponse::Builder response) {
  for (auto triangleIndices : response.getModelGeometry().getIndices()) {
    const auto b = triangleIndices.getB();
    triangleIndices.setB(triangleIndices.getC());
    triangleIndices.setC(b);
  }
}

// If the user mirrors the image, we need to flip the second and third index for a face to keep
// a triangle's current clockwise / counter-clockwise direction.
void mirror(FrameworkResponse::Builder response) {
  for (auto triangleIndices : response.getModelGeometry().getIndices()) {
    const auto b = triangleIndices.getB();
    triangleIndices.setB(triangleIndices.getC());
    triangleIndices.setC(b);
  }

  // mirror the UVs so the texture continues to go from left to right
  for (auto UV : response.getModelGeometry().getUvs()) {
    UV.setU(1.0f - UV.getU());
  }
}

void recenterAndScale(SurfaceVertex::Builder v, const HMatrix &o, float s) {
  HPoint3 nv = o * HPoint3(s * v.getX(), s * v.getY(), s * v.getZ());
  v.setX(nv.x());
  v.setY(nv.y());
  v.setZ(nv.z());
}

void recenterAndScale(CoordinateSystemConfiguration::Reader system, RealityResponse::Builder r) {
  float s = system.getScale() > 0.0f ? system.getScale() : 1.0f;
  auto op = toHVector(system.getOrigin().getPosition());
  auto oq = groundPlaneRotation(toQuaternion(system.getOrigin().getRotation()));
  auto o = cameraMotion(op, oq);

  // Camera position.
  recenterAndScale(r.getXRResponse().getCamera().getExtrinsic().getPosition(), o, s);
  recenterAndScale(r.getXRResponse().getCamera().getExtrinsic().getRotation(), oq);

  // VIO camera position.
  recenterAndScale(r.getPose().getExperimental().getVioTrackerTransform().getPosition(), o, s);
  recenterAndScale(r.getPose().getExperimental().getVioTrackerTransform().getRotation(), oq);

  // World points.
  for (auto p : r.getFeatureSet().getPoints()) {
    recenterAndScale(p.getPosition(), o, s);
  }

  // Image targets.
  for (auto t : r.getXRResponse().getDetection().getImages()) {
    recenterAndScale(t.getPlace().getPosition(), o, s);
    recenterAndScale(t.getPlace().getRotation(), oq);
    t.setWidthInMeters(t.getWidthInMeters() * s);
    t.setHeightInMeters(t.getHeightInMeters() * s);
  }

  // VPS Wayspot Anchor.
  for (auto t : r.getXRResponse().getDetection().getWayspotAnchors()) {
    recenterAndScale(t.getPlace().getPosition(), o, s);
    recenterAndScale(t.getPlace().getRotation(), oq);
  }

  // VPS Mesh.
  auto meshPlace = r.getXRResponse().getDetection().getMesh().getPlace();
  recenterAndScale(meshPlace.getPosition(), o, s);
  recenterAndScale(meshPlace.getRotation(), oq);

  // Surfaces.
  auto surfaceSet = r.getXRResponse().getSurfaces().getSet();
  for (auto sf : surfaceSet.getSurfaces()) {
    recenterAndScale(sf.getNormal().getPosition(), o, s);
    recenterAndScale(sf.getNormal().getRotation(), oq);
  }
  for (auto v : surfaceSet.getVertices()) {
    recenterAndScale(v, o, s);
  }
}

// Rotation correction doesn't include pitch.
void recenterAndScale(CameraCoordinates::Reader system, SemanticsResponse::Builder r) {
  float s = system.getScale() > 0.0f ? system.getScale() : 1.0f;
  auto op = toHVector(system.getOrigin().getPosition());
  auto oq = groundPlaneRotation(toQuaternion(system.getOrigin().getRotation()));
  auto o = cameraMotion(op, oq);

  // Camera position.
  recenterAndScale(r.getCamera().getExtrinsic().getPosition(), o, s);
  recenterAndScale(r.getCamera().getExtrinsic().getRotation(), oq);
}

void mirror(SurfaceVertex::Builder p) { p.setX(-p.getX()); }

void mirror(RealityResponse::Builder r) {
  // Camera position.
  mirror(r.getXRResponse().getCamera().getExtrinsic().getPosition());
  mirror(r.getXRResponse().getCamera().getExtrinsic().getRotation());

  // VIO camera position.
  mirror(r.getPose().getExperimental().getVioTrackerTransform().getPosition());
  mirror(r.getPose().getExperimental().getVioTrackerTransform().getRotation());

  // World points.
  for (auto p : r.getFeatureSet().getPoints()) {
    mirror(p.getPosition());
  }

  // Image targets.
  for (auto t : r.getXRResponse().getDetection().getImages()) {
    mirror(t.getPlace().getPosition());
    mirror(t.getPlace().getRotation());
  }

  // VPS Wayspot Anchor.
  for (auto t : r.getXRResponse().getDetection().getWayspotAnchors()) {
    mirror(t.getPlace().getPosition());
    mirror(t.getPlace().getRotation());
  }

  // VPS Mesh.
  auto meshPlace = r.getXRResponse().getDetection().getMesh().getPlace();
  mirror(meshPlace.getPosition());
  mirror(meshPlace.getRotation());

  // Surfaces.
  auto surfaceSet = r.getXRResponse().getSurfaces().getSet();
  for (auto sf : surfaceSet.getSurfaces()) {
    mirror(sf.getNormal().getPosition());
    mirror(sf.getNormal().getRotation());
  }
  for (auto v : surfaceSet.getVertices()) {
    mirror(v);
  }
}

void mirror(SemanticsResponse::Builder r) {
  // Camera position.
  mirror(r.getCamera().getExtrinsic().getPosition());
  mirror(r.getCamera().getExtrinsic().getRotation());
}

void recenterAndScale(CoordinateSystemConfiguration::Reader system, XrQueryResponse::Builder r) {
  float s = system.getScale() > 0.0f ? system.getScale() : 1.0f;
  auto op = toHVector(system.getOrigin().getPosition());
  auto oq = groundPlaneRotation(toQuaternion(system.getOrigin().getRotation()));
  auto o = cameraMotion(op, oq);

  for (auto h : r.getHitTest().getHits()) {
    recenterAndScale(h.getPlace().getPosition(), o, s);
    h.setDistance(h.getDistance() * s);
  }
}

void mirror(XrQueryResponse::Builder r) {
  for (auto h : r.getHitTest().getHits()) {
    mirror(h.getPlace().getPosition());
  }
}

// For face transformation
void recenterAndScale(CameraCoordinates::Reader system, FaceResponse::Builder r) {
  float s = system.getScale() > 0.0f ? system.getScale() : 1.0f;
  auto op = toHVector(system.getOrigin().getPosition());
  auto oq = groundPlaneRotation(toQuaternion(system.getOrigin().getRotation()));
  auto o = cameraMotion(op, oq);

  // Camera position.
  recenterAndScale(r.getCamera().getExtrinsic().getPosition(), o, s);
  recenterAndScale(r.getCamera().getExtrinsic().getRotation(), oq);

  // Update face transform and geometry
  for (auto face : r.getFaces()) {
    recenterAndScale(face.getTransform().getPosition(), o, s);
    recenterAndScale(face.getTransform().getRotation(), oq);
    face.getTransform().setScale(face.getTransform().getScale() * system.getScale());
  }

  // Mesh vertices and normals are already in local space.
}


void mirror(FaceResponse::Builder r) {
  // mirror the face transform and geometry
  for (auto face : r.getFaces()) {
    mirror(face.getTransform().getPosition());
    mirror(face.getTransform().getRotation());

    for (auto localPoint : face.getVertices()) {
      mirror(localPoint);
    }

    for (auto normal : face.getNormals()) {
      mirror(normal);
    }

    for (auto attachmentPoint : face.getAttachmentPoints()) {
      mirror(attachmentPoint.getPosition());
      mirror(attachmentPoint.getRotation());
    }

    // mirror the ears
    for (auto localPoint : face.getEarVertices()) {
      mirror(localPoint);
    }
    for (auto attachmentPoint : face.getEarAttachmentPoints()) {
      mirror(attachmentPoint.getPosition());
      mirror(attachmentPoint.getRotation());
    }
  }
}

}  // namespace

void deviceToPortraitOrientationPreprocess(
  AppContext::DeviceOrientation orientation, RealityRequest::Builder request) {
  switch (orientation) {
    case AppContext::DeviceOrientation::LANDSCAPE_RIGHT:
    case AppContext::DeviceOrientation::LANDSCAPE_LEFT:
      // Continue.
      break;
    default:
      // No correction needed.
      return;
  }
  if (request.getXRConfiguration().hasGraphicsIntrinsics()) {
    auto graphicsIntrinsics = request.getXRConfiguration().getGraphicsIntrinsics();
    auto w = graphicsIntrinsics.getTextureWidth();
    auto h = graphicsIntrinsics.getTextureHeight();
    graphicsIntrinsics.setTextureWidth(h);
    graphicsIntrinsics.setTextureHeight(w);
  }
}

void portraitToDeviceOrientationPostprocess(
  AppContext::DeviceOrientation orientation, RealityResponse::Builder r) {

  updateCameraIntrinsics(orientation, r.getXRResponse().getCamera().getIntrinsic());
  updateCameraExtrinsics(orientation, r);
}

void rewriteCoordinateSystemPostprocess(
  CoordinateSystemConfiguration::Reader system, RealityResponse::Builder response) {
  switch (system.getAxes()) {
    case CoordinateSystemConfiguration::CoordinateAxes::X_LEFT_Y_UP_Z_FORWARD:
      makeRightHanded(response);
      break;
    case CoordinateSystemConfiguration::CoordinateAxes::X_RIGHT_Y_UP_Z_FORWARD:
      // Already in the correct coordinate system.
      break;
    default:
      break;
  }

  // mirror coordinates if specified
  if (system.getMirroredDisplay()) {
    mirror(response);
  }

  recenterAndScale(system, response);
}

void rewriteCoordinateSystemPostprocess(
  CoordinateSystemConfiguration::Reader system, XrQueryResponse::Builder response) {
  switch (system.getAxes()) {
    case CoordinateSystemConfiguration::CoordinateAxes::X_LEFT_Y_UP_Z_FORWARD:
      makeRightHanded(response);
      break;
    case CoordinateSystemConfiguration::CoordinateAxes::X_RIGHT_Y_UP_Z_FORWARD:
      // Already in the correct coordinate system.
      break;
    default:
      break;
  }

  // mirror coordinates if specified
  if (system.getMirroredDisplay()) {
    mirror(response);
  }

  recenterAndScale(system, response);
}

// Adjust the coordinate system in the response based on the coordinate system specified in the
// request.  This is performed on a per-frame basis for the FaceResponse for variable data like
// vertices and normals.
void rewriteCoordinateSystemPostprocess(
  CameraCoordinates::Reader system, FaceResponse::Builder response) {
  switch (system.getAxes()) {
    case CameraCoordinates::Axes::RIGHT_HANDED:
      makeRightHanded(response);
      break;
    case CameraCoordinates::Axes::LEFT_HANDED:
      // Already in the correct coordinate system.
      break;
    default:
      break;
  }

  // mirror coordinates if specified
  if (system.getMirroredDisplay()) {
    mirror(response);
  }

  recenterAndScale(system, response);
}

void rewriteCoordinateSystemPostprocess(
  CameraCoordinates::Reader system, SemanticsResponse::Builder response) {
  switch (system.getAxes()) {
    case CameraCoordinates::Axes::RIGHT_HANDED:
      makeRightHanded(response);
      break;
    case CameraCoordinates::Axes::LEFT_HANDED:
      // Already in the correct coordinate system.
      break;
    default:
      break;
  }

  // mirror coordinates if specified
  if (system.getMirroredDisplay()) {
    mirror(response);
  }

  recenterAndScale(system, response);
}

// Adjust the coordinate system in the response based on the coordinate system specified in the
// request.  This is performed only once for the FrameworkResponse for static face data such as
// indices and uvs.
void rewriteCoordinateSystemFrameworkPostprocess(
  CameraCoordinates::Reader system, FrameworkResponse::Builder response) {
  switch (system.getAxes()) {
    case CameraCoordinates::Axes::RIGHT_HANDED:
      makeRightHanded(response);
      break;
    case CameraCoordinates::Axes::LEFT_HANDED:
      // Already in the correct coordinate system.
      break;
    default:
      break;
  }

  // When we mirror the image, we need to alter the indices to make sure they maintain their
  // current clockwise / counter-clockwise ordering.
  // We also need to mirror the UVs.  That way whatever the texture artist created on the texture
  // map continues to go from left to right.
  if (system.getMirroredDisplay()) {
    mirror(response);
  }
}

void makeRightHanded(Position32f::Builder p) { p.setZ(-p.getZ()); }

void makeRightHanded(Quaternion32f::Builder q) {
  q.setW(-q.getW());
  q.setZ(-q.getZ());
}

void recenterAndScale(Quaternion32f::Builder q, Quaternion o) {
  Quaternion nq = o.times(Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  setQuaternion32f(nq.w(), nq.x(), nq.y(), nq.z(), &q);
}

void recenterAndScale(Position32f::Builder p, const HMatrix &o, float s) {
  HPoint3 np = o * HPoint3(s * p.getX(), s * p.getY(), s * p.getZ());
  setPosition32f(np.x(), np.y(), np.z(), &p);
}

void mirror(Position32f::Builder p) { p.setX(-p.getX()); }

void mirror(Quaternion32f::Builder q) {
  q.setY(-1.0f * q.getY());
  q.setZ(-1.0f * q.getZ());
}

}  // namespace c8
