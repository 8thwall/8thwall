// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"object8.h"};
  deps = {
    ":geometry",
    ":material",
    ":sizing",
    "//c8:color",
    "//c8:exceptions",
    "//c8:hmatrix",
    "//c8:string",
    "//c8:vector",
    "//c8/geometry:intrinsics",
    "//c8/geometry:vectors",
    "//c8/geometry:mesh-types",
    "//c8/geometry:splat",
    "//c8/model:model-data-view",
    "//c8/pixels:pixel-buffer",
    "//c8/string:containers",
    "//c8/string:format",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x4adae75e);

#include <algorithm>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/mesh-types.h"
#include "c8/geometry/vectors.h"
#include "c8/pixels/render/object8.h"
#include "c8/string/containers.h"

namespace c8 {

int Object8::incrementingId = 0;

Vector<Object8 *> Object8::mutableChildren() {
  Vector<Object8 *> vec;
  for (auto &child : children_) {
    vec.push_back(child.get());
  }
  return vec;
}

Object8 &Object8::setName(const String &name) {
  name_ = name;
  return *this;
}

Object8 &Object8::setMetadata(const String &metadata) {
  metadata_ = metadata;
  return *this;
}

void Object8::updateWorld() {
  world_ = parent_ == nullptr ? local_ : parent_->world() * local_;
  for (auto &child : children_) {
    child->updateWorld();
  }
}

std::unique_ptr<Object8> Object8::remove() {
  if (parent_ == nullptr) {
    C8_THROW("Can't remove a root node.");
  }

  auto found =
    std::find_if(parent_->children_.begin(), parent_->children_.end(), [this](const auto &c) {
      return c.get() == this;
    });

  if (found == parent_->children_.end()) {
    C8_THROW("Removing a child from a parent that does not have the child.");
  }

  std::unique_ptr<Object8> freeChild;
  found->swap(freeChild);
  parent_->children_.erase(found);
  freeChild->parent_ = nullptr;
  return freeChild;
}

Object8 &Object8::setEnabled(bool enabled) {
  enabled_ = enabled;
  return *this;
}

Object8 &Object8::setLocal(const HMatrix &local) {
  local_ = local;
  updateWorld();
  return *this;
}

String Object8::toString() const noexcept {
  return format(
    "'%s' [%s %s %s]", name_.c_str(), type().c_str(), id_.c_str(), enabled_ ? "show" : "hide");
}

Camera &Camera::setProjection(const HMatrix &projection) {
  projection_ = projection;
  return *this;
}

Camera &Camera::flipY() { return setProjection(HMatrixGen::scaleY(-1.0f) * projection_); }

Light &Light::setKind(LightKind kind) {
  kind_ = kind;
  return *this;
}

Light &Light::setColor(Color color) {
  color_ = color;
  return *this;
}

Light &Light::setIntensity(float intensity) {
  intensity_ = intensity;
  return *this;
}

Renderable &Renderable::setKind(RenderableKind kind) {
  kind_ = kind;
  return *this;
}

Renderable &Renderable::setGeometry(std::unique_ptr<Geometry> &&geometry) {
  geometry_ = std::move(geometry);
  return *this;
}

Renderable &Renderable::setMaterial(std::unique_ptr<Material> &&material) {
  material_ = std::move(material);
  return *this;
}

Renderable &Renderable::setIgnoreProjection(bool ignoreProjection) {
  ignoreProjection_ = ignoreProjection;
  return *this;
}

Renderable &Renderable::setElementMetadata(const Vector<String> &elementMetadata) {
  elementMetadata_.assign(elementMetadata.begin(), elementMetadata.end());
  return *this;
}

void Scene::setRenderSpecs(const Vector<RenderSpec> &renderSpecs) {
  renderSpecs_.assign(renderSpecs.begin(), renderSpecs.end());
}

Scene &Scene::setRenderer(std::unique_ptr<SceneRenderer> &&renderer) {
  renderer_ = std::move(renderer);
  return *this;
}

const Vector<RenderSpec> &Scene::renderSpecs() const { return renderSpecs_; }

RenderSpec &Scene::renderSpec(const String &name) {
  for (auto &spec : renderSpecs_) {
    if (spec.name == name) {
      return spec;
    }
  }
  return renderSpecs_[0];
}

const RenderSpec &Scene::renderSpec(const String &name) const {
  return const_cast<Scene *>(this)->renderSpec(name);
}

namespace ObGen {

std::unique_ptr<Scene> scene(int pixelsWidth, int pixelsHeight) {
  auto scenePtr = std::unique_ptr<Scene>(new Scene());
  scenePtr->setRenderSpecs({{pixelsWidth, pixelsHeight}});
  return scenePtr;
}
std::unique_ptr<Scene> subScene(const String &name, const Vector<RenderSpec> &renderSpecs) {
  auto subScene = scene(0, 0);
  subScene->setName(name);
  subScene->setRenderSpecs(renderSpecs);
  return subScene;
}
std::unique_ptr<Camera> camera() { return std::unique_ptr<Camera>(new Camera()); }
std::unique_ptr<Group> group() { return std::unique_ptr<Group>(new Group()); }
std::unique_ptr<Light> light() { return std::unique_ptr<Light>(new Light()); }
std::unique_ptr<Renderable> renderable() { return std::unique_ptr<Renderable>(new Renderable()); }

std::unique_ptr<Renderable> quad(bool interleaved) {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::quad(interleaved))
    .setMaterial(MatGen::colorOnly())
    .setName("quad");
  return el;
}

std::unique_ptr<Renderable> backQuad() {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::backQuad())
    .setMaterial(MatGen::colorOnly())
    .setIgnoreProjection(true)
    .setName("back-quad");
  return el;
}

