// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <memory>

#include "c8/color.h"
#include "c8/geometry/mesh-types.h"
#include "c8/geometry/splat.h"
#include "c8/hmatrix.h"
#include "c8/model/model-data-view.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/render/geometry.h"
#include "c8/pixels/render/material.h"
#include "c8/pixels/render/sizing.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/vector.h"

namespace c8 {
constexpr const float CAM_PROJECTION_NEAR_CLIP = 1e-2f;
constexpr const float CAM_PROJECTION_FAR_CLIP = 1e3f;

// Defines the Object8 scene graph framework. An Object8 lays out a 3D scene and its contents as a
// tree hierarchy.
//
// Object8 cannot be directly instantiated, but each item in the scene graph is a type of Object8:
//
// Types of Object8:
// - Scene - the root node
// - Camera - defines view/projection semantics when capturing the scene.
// - Light - defines how lighting works in the scene.
// - Renderable - something that will be drawn. It has a geometry and a material.
// - Group - an empty container for other objects.
class Object8 {
  static int incrementingId;

public:
  Object8() : id_(format("%d", incrementingId++)){};
  virtual ~Object8() = default;

  const Object8 *parent() const { return parent_; };
  Object8 *parent() { return parent_; };

  // Make a pure virtual function that subclasses must override so that Object8 is not directly
  // instantiable.
  virtual String type() const = 0;

  const Vector<std::unique_ptr<Object8>> &children() const { return children_; }
  Vector<Object8 *> mutableChildren();

  // Find a descendant with the specified class. Throws c8::OutOfRange if not found.
  template <typename T>
  T &find() {
    return findKind<T>();
  }

  template <typename T>
  const T &find() const {
    return findKind<T>();
  }

  // Check whether some node with the specified class is a descendant.
  template <typename T>
  bool has() const {
    return findKindImpl<T>() != nullptr;
  }

  // Check whether some node with the specified name is a descendant.
  template <typename T>
  bool has(const String &name) const {
    return this->findNameOfKindImpl<T>(name) != nullptr;
  }

  // Find a descendant with the specified name. Throws c8::OutOfRange if not found.
  template <typename T>
  T &find(const String &name) {
    return findNameOfKind<T>(name);
  }

  template <typename T>
  const T &find(const String &name) const {
    return findNameOfKind<T>(name);
  }

  String id() const { return id_; }
  bool enabled() const { return enabled_; }
  const HMatrix &local() const { return local_; }
  const HMatrix &world() const { return world_; }
  const String &name() const { return name_; }
  const String &metadata() const { return metadata_; };

  Object8 &setEnabled(bool enabled);
  Object8 &setLocal(const HMatrix &local);
  Object8 &setName(const String &name);
  Object8 &setMetadata(const String &metadata);

  // Add a root node as a child of an existing node, transferring ownership. After this operation,
  // the passed in unique_ptr will contain nullptr, and this Object8 will own the memory for the
  // child until it is removed.
  //
  // This method returns the newly added child to make inline construction easier, e.g.
  //
  // auto scene = ObGen::scene();
  // auto &camera = scene->add(ObGen::perspectiveCameraFovDegrees(60, 640, 480));
  template <typename T>
  T &add(std::unique_ptr<T> &&child) {
    children_.push_back(std::move(child));
    children_.back()->parent_ = this;
    children_.back()->updateWorld();
    return children_.back()->as<T>();
  }

  // Remove this node from its parent, and transfer ownership to the caller. If the caller lets
  // this removed node go out of scope, the node and all its children are deallocated.
  std::unique_ptr<Object8> remove();

  // Checks whether Object8 is of the correct subclass.
  // e.g. scene->is<Scene>() returns true if "scene" is a Scene.
  template <typename T>
  bool is() const {
    return dynamic_cast<const T *>(this) != nullptr;
  }

  // Get the object as the asserted type, throwing std::bad_cast if is<T>() returns false.
  // e.g. camera->as<Camera>().projection()
  template <typename T>
  T &as() {
    return dynamic_cast<T &>(*this);
  }

