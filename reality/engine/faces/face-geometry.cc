// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "face-geometry.h",
  };
  deps = {
    "//c8:vector",
    "//c8/geometry:face-types",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:mesh",
    "//c8/geometry:mesh-types",
    "//c8/stats:scope-timer",
    "//reality/engine/geometry:bundle",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xbb488d25);

#include <algorithm>

#include "c8/c8-log.h"
#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/mesh-types.h"
#include "c8/geometry/mesh.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/faces/face-geometry.h"
#include "reality/engine/geometry/bundle.h"

namespace c8 {

namespace {

// We need something like an intrinsic matrix for clip space. This should preserve the z channel
// and use the homogeneous coordinate for translation.
HMatrix clipIntrinsic(const c8_PixelPinholeCameraModel &m) noexcept {
  auto a = m.focalLengthHorizontal;
  auto b = -1.0f * m.focalLengthVertical;
  auto x = m.centerPointX;
  auto y = m.centerPointY;
  auto ai = 1.0f / a;
  auto bi = 1.0f / b;
  auto xi = -x * ai;
  auto yi = -y * bi;
  return HMatrix{
    {a, 0.0000000f, 0.0f, x},
    {0.0000000f, b, 0.0f, y},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {ai, 0.00000f, 0.0f, xi},
    {0.00000f, bi, 0.0f, yi},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
  };
}

HMatrix texToClip() {
  return {
    {2.0f, 0.0f, 0.0f, -1.0f},  // row 0
    {0.0f, 2.0f, 0.0f, -1.0f},  // row 1
    {0.0f, 0.0f, 1.0f, 0.0f},   // row 2
    {0.0f, 0.0f, 0.0f, 1.0f},   // row 3
    {0.5f, 0.0f, 0.0f, 0.5f},   // irow 0
    {0.0f, 0.5f, 0.0f, 0.5f},   // irow 1
    {0.0f, 0.0f, 1.0f, 0.0f},   // irow 2
    {0.0f, 0.0f, 0.0f, 1.0f},   // irow 3
  };
}

HMatrix texToRay(const c8_PixelPinholeCameraModel &k) {
  return clipIntrinsic(k).inv()
    * HMatrixGen::scale(k.pixelsWidth - 1.0f, k.pixelsHeight - 1.0f, 1.0f);
}

}  // namespace

// Transform from rendered texture (in 0-1) to image texture (in 0-1).
HMatrix renderTexToImageTex(const DetectionRoi &roi) {
  // Convert texture to clip, then apply inverse roi, then transform back to texture space.
  return texToClip().inv() * roi.warp.inv() * texToClip();
}

// Transform from rendered texture in fromRoi to rendered texture in toRoi
HMatrix renderTexToRenderTex(const DetectionRoi &fromRoi, const DetectionRoi &toRoi) {
  return texToClip().inv() * toRoi.warp * fromRoi.warp.inv() * texToClip();
}

// Transform from rendered texture (in 0-1) to ray space.
HMatrix renderTexToRaySpace(const DetectionRoi &roi, const c8_PixelPinholeCameraModel &k) {
  // Convert texture to clip, then apply inverse roi, then transform back to texture space.
  return texToRay(k) * renderTexToImageTex(roi);
}

namespace {

Vector<HPoint3> renderTextureToImageTexture(
  const DetectionRoi &roi, const Vector<HPoint3> &points) {
  return renderTexToImageTex(roi) * points;
}

Vector<HPoint3> renderTextureToRaySpace(
  const DetectionRoi &roi, const c8_PixelPinholeCameraModel &k, const Vector<HPoint3> &pts) {

  return renderTexToRaySpace(roi, k) * pts;
}

}  // namespace

BoundingBox uprightBBox(const BoundingBox &box1) {
  auto maxY =
    std::max({box1.upperLeft.y(), box1.upperRight.y(), box1.lowerLeft.y(), box1.lowerRight.y()});
  auto minY =
    std::min({box1.upperLeft.y(), box1.upperRight.y(), box1.lowerLeft.y(), box1.lowerRight.y()});
  auto maxX =
    std::max({box1.upperLeft.x(), box1.upperRight.x(), box1.lowerLeft.x(), box1.lowerRight.x()});
  auto minX =
    std::min({box1.upperLeft.x(), box1.upperRight.x(), box1.lowerLeft.x(), box1.lowerRight.x()});

  return {
    {minX, maxY},
    {maxX, maxY},
    {minX, minY},
    {maxX, minY},
  };
}

float intersectionOverUnion(const BoundingBox &box1, const BoundingBox &box2) {
  // Compute the coordinates of the intersection rectangle.
  float x1 = std::max(box1.upperLeft.x(), box2.upperLeft.x());
  float y1 = std::min(box1.upperLeft.y(), box2.upperLeft.y());
  float x2 = std::min(box1.lowerRight.x(), box2.lowerRight.x());
  float y2 = std::max(box1.lowerRight.y(), box2.lowerRight.y());

  // Compute the width and height of the intersection rectangle.
  float intersectionWidth = std::max(0.0f, x2 - x1);
  // This operates in a world where y+ is up, so this method wouldn't work in a y+ is down such as
  // pixel space.
  float intersectionHeight = std::max(0.0f, y1 - y2);

  // Compute the area of intersection.
  float intersectionArea = intersectionWidth * intersectionHeight;

  // Compute the area of the two bounding boxes.
  float box1Area = (box1.width()) * (box1.height());
  float box2Area = (box2.width()) * (box2.height());

  // Compute the area of the union.
  float unionArea = box1Area + box2Area - intersectionArea;

  // Compute the Intersection over Union.
  if (unionArea == 0) {
    return 0;
  }
  return intersectionArea / unionArea;
}

float minBBoxDistance(const BoundingBox &a, const BoundingBox &b) {
  auto left = std::max(a.lowerLeft.x(), b.lowerLeft.x());
  auto right = std::min(a.upperRight.x(), b.upperRight.x());
  auto top = std::min(a.upperLeft.y(), b.upperLeft.y());
  auto bottom = std::max(a.lowerLeft.y(), b.lowerLeft.y());

  auto x_overlap = std::max(0.0f, right - left);
  auto y_overlap = std::max(0.0f, top - bottom);

  if (x_overlap > 0.f && y_overlap > 0.f) {
    return 0.f;  // The bounding boxes are overlapping.
  }

  auto x_distance = x_overlap > 0.f
    ? 0.f
    : std::min(
      std::abs(a.upperRight.x() - b.lowerLeft.x()), std::abs(b.upperRight.x() - a.lowerLeft.x()));
  auto y_distance = y_overlap > 0.f
    ? 0.f
    : std::min(
      std::abs(a.upperLeft.y() - b.lowerLeft.y()), std::abs(b.upperLeft.y() - a.lowerLeft.y()));

  return std::sqrt(x_distance * x_distance + y_distance * y_distance);
}

void calculateBoundingCube(const Vector<HPoint3> &points, HPoint3 *minVal, HPoint3 *maxVal) {
  *minVal = {
    std::numeric_limits<float>::max(),
    std::numeric_limits<float>::max(),
    std::numeric_limits<float>::max(),
  };
  *maxVal = {
    -std::numeric_limits<float>::max(),
    -std::numeric_limits<float>::max(),
    -std::numeric_limits<float>::max(),
  };

  for (const auto &val : points) {
    *minVal = {
      std::min(val.x(), minVal->x()),
      std::min(val.y(), minVal->y()),
      std::min(val.z(), minVal->z()),
    };
    *maxVal = {
      std::max(val.x(), maxVal->x()),
      std::max(val.y(), maxVal->y()),
      std::max(val.z(), maxVal->z()),
    };
  }
}

// Return the corners of roi region in the source image.
Vector<HPoint2> roiCornersInImage(const DetectedPoints &pts) {
  return flatten<2>(renderTextureToImageTexture(
    pts.roi,
    {
      {0.0f, 0.0f, 1.0f},
      {0.0f, 1.0f, 1.0f},
      {1.0f, 1.0f, 1.0f},
      {1.0f, 0.0f, 1.0f},
    }));
}

DetectionRoi detectionRoiNoPadding(const DetectedPoints &d) {
  auto bb = d.boundingBox;

  // Convert texture to ray space, so we can work with un-warped corners.
  auto texToRayMat = renderTexToRaySpace(d.roi, d.intrinsics);
  Vector<HPoint2> corners{bb.upperLeft, bb.upperRight, bb.lowerLeft, bb.lowerRight};
  auto rayCorners = texToRayMat * extrude<3>(corners);

  // Find the transform that centers the bounding box around the origin.
  float cx = .25f * (rayCorners[0].x() + rayCorners[1].x() + rayCorners[2].x() + rayCorners[3].x());
  float cy = .25f * (rayCorners[0].y() + rayCorners[1].y() + rayCorners[2].y() + rayCorners[3].y());
  auto center = HMatrixGen::translation(-cx, -cy, 0);
  auto centerRayCorners = center * rayCorners;

  // Find the z-axis rotation that will make the box upright in ray space and rotate the points.
  // -- atan2 takes (y, x)
  // -- we want to undo the box rotation
  // -- Here we are assuming the box is rectangular, so we just look at rotation of the top edge.
  auto zRotRad = -std::atan2(
    centerRayCorners[1].y() - centerRayCorners[0].y(),
    centerRayCorners[1].x() - centerRayCorners[0].x());
  auto rotate = HMatrixGen::zRadians(zRotRad);
  auto rotatedRayCorners = rotate * centerRayCorners;

  // Convert them back to clip space.
  auto clip = texToClip() * texToRay(d.intrinsics).inv();
  auto rotatedClipCorners = clip * rotatedRayCorners;

  // Stretch the bottom right corner of the box to clip coordinate (1, 1)
  auto scale =
    HMatrixGen::scale(1.0f / rotatedClipCorners[3].x(), 1.0f / rotatedClipCorners[3].y(), 1.0f);

  // Compute and return the final transform.
  // -- (1) clip space to texture space.
  // -- (2) texture space to ray space.
  // -- (3) center box.
  // -- (4) rotate the box.
  // -- (5) bring it back to clip space.
  // -- (6) stretch it to fill the viewport.
  return {
    ImageRoi::Source::FACE,
    0,
    "",
    scale * clip * rotate * center * texToRay(d.intrinsics) * texToClip().inv()};
}

DetectionRoi detectionRoi(const DetectedPoints &detection) {
  auto roi = detectionRoiNoPadding(detection);
  roi.warp = HMatrixGen::scale(1.0f / 1.5f, 1.0f / 1.5f, 1.0f) * roi.warp;
  return roi;
}

// Maps from a point in the rendered viewport (0,1) to a point in 2d ray space. The z value is
// preserved in this transformation for later processing, but the points should not be interpreted
// directly as 3d points.
DetectedRayPoints detectionToRaySpace(const DetectedPoints &dt) {
  DetectedImagePoints pts = dt;
  pts.points = renderTextureToRaySpace(dt.roi, dt.intrinsics, dt.points);

  // transform boundingBox to ray space
  auto &bb = pts.boundingBox;
  Vector<HPoint3> bbpts = {
    {bb.upperLeft.x(), bb.upperLeft.y(), 1.0f},    // upper left
    {bb.upperRight.x(), bb.upperRight.y(), 1.0f},  // upper right
    {bb.lowerLeft.x(), bb.lowerLeft.y(), 1.0f},    // lower left
    {bb.lowerRight.x(), bb.lowerRight.y(), 1.0f},  // lower right
  };

  // Use "truncate" instead of "flatten" to discard side channel depth data.
  auto tbbpts = truncate<2>(renderTextureToRaySpace(dt.roi, dt.intrinsics, bbpts));
  pts.boundingBox = {
    tbbpts[0],  // upper left
    tbbpts[1],  // upper right
    tbbpts[2],  // lower left
    tbbpts[3],  // lower right
  };

  // transform extended bbox to ray space
  auto &exBb = pts.extendedBoundingBox;
  if (exBb.width() > 0.0f && exBb.height() > 0.0f) {
    Vector<HPoint3> exbbpts = {
      {exBb.upperLeft.x(), exBb.upperLeft.y(), 1.0f},    // upper left
      {exBb.upperRight.x(), exBb.upperRight.y(), 1.0f},  // upper right
      {exBb.lowerLeft.x(), exBb.lowerLeft.y(), 1.0f},    // lower left
      {exBb.lowerRight.x(), exBb.lowerRight.y(), 1.0f},  // lower right
    };

    // Use "truncate" instead of "flatten" to discard side channel depth data.
    auto tExbbpts = truncate<2>(renderTextureToRaySpace(dt.roi, dt.intrinsics, exbbpts));
    pts.extendedBoundingBox = {
      tExbbpts[0],  // upper left
      tExbbpts[1],  // upper right
      tExbbpts[2],  // lower left
      tExbbpts[3],  // lower right
    };
  }

  return pts;
}

// Maps from a point in the rendered viewport (0,1) to a point in the image (0, 1) by using the
// geometry of the roi in the source image.
DetectedImagePoints detectionToImageSpace(const DetectedPoints &dt) {
  DetectedImagePoints pts = dt;
  pts.points = renderTextureToImageTexture(dt.roi, dt.points);

  auto bb = pts.boundingBox;
  Vector<HPoint3> bbpts = {
    {bb.upperLeft.x(), bb.upperLeft.y(), 1.0f},    // upper left
    {bb.upperRight.x(), bb.upperRight.y(), 1.0f},  // upper right
    {bb.lowerLeft.x(), bb.lowerLeft.y(), 1.0f},    // lower left
    {bb.lowerRight.x(), bb.lowerRight.y(), 1.0f},  // lower right
  };

  // Use "truncate" instead of "flatten" to discard side channel depth data.
  auto tbbpts = truncate<2>(renderTextureToImageTexture(dt.roi, bbpts));
  pts.boundingBox = {
    tbbpts[0],  // upper left
    tbbpts[1],  // upper right
    tbbpts[2],  // lower left
    tbbpts[3],  // lower right
  };

  return pts;
}

Vector<DetectedImagePoints> detectionToImageSpace(const Vector<DetectedPoints> &pts) {
  Vector<DetectedImagePoints> impts;
  std::transform(
    pts.begin(),
    pts.end(),
    std::back_inserter(impts),
    [](const DetectedPoints &p) -> DetectedImagePoints { return detectionToImageSpace(p); });
  return impts;
}

// Takes the detections and puts them into the space of the hand crop, 0 to 1 space.
BoundingBox detectionBoundingBoxToImageSpace(const BoundingBox &bbox, const DetectionRoi &roi) {
  Vector<HPoint3> bbpts = {
    {bbox.upperLeft.x(), bbox.upperLeft.y(), 1.0f},    // upper left
    {bbox.upperRight.x(), bbox.upperRight.y(), 1.0f},  // upper right
    {bbox.lowerLeft.x(), bbox.lowerLeft.y(), 1.0f},    // lower left
    {bbox.lowerRight.x(), bbox.lowerRight.y(), 1.0f},  // lower right
  };

  // Use "truncate" instead of "flatten" to discard side channel depth data.
  auto tbbpts = truncate<2>(renderTextureToImageTexture(roi, bbpts));
  BoundingBox bb = {
    tbbpts[0],  // upper left
    tbbpts[1],  // upper right
    tbbpts[2],  // lower left
    tbbpts[3],  // lower right
  };
  return bb;
}

FaceAnchorTransform computeAnchorTransform(const DetectedRayPoints &rayPoints) {
  auto rayCenter = rayPoints.boundingBox.center();
  const float rayCenterX = rayCenter.x();
  const float rayCenterY = rayCenter.y();

  const auto rayDistance = FACE_WIDTH / rayPoints.boundingBox.width();
  HPoint3 rayCenter3 = {rayCenterX * rayDistance, rayCenterY * rayDistance, rayDistance};

  // scaledWidth, scaledHeight, and scaledDepth come from face-model-stats.cc.
  return {
    rayCenter3,
    {0, 0, 1, 0},  // rotated 180 degrees to face the camera
    FACE_WIDTH,    // scale
    1.00f,         // scaledWidth
    1.75f,         // scaledHeight
    1.15f,         // scaledDepth
  };
}

Vector<HPoint3> worldPoints(
  const DetectedRayPoints &rayPoints,
  const FaceAnchorTransform &headAnchor,
  const DetectedPoints &pts) {
  // The ratio between the facemesh width and facemesh depth is fixed.  Facemesh z output gives the
  // offset relative to the center z of the mesh.  However much we are scaling the width, we should
  // also scale the z.  The scaled z will give us the depth offset of each point.
  float zScaling = FACE_WIDTH / pts.boundingBox.width();

  // scale the points so they are on the same plane as the head transform.  This scaling is what
  // converts the points from ray space to world space.
  Vector<HPoint3> worldPoints;
  worldPoints.resize(rayPoints.points.size());

  for (int i = 0; i < rayPoints.points.size(); i++) {
    float offset = rayPoints.points[i].z() * zScaling;
    float zDepth = headAnchor.position.z() + offset;
    worldPoints[i] = {rayPoints.points[i].x() * zDepth, rayPoints.points[i].y() * zDepth, zDepth};
  }

  return worldPoints;
}

Vector<HPoint3> worldToLocal(
  const Vector<HPoint3> &worldPoints, const FaceAnchorTransform &anchor) {
  return anchorMatrix(anchor).inv() * worldPoints;
}

HMatrix anchorMatrix(const FaceAnchorTransform &t) {
  return HMatrixGen::translation(t.position.x(), t.position.y(), t.position.z())
    * t.rotation.toRotationMat() * HMatrixGen::scale(t.scale, t.scale, t.scale);
}

// Adjust the refence anchor based on a computed pose in the anchor space.
FaceAnchorTransform adjustAnchor(const FaceAnchorTransform &anchor, const HMatrix &localPose) {
  auto headTransform = anchor;
  headTransform.position = anchorMatrix(anchor) * localPose * HPoint3{0.0f, 0.0f, 0.0f};
  headTransform.rotation = anchor.rotation.times(Quaternion::fromHMatrix(localPose));
  return headTransform;
}

Vector<HPoint3> computeHeadMesh(
  const DetectedRayPoints &rayPoints,
  const FaceAnchorTransform &headAnchor,
  const HMatrix &headAnchorTransform,
  const DetectedPoints &pts) {
  return worldToLocal(worldPoints(rayPoints, headAnchor, pts), headAnchor);
}

// Returns a subset of the face points that will be used for determining the pose
Vector<HPoint3> getPosePointsSubset(const Vector<HPoint3> &facePoints) {
  return getSubset(facePoints, FACE_POSE_INDICES);
}

// Returns a subset of the face points that will be used for determining the pose
Vector<HPoint3> getSubset(const Vector<HPoint3> &points, const Vector<int> &subsetIndices) {
  Vector<HPoint3> pointSubset;
  pointSubset.reserve(subsetIndices.size());

  for (int subsetIndex : subsetIndices) {
    pointSubset.push_back(points[subsetIndex]);
  }

  return pointSubset;
}

// Create a face anchor
AttachmentPoint createAttachmentPoint(
  const AttachmentPointMsg::AttachmentName &name,
  int index,
  const Vector<HPoint3> &localVertices,
  const Vector<HVector3> &vertexNormals) {
  return createAttachmentPoint(name, localVertices[index], vertexNormals[index]);
}

AttachmentPoint createAttachmentPoint(
  const AttachmentPointMsg::AttachmentName &name,
  const HPoint3 &localVertex,
  const HVector3 &vertexNormal) {
  // rotMat solves for the rotation from mesh surface points to face-local points such that
  // such that points aligned relative to the mesh surface can be rotated to be aligned with the
  // face, i.e. localSpacePts = rotMat * meshSpacePts. This solution is ambgiguous and unstable
  // along the y-axis, meaning that rotMat * HMatrixGen::yRadians(angle) is an equally valid
  // solution for all angles. In order for this to be stable and usable, we need to solve for the
  // angle such that mesh space and texture space are aligned. Given the vertex point (0,0,0) in
  // mesh local space and its corresponding (u, v), we want the mesh local point (dx, 0, dz) to
  // correspond to (u + s * du, v + s * dv) for some s.
  auto rotMat = HMatrixGen::unitRotationAlignment({0.0f, 1.0f, 0.0f}, vertexNormal);

  return {name, localVertex, Quaternion::fromHMatrix(rotMat)};
}

// Create a face anchor given two indices that are averaged together.  We use this when we want a
// face anchor at a spot where there isn't a point, such as the center of the mouth or eyes.
AttachmentPoint createAttachmentPoint(
  const AttachmentPointMsg::AttachmentName &name,
  int indexA,
  int indexB,
  const Vector<HPoint3> &localVertices,
  const Vector<HVector3> &vertexNormals) {

  const auto pointA = localVertices[indexA];
  const auto pointB = localVertices[indexB];
  const HPoint3 averagedPoint = {
    (pointA.x() + pointB.x()) * 0.5f,
    (pointA.y() + pointB.y()) * 0.5f,
    (pointA.z() + pointB.z()) * 0.5f};

  const HVector3 averagedNormal = 0.5f * (vertexNormals[indexA] + vertexNormals[indexA]);

  auto rotMat = HMatrixGen::unitRotationAlignment({0.0f, 1.0f, 0.0f}, averagedNormal);

  // rotMat solves for the rotation from mesh surface points to face-local points such that
  // such that points aligned relative to the mesh surface can be rotated to be aligned with the
  // face, i.e. localSpacePts = rotMat * meshSpacePts. This solution is ambgiguous and unstable
  // along the y-axis, meaning that rotMat * HMatrixGen::yRadians(angle) is an equally valid
  // solution for all angles. In order for this to be stable and usable, we need to solve for the
  // angle such that mesh space and texture space are aligned. Given the vertex point (0,0,0) in
  // mesh local space and its corresponding (u, v), we want the mesh local point (dx, 0, dz) to
  // correspond to (u + s * du, v + s * dv) for some s.
  return {name, averagedPoint, Quaternion::fromHMatrix(rotMat)};
}

// Gets key anchor positions and rotations
Vector<AttachmentPoint> getAttachmentPoints(
  const Vector<HPoint3> &localVertices, const Vector<HVector3> &vertexNormals) {

  return {
    // top of the face
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::FOREHEAD,
      FACEMESH_FOREHEAD,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_EYEBROW_INNER,
      FACEMESH_R_EYEBROW_INNER,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_EYEBROW_MIDDLE,
      FACEMESH_R_EYEBROW_MIDDLE,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_EYEBROW_OUTER,
      FACEMESH_R_EYEBROW_OUTER,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_EYEBROW_INNER,
      FACEMESH_L_EYEBROW_INNER,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_EYEBROW_MIDDLE,
      FACEMESH_L_EYEBROW_MIDDLE,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_EYEBROW_OUTER,
      FACEMESH_L_EYEBROW_OUTER,
      localVertices,
      vertexNormals),

    // sides of the face
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_EAR, FACEMESH_L_EAR, localVertices, vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_EAR, FACEMESH_R_EAR, localVertices, vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_CHEEK,
      FACEMESH_L_CHEEK,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_CHEEK,
      FACEMESH_R_CHEEK,
      localVertices,
      vertexNormals),