std::unique_ptr<Renderable> frontQuad() {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::frontQuad())
    .setMaterial(MatGen::colorOnly())
    .setIgnoreProjection(true)
    .setName("front-quad");
  return el;
}

std::unique_ptr<Renderable> pixelQuad(float x, float y, float width, float height) {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::pixelQuad(x, y, width, height))
    .setMaterial(MatGen::colorOnly())
    .setName("pixel-quad");
  return el;
}

std::unique_ptr<Renderable> pixelQuad(Rect rect) {
  return pixelQuad(rect.x, rect.y, rect.width, rect.height);
}

std::unique_ptr<Renderable> quadPoints() {
  auto el = renderable();
  el->setKind(Renderable::POINTS)
    .setGeometry(GeoGen::quadPoints())
    .setMaterial(MatGen::colorOnly())
    .setName("quad-points");
  return el;
}

// Create a basic cube with a color material
std::unique_ptr<Renderable> cubeMesh() {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::cubeMesh())
    .setMaterial(MatGen::physical())
    .setName("cube-mesh");
  return el;
}

std::unique_ptr<Renderable> curvyMesh(const GeoGen::CurvyMeshParams &params) {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::curvyMesh(params))
    .setMaterial(MatGen::physical())
    .setName("curvy-mesh");
  return el;
}

std::unique_ptr<Renderable> pointCloud(const Vector<HPoint3> &pts, float ptSize) {
  auto el = renderable();
  el->setKind(Renderable::POINTS)
    .setGeometry(GeoGen::empty())
    .setMaterial(MatGen::pointCloudPhysical())
    .setName("point-cloud");
  el->material().set(Shaders::POINT_CLOUD_POINT_SIZE, ptSize);
  updatePointCloud(el.get(), pts);
  return el;
}

std::unique_ptr<Renderable> pointCloud(const Vector<HPoint3> &pts, Color color, float ptSize) {
  auto el = renderable();
  el->setKind(Renderable::POINTS)
    .setGeometry(GeoGen::empty())
    .setMaterial(MatGen::pointCloudPhysical())
    .setName("point-cloud");
  el->material().set(Shaders::POINT_CLOUD_POINT_SIZE, ptSize);
  updatePointCloud(el.get(), pts, color);
  return el;
}

std::unique_ptr<Renderable> pointCloud(
  const Vector<HPoint3> &pts, const Vector<Color> &color, float ptSize) {
  return pointCloud(pts.data(), pts.size(), color.data(), color.size(), ptSize);
}

std::unique_ptr<Renderable> pointCloud(
  const HPoint3 *pts, int numPoints, const Color *color, int numColors, float ptSize) {
  auto el = renderable();
  el->

    setKind(Renderable::POINTS)
      .setGeometry(GeoGen::empty())
      .setMaterial(MatGen::pointCloudPhysical())
      .setName("point-cloud");
  el->material().set(Shaders::POINT_CLOUD_POINT_SIZE, ptSize);
  updatePointCloud(el.get(), pts, numPoints, color, numColors);
  return el;
}

void updatePointCloud(Renderable *ptCloud, const Vector<HPoint3> &newPts) {
  // In-place update, same size and colors as before, just different positions.
  if (newPts.size() == ptCloud->geometry().vertices().size()) {
    ptCloud->geometry().setVertices(newPts);
    return;
  }

  auto colors = ptCloud->geometry().colors();
  auto inds = ptCloud->geometry().points();

  // Fewer points than before. Keep existing colors and indexes for first n points.
  if (newPts.size() < ptCloud->geometry().vertices().size()) {
    colors.resize(newPts.size());
    inds.resize(newPts.size());
    ptCloud->geometry().setVertices(newPts).setColors(colors).setPoints(inds);
    return;
  }

  // More points than before. Extend with the same color as the current last point, or purple if
  // none. Add indexes of the new points.
  auto extendColor = colors.empty() ? Color::PURPLE : colors.back();
  for (uint32_t i = inds.size(); i < newPts.size(); ++i) {
    colors.push_back(extendColor);
    inds.push_back({i});
  }
  ptCloud->geometry().setVertices(newPts).setColors(colors).setPoints(inds);
}

void updatePointCloud(Renderable *ptCloud, const Vector<HPoint3> &newPts, Color color) {
  // Add indexes for new points if needed.
  auto inds = ptCloud->geometry().points();
  for (uint32_t i = inds.size(); i < newPts.size(); ++i) {
    inds.push_back({i});
  }
  // Truncate indexes if needed.
  inds.resize(newPts.size());
  // Update geometry.
  ptCloud->geometry().setVertices(newPts).setColor(color).setPoints(inds);
}

void updatePointCloud(
  Renderable *ptCloud,
  const Vector<HPoint3> &newPts,
  const uint8_t (&colorMap)[256][3],
  const Vector<double> &values) {
  if (newPts.empty()) {
    return;
  }
  if (newPts.size() != values.size()) {
    C8_THROW(
      "[render] updatePointCloud newPts and values must be the same size %d != %d",
      newPts.size(),
      values.size());
  }

  const auto [minVal, maxVal] = std::minmax_element(begin(values), end(values));
  Vector<Color> colors;
  colors.reserve(values.size());
  auto delta = (*maxVal - *minVal);
  for (const auto &val : values) {
    auto normalizedVal = (val - *minVal) / delta;
    colors.push_back(Color::fromColorMap(colorMap, normalizedVal));
  }

  updatePointCloud(ptCloud, newPts, colors);
}

