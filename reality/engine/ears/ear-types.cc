// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "ear-types.h",
  };
  deps = {
    "//c8:map",
    "//c8:parameter-data",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:intrinsics",
    "//c8/geometry:two-d",
    "//c8/pixels/render:object8",
    "//c8/geometry:face-types",
    "//reality/engine/faces:face-geometry",
    "//reality/engine/tracking:ray-point-filter",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x9a938b94);

#include "c8/geometry/egomotion.h"
#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/two-d.h"
#include "c8/parameter-data.h"
#include "c8/pixels/render/object8.h"
#include "reality/engine/ears/ear-types.h"
#include "reality/engine/faces/face-geometry.h"
#include "reality/engine/tracking/ray-point-filter.h"

// see MediapipePoints.CROP_BOX_ANCHOR_IDXS in -
// https://gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com/niantic-ar/research/face/-/blob/main/src/face/datasets/cropping.py#L39
const Vector<int> anchorIndicesLeft = {FACEMESH_L_EAR_LOW, FACEMESH_L_EAR};   // (361, 356)
const Vector<int> anchorIndicesRight = {FACEMESH_R_EAR_LOW, FACEMESH_R_EAR};  // (132, 127)

// face mesh vertex indices used for lift3D
// search for 'MediapipePairs' in
// https://gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com/niantic-ar/research/face/-/blob/main/src/face/utils/lift_3d.py
extern const c8::Vector<c8::Vector<int>> EAR_SIDES_MODELS_LEFT = {
  {389, 368},
  {356, 264},
  {454, 447},
  {323, 366},
  {361, 401},
};
extern const c8::Vector<c8::Vector<int>> EAR_SIDES_MODELS_RIGHT = {
  {162, 139},
  {127, 34},
  {234, 227},
  {93, 137},
  {132, 177},
};