  template <typename T>
  const T &as() const {
    return dynamic_cast<const T &>(*this);
  }

  String toString() const noexcept;

private:
  const String id_;
  bool enabled_ = true;
  HMatrix local_ = HMatrixGen::i();  // Local model matrix.
  HMatrix world_ = HMatrixGen::i();  // Cached world model matrix, parent_->world() * local_;
  Object8 *parent_ = nullptr;
  Vector<std::unique_ptr<Object8>> children_;
  String name_ = "";
  // Used for adding additional metadata that will be rendered on Omniscope's Inspector
  String metadata_ = "";

  void updateWorld();
  const Object8 *findNameImpl(const String &name) const;

  template <typename T>
  T &findKind() {
    return const_cast<T &>(const_cast<const Object8 *>(this)->findKind<T>());
  }

  template <typename T>
  const T &findKind() const {
    const auto *found = findKindImpl<T>();
    if (found == nullptr) {
      C8_THROW_OUT_OF_RANGE(String("No node with requested kind: ") + typeid(T).name());
    }
    return *found;
  }

  template <typename T>
  const T *findKindImpl() const {
    if (this->is<T>()) {
      return &this->as<T>();
    }
    for (const auto &child : children_) {
      // do not go into subscenes looking for items
      if (child->type() == "Scene") {
        if (child->is<T>()) {
          return &child->as<T>();
        }
        continue;
      }
      auto *found = child->findKindImpl<T>();
      if (found != nullptr) {
        return found;
      }
    }
    return nullptr;
  }

  template <typename T>
  T &findNameOfKind(const String &name) {
    return const_cast<T &>(const_cast<const Object8 *>(this)->findNameOfKind<T>(name));
  }

  template <typename T>
  const T &findNameOfKind(const String &name) const {
    const auto *found = findNameOfKindImpl<T>(name);
    if (found == nullptr) {
      String errorMsg =
        format("No node with requested kind '%s' and name '%s'", typeid(T).name(), name.c_str());
      C8_THROW_OUT_OF_RANGE(errorMsg);
    }
    return *found;
  }

  // Return the first object that matches the name and type
  template <typename T>
  const T *findNameOfKindImpl(const String &name) const {
    if (name == name_ && this->is<T>()) {
      return &this->as<T>();
    }
    for (const auto &child : children_) {
      // do not go into subscenes looking for items
      if (child->type() == "Scene") {
        if (child->name() == name && child->is<T>()) {
          return &child->as<T>();
        }
        continue;
      }
      auto *found = child->findNameOfKindImpl<T>(name);
      if (found != nullptr) {
        return found;
      }
    }
    return nullptr;
  }
};

// Camera - defines view/projection semantics when capturing the scene.
// Create cameras with different properties with factory functions, e.g.
// - perspectiveCamera(intrinsics, viewWidth, viewHeight)
// - perspectiveCamera(fov, viewWidth, viewHeight)
// - orthographicCamera(fov, viewWidth, viewHeight)
class Camera : public Object8 {
public:
  String type() const override { return "Camera"; }
  const HMatrix &projection() const { return projection_; }
  Camera &setProjection(const HMatrix &projection);

  // Toggle the y direction of this projection. This can be useful for systems that expect texture
  // data to be inverted.
  Camera &flipY();

private:
  HMatrix projection_ = HMatrixGen::i();
};

// Light - defines how lighting works in the scene.
// Create lights with different properties with factory functions, e.g.
// - ambientLight(Color::PURPLE, 0.75f);
// - positioned(pointLight(Color::PURPLE, 0.75f), HMatrixGen::translation(1.0f, 1.0f, 0.0f));
// - positioned(directionalLight(Color::GREEN, 1.f, true),
//     rotationToVector({0.f, 1.f, 0.f}).toRotationMat()));

// If no lights are added, we will default to a white ambient light.
class Light : public Object8 {
public:
  enum LightKind {
    UNSPECIFIED = 0,
    AMBIENT = 1,
    POINT = 2,
    DIRECTIONAL = 3,  // Defaults to point forward along z.
  };