void updatePointCloud(
  Renderable *ptCloud, const Vector<HPoint3> &newPts, const Vector<Color> &color) {
  updatePointCloud(ptCloud, newPts.data(), newPts.size(), color.data(), color.size());
}

void updatePointCloud(
  Renderable *ptCloud, const HPoint3 *newPts, int numPoints, const Color *color, int numColors) {
  // Add indexes for new points if needed.
  auto inds = ptCloud->geometry().points();
  for (uint32_t i = inds.size(); i < numPoints; ++i) {
    inds.push_back({i});
  }
  // Truncate indexes if needed.
  inds.resize(numPoints);
  // Update geometry.
  ptCloud->geometry().setVertices(newPts, numPoints).setColors(color, numColors).setPoints(inds);
}

std::unique_ptr<Renderable> barCloud() {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::empty())
    .setMaterial(MatGen::physical())
    .setName("bar-cloud");
  return el;
}

std::unique_ptr<Renderable> barCloud(
  const Vector<std::pair<HPoint3, HPoint3>> &lines, Color color, float barRadius) {
  auto b = barCloud();
  updateBarCloud(b.get(), lines, color, barRadius);
  return b;
}

std::unique_ptr<Renderable> barCloud(
  const Vector<std::pair<HPoint3, HPoint3>> &lines, const Vector<Color> &color, float barRadius) {
  auto b = barCloud();
  updateBarCloud(b.get(), lines, color, barRadius);
  return b;
}

void updateBarCloud(
  Renderable *barCloud,
  const Vector<std::pair<HPoint3, HPoint3>> &newLines,
  Color color,
  float barRadius) {
  updateBarCloud(barCloud, newLines, Vector<Color>{newLines.size(), color}, barRadius);
}

void updateBarCloud(
  Renderable *barCloud,
  const Vector<std::pair<HPoint3, HPoint3>> &newLines,
  const Vector<Color> &lineColors,
  float barRadius) {
  Vector<HPoint3> vertices;
  Vector<HVector3> normals;
  Vector<HVector2> uvs;
  Vector<MeshIndices> triangles;
  Vector<Color> colors;

  // Make a template cylinder. This will be copied and transformed for each bar in our cloud.
  auto barTemplate = GeoGen::curvyMesh({});  // radius: 1, height: 1, segments: 8.
  const auto &barVertices = barTemplate->vertices();
  const auto &barNormals = barTemplate->normals();
  const auto &barUvs = barTemplate->uvs();
  const auto &barTriangles = barTemplate->triangles();

  // Add vertices for each bar.  TODO: switch to instanced rendering.
  int indexOffset = 0;
  for (int i = 0; i < newLines.size(); ++i) {
    // Get the transform from the template cylinder to the current line.
    auto line = newLines[i];
    auto color = lineColors[i];
    auto a = line.first;
    auto b = line.second;

    auto direction = HVector3{b.x() - a.x(), b.y() - a.y(), b.z() - a.z()};
    auto len = direction.l2Norm();
    auto center = HPoint3{(b.x() + a.x()) * 0.5f, (b.y() + a.y()) * 0.5f, (b.z() + a.z()) * 0.5f};

    auto transform = HMatrixGen::translation(center.x(), center.y(), center.z())
      * rotationToVector({0.0f, 1.0f, 0.0f}, direction).toRotationMat()
      * HMatrixGen::scale(barRadius, len, barRadius);

    // Compute the vertices and normals for this bar.
    auto newVertices = transform * barVertices;
    auto newNormals = transform * barNormals;
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
    normals.insert(normals.end(), newNormals.begin(), newNormals.end());

    // Copy standard cylinder UVs.
    uvs.insert(uvs.end(), barUvs.begin(), barUvs.end());

    // Use this bar's color for every vertex in this bar.
    colors.resize(colors.size() + barVertices.size(), color);

    // The triangle indices are the same for every bar, but relative to this set of vertices.
    for (auto t : barTriangles) {
      triangles.push_back({t.a + indexOffset, t.b + indexOffset, t.c + indexOffset});
    }

    // Update the index offset for the next bar.
    indexOffset += barVertices.size();
  }

  // Update geometry.
  barCloud->geometry()
    .setVertices(vertices)
    .setColors(colors)
    .setTriangles(triangles)
    .setUvs(uvs)
    .setNormals(normals);
}

std::unique_ptr<Renderable> quadCloud(
  const Vector<HMatrix> &positions,
  const Vector<Vector<HVector2>> &uvs,
  const Vector<float> &scales) {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::empty())
    .setMaterial(MatGen::image())
    .setName("quad-cloud");
  updateQuadCloud(el.get(), positions, uvs, scales);
  return el;
}