    // center of the face
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::NOSE_BRIDGE,
      FACEMESH_NOSE_TOP,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::NOSE_TIP, FACEMESH_NOSE, localVertices, vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_EYE,
      FACEMESH_UPPER_L_EYE,
      FACEMESH_LOWER_L_EYE,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_EYE,
      FACEMESH_UPPER_R_EYE,
      FACEMESH_LOWER_R_EYE,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_EYE_OUTER_CORNER,
      FACEMESH_L_EYE_OUTER_CORNER,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_EYE_OUTER_CORNER,
      FACEMESH_R_EYE_OUTER_CORNER,
      localVertices,
      vertexNormals),

    // bottom of the face
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::UPPER_LIP,
      FACEMESH_UPPER_LIP,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LOWER_LIP,
      FACEMESH_LOWER_LIP,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::MOUTH,
      FACEMESH_UPPER_LIP,
      FACEMESH_LOWER_LIP,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::MOUTH_RIGHT_CORNER,
      FACEMESH_MOUTH_R_CORNER,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::MOUTH_LEFT_CORNER,
      FACEMESH_MOUTH_L_CORNER,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::CHIN, FACEMESH_CHIN, localVertices, vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_IRIS, FACEMESH_L_IRIS, localVertices, vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_IRIS,
      FACEMESH_R_IRIS,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_UPPER_EYELID,
      FACEMESH_UPPER_L_EYE,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_UPPER_EYELID,
      FACEMESH_UPPER_R_EYE,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::LEFT_LOWER_EYELID,
      FACEMESH_LOWER_L_EYE,
      localVertices,
      vertexNormals),
    createAttachmentPoint(
      AttachmentPointMsg::AttachmentName::RIGHT_LOWER_EYELID,
      FACEMESH_LOWER_R_EYE,
      localVertices,
      vertexNormals),
  };
}

