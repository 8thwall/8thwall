// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"hit-test.h"};
  deps = {
    "//c8:c8-log",
    "//c8:half",
    "//c8:hvector",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//c8/geometry:two-d",
    "//c8/geometry:splat",
    "//c8/pixels/render:object8",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x45f56af5);

#include <cmath>

#include "c8/c8-log.h"
#include "c8/geometry/splat.h"
#include "c8/half.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/two-d.h"
#include "c8/pixels/render/hit-test.h"

namespace c8 {
namespace {

bool isSplatRenderable(const Renderable &r) {
  return r.material().texture(Shaders::RENDERER_SPLAT_DATA_TEX) != nullptr
    || !r.geometry().instanceIds().empty();
}

const Scene &findScene(const Object8 &node) {
  if (node.is<Scene>()) {
    return node.as<Scene>();
  }
  if (!node.parent()) {
    C8_THROW("Object8 is not a Scene and has no parent.");
  }
  return findScene(*node.parent());
}

std::array<float, 6> unpackCoeffs(std::array<uint32_t, 3> coeffs) {
  auto c1 = unpackHalf2x16(coeffs[0]);
  auto c2 = unpackHalf2x16(coeffs[1]);
  auto c3 = unpackHalf2x16(coeffs[2]);
  return {0.25f * c1[0], 0.25f * c1[1], 0.25f * c2[0], 0.25f * c2[1], 0.25f * c3[0], 0.25f * c3[1]};
}

Vector<HitTestResult> hitsForSplat(
  const PackedSplat *splats,
  int maxNumSplats,
  const uint32_t *ids,
  int numSplats,
  const HMatrix &mv,
  const HMatrix &p,
  HVector2 pt) {
  Vector<HitTestResult> result;

  if (splats == nullptr || ids == nullptr || numSplats == 0) {
    return result;
  }

  float minAlpha = 10.0f / 255.0f;
  auto mvrt = rotationMat(mv).t();

  for (int i = 0; i < numSplats; ++i) {
    auto id = ids[i];
    if (id > maxNumSplats) {
      continue;
    }

    const auto &splat = splats[id];
    auto position = splat.position;
    auto alpha = splat.color[3] / 255.0f;

    if (alpha < minAlpha) {
      continue;
    }

    auto coeffs = unpackCoeffs(splat.encodedCoeffs);

    auto splat3d = mv * HPoint3{position[0], position[1], position[2]};
    auto splat2d = p * splat3d;

    if (splat2d.z() < -1.0f || splat2d.z() > 1.0f) {
      continue;
    }

    // clang-format off
    auto rssr = HMatrix{
      {coeffs[0], coeffs[1], coeffs[2], 0.0f},
      {coeffs[1], coeffs[3], coeffs[4], 0.0f},
      {coeffs[2], coeffs[4], coeffs[5], 0.0f},
      {     0.0f,      0.0f,      0.0f, 1.0f},
      false   // Don't precompute inverse.
    };

    auto J = HMatrix{
      {                      1.0f,                       0.0f, 0.0f, 0.0f},
      {                      0.0f,                       1.0f, 0.0f, 0.0f},
      {-splat3d.x() / splat3d.z(), -splat3d.y() / splat3d.z(), 0.0f, 0.0f},
      {                      0.0f,                       0.0f, 0.0f, 1.0f},
      false   // Don't precompute inverse.
    };
    // clang-format on

    auto T = mvrt * J;
    auto cov2d = T.t() * rssr * T;

    float lowPassPaddingStd = 0;  // TODO? splat3d.z / (1.7320508 * raysToPix);
    float lowPassPaddingVar = lowPassPaddingStd * lowPassPaddingStd;

    float diagonal1 = cov2d(0, 0) + lowPassPaddingVar;
    float offDiagonal = cov2d(0, 1);
    float diagonal2 = cov2d(1, 1) + lowPassPaddingVar;
    float mid = 0.5 * (diagonal1 + diagonal2);
    float r1 = 0.5 * (diagonal1 - diagonal2);
    float r2 = offDiagonal;
    float radius = std::sqrt(r1 * r1 + r2 * r2);
    float lambda1 = mid + radius;
    float lambda2 = mid - radius;

    if (lambda2 <= 0.0) {
      continue;
    }

    float diagX = offDiagonal;
    float diagY = lambda1 - diagonal1;

    float dn = std::sqrt(diagX * diagX + diagY * diagY);

    if (dn < 1e-12f) {
      // Splats that are too long and thin are culled.
      continue;
    } else {
      diagX /= dn;
      diagY /= dn;
    }

    float std1 = std::sqrt(lambda1);
    float std2 = std::sqrt(lambda2);

    float projectedStd1 = std1 / splat3d.z();

    if (projectedStd1 > 1.0f / std::min(p(0, 0), p(1, 1))) {
      continue;
    }

    float maxSqEccentricity = -2.0f * std::log(minAlpha / alpha);

    float maxSigma = std::sqrt(maxSqEccentricity);
    float mjX = maxSigma * std1 * diagX;
    float mjY = maxSigma * std1 * diagY;
    float mnX = maxSigma * std2 * diagY;
    float mnY = -maxSigma * std2 * diagX;

    // clang-format off
    HMatrix transform = HMatrix{
      {        mjX,         mnX, 0.0f, splat3d.x()},
      {        mjY,         mnY, 0.0f, splat3d.y()},
      {       0.0f,        0.0f, 1.0f, splat3d.z()},
      {       0.0f, splat3d.y(), 0.0f,        1.0f},
      false   // Don't precompute inverse.
    };

    Vector<HPoint3> corners = {
      {-1.0f, -1.0f, 0.0f},  // lower left
      {-1.0f,  1.0f, 0.0f},  // upper left
      { 1.0f,  1.0f, 0.0f},  // upper right
      { 1.0f, -1.0f, 0.0f},  // lower right
    };
    // clang-format on

    auto q = p * transform * corners;

    auto quad = Poly2(
      {{q[0].x(), q[0].y()}, {q[1].x(), q[1].y()}, {q[2].x(), q[2].y()}, {q[3].x(), q[3].y()}});

    // Get area of the quad. If the quad is too small, don't try to hit test.
    if (quad.areaAssumingConvex() < 1e-10f) {
      continue;
    }

    // Test whether the hit test point is within the triangle. If not, reject.
    if (!quad.containsPointAssumingConvex({pt.x(), pt.y()})) {
      continue;
    }

    // Set "node" to nullptr here, this will get filled out later.
    result.push_back({nullptr, static_cast<int>(id), splat2d.z(), HVector2{0.5f, 0.5f}});
  }

  return result;
}

Vector<HitTestResult> hitsForSplat(const Renderable &node, const Camera &camera, HVector2 pt) {
  Vector<HitTestResult> result;

  auto mv = camera.world().inv() * node.world();
  auto p = node.ignoreProjection() ? HMatrixGen::i() : camera.projection();

  // For the case where splat data is stored in the "color" texture, and instance ids are stored as
  // instance attributes.
  if (node.material().colorTexture() != nullptr) {
    const auto *splatTex = node.material().colorTexture();
    const PackedSplat *splats =
      reinterpret_cast<const PackedSplat *>(splatTex->rgba32Pixels().pixels());
    int maxNumSplats = 
       (splatTex->rgba32Pixels().rowElements() * splatTex->rgba32Pixels().rows() * sizeof(uint32_t))
       / sizeof(PackedSplat);
    int numSplats = node.geometry().instanceIds().size();

    result = hitsForSplat(
      splats, maxNumSplats, node.geometry().instanceIds().data(), numSplats, mv, p, pt);
  }

  // For the "stacked" case where there are 128 ids / quads per instance.
  if (node.material().texture(Shaders::RENDERER_SPLAT_DATA_TEX) != nullptr) {
    const auto *splatTex = node.material().texture(Shaders::RENDERER_SPLAT_DATA_TEX);
    const PackedSplat *splats =
      reinterpret_cast<const PackedSplat *>(splatTex->rgba32Pixels().pixels());
    int maxNumSplats = 
       (splatTex->rgba32Pixels().rowElements() * splatTex->rgba32Pixels().rows() * sizeof(uint32_t))
       / sizeof(PackedSplat);
    int numSplats = node.geometry().instanceCount() * 128;
    const uint32_t *ids =
      node.material().texture(Shaders::RENDERER_SPLAT_IDS_TEX)->r32Pixels().pixels();
    result = hitsForSplat(splats, maxNumSplats, ids, numSplats, mv, p, pt);
  }

  for (auto &r : result) {
    r.target = &node;
  }

  return result;
}

// Detect hits in a point cloud.
Vector<HitTestResult> hitsForPoints(const Renderable &node, const Camera &camera, HVector2 pt) {
  auto mv = camera.world().inv() * node.world();
  auto mvp = node.ignoreProjection() ? node.world() : camera.projection() * mv;

  auto pts3d = mv * node.geometry().vertices();
  auto clipVerts = mvp * node.geometry().vertices();
  auto halfPointSize = node.material().float1().at(Shaders::POINT_CLOUD_POINT_SIZE)[0] * 0.5f;

  float pixToRayX = 1.0f;
  float pixToRayY = 1.0f;

  if (node.ignoreProjection()) {
    const auto &s = findScene(node);
    auto w = s.renderSpec().resolutionWidth;
    auto h = s.renderSpec().resolutionHeight;
    pixToRayX = 2.0f / w;
    pixToRayY = 2.0f / h;
  }

  auto rayToClip = node.ignoreProjection() ? HPoint3{pixToRayX, pixToRayY, 1.0f}
                                           : camera.projection() * HPoint3{1.0f, 1.0f, 1.0f};
  auto halfPointSizeClipX = halfPointSize * rayToClip.x();
  auto halfPointSizeClipY = halfPointSize * rayToClip.y();

  // Test each triangle for a hit.
  Vector<HitTestResult> result;
  for (int i = 0; i < clipVerts.size(); ++i) {
    if (clipVerts[i].z() > 1.0f || clipVerts[i].z() < -1.0f) {
      continue;
    }

    auto iz = node.ignoreProjection() ? 1.0f : 1.0f / pts3d[i].z();
    auto rx = halfPointSizeClipX * iz;
    auto ry = halfPointSizeClipY * iz;

    auto dx = pt.x() - clipVerts[i].x();
    auto dy = pt.y() - clipVerts[i].y();

    if (dx > rx || dx < -rx) {
      continue;
    }

    if (dy > ry || dy < -ry) {
      continue;
    }

    result.push_back({&node, i, clipVerts[i].z(), {0.5f * dx / rx + 0.5f, 0.5f * dy / ry + 0.5f}});
  }

  return result;
}

// Detect hits for a mesh.
Vector<HitTestResult> hitsForMesh(const Renderable &node, const Camera &camera, HVector2 pt) {
  if (!node.geometry().instanceIds().empty() || !node.geometry().instancePositions().empty()) {
    // Instance rendering is not yet supported for hit testing.
    return {};
  }

  // Get the transform for this node into clip space.
  auto transform = node.ignoreProjection()
    ? node.world()
    : camera.projection() * camera.world().inv() * node.world();

  // Get this node's vertices in clip space.
  auto clipVerts = transform * node.geometry().vertices();

  // Test each triangle for a hit.
  Vector<HitTestResult> result;
  for (int i = 0; i < node.geometry().triangles().size(); ++i) {
    auto t = node.geometry().triangles()[i];

    // clip space vertices for this triangle.
    auto cva = clipVerts[t.a];
    auto cvb = clipVerts[t.b];
    auto cvc = clipVerts[t.c];

    // clip space vertices of this triangle as 2d x, y positions (ignoring z).
    auto va = HVector2{cva.x(), cva.y()};
    auto vb = HVector2{cvb.x(), cvb.y()};
    auto vc = HVector2{cvc.x(), cvc.y()};

    // TODO(nb): switch to simpler code optimized for this triangle computation to minimize flops.
    auto abc = Poly2({{va.x(), va.y()}, {vb.x(), vb.y()}, {vc.x(), vc.y()}});

    // Get area of the triangle. If the triangle is too small, don't try to hit test.
    auto abcArea = abc.areaAssumingConvex();
    if (abc.areaAssumingConvex() < 1e-10f) {
      continue;
    }

    // Test whether the hit test point is within the triangle. If not, reject.
    if (!abc.containsPointAssumingConvex({pt.x(), pt.y()})) {
      continue;
    }

    // Compute barycentric coordinate weights for triangle vertex interpolation.
    auto cap = Poly2({HPoint2{vc.x(), vc.y()}, HPoint2{va.x(), va.y()}, HPoint2{pt.x(), pt.y()}});

    auto bcp = Poly2({HPoint2{vb.x(), vb.y()}, HPoint2{vc.x(), vc.y()}, HPoint2{pt.x(), pt.y()}});

    float invArea = 1.0f / abcArea;

    float a = bcp.areaAssumingConvex() * invArea;
    float b = cap.areaAssumingConvex() * invArea;
    float c = 1.0f - a - b;

    // Interpolate z.
    auto z = a * cva.z() + b * cvb.z() + c * cvc.z();

    // If interpolated z is outside clip space, reject.
    if (z < -1.0f || z > 1.0f) {
      continue;
    }

    // Interpolate uv.
    auto uv = HVector2{0.5f, 0.5f};
    if (!node.geometry().uvs().empty()) {
      auto uva = node.geometry().uvs()[t.a];
      auto uvb = node.geometry().uvs()[t.b];
      auto uvc = node.geometry().uvs()[t.c];
      uv = (a * uva) + (b * uvb) + (c * uvc);
    }

    // Add to hits.
    result.push_back({&node, i, z, uv});
  }

  return result;
}

// Find the hit test results from a single node without recursion.
Vector<HitTestResult> hitTestObject(const Object8 &node, const Camera &camera, HVector2 pt) {
  // Hits can only come from renderable objects.
  if (!node.is<Renderable>()) {
    return {};
  }

  return hitTestRenderable(node.as<Renderable>(), camera, pt);
}

// Check whether this node has a subscene texture, and if so, find the subscene and camera.
bool findSubscene(
  const Scene &scene, const Object8 &node, const Scene **subscene, const Camera **subcamera) {
  *subscene = nullptr;
  *subcamera = nullptr;
  // Only renderable objects can have subscenes.
  if (!node.is<Renderable>()) {
    return false;
  }

  const auto &renderable = node.as<Renderable>();

  // Get any subscenes that this renderable depends on.
  // TODO(nb): use a more general method for getting textures or renderspecs from a material.
  const auto *texture = renderable.material().colorTexture();
  if (texture == nullptr) {
    return false;
  }

  const auto *renderSpec = texture->sceneMaterialSpec();
  if (renderSpec == nullptr) {
    return false;
  }

  *subscene = &scene.find<Scene>(renderSpec->subsceneName);

  // Get the camera that is rendering this subscene.
  const auto &sceneSpecs = (*subscene)->renderSpecs();
  auto spec = std::find_if(sceneSpecs.begin(), sceneSpecs.end(), [renderSpec](const auto &s) {
    return s.name == renderSpec->renderSpecName;
  });
  *subcamera = !spec->cameraName.empty() ? &(*subscene)->find<Camera>(spec->cameraName)
                                         : &(*subscene)->find<Camera>();
  return true;
}

Vector<HitTestResult> hitTestNode(
  const Scene &scene, const Object8 &node, const Camera &camera, HVector2 pt) {
  // Don't get hits for node trees that aren't enabled.
  if (!node.enabled()) {
    return {};
  }

  // Get hits for this node.
  auto result = hitTestObject(node, camera, pt);

  // Get hits for this node's subscenes and add them to result.
  const Scene *subscene = nullptr;
  const Camera *subcamera = nullptr;
  if (!result.empty() && findSubscene(scene, node, &subscene, &subcamera)) {
    // Mark this node's hit test results as rendering a subscene.
    for (auto &r : result) {
      r.rendersSubscene = true;
    }

    // Recursive call for subscene.
    auto newClipCoordinates = (2.0f * result[0].uv) + HVector2{-1.0f, -1.0f};
    auto subsceneResults = hitTestNode(*subscene, *subscene, *subcamera, newClipCoordinates);
    result.insert(result.end(), subsceneResults.begin(), subsceneResults.end());
  }

  // Get hits for children and add them to result.
  for (const auto &child : node.children()) {
    // Don't recurse into child scenes directly.
    if (child->is<Scene>()) {
      continue;
    }

    // Recursive call for child.
    auto childResults = hitTestNode(scene, *child, camera, pt);
    result.insert(result.end(), childResults.begin(), childResults.end());
  }

  return result;
}

}  // namespace

Vector<HitTestResult> hitTestRenderable(const Renderable &r, const Camera &camera, HVector2 pt) {
  // Delegate to a hit test method for this object type.
  switch (r.kind()) {
    case Renderable::RenderableKind::POINTS:
      return hitsForPoints(r, camera, pt);
    case Renderable::RenderableKind::MESH:
      // If the mesh has instance ids, guess that it's a splat.
      if (isSplatRenderable(r)) {
        return hitsForSplat(r, camera, pt);
      }
      // Otherwise, check as a normal mesh.
      return hitsForMesh(r, camera, pt);
    default:
      return {};
  }
}

Vector<HitTestResult> hitTestPixel(const Scene &scene, HVector2 pixel) {
  // Convert pixels to clip space using the scene's renderspec, and delegate to clip space
  // implementation.
  const auto &r = scene.renderSpecs()[0];

  // TODO: Fix the implementation below, this needs to invert the y coordinate of the pixel when
  // converting to clip space.
  return hitTestClipSpace(
    scene,
    {(pixel.x() + 0.5f) / static_cast<float>(r.resolutionWidth) * 2.0f - 1.0f,
     (pixel.y() + 0.5f) / static_cast<float>(r.resolutionHeight) * 2.0f - 1.0f});
}

Vector<HitTestResult> hitTestClipSpace(const Scene &scene, HVector2 clipCoordinates) {
  // Reject queries that are outside clip space.
  if (
    clipCoordinates.x() < -1.0f || clipCoordinates.y() < -1.0f || clipCoordinates.x() > 1.0f
    || clipCoordinates.y() > 1.0f) {
    return {};
  }

  // Get unsorted list of hits.
  auto allHits = hitTestNode(scene, scene, scene.find<Camera>(), clipCoordinates);

  // Sort by distance from camera.
  std::sort(allHits.begin(), allHits.end(), [](const auto &a, const auto &b) {
    // for things at the same clip space, return subscenes first.
    if (a.clipSpaceZ == b.clipSpaceZ) {
      return a.rendersSubscene < b.rendersSubscene;
    }
    // Primarily sort by camera distance.
    return a.clipSpaceZ < b.clipSpaceZ;
  });
  return allHits;
}

}  // namespace c8