void updateQuadCloud(
  Renderable *quadCloud,
  const Vector<HMatrix> &positions,
  const Vector<Vector<HVector2>> &newUvs,
  const Vector<float> &newScales) {
  Vector<HPoint3> vertices;
  Vector<HVector3> normals;
  Vector<HVector2> uvs;
  Vector<MeshIndices> triangles;

  // Make a template quad. This will be copied and transformed for each quad in our cloud.
  auto quadTemplate = GeoGen::quad(false);
  const auto &quadTriangles = quadTemplate->triangles();

  // Add vertices for each quad.  TODO: switch to instanced rendering.
  int indexOffset = 0;
  for (int i = 0; i < positions.size(); ++i) {
    const auto quadVertices = HMatrixGen::scale(newScales[i]) * quadTemplate->vertices();
    // Compute the vertices and normals for this quad.
    auto newVertices = positions[i] * quadVertices;
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());

    // Copy standard quad UVs.
    uvs.insert(uvs.end(), newUvs[i].begin(), newUvs[i].end());

    // The triangle indices are the same for every quad, but relative to this set of vertices.
    for (const auto &t : quadTriangles) {
      triangles.push_back({t.a + indexOffset, t.b + indexOffset, t.c + indexOffset});
    }

    // Update the index offset for the next quad.
    indexOffset += quadVertices.size();
  }
  // Update geometry.
  quadCloud->geometry().setVertices(vertices).setTriangles(triangles).setUvs(uvs).setNormals(
    normals);
}

std::unique_ptr<Renderable> pixelLines() {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::empty())
    .setMaterial(MatGen::colorOnly())
    .setIgnoreProjection(true)
    .setName("pixel-lines");
  return el;
}

std::unique_ptr<Renderable> pixelLines(
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,
  Color color,
  int scenePixelsWidth,
  int scenePixelsHeight,
  float linePixelThickness,
  float vertexDepth) {
  auto el = pixelLines();
  updatePixelLines(
    el.get(),
    linePixels,
    color,
    scenePixelsWidth,
    scenePixelsHeight,
    linePixelThickness,
    vertexDepth);
  return el;
}

std::unique_ptr<Renderable> pixelLines(
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,
  const Vector<Color> &color,
  int scenePixelsWidth,
  int scenePixelsHeight,
  float linePixelThickness,
  float vertexDepth) {
  auto el = pixelLines();
  updatePixelLines(
    el.get(),
    linePixels,
    color,
    scenePixelsWidth,
    scenePixelsHeight,
    linePixelThickness,
    vertexDepth);
  return el;
}

void updatePixelLines(
  Renderable *lineCloud,
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,
  Color color,
  int scenePixelsWidth,
  int scenePixelsHeight,
  float linePixelThickness,
  float vertexDepth) {
  updatePixelLines(
    lineCloud,
    linePixels,
    Vector<Color>{linePixels.size(), color},
    scenePixelsWidth,
    scenePixelsHeight,
    linePixelThickness,
    vertexDepth);
}

void updatePixelLines(
  Renderable *lineCloud,
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,
  std::pair<Color, Color> color,
  int scenePixelsWidth,
  int scenePixelsHeight,
  float linePixelThickness,
  float vertexDepth) {
  updatePixelLines(
    lineCloud,
    linePixels,
    Vector<std::pair<Color, Color>>{linePixels.size(), color},
    scenePixelsWidth,
    scenePixelsHeight,
    linePixelThickness,
    vertexDepth);
}

void updatePixelLines(
  Renderable *lineCloud,
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,
  const Vector<Color> &color,
  int scenePixelsWidth,
  int scenePixelsHeight,
  float linePixelThickness,
  float vertexDepth) {
  updatePixelLines(
    lineCloud,
    linePixels,
    map<Color, std::pair<Color, Color>>(
      color,
      [](const auto &c) {
        return std::pair<Color, Color>{c, c};
      }),
    scenePixelsWidth,
    scenePixelsHeight,
    linePixelThickness,
    vertexDepth);
}
void updatePixelLines(
  Renderable *lineCloud,
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,
  const Vector<std::pair<Color, Color>> &color,
  int scenePixelsWidth,
  int scenePixelsHeight,
  float linePixelThickness,
  float vertexDepth) {
  Vector<HPoint3> vertices;
  Vector<HVector3> normals;
  Vector<Color> colors;
  Vector<HVector2> uvs;
  Vector<MeshIndices> triangles;

  auto numVerts = 4 * linePixels.size();
  auto numTriangles = 2 * linePixels.size();
  vertices.reserve(numVerts);
  normals.reserve(numVerts);
  colors.reserve(numVerts);
  uvs.reserve(numVerts);
  triangles.reserve(numTriangles);

  float xScale = 2.0f / scenePixelsWidth;
  float yScale = -2.0f / scenePixelsHeight;
  auto r = linePixelThickness * 0.5f;

  // Create a quad per line.
  for (int i = 0; i < linePixels.size(); ++i) {
    auto line = linePixels[i];
    auto lineColor = color[i];
    uint32_t indexOffset = vertices.size();  // Offset for triangle indices.

    // A line from pixel 0 to pixel 639 needs to cover 640 pixels. We achieve this by
    // symmetrically stretching the line by one pixel on either side. First add 0.5 to both ends,
    // and then later subtract 0.5 from the first end and add 0.5 to the second.
    auto x1 = line.first.x() + 0.5f;
    auto x2 = line.second.x() + 0.5f;
    auto y1 = line.first.y() + 0.5f;
    auto y2 = line.second.y() + 0.5f;
    auto dir = HVector2{x2 - x1, y2 - y1};       // Direction of the line.
    auto normDir = HVector2{-dir.y(), dir.x()};  // Direction normal to the line.
    auto sqlen = dir.dot(dir);
    if (sqlen > 1e-10f) {
      auto invSize = 1.0f / std::sqrt(sqlen);  // Normalize direction vectors.
      normDir = invSize * normDir;
      dir = invSize * dir;
    }
    // Stretch both ends of the line by half a pixel along the direction of the line.
    x1 -= 0.5f * dir.x();
    y1 -= 0.5f * dir.y();
    x2 += 0.5f * dir.x();
    y2 += 0.5f * dir.y();

    // Offset from the point to the corners of the quad.
    auto rx = normDir.x() * r;
    auto ry = normDir.y() * r;

    // Compute the corners of the quad, accounting for thickness, and convert to clip space.
    vertices.push_back({(x1 - rx) * xScale - 1.0f, (y1 - ry) * yScale + 1.0f, vertexDepth});
    vertices.push_back({(x1 + rx) * xScale - 1.0f, (y1 + ry) * yScale + 1.0f, vertexDepth});
    vertices.push_back({(x2 + rx) * xScale - 1.0f, (y2 + ry) * yScale + 1.0f, vertexDepth});
    vertices.push_back({(x2 - rx) * xScale - 1.0f, (y2 - ry) * yScale + 1.0f, vertexDepth});

    // Fill in UVs, normals, colors and triangles.
    uvs.push_back({0.0f, 0.0f});
    uvs.push_back({0.0f, 1.0f});
    uvs.push_back({1.0f, 1.0f});
    uvs.push_back({1.0f, 0.0f});
    normals.resize(normals.size() + 4, {0.0f, 0.0f, -1.0f});
    colors.resize(colors.size() + 2, lineColor.first);
    colors.resize(colors.size() + 2, lineColor.second);
    triangles.push_back({indexOffset + 0, indexOffset + 1, indexOffset + 2});
    triangles.push_back({indexOffset + 2, indexOffset + 3, indexOffset + 0});
  }
  lineCloud->geometry().setVertices(vertices);
  lineCloud->geometry().setNormals(normals);
  lineCloud->geometry().setColors(colors);
  lineCloud->geometry().setUvs(uvs);
  lineCloud->geometry().setTriangles(triangles);
}