  String type() const override { return "Light"; }
  LightKind kind() const { return kind_; }
  Color color() const { return color_; }
  float intensity() const { return intensity_; }

  Light &setKind(LightKind kind);
  Light &setColor(Color color);
  Light &setIntensity(float intensity);

private:
  LightKind kind_ = LightKind::UNSPECIFIED;
  Color color_ = Color::WHITE;
  float intensity_ = 1.0f;
};

// Group - an empty container for other objects.
class Group : public Object8 {
  String type() const override { return "Group"; }
};

struct Viewport {
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
};

enum class BufferClear { UNSPECIFIED, ALL, NONE };

// A RenderSpec is associated with a sub-scene.  It is referenced by name via a SceneMaterial in
// order to render a scene as a texture.  This is most commonly going to be used for rendering a
// sub-scene to a quad in order to render the scene to a portion of the viewport.
struct RenderSpec {
  // defines the resolution of the offscreen framebuffer that the sub-scene
  int resolutionWidth = 0;
  int resolutionHeight = 0;

  // the name of the render spec, this allows it to be referenced by SceneMaterial
  String name;
  // name of the camera that will render the sub-scene for this render spec.
  String cameraName;
  Viewport viewport;
  BufferClear clearBuffers = BufferClear::UNSPECIFIED;
  Color clearColor = Color::BLACK;

  // Blit the scene to the screen after rendering.
  bool blitToScreen = false;
  Viewport screenViewport;

  // Draw to a framebuffer instead of the screen.
  int outputFramebuffer = 0;
};

// Abstract base class for rendering a scene. The implementation of this is specific to a
// gpu framework (e.g. vulkan vs. metal vs. opengl) and this will be set by the renderer.
class SceneRenderer {
public:
  // render the scene with the corresponding renderSpec settings
  virtual void render(const RenderSpec &renderSpec) = 0;
  // returns the ID of the texture that renderspec renders to.
  virtual int32_t nativeTextureId(const String &renderSpecName) = 0;
  virtual ~SceneRenderer() = default;
};

// Scene - used as either the root node or as a sub-scene
class Scene : public Object8 {
public:
  String type() const override { return "Scene"; }

  // only necessary for sub-scenes
  void setRenderSpecs(const Vector<RenderSpec> &renderSpecs);
  const Vector<RenderSpec> &renderSpecs() const;
  RenderSpec &renderSpec(const String &name = "");
  const RenderSpec &renderSpec(const String &name = "") const;

  // Get the renderer for this scene. This is set by the Renderer, and its implementation is
  // specific to the renderer.
  SceneRenderer *renderer() { return renderer_.get(); }
  Scene &setRenderer(std::unique_ptr<SceneRenderer> &&renderer);

  bool needsRerender() const { return needsRerender_; }
  void setNeedsRerender(bool val) { needsRerender_ = val; }

private:
  Vector<RenderSpec> renderSpecs_;
  std::unique_ptr<SceneRenderer> renderer_;
  // Layout thread sets to request that the scene is re-rendered.
  bool needsRerender_ = false;
  // TODO: fog?
  // TODO: background -- color? texture? material?
  // TODO: environment?
};

// Renderable - something that will be drawn. It has a geometry and a material.
// Create renderables with factory functions, e.g.
// - quadMesh({1.0f, 1.0f}, color)
// - cubeMesh({1.0f, 1.0f, 1.f}, material)
// - pointCloud(worldPts, colors)
class Renderable : public Object8 {
public:
  enum RenderableKind {
    UNSPECIFIED = 0,
    POINTS = 1,
    MESH = 2,
  };
  String type() const override { return "Renderable"; }
  RenderableKind kind() const { return kind_; }

  const Geometry &geometry() const { return *geometry_; }
  Geometry &geometry() { return *geometry_; }

  const Material &material() const { return *material_; }
  Material &material() { return *material_; }

  // If true, ignore the scene camera when drawing this geometry. The world matrix is passed
  // directly as the mvp parameter to the material, and can be used to manipulate vertex positions
  // in clip space directly.
  bool ignoreProjection() const { return ignoreProjection_; }

