// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <memory>

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/render/object8.h"
#include "c8/pixels/render/uniform.h"

namespace c8 {

#ifdef __EMSCRIPTEN__
bool isWebGL2();
#endif

// Empty class that is specialized in the internal implementation. This prevents gl headers from
// leaking outside of the renderer.
class RendererState {
public:
  virtual ~RendererState() = default;
};

using SceneRenderSpecPair = std::pair<Scene *, const RenderSpec *>;
using SceneRenderSpecNamePair = std::pair<Scene *, String>;

// Exposing these functions so they can be tested
bool getSceneRenderingOrder(Scene &rootScene, Vector<SceneRenderSpecPair> *renderingOrder);
void getSubscenes(Object8 &root, Vector<Scene *> *subscenes);

// Graphics-framework agnostic representation of the render output. This prevents gl headers from
// leaking outside of the renderer.
struct RenderResult {
public:
  int texId = -1;
  int bufferId = -1;
  int width = 0;
  int height = 0;
};

class Renderer {
public:
  Renderer();
  // Render the scene (and all possible sub-scenes based on render specs)
  RenderResult render(Scene &scene);
  // Convenience method to get the result as an image. Allocate new mem.
  RGBA8888PlanePixelBuffer result() const;

  // Allocate the (scratch, dest) buffer pair for use with result(scratch, buf)
  std::pair<RGBA8888PlanePixelBuffer, RGBA8888PlanePixelBuffer> allocateForResult() const;

  // Get the result as an image into an existing buffer. No allocation is done.
  // use allocateForResult method to get buffers ready for repeated calls of this method
  // @param scratchFlipBuf scratch space containing the vertical-flipped output
  void result(RGBA8888PlanePixels scratchFlipBuf, RGBA8888PlanePixels dest) const;

  // Get the result as an image into an existing buffer without flipping. This could result in a
  // flipped result. No allocation is done, and the caller is responsible for ensuring the output
  // buffer is the correct size.
  void result(RGBA8888PlanePixels dest) const;

  // Specifies a shader that can be created on the GPU, without actualy creating it. If a material
  // references this shader, it will be created on the GPU at that time.
  void registerShader(
    const String &name,
    const String &vertexShaderCode,
    const String &fragmentShaderCode,
    const Vector<String> &vertexAttribs,
    const Vector<Uniform> &uniforms);

private:
  std::unique_ptr<RendererState> state_;
};

}  // namespace c8