std::unique_ptr<Renderable> pixelPoints() {
  auto el = renderable();
  el->setKind(Renderable::POINTS)
    .setGeometry(GeoGen::empty())
    .setMaterial(MatGen::pointCloudColorOnly())
    .setIgnoreProjection(true)
    .setName("pixel-points");
  return el;
}

void updatePixelPoints(
  Renderable *ptCloud,
  const Vector<HPoint2> &newPts,
  Color color,
  int scenePixelsWidth,
  int scenePixelsHeight,
  float pointPixels,
  float vertexDepth) {
  updatePixelPoints(
    ptCloud,
    newPts,
    Vector<Color>{newPts.size(), color},
    scenePixelsWidth,
    scenePixelsHeight,
    pointPixels,
    vertexDepth);
}

void updatePixelPoints(
  Renderable *ptCloud,
  const Vector<HPoint2> &newPts,
  const Vector<Color> &color,
  int scenePixelsWidth,
  int scenePixelsHeight,
  float pointPixels,
  float vertexDepth) {

  float xScale = 2.0f / scenePixelsWidth;
  float yScale = -2.0f / scenePixelsHeight;

  // Add indexes for new points if needed.
  auto inds = ptCloud->geometry().points();
  for (uint32_t i = inds.size(); i < newPts.size(); ++i) {
    inds.push_back({i});
  }
  // Truncate indexes if needed.
  inds.resize(newPts.size());
  Vector<HPoint3> ptsAtDepth;
  for (const auto &pt : newPts) {
    // Correct pixel locations to be pixel centers (+ .5). Note that this is different from
    // the pixel lines implementation above which specifies the extent of quads to pixel borders.
    ptsAtDepth.push_back(
      {(pt.x() + 0.5f) * xScale - 1.0f, (pt.y() + 0.5f) * yScale + 1.0f, vertexDepth});
  }
  // Update geometry.
  ptCloud->geometry().setVertices(ptsAtDepth).setColors(color).setPoints(inds);
  ptCloud->material().set(Shaders::POINT_CLOUD_POINT_SIZE, pointPixels);
}

std::unique_ptr<Camera> orthographicCamera(
  float xLeft, float xRight, float yUp, float yDown, float zNear, float zFar) {
  auto cam = camera();
  // Flip near and far planes for left-handed clip space
  cam->setProjection(
    Intrinsics::orthographicProjectionRightHanded(xLeft, xRight, yUp, yDown, -zNear, -zFar));
  return cam;
}

std::unique_ptr<Camera> perspectiveCamera(
  c8_PixelPinholeCameraModel intrinsics, int viewportWidth, int viewportHeight) {
  auto intrinsicsForViewport =
    Intrinsics::rotateCropAndScaleIntrinsics(intrinsics, viewportWidth, viewportHeight);
  auto cam = camera();
  cam->setProjection(Intrinsics::toClipSpaceMatLeftHanded(
    intrinsicsForViewport, CAM_PROJECTION_NEAR_CLIP, CAM_PROJECTION_FAR_CLIP));
  return cam;
}

void updatePerspectiveCamera(
  Camera *cam, c8_PixelPinholeCameraModel intrinsics, int viewportWidth, int viewportHeight) {
  auto intrinsicsForViewport =
    Intrinsics::rotateCropAndScaleIntrinsics(intrinsics, viewportWidth, viewportHeight);
  cam->setProjection(Intrinsics::toClipSpaceMatLeftHanded(
    intrinsicsForViewport, CAM_PROJECTION_NEAR_CLIP, CAM_PROJECTION_FAR_CLIP));
}