  // TODO(nb): Material
  Renderable &setKind(RenderableKind kind);
  Renderable &setGeometry(std::unique_ptr<Geometry> &&geometry);
  Renderable &setMaterial(std::unique_ptr<Material> &&material);
  Renderable &setIgnoreProjection(bool ignoreProjection);

  const Vector<String> &elementMetadata() const { return elementMetadata_; };
  Renderable &setElementMetadata(const Vector<String> &elementMetadata);

private:
  RenderableKind kind_;
  // Having these be heap types keeps their address stable across moves and allows us to associate
  // them with GPU resources.
  std::unique_ptr<Geometry> geometry_ = GeoGen::empty();
  std::unique_ptr<Material> material_ = MatGen::empty();
  bool ignoreProjection_ = false;
  // All Object8 instances have metadata, but Renderables also have elementMetadata.  This is
  // typically used for adding metadata to points or lines of a Renderable instance.
  Vector<String> elementMetadata_;
};

// Factory methods for creating Object8 instances.
namespace ObGen {

// Convenience method for preserving type on a generated object when setting its properties.
// For example:
// auto lightPos = HMatrixGen::translateY(10.0f);  // 10 units above the origin.
// Light &l = scene->add(positioned(named(ObGen::light(), "ambient"), lightPos));
template <typename T>
std::unique_ptr<T> &&named(std::unique_ptr<T> &&object, const String &name) {
  object->setName(name);
  return std::move(object);
}

template <typename T>
std::unique_ptr<T> &&positioned(std::unique_ptr<T> &&object, const HMatrix &pos) {
  object->setLocal(pos);
  return std::move(object);
}

// Makes all renderables in the supplied hierarchy transparent with the specified opacity.
void setTransparent(Object8 &node, float opacity);

// Makes all renderables in the supplied hierarchy transparent with the specified opacity.
template <typename T>
std::unique_ptr<T> &&transparent(std::unique_ptr<T> &&n, float opacity) {
  setTransparent(*n, opacity);
  return std::move(n);
}

// Basic factory methods for creating an object with default data.
std::unique_ptr<Scene> scene(int pixelsWidth, int pixelsHeight);
std::unique_ptr<Scene> subScene(const String &name, const Vector<RenderSpec> &renderSpecs);
std::unique_ptr<Camera> camera();
std::unique_ptr<Group> group();
std::unique_ptr<Light> light();
std::unique_ptr<Renderable> renderable();

// More advanced factory methods.

// Get a basic quad at z=0, with x,y corners at -1:1.
std::unique_ptr<Renderable> quad(bool interleaved = false);
std::unique_ptr<Renderable> quadPoints();

// Create a basic cube with a color material
std::unique_ptr<Renderable> cubeMesh();

// Get a basic quad at z=1, with x,y corners at -1:1. This is the back plane of clip-space, and
// it is configured with Object8::setIgnoreProjection(true), which can be used to draw directly to
// the back of the whole viewport.
std::unique_ptr<Renderable> backQuad();

// Get a basic quad at z=-1, with x,y corners at -1:1. This is the front plane of clip-space, and
// it is configured with Object8::setIgnoreProjection(true), which can be used to draw directly to
// the front of the whole viewport.
std::unique_ptr<Renderable> frontQuad();

// This can be used to draw to a portion of the viewport.  The parameters are all in pixel space.
std::unique_ptr<Renderable> pixelQuad(float x, float y, float width, float height);
std::unique_ptr<Renderable> pixelQuad(Rect rect);

// Get a cylinder or cone.
std::unique_ptr<Renderable> curvyMesh(const GeoGen::CurvyMeshParams &params);

// Get or update a point cloud.
std::unique_ptr<Renderable> pointCloud(const Vector<HPoint3> &pts, float ptSize = 0.015f);
std::unique_ptr<Renderable> pointCloud(
  const Vector<HPoint3> &pts, Color color, float ptSize = 0.015f);
std::unique_ptr<Renderable> pointCloud(
  const Vector<HPoint3> &pts, const Vector<Color> &color, float ptSize = 0.015f);
std::unique_ptr<Renderable> pointCloud(
  const HPoint3 *pts, int numPoints, const Color *color, int numColors, float ptSize = 0.015f);

void updatePointCloud(Renderable *ptCloud, const Vector<HPoint3> &newPts);
void updatePointCloud(Renderable *ptCloud, const Vector<HPoint3> &newPts, Color color);
void updatePointCloud(
  Renderable *ptCloud, const Vector<HPoint3> &newPts, const Vector<Color> &color);
void updatePointCloud(
  Renderable *ptCloud, const HPoint3 *newPts, int numPoints, const Color *color, int numColors);
// Expects the newPts and values to be of the same length. Colors the points based off the
// corresponding values using the given colorMap.
void updatePointCloud(
  Renderable *ptCloud,
  const Vector<HPoint3> &newPts,
  const uint8_t (&colorMap)[256][3],
  const Vector<double> &values);

// Get or update a bar cloud.
std::unique_ptr<Renderable> barCloud();

std::unique_ptr<Renderable> barCloud(
  const Vector<std::pair<HPoint3, HPoint3>> &lines, Color color, float barRadius = 0.015f);

std::unique_ptr<Renderable> barCloud(
  const Vector<std::pair<HPoint3, HPoint3>> &lines,
  const Vector<Color> &color,
  float barRadius = 0.015f);

void updateBarCloud(
  Renderable *barCloud,
  const Vector<std::pair<HPoint3, HPoint3>> &newLines,
  Color color,
  float barRadius = 0.015f);

void updateBarCloud(
  Renderable *barCloud,
  const Vector<std::pair<HPoint3, HPoint3>> &newLines,
  const Vector<Color> &color,
  float barRadius = 0.015f);

/**
 * Create a cloud of image quads with the same scale.
 * @param positions position and orientation of each quad.
 */
std::unique_ptr<Renderable> quadCloud(
  const Vector<HMatrix> &positions,
  const Vector<Vector<HVector2>> &uvs,
  const Vector<float> &scales);

/**
 * Update a cloud of image quads with the same scale.
 * @param positions position and orientation of each quad.
 */
void updateQuadCloud(
  Renderable *quadCloud,
  const Vector<HMatrix> &positions,
  const Vector<Vector<HVector2>> &newUvs,
  const Vector<float> &newScales);

// Get lines as quads positioned directly in clip space, with `ignoreProjection` set.
std::unique_ptr<Renderable> pixelLines();

std::unique_ptr<Renderable> pixelLines(
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,  // Locations for line start and end.
  Color color,                                            // Color of the line.
  int scenePixelsWidth,                                   // The width of the scene in pixels.
  int scenePixelsHeight,                                  // The height of the scene in pixels.
  float linePixelThickness = 2.0f,  // Thickness of the line in pixels assuming scene size above.
  float vertexDepth = -1.0f         // Render on the very front of the quad.
);

std::unique_ptr<Renderable> pixelLines(
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,  // Locations for line start and end.
  const Vector<Color> &color,                             // Color of the line.
  int scenePixelsWidth,                                   // The width of the scene in pixels.
  int scenePixelsHeight,                                  // The height of the scene in pixels.
  float linePixelThickness = 2.0f,  // Thickness of the line in pixels assuming scene size above.
  float vertexDepth = -1.0f         // Render on the very front of the quad.
);

void updatePixelLines(
  Renderable *lineCloud,                                  // Renderable object to update.
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,  // Locations for line start and end.
  Color color,                                            // Color of the line.
  int scenePixelsWidth,                                   // The width of the scene in pixels.
  int scenePixelsHeight,                                  // The height of the scene in pixels.
  float linePixelThickness = 2.0f,  // Thickness of the line in pixels assuming scene size above.
  float vertexDepth = -1.0f         // Render on the very front of the quad.
);

void updatePixelLines(
  Renderable *lineCloud,                                  // Renderable object to update.
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,  // Locations for line start and end.
  const Vector<Color> &color,                             // Color of the line.
  int scenePixelsWidth,                                   // The width of the scene in pixels.
  int scenePixelsHeight,                                  // The height of the scene in pixels.
  float linePixelThickness = 2.0f,  // Thickness of the line in pixels assuming scene size above.
  float vertexDepth = -1.0f         // Render on the very front of the quad.
);

void updatePixelLines(
  Renderable *lineCloud,                                  // Renderable object to update.
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,  // Locations for line start and end.
  std::pair<Color, Color> color,                          // Different colors for the line ends.
  int scenePixelsWidth,                                   // The width of the scene in pixels.
  int scenePixelsHeight,                                  // The height of the scene in pixels.
  float linePixelThickness = 2.0f,  // Thickness of the line in pixels assuming scene size above.
  float vertexDepth = -1.0f         // Render on the very front of the quad.
);

void updatePixelLines(
  Renderable *lineCloud,                                  // Renderable object to update.
  const Vector<std::pair<HPoint2, HPoint2>> &linePixels,  // Locations for line start and end.
  const Vector<std::pair<Color, Color>> &color,           // Different colors for the line ends.
  int scenePixelsWidth,                                   // The width of the scene in pixels.
  int scenePixelsHeight,                                  // The height of the scene in pixels.
  float linePixelThickness = 2.0f,  // Thickness of the line in pixels assuming scene size above.
  float vertexDepth = -1.0f         // Render on the very front of the quad.
);

// Get pixels as circles positioned directly in clip space, with `ignoreProjection` set.
std::unique_ptr<Renderable> pixelPoints();

void updatePixelPoints(
  Renderable *ptCloud,            // Renderable object to update.
  const Vector<HPoint2> &newPts,  // Center pixels of the points.
  Color color,                    // Color of the points.
  int scenePixelsWidth,           // The width of the scene in pixels.
  int scenePixelsHeight,          // The height of the scene in pixels.
  float pointPixels = 3.0f,       // Size (diameter) of the point, in pixels.
  float vertexDepth = -1.0f       // Render on the very front of the quad.
);

void updatePixelPoints(
  Renderable *ptCloud,            // Renderable object to update.
  const Vector<HPoint2> &newPts,  // Center pixels of the points.
  const Vector<Color> &color,     // Colors of the points.
  int scenePixelsWidth,           // The width of the scene in pixels.
  int scenePixelsHeight,          // The height of the scene in pixels.
  float pointPixels = 3.0f,       // Size (diameter) of the point, in pixels.
  float vertexDepth = -1.0f       // Render on the very front of the quad.
);

// Get an orthographic camera with the specified frustum
std::unique_ptr<Camera> orthographicCamera(
  float xLeft, float xRight, float yUp, float yDown, float zNear, float zFar);

// Get a rendering camera with the same field of view as the supplied camera model.
std::unique_ptr<Camera> perspectiveCamera(
  c8_PixelPinholeCameraModel intrinsics, int pixelsWidth, int pixelsHeight);

void updatePerspectiveCamera(
  Camera *camera, c8_PixelPinholeCameraModel intrinsics, int pixelsWidth, int pixelsHeight);

void updatePerspectiveCamera(
  Camera *cam,
  c8_PixelPinholeCameraModel intrinsics,
  int viewportWidth,
  int viewportHeight,
  float zNear,
  float zFar);

// Converts pixelQuad() from pixel space to clip space.  Usually used a level below the root scene
// with pixelQuad() having a SceneMaterial using the output of subscenes as their texture.
std::unique_ptr<Camera> pixelCamera(int pixelsWidth, int pixelsHeight);
std::unique_ptr<Camera> pixelCamera(Rect rect);

// Creates an ambient light.
std::unique_ptr<Light> ambientLight(Color color, float intensity);

// Creates a point light & optionally renders an object at the source.
std::unique_ptr<Light> pointLight(Color color, float intensity, bool render = false);

// Creates a directional light & optionally renders an object at the source.
std::unique_ptr<Light> directionalLight(Color color, float intensity, bool render = false);

// Creates a 3d arrow centered at the origin, pointing forward along the z-axis.
std::unique_ptr<Renderable> arrow(Color color);

// Creates a renderable from a MeshGeometry. We are using colorOnly because the MeshGeometry is not
// expected to have a texture but instead have colors per vertex.
std::unique_ptr<Renderable> meshGeometry(const MeshGeometry &mesh);

// Creates a renderable from a MeshGeometry. We are using colorOnly because the MeshGeometry is not
// expected to have a texture but instead have colors per vertex.
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
  int numUvs);

// Creates a unit size cube, needs to scale up for background cubemap rendering.
std::unique_ptr<Renderable> cubemapMesh();

// Get a mesh that renders with a splat shader via instance attributes.
std::unique_ptr<Renderable> splatAttributes();
std::unique_ptr<Renderable> splatAttributes(
  int numPoints,                // number of points in the splat.
  const HPoint3 *positions,     // Center of each splat.
  const Quaternion *rotations,  // Rotation of each splat.
  const HVector3 *scales,       // Scale of each splat.
  const Color *colors           // Colors of each splat.
);
std::unique_ptr<Renderable> splatAttributes(
  const Vector<HPoint3> &positions,     // Center of each splat.
  const Vector<Quaternion> &rotations,  // Rotation of each splat.
  const Vector<HVector3> &scales,       // Scale of each splat.
  const Vector<Color> &colors           // Colors of each splat.
);

void updateSplatAttributes(
  Renderable *splat,            // Renderable object to update.
  int numPoints,                // number of points in the splat.
  const HPoint3 *positions,     // Center of each splat.
  const Quaternion *rotations,  // Rotation of each splat.
  const HVector3 *scales,       // Scale of each splat.
  const Color *colors           // Colors of each splat.
);
void updateSplatAttributes(
  Renderable *splat,                       // Renderable object to update.
  const Vector<HPoint3> &newPositions,     // Center of each splat.
  const Vector<Quaternion> &newRotations,  // Rotation of each splat.
  const Vector<HVector3> &newScales,       // Scale of each splat.
  const Vector<Color> &newColors           // Colors of each splat.
);

std::unique_ptr<Renderable> splatSortedTexture(const SplatMetadata &header);

// Get a mesh that renders with a splat shader via texture and instance indices.
std::unique_ptr<Renderable> splatTexture(
  const SplatMetadata &header, RGBA32PlanePixelBuffer &&texture);
std::unique_ptr<Renderable> splatTexture(
  const SplatMetadata &header,       // Metadata of splat data.
  RGBA32PlanePixelBuffer &&texture,  // Texture of splat data.
  const Vector<uint32_t> &indices    // Indices of splat data.
);
std::unique_ptr<Renderable> splatTexture(
  const SplatMetadata &header,       // Metadata of splat data.
  RGBA32PlanePixelBuffer &&texture,  // Texture of splat data.
  const uint32_t *indices,           // Indices of splat data.
  int numIndices                     // Number of indices.
);

void updateSplatTexture(
  Renderable *splat,               // Renderable object to update.
  const Vector<uint32_t> &indices  // Indices of splat data.
);
void updateSplatTexture(
  Renderable *splat,        // Renderable object to update.
  const uint32_t *indices,  // Indices of splat data.
  int numIndices            // Indices of splat data.
);

std::unique_ptr<Renderable> splatTextureStacked(
  const SplatMetadata &header, RGBA32PlanePixelBuffer &&texture, int numStacks);

void updateSplatTextureStacked(
  Renderable *splat,        // Renderable object to update.
  const uint32_t *indices,  // Indices of splat data.
  int numIndices            // Indices of splat data.
);

std::unique_ptr<Renderable> splatMultiTex(const SplatMultiTextureView &splat);
void updateSplatMultiTex(Renderable *splat, const uint32_t* indices, int numIndices);
void updateSplatMultiTexCamera(Renderable *splat, const Camera &sceneCamera);

}  // namespace ObGen

}  // namespace c8