// Converts a detected point into 3D space.  This function should not be used in production, but
// should only be used when you want a Face3d struct that doesn't contain smoothing of the points
// via filtering.  Otherwise, you should be using TrackedFaceState::locateFace.
Face3d detectionToMeshNoFilter(const DetectedPoints &faceDetection) {
  ScopeTimer t("detection-to-mesh");

  // Convert detections to camera ray space.
  const auto raySpaceDetection = detectionToRaySpace(faceDetection);

  // Get the reference anchor for this detection result for the detected face position and size.
  auto referenceTransform = computeAnchorTransform(raySpaceDetection);

  // Use the refence anchor to compute the location of the mesh points in 3d relative to the camera.
  auto worldVertices = worldPoints(raySpaceDetection, referenceTransform, faceDetection);

  // Convert world points to the anchor's local space.
  auto vertices = worldToLocal(worldVertices, referenceTransform);

  auto localPose = HMatrixGen::i();

  // by only using a subset of the points, we're able to get ~37x performance boost
  poseEstimationFull3d(
    getPosePointsSubset(vertices), getPosePointsSubset(FACEMESH_SAMPLE_VERTICES), &localPose);

  // Adjust the refence anchor based on the computed pose in the anchor space.
  auto headTransform = adjustAnchor(referenceTransform, localPose);

  // Re-compute local points with rotation in head transform.
  vertices = worldToLocal(worldVertices, headTransform);

  Vector<HVector3> normals;
  computeVertexNormals(vertices, FACEMESH_INDICES, &normals);

  // Get face anchors
  const auto attachmentPoints = getAttachmentPoints(vertices, normals);
  return {
    headTransform, vertices, normals, Face3d::TrackingStatus::UNSPECIFIED, -1, attachmentPoints};
}