void updatePerspectiveCamera(
  Camera *cam,
  c8_PixelPinholeCameraModel intrinsics,
  int viewportWidth,
  int viewportHeight,
  float zNear,
  float zFar) {
  auto intrinsicsForViewport =
    Intrinsics::rotateCropAndScaleIntrinsics(intrinsics, viewportWidth, viewportHeight);
  cam->setProjection(Intrinsics::toClipSpaceMatLeftHanded(intrinsicsForViewport, zNear, zFar));
}

std::unique_ptr<Camera> pixelCamera(int viewportWidth, int viewportHeight) {
  auto cam = camera();
  // Note: This should be flipping y, but we're not doing that here.
  cam->setProjection(HMatrix{
    {2.0f / viewportWidth, 0.0f, 0.0f, -1.0f},
    {0.0f, 2.0f / viewportHeight, 0.0f, -1.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {viewportWidth / 2.0f, 0.0f, 0.0f, viewportWidth / 2.0f},
    {0.0f, viewportHeight / 2.0f, 0.0f, viewportHeight / 2.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}});
  return cam;
}

std::unique_ptr<Camera> pixelCamera(Rect rect) { return pixelCamera(rect.width, rect.height); }

std::unique_ptr<Light> ambientLight(Color color, float intensity) {
  auto l = light();
  l->setKind(Light::LightKind::AMBIENT)
    .setColor(color)
    .setIntensity(intensity)
    .setName("ambient-light");
  return l;
}

std::unique_ptr<Light> pointLight(Color color, float intensity, bool render) {
  auto l = light();
  l->setKind(Light::LightKind::POINT)
    .setColor(color)
    .setIntensity(intensity)
    .setName("point-light");
  ;
  if (render) {
    auto &cube = l->add(renderable())
                   .setKind(Renderable::MESH)
                   .setGeometry(GeoGen::cubeMesh())
                   .setMaterial(MatGen::colorOnly());
    auto s = HMatrixGen::scale(0.1f, 0.1f, 0.1f);
    cube.setLocal(s);
    cube.geometry().setColor(color);
  }
  return l;
}

std::unique_ptr<Renderable> arrow(Color color) {
  float stemHeight = 0.80f;
  float headHeight = 1.0f - stemHeight;
  float stemWidth = 0.025f / stemHeight;
  float headWidth = 0.06f / headHeight;
  auto c1Pos = HMatrixGen::xDegrees(90.0f) * HMatrixGen::translateY(-headHeight * 0.5f)
    * HMatrixGen::scale(stemHeight);
  auto c2Pos = HMatrixGen::xDegrees(90.0f) * HMatrixGen::translateY(stemHeight * 0.5f)
    * HMatrixGen::scale(headHeight);

  auto rod = ObGen::curvyMesh({stemWidth, stemWidth});
  auto cone = ObGen::curvyMesh({0.0f, headWidth});

  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::empty())
    .setMaterial(MatGen::physical())
    .setName("arrow");
  Vector<HPoint3> vertices;
  Vector<HVector3> normals;
  Vector<HVector2> uvs;
  Vector<MeshIndices> triangles;

  {
    // Rod.
    auto newVertices = c1Pos * rod->geometry().vertices();
    auto newNormals = c1Pos * rod->geometry().normals();
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
    normals.insert(normals.end(), newNormals.begin(), newNormals.end());
    uvs.insert(uvs.end(), rod->geometry().uvs().begin(), rod->geometry().uvs().end());
    triangles.insert(
      triangles.end(), rod->geometry().triangles().begin(), rod->geometry().triangles().end());
  }

  {
    // Cone.
    int indexOffset = vertices.size();

    auto newVertices = c2Pos * cone->geometry().vertices();
    auto newNormals = c2Pos * cone->geometry().normals();
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
    normals.insert(normals.end(), newNormals.begin(), newNormals.end());
    uvs.insert(uvs.end(), cone->geometry().uvs().begin(), cone->geometry().uvs().end());
    // Triangle indices are relative to vertices.
    const auto &coneTriangles = cone->geometry().triangles();
    for (const auto &t : coneTriangles) {
      triangles.push_back({t.a + indexOffset, t.b + indexOffset, t.c + indexOffset});
    }
  }

  // Update geometry.
  el->geometry()
    .setVertices(vertices)
    .setColor(color)
    .setTriangles(triangles)
    .setUvs(uvs)
    .setNormals(normals);
  return el;
}

std::unique_ptr<Light> directionalLight(Color color, float intensity, bool render) {
  auto l = light();
  l->setKind(Light::LightKind::DIRECTIONAL)
    .setColor(color)
    .setIntensity(intensity)
    .setName("directional-light");
  if (render) {
    l->add(arrow(color));
  }
  return l;
}

std::unique_ptr<Renderable> meshGeometry(const MeshGeometry &mesh) {
  return meshGeometry(
    mesh.points.data(),
    mesh.points.size(),
    mesh.colors.data(),
    mesh.colors.size(),
    mesh.triangles.data(),
    mesh.triangles.size(),
    mesh.normals.data(),
    mesh.normals.size(),
    mesh.uvs.data(),
    mesh.uvs.size());
}

std::unique_ptr<Renderable> meshGeometry(
  const HPoint3 *points,
  int numPoints,
  const Color *colors,
  int numColors,
  const MeshIndices *triangles,
  int numTriangles,
  const HVector3 *normals,
  int numNormals,
  const HVector2 *uvs,
  int numUvs) {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::empty())
    .setMaterial(MatGen::colorOnly())
    .setName("mesh-geo");

  // Update geometry.
  if (numPoints > 0) {
    el->geometry().setVertices(points, numPoints);
  }
  if (numColors > 0) {
    el->geometry().setColors(colors, numColors);
  }
  if (numTriangles > 0) {
    el->geometry().setTriangles(triangles, numTriangles);
  }
  if (numUvs > 0) {
    el->geometry().setUvs(uvs, numUvs);
  }
  if (numNormals > 0) {
    el->geometry().setNormals(normals, numNormals);
  }
  return el;
}