namespace c8 {

namespace {
// Filters for reference points on the face mesh
RayPointFilterConfig filterConfig = createRayPointFilterConfig();
HashMap<int, Vector<RayPointFilter3>> faceIdToEarAnchorFilters;

struct Settings {
  bool filterEarAnchors;
  float earLandmarkDetectionCropAlpha;
  int averageSamples;
  float vertexTrustableThreshold;
};

const Settings &settings() {
  static const Settings settings_{
    globalParams().getOrSet("EarTypes.filterEarAnchors", false),
    globalParams().getOrSet("EarTypes.earLandmarkDetectionCropAlpha", 0.6f),
    globalParams().getOrSet("EarTypes.averageSamples", 20),
    globalParams().getOrSet("EarTypes.vertexTrustableThreshold", 0.6f),
  };
  return settings_;
}

void computeEarBoundingBox(
  const HPoint3 &m0,
  const HPoint3 &m1,
  const HMatrix &texToRayM,
  const bool isLeftEar,
  DetectedPoints *ear) {
  if (!ear) {
    return;
  }

  // extension ratio
  const float halfR = 0.5 + settings().earLandmarkDetectionCropAlpha;

  float length = m0.dist(m1);
  float width = length * EAR_LANDMARK_DETECTION_INPUT_ASPECT_RATIO;

  HVector2 v(m1.x() - m0.x(), m1.y() - m0.y());
  v = v.unit();
  // rotate counter-clockwise 90 degree for left ear
  // and clockwise 90 degree for right ear
  HVector2 w;
  if (isLeftEar) {
    w = HVector2(-v.y(), v.x());
  } else {
    w = HVector2(v.y(), -v.x());
  }

  HPoint2 m0m1Center((m0.x() + m1.x()) / 2, (m0.y() + m1.y()) / 2);
  float cx = m0m1Center.x() + w.x() * width / 2;
  float cy = m0m1Center.y() + w.y() * width / 2;

  // atan2 takes arguments: y, x
  auto zRotRad = std::atan2(m1.y() - m0.y(), m1.x() - m0.x());
  zRotRad = M_PI_2 + zRotRad;

  auto transformForBox =
    HMatrixGen::zRadians(-zRotRad) * HMatrixGen::translation(-cx, -cy, 1.0f) * texToRayM;

  // get ear bbox
  Vector<HPoint3> boxRays = {
    {-halfR * width, halfR * length, 1.0f},   // lower left
    {halfR * width, halfR * length, 1.0f},    // lower right
    {-halfR * width, -halfR * length, 1.0f},  // upper left
    {halfR * width, -halfR * length, 1.0f},   // upper right
  };

  auto boxCorners = transformForBox.inv() * boxRays;

  // Return the rectangular bounding box from ear. Note that this will get drawn to a
  // square region for processing, with distortion, but that is what ear wants.
  ear->boundingBox = {
    {boxCorners[0].x(), boxCorners[0].y()},  // upper left
    {boxCorners[1].x(), boxCorners[1].y()},  // upper right
    {boxCorners[2].x(), boxCorners[2].y()},  // lower left
    {boxCorners[3].x(), boxCorners[3].y()},  // lower right
  };
}

BoundingBox transformBoundingBox(const HMatrix &m, const BoundingBox &bb) {
  // get the list of corner points
  Vector<HPoint3> bbpts = {
    {bb.upperLeft.x(), bb.upperLeft.y(), 1.0f},    // upper left
    {bb.upperRight.x(), bb.upperRight.y(), 1.0f},  // upper right
    {bb.lowerLeft.x(), bb.lowerLeft.y(), 1.0f},    // lower left
    {bb.lowerRight.x(), bb.lowerRight.y(), 1.0f},  // lower right
  };

  // Use "truncate" instead of "flatten" to discard side channel depth data.
  auto tbbpts = truncate<2>(m * bbpts);
  return {
    tbbpts[0],  // upper left
    tbbpts[1],  // upper right
    tbbpts[2],  // lower left
    tbbpts[3],  // lower right
  };
}

DetectedPoints transformFromFaceRoiToEarRoi(
  const DetectedPoints &detInFaceRoi, const DetectionRoi &earRoi) {
  DetectedPoints d = detInFaceRoi;

  const HMatrix m = renderTexToRenderTex(detInFaceRoi.roi, earRoi);

  d.roi.warp = earRoi.warp;
  d.points = m * detInFaceRoi.points;

  // transform boundingBox to earRoi
  d.boundingBox = transformBoundingBox(m, detInFaceRoi.boundingBox);
  d.extendedBoundingBox = transformBoundingBox(m, detInFaceRoi.extendedBoundingBox);

  return d;
}

HMatrix texToClipYFlip() {
  return {
    {2.0f, 0.0f, 0.0f, -1.0f},  // row 0
    {0.0f, -2.0f, 0.0f, 1.0f},  // row 1
    {0.0f, 0.0f, 1.0f, 0.0f},   // row 2
    {0.0f, 0.0f, 0.0f, 1.0f},   // row 3
    {0.5f, 0.0f, 0.0f, 0.5f},   // irow 0
    {0.0f, -0.5f, 0.0f, 0.5f},  // irow 1
    {0.0f, 0.0f, 1.0f, 0.0f},   // irow 2
    {0.0f, 0.0f, 0.0f, 1.0f},   // irow 3
  };
}

float median(Vector<float> &v) {
  size_t n = v.size() / 2;
  nth_element(v.begin(), v.begin() + n, v.end());
  return v[n];
}

// mirroring by flipping X by default
HPoint3 mirrorPointToRefPoint(
  const HPoint3 &originP,
  const HPoint3 &originRefP,
  const HPoint3 &newRefP,
  const HVector3 &scale = {-1.0f, 1.0f, 1.0f}) {
  float dx = originP.x() - originRefP.x();
  float dy = originP.y() - originRefP.y();
  float dz = originP.z() - originRefP.z();
  return {newRefP.x() + scale.x() * dx, newRefP.y() + scale.y() * dy, newRefP.z() + scale.z() * dz};
}

}  // namespace

bool isEarVertexVisible(float visibility) {
  return (visibility > EAR_LANDMARK_DETECTION_VISIBILITY_THRESHOLD);
}

bool isEarVertexTrustable(float visibility) {
  return (visibility > settings().vertexTrustableThreshold);
}

// Always return 2 ear ROIs with first one is left ear and second one is right ear.
Vector<DetectedPoints> earRoisByFaceMesh(const DetectedPoints &face, EarBoundingBoxDebug *earBb) {
  // Get reference points in ray space
  auto texToRay = renderTexToRaySpace(face.roi, face.intrinsics);
  Vector<HPoint3> referencePts = {
    {face.points[FACEMESH_L_EAR].x(), face.points[FACEMESH_L_EAR].y(), 1.0f},  // left ear top
    {face.points[FACEMESH_L_EAR_LOW].x(),
     face.points[FACEMESH_L_EAR_LOW].y(),
     1.0f},                                                                    // left ear bottom
    {face.points[FACEMESH_R_EAR].x(), face.points[FACEMESH_R_EAR].y(), 1.0f},  // right ear top
    {face.points[FACEMESH_R_EAR_LOW].x(),
     face.points[FACEMESH_R_EAR_LOW].y(),
     1.0f},  // right ear bottom
  };
  auto rayRefPts = texToRay * referencePts;

  // Compute left ear ROI
  DetectedPoints leftInFaceRoi{
    1,
    0,
    face.viewport,
    face.roi,
    {},  // bounding box -- will fill later.
    {},  // points -- will fill later.
    face.intrinsics,
  };

  leftInFaceRoi.points.push_back(face.points[FACEMESH_L_EAR]);
  leftInFaceRoi.points.push_back(face.points[FACEMESH_L_EAR_LOW]);

  // initialize the filters
  auto &earAnchorFilter = faceIdToEarAnchorFilters[face.roi.faceId];
  if (settings().filterEarAnchors && earAnchorFilter.size() < rayRefPts.size()) {
    earAnchorFilter.clear();
    for (size_t k = 0; k < rayRefPts.size(); ++k) {
      earAnchorFilter.push_back(RayPointFilter3(rayRefPts[k], filterConfig));
    }
  }

  // filter left ear rayRefPts
  if (settings().filterEarAnchors) {
    computeEarBoundingBox(
      earAnchorFilter[0].filter(rayRefPts[0]),
      earAnchorFilter[1].filter(rayRefPts[1]),
      texToRay,
      true,
      &leftInFaceRoi);
  } else {
    computeEarBoundingBox(rayRefPts[0], rayRefPts[1], texToRay, true, &leftInFaceRoi);
  }

  // use detection under face.roi to compute local left ear ROI
  DetectionRoi leftRoi = detectionRoiNoPadding(leftInFaceRoi);
  DetectedPoints left = transformFromFaceRoiToEarRoi(leftInFaceRoi, leftRoi);
  left.roi.source = ImageRoi::Source::EAR_LEFT;
  if (earBb) {
    earBb->left = leftInFaceRoi.boundingBox;
  }

  // compute right ear ROI
  DetectedPoints rightInFaceRoi{
    1,
    0,
    face.viewport,
    face.roi,
    {},  // bounding box -- will fill later.
    {},  // points -- will fill later.
    face.intrinsics,
  };

  rightInFaceRoi.points.push_back(face.points[FACEMESH_R_EAR]);
  rightInFaceRoi.points.push_back(face.points[FACEMESH_R_EAR_LOW]);

  // filter right ear rayRefPts
  if (settings().filterEarAnchors) {
    computeEarBoundingBox(
      earAnchorFilter[2].filter(rayRefPts[2]),
      earAnchorFilter[3].filter(rayRefPts[3]),
      texToRay,
      false,
      &rightInFaceRoi);
  } else {
    computeEarBoundingBox(rayRefPts[2], rayRefPts[3], texToRay, false, &rightInFaceRoi);
  }

  // use detection under face.roi to compute local right ear ROI
  DetectionRoi rightRoi = detectionRoiNoPadding(rightInFaceRoi);
  DetectedPoints right = transformFromFaceRoiToEarRoi(rightInFaceRoi, rightRoi);
  right.roi.source = ImageRoi::Source::EAR_RIGHT;

  if (earBb) {
    earBb->right = rightInFaceRoi.boundingBox;
  }
  return {left, right};
}

// https://gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com/niantic-ar/research/face/-/blob/main/src/face/utils/lift_3d.py
Vector<HPoint3> earLiftTo3DBySide(
  const DetectedPoints &ear,
  const Face3d &face,
  const HMatrix &meshToImageSpace,
  const Vector<Vector<int>> &vertPairs,
  const Vector<int> &anchorIndices) {
  // local ear to image space
  // the first 2 points should be anchor points on the face mesh,
  // and the other 3 are the new ear points
  DetectedImagePoints earImagePts = detectionToImageSpace(ear);

  // face mesh points need to be transformed into image space
  Vector<float> allZDiffs;
  for (auto &vPair : vertPairs) {
    auto &p0 = face.vertices[vPair[0]];
    auto &p1 = face.vertices[vPair[1]];
    HPoint3 imgP0 = meshToImageSpace * p0;
    HPoint3 imgP1 = meshToImageSpace * p1;
    allZDiffs.push_back(imgP0.z() - imgP1.z());
  }
  float zDiff = median(allZDiffs);

  // transform anchor points to image space
  // p1 & p2 should be in the same positions as the first 2 points in earImagePts.points
  HPoint3 p1 = meshToImageSpace * face.vertices[anchorIndices[0]];
  HPoint3 p2 = meshToImageSpace * face.vertices[anchorIndices[1]];

  HPoint2 p1xy{p1.x(), p1.y()};
  HPoint2 p2xy{p2.x(), p2.y()};
  Line2 line(p1xy, p2xy);

  // compute 3D data from image space to 3D face mesh space
  Vector<HPoint3> vertImageSpace = {p2, p1};
  for (size_t i = 2; i < earImagePts.points.size(); ++i) {
    // ear points in 2D image space
    HPoint3 p = earImagePts.points[i];
    HPoint2 p2d{p.x(), p.y()};
    float alpha = line.linearProjection(p2d);
    float z = p1.z() * (1.0f - alpha) + p2.z() * alpha + zDiff;
    vertImageSpace.push_back({p.x(), p.y(), z});
  }

  Vector<HPoint3> meshVerts = meshToImageSpace.inv() * vertImageSpace;
  return meshVerts;
}

void mirrorInvisibleEarVertices(const HPoint3 &leftRefPt, const HPoint3 &rightRefPt, Ear3d *ear) {
  if (!ear) {
    return;
  }

  // only mirror the invisible vertices on ears
  for (size_t i = 0; i < ear->leftVisibilities.size(); ++i) {
    if (
      !isEarVertexVisible(ear->leftVisibilities[i])
      && isEarVertexVisible(ear->rightVisibilities[i])) {
      ear->leftVertices[i] = mirrorPointToRefPoint(ear->rightVertices[i], rightRefPt, leftRefPt);
    }
  }

  for (size_t i = 0; i < ear->rightVisibilities.size(); ++i) {
    if (
      isEarVertexVisible(ear->leftVisibilities[i])
      && !isEarVertexVisible(ear->rightVisibilities[i])) {
      ear->rightVertices[i] = mirrorPointToRefPoint(ear->leftVertices[i], leftRefPt, rightRefPt);
    }
  }
}

// compute attachment points
void computeEarAttachmentPoints(Ear3d *ear3d) {
  if (!ear3d) {
    return;
  }

  // TODO(dat): Provide better normals?
  ear3d->leftAttachmentPoints.clear();
  ear3d->leftAttachmentPoints.push_back(createAttachmentPoint(
    AttachmentPointMsg::AttachmentName::EAR_LEFT_LOBE,
    ear3d->leftVertices[EAR_LANDMARK_DETECTION_LOBE],
    {0.f, 0.f, 0.f}));
  ear3d->leftAttachmentPoints.push_back(createAttachmentPoint(
    AttachmentPointMsg::AttachmentName::EAR_LEFT_CANAL,
    ear3d->leftVertices[EAR_LANDMARK_DETECTION_CANAL],
    {0.f, 0.f, 0.f}));
  ear3d->leftAttachmentPoints.push_back(createAttachmentPoint(
    AttachmentPointMsg::AttachmentName::EAR_LEFT_HELIX,
    ear3d->leftVertices[EAR_LANDMARK_DETECTION_HELIX],
    {0.f, 0.f, 0.f}));

  ear3d->rightAttachmentPoints.clear();
  ear3d->rightAttachmentPoints.push_back(createAttachmentPoint(
    AttachmentPointMsg::AttachmentName::EAR_RIGHT_LOBE,
    ear3d->rightVertices[EAR_LANDMARK_DETECTION_LOBE],
    {0.f, 0.f, 0.f}));
  ear3d->rightAttachmentPoints.push_back(createAttachmentPoint(
    AttachmentPointMsg::AttachmentName::EAR_RIGHT_CANAL,
    ear3d->rightVertices[EAR_LANDMARK_DETECTION_CANAL],
    {0.f, 0.f, 0.f}));
  ear3d->rightAttachmentPoints.push_back(createAttachmentPoint(
    AttachmentPointMsg::AttachmentName::EAR_RIGHT_HELIX,
    ear3d->rightVertices[EAR_LANDMARK_DETECTION_HELIX],
    {0.f, 0.f, 0.f}));
}

void updateEarStates(Ear3d *ear3d) {
  if (!ear3d) {
    return;
  }

  auto &earStates = ear3d->earStatesOutput;
  earStates.leftLobeFound =
    isEarVertexVisible(ear3d->leftVisibilities[EAR_LANDMARK_DETECTION_LOBE]);
  earStates.leftCanalFound =
    isEarVertexVisible(ear3d->leftVisibilities[EAR_LANDMARK_DETECTION_CANAL]);
  earStates.leftHelixFound =
    isEarVertexVisible(ear3d->leftVisibilities[EAR_LANDMARK_DETECTION_HELIX]);
  earStates.leftEarFound =
    earStates.leftLobeFound && earStates.leftCanalFound && earStates.leftHelixFound;

  earStates.rightLobeFound =
    isEarVertexVisible(ear3d->rightVisibilities[EAR_LANDMARK_DETECTION_LOBE]);
  earStates.rightCanalFound =
    isEarVertexVisible(ear3d->rightVisibilities[EAR_LANDMARK_DETECTION_CANAL]);
  earStates.rightHelixFound =
    isEarVertexVisible(ear3d->rightVisibilities[EAR_LANDMARK_DETECTION_HELIX]);
  earStates.rightEarFound =
    earStates.rightLobeFound && earStates.rightCanalFound && earStates.rightHelixFound;
}

// Lift ear 2D landmark data into 3D vertices
// The ear point depths are computed in (0,1) image space.
Ear3d earLiftLandmarksTo3D(
  const DetectedPoints &localFace,
  const DetectedPoints &leftEar,
  const DetectedPoints &rightEar,
  const Face3d &face,
  const c8_PixelPinholeCameraModel &intrinsics) {
  if (
    face.vertices.empty() || localFace.points.empty() || leftEar.points.empty()
    || rightEar.points.empty()) {
    return {};
  }

  Ear3d ear3d{
    face.id,
    face.transform,
    {},  // leftVisibilities
    {},  // rightVisibilities
    {},  // leftVertices
    {},  // rightVertices
    {},  // leftAttachmentPoints
    {},  // rightAttachmentPoints
  };

  // Skip first 2 each side as they are face mesh anchor points
  for (size_t i = 2; i < leftEar.points.size(); ++i) {
    ear3d.leftVisibilities.push_back(leftEar.points[i].z());
  }
  for (size_t i = 2; i < rightEar.points.size(); ++i) {
    ear3d.rightVisibilities.push_back(rightEar.points[i].z());
  }

  // Face3d mesh to image space
  // face mesh Model View matrix
  const HMatrix mv = trsMat(face.transform.position, face.transform.rotation, face.transform.scale);

  // projection matrix to clip space
  auto intrinsicsForViewport = Intrinsics::rotateCropAndScaleIntrinsics(
    intrinsics, intrinsics.pixelsWidth, intrinsics.pixelsHeight);
  auto projToClipM = Intrinsics::toClipSpaceMatLeftHanded(
    intrinsicsForViewport, CAM_PROJECTION_NEAR_CLIP, CAM_PROJECTION_FAR_CLIP);

  // transform from clip space to image space
  const HMatrix mvpToImageSpaceM = texToClipYFlip().inv() * projToClipM * mv;

  // compute depth in image space, then transform back to the face mesh space
  ear3d.leftVertices =
    earLiftTo3DBySide(leftEar, face, mvpToImageSpaceM, EAR_SIDES_MODELS_LEFT, anchorIndicesLeft);
  ear3d.rightVertices =
    earLiftTo3DBySide(rightEar, face, mvpToImageSpaceM, EAR_SIDES_MODELS_RIGHT, anchorIndicesRight);

  // Until this stage, there are 2 points each side that are on the face mesh as references
  // for ear vertex computations.
  // Remove the reference points on the face mesh and only return vertices that are on ears.
  auto &leftV = ear3d.leftVertices;
  leftV.erase(leftV.begin(), leftV.begin() + 2);
  auto &rightV = ear3d.rightVertices;
  rightV.erase(rightV.begin(), rightV.begin() + 2);

  // if ear vertices are not visible, mirror them from the visible side
  mirrorInvisibleEarVertices(face.vertices[FACEMESH_L_EAR], face.vertices[FACEMESH_R_EAR], &ear3d);

  return ear3d;
}

bool areEarPointsDrifted(
  const Vector<HPoint3> &verts,
  const Vector<float> &vertVisScore,
  const Vector<HPoint3> &cacheVerts,
  float distanceThreshold) {
  // Only test against visible vertices
  for (size_t i = 0; i < verts.size(); ++i) {
    if (!isEarVertexVisible(vertVisScore[i])) {
      continue;
    }

    float d = cacheVerts[i].dist(verts[i]);
    if (d > distanceThreshold) {
      return true;
    }
  }
  return false;
}

void resetEarCacheSample(
  const Ear3d &ear3d, EarSampledVerticesFullView::EarSampledVertices *sample) {
  if (!sample) {
    return;
  }

  sample->leftVertices = ear3d.leftVertices;
  sample->rightVertices = ear3d.rightVertices;
  sample->leftVis = ear3d.leftVisibilities;
  sample->rightVis = ear3d.rightVisibilities;
  sample->leftVertCount.resize(ear3d.leftVertices.size());
  std::fill(sample->leftVertCount.begin(), sample->leftVertCount.end(), 0);
  sample->rightVertCount.resize(ear3d.rightVertices.size());
  std::fill(sample->rightVertCount.begin(), sample->rightVertCount.end(), 0);
}

void averageVertices(
  const Vector<HPoint3> &verts,
  const Vector<float> &visScore,
  Vector<HPoint3> *avgVerts,
  Vector<float> *avgVisScore,
  Vector<size_t> *avgCount) {
  if (!avgVerts || !avgVisScore || !avgCount) {
    return;
  }

  // allocate cache data
  if (avgVerts->size() != verts.size()) {
    avgVerts->resize(verts.size());
  }
  if (avgCount->size() != verts.size()) {
    avgCount->resize(verts.size());
    std::fill(avgCount->begin(), avgCount->end(), 0);
    avgVisScore->resize(verts.size());
    std::fill(avgVisScore->begin(), avgVisScore->end(), 0);
  }

  // if enough samples, return the averaged vertices
  const float aCount = settings().averageSamples;
  for (size_t i = 0; i < verts.size(); ++i) {
    // if there are enough samples, skip
    if ((*avgCount)[i] >= aCount) {
      continue;
    }

    // only accumulate visible vertices
    if (isEarVertexVisible(visScore[i])) {
      size_t prevC = (*avgCount)[i];
      float vis = (*avgVisScore)[i];
      auto &p = (*avgVerts)[i];
      auto &np = verts[i];

      float x = prevC * p.x() + np.x();
      float y = prevC * p.y() + np.y();
      float z = prevC * p.z() + np.z();
      float nvis = prevC * vis + visScore[i];

      size_t currentC = prevC + 1;
      (*avgVerts)[i] = {x / currentC, y / currentC, z / currentC};
      (*avgVisScore)[i] = nvis / currentC;
      (*avgCount)[i] = currentC;
    } else if ((*avgCount)[i] <= 0.0f) {
      // only update vertex positions when it's never seen before
      (*avgVerts)[i] = verts[i];
      (*avgVisScore)[i] = visScore[i];
    }
  }
}

// overwrite the vertices for current face's angle bracket
void getSampledEarVertices(
  EarSampledVerticesFullView &earSample, const EarPoseBinIdx &binIdx, Ear3d *ear3d) {
  if (!ear3d) {
    return;
  }

  auto &cache = earSample.getSampledVertices(binIdx);
  ear3d->leftVertices = cache.leftVertices;
  ear3d->rightVertices = cache.rightVertices;
  ear3d->leftVisibilities = cache.leftVis;
  ear3d->rightVisibilities = cache.rightVis;
}

// If the vertices in 'ear3d' are too far from the cached data, reset the cache data
// Or if the vertex visibilities are too off
void resetEarCacheByDistance(
  const Ear3d &ear3d,
  const EarPoseBinIdx &binIdx,
  float leftResetThreshold,
  float rightResetThreshold,
  EarSampledVerticesFullView *earSample) {
  auto &sample = earSample->getSampledVertices(binIdx);
  if (
    sample.leftVertices.empty() || sample.rightVertices.empty() || ear3d.leftVertices.empty()
    || ear3d.rightVertices.empty()) {
    return;
  }

  // if sizes mismatch, reset and return
  if (
    sample.leftVertices.size() != ear3d.leftVertices.size()
    || sample.rightVertices.size() != ear3d.rightVertices.size()) {
    resetEarCacheSample(ear3d, &sample);
    return;
  }

  // Test if visibilities are mismatched
  bool visibilityMismatch = false;
  for (size_t i = 0; i < sample.leftVis.size(); ++i) {
    if (isEarVertexVisible(ear3d.leftVisibilities[i]) != isEarVertexVisible(sample.leftVis[i])) {
      visibilityMismatch = true;
    }
  }
  for (size_t i = 0; i < sample.rightVis.size(); ++i) {
    if (isEarVertexVisible(ear3d.rightVisibilities[i]) != isEarVertexVisible(sample.rightVis[i])) {
      visibilityMismatch = true;
    }
  }

  // test if these two sets are too far away
  bool driftTooMuch = areEarPointsDrifted(
    ear3d.leftVertices, ear3d.leftVisibilities, sample.leftVertices, leftResetThreshold);

  driftTooMuch |= areEarPointsDrifted(
    ear3d.rightVertices, ear3d.rightVisibilities, sample.rightVertices, rightResetThreshold);

  if (visibilityMismatch || driftTooMuch) {
    resetEarCacheSample(ear3d, &sample);
  }
}

void accumulateEar3dSample(
  const Ear3d &ear3d, const EarPoseBinIdx &binIdx, EarSampledVerticesFullView *earSample) {
  if (!earSample) {
    return;
  }

  auto &sample = earSample->getSampledVertices(binIdx);
  averageVertices(
    ear3d.leftVertices,
    ear3d.leftVisibilities,
    &sample.leftVertices,
    &sample.leftVis,
    &sample.leftVertCount);
  averageVertices(
    ear3d.rightVertices,
    ear3d.rightVisibilities,
    &sample.rightVertices,
    &sample.rightVis,
    &sample.rightVertCount);
}

Ear3d averageEar3d(
  const Ear3d &ear3d,
  const EarPoseBinIdx &binIdx,
  const EarPoseBinIdx &binDims,
  float leftResetThreshold,
  float rightResetThreshold,
  HashMap<int, EarSampledVerticesFullView> *avgEarMap) {
  Ear3d earData = ear3d;
  if (!avgEarMap) {
    return earData;
  }

  // if the average vertices are already computed, return them
  auto record = avgEarMap->find(ear3d.faceId);
  if (record != avgEarMap->end()) {
    auto &earCache = record->second;
    // if the cache dimensions do not match, reset the whole cache for this face
    if (
      earCache.sampledAngleMap.empty() || earCache.sampledAngleMap.size() != binDims.zBin
      || earCache.sampledAngleMap[0].size() != binDims.yBin) {
      earCache.resizeAngleCache(binDims.yBin, binDims.zBin);
    }

    resetEarCacheByDistance(ear3d, binIdx, leftResetThreshold, rightResetThreshold, &earCache);
    accumulateEar3dSample(ear3d, binIdx, &earCache);
    getSampledEarVertices(earCache, binIdx, &earData);
  } else {
    // if there is no record, insert and return
    EarSampledVerticesFullView earRecord;
    earRecord.resizeAngleCache(binDims.yBin, binDims.zBin);
    accumulateEar3dSample(ear3d, binIdx, &earRecord);
    (*avgEarMap)[ear3d.faceId] = earRecord;
  }

  return earData;
}

void updateAverageEarVertices(
  const Vector<HPoint3> &verts,
  const Vector<float> &visScore,
  Vector<HPoint3> *cacheVertices,
  Vector<RayPointFilter3a> *cacheFilters) {
  static Vector<RayPointFilterConfig> earFilterConfigs{
    createRayPointFilterConfig(0.1f, 0.6f, 0.9f),  // helix
    createRayPointFilterConfig(0.1f, 0.5f, 0.8f),  // canal
    createRayPointFilterConfig(0.1f, 0.4f, 0.7f),  // lobe
  };

  // allocate cache data
  if (cacheVertices->size() != verts.size()) {
    cacheVertices->resize(verts.size());
  }
  // Fill cacheFilters to the same size as verts
  if (verts.size() > earFilterConfigs.size()) {
    // NOTE(dat): Would be nice to have C8_WARN as well
    C8Log(
      "[ear-types] Seeing more vertices than preset %d earFilterConfigs", earFilterConfigs.size());
  }
  for (size_t i = cacheFilters->size(); i < verts.size(); i++) {
    cacheFilters->push_back({verts[i], earFilterConfigs[i % earFilterConfigs.size()]});
  }

  for (size_t i = 0; i < verts.size(); ++i) {
    // only accumulate trustable vertices
    if (!isEarVertexTrustable(visScore[i])) {
      continue;
    }

    (*cacheVertices)[i] = (*cacheFilters)[i].filter(verts[i]);

#ifndef NDEBUG
    const auto debugData = (*cacheFilters)[i].debugData();
    C8Log(
      "[ear-types] debugData filter=%d jointVelocity=%f updateAlpha=%f scaledVelocity=%f",
      i,
      debugData.jointVelocity,
      debugData.updateAlpha,
      debugData.scaledVelocity);
#endif
  }
}

Ear3d updateAverageEar(const Ear3d &ear3d, HashMap<int, EarAveragedVertices> *faceIdToAverageEar) {
  // NOTE(dat): This should NEVER happen. If you don't have a cache, you should not be calling this.
  if (!faceIdToAverageEar) {
    return ear3d;
  }

  // Create a cached result if it does not yet exist
  auto &earCache = (*faceIdToAverageEar)[ear3d.faceId];

#ifndef NDEBUG
  C8Log("[ear-types] updating average ear vertices left num=%d", ear3d.leftVertices.size());
#endif
  updateAverageEarVertices(
    ear3d.leftVertices, ear3d.leftVisibilities, &earCache.leftVertices, &earCache.leftFilters);
#ifndef NDEBUG
  C8Log("[ear-types] updating average ear vertices right num=%d", ear3d.leftVertices.size());
#endif
  updateAverageEarVertices(
    ear3d.rightVertices, ear3d.rightVisibilities, &earCache.rightVertices, &earCache.rightFilters);

  // copy data from input ear3d
  Ear3d earData = ear3d;
  // overwrite with cached vertices
  earData.leftVertices = earCache.leftVertices;
  earData.rightVertices = earCache.rightVertices;
  // Do not update visibility.

  return earData;
}

bool isEar3DSampled(
  HashMap<int, EarSampledVerticesFullView> &avgEarMap, int faceId, const EarPoseBinIdx &binIdx) {
  auto faceRecord = avgEarMap.find(faceId);
  if (faceRecord == avgEarMap.end()) {
    return false;
  }

  // see if ear cache exists
  auto &f = faceRecord->second;
  auto &earCache = f.getSampledVertices(binIdx);
  if (earCache.leftVertices.empty() || earCache.rightVertices.empty()) {
    return false;
  }

  // if every visible vertex has been sampled enough times
  const float aCount = settings().averageSamples;
  for (size_t i = 0; i < earCache.leftVertices.size(); ++i) {
    if (isEarVertexVisible(earCache.leftVis[i]) && earCache.leftVertCount[i] < aCount) {
      return false;
    }
  }
  for (size_t i = 0; i < earCache.rightVertices.size(); ++i) {
    if (isEarVertexVisible(earCache.rightVis[i]) && earCache.rightVertCount[i] < aCount) {
      return false;
    }
  }

  return true;
}

void getEar3DSampled(
  HashMap<int, EarSampledVerticesFullView> &avgEarMap,
  const Face3d &face,
  const EarPoseBinIdx &binIdx,
  Ear3d *ear3d) {
  if (!ear3d) {
    return;
  }

  auto faceRecord = avgEarMap.find(face.id);
  if (faceRecord == avgEarMap.end()) {
    return;
  }

  auto &f = faceRecord->second;
  ear3d->faceId = face.id;
  ear3d->transform = face.transform;

  getSampledEarVertices(f, binIdx, ear3d);

  mirrorInvisibleEarVertices(face.vertices[FACEMESH_NOSE], face.vertices[FACEMESH_NOSE], ear3d);
}

}  // namespace c8