Vector<MeshIndices> meshIndicesFromMeshGeometry(const FaceMeshGeometryConfig &config) {
  Vector<MeshIndices> userIndices;
  if (config.showFace) {
    userIndices.insert(
      userIndices.end(),
      FACEMESH_INDICES.begin(),
      FACEMESH_INDICES.begin() + FACEMESH_EYE_INDICES_START);
  }
  if (config.showEyes) {
    userIndices.insert(
      userIndices.end(),
      FACEMESH_INDICES.begin() + FACEMESH_EYE_INDICES_START,
      FACEMESH_INDICES.begin() + FACEMESH_MOUTH_INDICES_START);
  }
  // TODO(nathan): figure out how to make the iris render behind the face mesh but in front of the
  // "eye" indices.
  if (config.showIris) {
    userIndices.insert(
      userIndices.end(),
      FACEMESH_INDICES.begin() + FACEMESH_IRIS_INDICES_START,
      FACEMESH_INDICES.end());
  }
  if (config.showMouth) {
    userIndices.insert(
      userIndices.end(),
      FACEMESH_INDICES.begin() + FACEMESH_MOUTH_INDICES_START,
      FACEMESH_INDICES.begin() + FACEMESH_IRIS_INDICES_START);
  }

  return userIndices;
}

}  // namespace c8