std::unique_ptr<Renderable> cubemapMesh() {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::cubeVertexOnlyMesh())
    .setMaterial(MatGen::cubemap())
    .setName("cubemap-mesh");
  return el;
}

// Makes all renderables in the supplied hierarchy transparent with the specified opacity.
void setTransparent(Object8 &n, float opacity) {
  if (n.is<Renderable>()) {
    n.as<Renderable>().material().setTransparent(true).setOpacity(opacity);
  }
  for (auto *c : n.mutableChildren()) {
    // Don't recurse into scenes.
    if (c->is<Scene>()) {
      continue;
    }
    setTransparent(*c, opacity);
  }
}

std::unique_ptr<Renderable> splatAttributes() {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::quad(false))
    .setMaterial(MatGen::splatAttributes())
    .setName("splat");
  return el;
}

std::unique_ptr<Renderable> splatAttributes(
  const Vector<HPoint3> &positions,
  const Vector<Quaternion> &rotations,
  const Vector<HVector3> &scales,
  const Vector<Color> &colors) {
  auto el = splatAttributes();
  updateSplatAttributes(el.get(), positions, rotations, scales, colors);
  return el;
}

void updateSplatAttributes(
  Renderable *splat,
  const Vector<HPoint3> &newPositions,
  const Vector<Quaternion> &newRotations,
  const Vector<HVector3> &newScales,
  const Vector<Color> &newColors) {
  auto &g = splat->geometry();
  g.setInstancePositions(newPositions);
  g.setInstanceRotations(newRotations);
  g.setInstanceScales(newScales);
  g.setInstanceColors(newColors);
}

std::unique_ptr<Renderable> splatAttributes(
  int numPoints,                // number of points in the splat.
  const HPoint3 *positions,     // Center of each splat.
  const Quaternion *rotations,  // Rotation of each splat.
  const HVector3 *scales,       // Scale of each splat.
  const Color *colors           // Colors of each splat.
) {
  auto el = splatAttributes();
  updateSplatAttributes(el.get(), numPoints, positions, rotations, scales, colors);
  return el;
}

void updateSplatAttributes(
  Renderable *splat,               // Renderable object to update.
  int numPoints,                   // number of points in the splat.
  const HPoint3 *newPositions,     // Center of each splat.
  const Quaternion *newRotations,  // Rotation of each splat.
  const HVector3 *newScales,       // Scale of each splat.
  const Color *newColors           // Colors of each splat.
) {
  auto &g = splat->geometry();
  g.setInstancePositions(newPositions, numPoints);
  g.setInstanceRotations(newRotations, numPoints);
  g.setInstanceScales(newScales, numPoints);
  g.setInstanceColors(newColors, numPoints);
}

std::unique_ptr<Renderable> splatSortedTexture(const SplatMetadata &header) {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::quad(false))
    .setMaterial(MatGen::splatSortedTexture())
    .setName("splat");
  el->material().set(Shaders::RENDERER_ANTIALIASED, static_cast<float>(header.antialiased));
  return el;
}

std::unique_ptr<Renderable> splatTexture(
  const SplatMetadata &header, RGBA32PlanePixelBuffer &&texture) {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::quad(false))
    .setMaterial(MatGen::splatTexture())
    .setName("splat");
  el->material().setColorTexture(TexGen::rgba32PixelBuffer(std::move(texture)));
  el->material().set(Shaders::RENDERER_ANTIALIASED, static_cast<float>(header.antialiased));
  return el;
}

std::unique_ptr<Renderable> splatTexture(
  const SplatMetadata &header, RGBA32PlanePixelBuffer &&texture, const Vector<uint32_t> &indices) {
  auto el = splatTexture(header, std::move(texture));
  updateSplatTexture(el.get(), indices);
  return el;
}

std::unique_ptr<Renderable> splatTexture(
  const SplatMetadata &header,
  RGBA32PlanePixelBuffer &&texture,
  const uint32_t *indices,
  int numIndices) {
  auto el = splatTexture(header, std::move(texture));
  updateSplatTexture(el.get(), indices, numIndices);
  return el;
}

void updateSplatTexture(Renderable *splat, const Vector<uint32_t> &indices) {
  splat->geometry().setInstanceIds(indices);
}

void updateSplatTexture(Renderable *splat, const uint32_t *indices, int numIndices) {
  splat->geometry().setInstanceIds(indices, numIndices);
}

std::unique_ptr<Renderable> splatTextureStacked(
  const SplatMetadata &header, RGBA32PlanePixelBuffer &&texture, int numStacks) {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::stackedQuad(numStacks))
    .setMaterial(MatGen::splatTextureStacked())
    .setName("splat");
  // Tex slot 0.
  el->material().setTexture(
    Shaders::RENDERER_SPLAT_DATA_TEX, TexGen::rgba32PixelBuffer(std::move(texture)));

  // Tex slot 1.
  int idTexSize = std::ceil(std::sqrt(header.maxNumPoints));
  el->material().setTexture(
    Shaders::RENDERER_SPLAT_IDS_TEX,
    TexGen::r32PixelBuffer(R32PlanePixelBuffer(idTexSize, idTexSize)));

  // Uniforms
  el->material().set(Shaders::RENDERER_ANTIALIASED, static_cast<float>(header.antialiased));
  el->material().set(Shaders::RENDERER_STACKED_INSTANCE_COUNT, static_cast<float>(numStacks));
  return el;
}

void updateSplatTextureStacked(
  Renderable *splat,        // Renderable object to update.
  const uint32_t *indices,  // Indices of splat data.
  int numIndices            // Indices of splat data.
) {
  auto pix = splat->material().texture(Shaders::RENDERER_SPLAT_IDS_TEX)->mutableR32Pixels();
  if (numIndices > pix.rows() * pix.cols()) {
    C8Log(
      "[object8] WARNING: Too many indices for splat stacked texture (%d vs %d)",
      numIndices,
      pix.rows() * pix.cols());
    return;
  }

  std::memcpy(pix.pixels(), indices, numIndices * sizeof(uint32_t));
  std::fill(pix.pixels() + numIndices, pix.pixels() + pix.rows() * pix.cols(), 0xFFFFFFFF);

  int numStacks = splat->geometry().vertices().size() / 4;
  if (numStacks * 4 != splat->geometry().vertices().size()) {
    C8Log(
      "[object8] WARNING: Stacked splat texture has %d vertices, which is not a multiple of 4",
      splat->geometry().vertices().size());
    return;
  }

  splat->geometry().setInstanceCount(std::ceil(numIndices / static_cast<float>(numStacks)));
}

std::unique_ptr<Renderable> splatMultiTex(const SplatMultiTextureView &splat) {
  auto el = renderable();
  el->setKind(Renderable::MESH)
    .setGeometry(GeoGen::stackedQuad(128))
    .setMaterial(MatGen::splatMultiTexFinal())
    .setName("splat");

  Vector<RenderSpec> shRenderSpecs = Vector<RenderSpec>{
    RenderSpec{splat.shRG.cols(), splat.shRG.rows(), "shColor", "shCam"},
  };
  shRenderSpecs[0].clearBuffers = BufferClear::NONE;
  auto &shScene = el->add(ObGen::subScene("shScene", shRenderSpecs));
  shScene.add(ObGen::named(camera(), "shCam"));
  auto &shQuad = shScene.add(ObGen::quad());
  shQuad.setMaterial(MatGen::splatMultiTexSh())
    .material()
    .setTexture(
      Shaders::RENDERER_SPLAT_POSITIONCOLOR_TEX, TexGen::rgba32Pixels(splat.positionColor))
    .setTexture(Shaders::RENDERER_SPLAT_SHRG_TEX, TexGen::rgba32Pixels(splat.shRG))
    .setTexture(Shaders::RENDERER_SPLAT_SHB_TEX, TexGen::rgba32Pixels(splat.shB));

  // Tex slot 0.
  int idTexSize = std::ceil(std::sqrt(splat.header.maxNumPoints));
  el->material()
    .setTexture(
      Shaders::RENDERER_SPLAT_IDS_TEX,
      TexGen::r32PixelBuffer(R32PlanePixelBuffer(idTexSize, idTexSize)))
    .setTexture(
      Shaders::RENDERER_SPLAT_POSITIONCOLOR_TEX, TexGen::rgba32Pixels(splat.positionColor))
    .setTexture(
      Shaders::RENDERER_SPLAT_ROTATIONSCALE_TEX, TexGen::rgba32Pixels(splat.rotationScale))
    .setTexture(
      Shaders::RENDERER_SPLAT_SHCOLOR_TEX, TexGen::sceneTexture("shScene", "shColor"));


  // Uniforms
  el->material().set(Shaders::RENDERER_ANTIALIASED, static_cast<float>(splat.header.antialiased));
  el->material().set(Shaders::RENDERER_STACKED_INSTANCE_COUNT, static_cast<float>(128));
  return el;
}

void updateSplatMultiTex(Renderable *splat, const uint32_t *indices, int numIndices) {
  auto pix = splat->material().texture(Shaders::RENDERER_SPLAT_IDS_TEX)->mutableR32Pixels();
  if (numIndices > pix.rows() * pix.cols()) {
    C8Log(
      "[object8] WARNING: Too many indices for splat stacked texture (%d vs %d)",
      numIndices,
      pix.rows() * pix.cols());
    return;
  }

  std::memcpy(pix.pixels(), indices, numIndices * sizeof(uint32_t));
  std::fill(pix.pixels() + numIndices, pix.pixels() + pix.rows() * pix.cols(), 0xFFFFFFFF);

  int numStacks = splat->geometry().vertices().size() / 4;
  if (numStacks * 4 != splat->geometry().vertices().size()) {
    C8Log(
      "[object8] WARNING: Stacked splat texture has %d vertices, which is not a multiple of 4",
      splat->geometry().vertices().size());
    return;
  }

  splat->geometry().setInstanceCount(std::ceil(numIndices / static_cast<float>(numStacks)));
}

void updateSplatMultiTexCamera(Renderable *splat, const Camera &sceneCamera) {
  if (splat == nullptr) {
    return;
  }
  if (!splat->has<Scene>("shScene")) {
    return;
  }
  auto &shScene = splat->find<Scene>("shScene");
  if (!shScene.has<Camera>("shCam")) {
    return;
  }
  // TODO: Just pass in translation for SH.
  auto camInModel = splat->world().inv() * sceneCamera.world();
  splat->find<Scene>("shScene").find<Camera>("shCam").setLocal(camInModel);
}

}  // namespace ObGen

}  // namespace c8
