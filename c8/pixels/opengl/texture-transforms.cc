// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:public",
  };
  hdrs = {"texture-transforms.h"};
  deps = {
    ":client-gl",
    ":gl-extensions",
    ":gl-buffer-object",
    ":gl-framebuffer",
    ":gl-headers",
    ":gl-pixel-buffer",
    ":gl-program",
    ":gl-texture",
    ":gl-quad",
    ":gl-vertex-array",
    "//c8:hmatrix",
    "//c8/geometry:parameterized-geometry",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixels",
  };
}
cc_end(0x5e4fe8cb);

#include "c8/c8-log.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-buffer-object.h"
#include "c8/pixels/opengl/gl-extensions.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-pixel-buffer.h"
#include "c8/pixels/opengl/gl-program.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/glext.h"
#include "c8/pixels/opengl/texture-transforms.h"

namespace c8 {

namespace {

// Move an object into a shared_ptr
template <typename T>
decltype(auto) sharedMove(T &&object) {
  return std::make_shared<T>(std::move(object));
}

#if C8_OPENGL_VERSION_3
static constexpr char const CURVY_UV_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
in vec2 texUv;
uniform sampler2D sampler;
uniform vec2 targetDims;
uniform vec2 roiDims;
uniform vec2 searchDims;
uniform mat4 intrinsics;
uniform mat4 globalPoseInverse;
uniform vec2 scales;
uniform float shift;
uniform float radius;
uniform float radiusBottom;
uniform int rotation;
uniform int curvyPostRotation;
out vec4 fragmentColor;
const float PI = 3.1415926535897932384626433832795;
void main() {
  vec2 pointInPixelSpace = vec2(texUv.x * roiDims.x, texUv.y * roiDims.y);
  if (pointInPixelSpace.x > targetDims.x || pointInPixelSpace.y > targetDims.y) {
    fragmentColor = vec4(0.0, 0.925, 0.683, 1.0);
    return;
  }

  float v_x = 2.0 * PI * (shift + (scales.x * pointInPixelSpace.x));
  float radiusAtY = texUv.y * radiusBottom + radius * (1.0 - texUv.y);
  vec4 searchNormal = vec4(-radiusAtY * sin(v_x),
                          radiusAtY * (radiusBottom - radius),
                          radiusAtY * cos(v_x),
                          0.0);
  vec4 transformedNormal = globalPoseInverse * searchNormal;

  vec4 searchPt = vec4(-radiusAtY * sin(v_x),
                       -scales.y * pointInPixelSpace.y + 0.5,
                       radiusAtY * cos(v_x),
                       1.0);
  vec4 rayPt3 = globalPoseInverse * searchPt;
  float oneOverZ = 1.0 / rayPt3.z;
  vec2 rayPt = rayPt3.xy * oneOverZ;
  vec3 pixPt3 = (intrinsics * rayPt3).xyz;
  vec2 pixPt = pixPt3.xy * oneOverZ;
  if (dot(transformedNormal.xyz, vec3(rayPt.x, rayPt.y, 1.0)) >= 0.0) {
    fragmentColor = vec4(0.863, 0.0, 0.394, 1.0);
    return;
  }

  vec2 pixPtClipSpace;
  if (curvyPostRotation == -90 || curvyPostRotation == 270) {
    pixPtClipSpace = vec2(1.0 - (pixPt.y / searchDims.y), pixPt.x / searchDims.x);
  } else if (curvyPostRotation == 180 || curvyPostRotation == -180) {
    pixPtClipSpace = vec2(1.0 - (pixPt.x / searchDims.x), 1.0 - (pixPt.y / searchDims.y));
  } else if (curvyPostRotation == -270 || curvyPostRotation == 90) {
    pixPtClipSpace = vec2(pixPt.y / searchDims.y, 1.0 - (pixPt.x / searchDims.x));
  } else {
    pixPtClipSpace = vec2(pixPt.x / searchDims.x, pixPt.y / searchDims.y);
  }

  // Prevent clamping
  if (pixPtClipSpace.x > 1.0 || pixPtClipSpace.x < 0.0) {
    fragmentColor = vec4(0.460, 0.066, 0.710, 1.0);
    return;
  }
  if (pixPtClipSpace.y > 1.0 || pixPtClipSpace.y < 0.0) {
    fragmentColor = vec4(0.0, 0.925, 0.683, 1.0);
    return;
  }

  fragmentColor = texture(sampler, pixPtClipSpace);
}
)";
#else  // OpenGL 2
static constexpr char CURVY_UV_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
varying vec2 texUv;
uniform sampler2D sampler;
uniform vec2 targetDims;
uniform vec2 roiDims;
uniform vec2 searchDims;
uniform mat4 intrinsics;
uniform mat4 globalPoseInverse;
uniform vec2 scales;
uniform float shift;
uniform float radius;
uniform float radiusBottom;
uniform int rotation;
uniform int curvyPostRotation;
const float PI = 3.1415926535897932384626433832795;
void main() {
  vec2 pointInPixelSpace = vec2(texUv.x * roiDims.x, texUv.y * roiDims.y);
  if (pointInPixelSpace.x > targetDims.x || pointInPixelSpace.y > targetDims.y) {
    gl_FragColor = vec4(0.0, 0.925, 0.683, 1.0);
    return;
  }

  float v_x = 2.0 * PI * (shift + (scales.x * pointInPixelSpace.x));
  float radiusAtY = texUv.y * radiusBottom + radius * (1.0 - texUv.y);
  vec4 searchNormal = vec4(-radiusAtY * sin(v_x),
                          radiusAtY * (radiusBottom - radius),
                          radiusAtY * cos(v_x),
                          0.0);
  vec4 transformedNormal = globalPoseInverse * searchNormal;

  vec4 searchPt = vec4(-radiusAtY * sin(v_x),
                       -scales.y * pointInPixelSpace.y + 0.5,
                       radiusAtY * cos(v_x),
                       1.0);
  vec4 rayPt3 = globalPoseInverse * searchPt;
  float oneOverZ = 1.0 / rayPt3.z;
  vec2 rayPt = rayPt3.xy * oneOverZ;
  vec3 pixPt3 = (intrinsics * rayPt3).xyz;
  vec2 pixPt = pixPt3.xy * oneOverZ;
  if (dot(transformedNormal.xyz, vec3(rayPt.x, rayPt.y, 1.0)) >= 0.0) {
    gl_FragColor = vec4(0.863, 0.0, 0.394, 1.0);
    return;
  }

  vec2 pixPtClipSpace;
  if (curvyPostRotation == -90 || curvyPostRotation == 270) {
    pixPtClipSpace = vec2(1.0 - (pixPt.y / searchDims.y), pixPt.x / searchDims.x);
  } else if (curvyPostRotation == 180 || curvyPostRotation == -180) {
    pixPtClipSpace = vec2(1.0 - (pixPt.x / searchDims.x), 1.0 - (pixPt.y / searchDims.y));
  } else if (curvyPostRotation == -270 || curvyPostRotation == 90) {
    pixPtClipSpace = vec2(pixPt.y / searchDims.y, 1.0 - (pixPt.x / searchDims.x));
  } else {
    pixPtClipSpace = vec2(pixPt.x / searchDims.x, pixPt.y / searchDims.y);
  }

  // Prevent clamping
  if (pixPtClipSpace.x > 1.0 || pixPtClipSpace.x < 0.0) {
    gl_FragColor = vec4(0.460, 0.066, 0.710, 1.0);
    return;
  }
  if (pixPtClipSpace.y > 1.0 || pixPtClipSpace.y < 0.0) {
    gl_FragColor = vec4(0.0, 0.925, 0.683, 1.0);
    return;
  }

  gl_FragColor = texture2D(sampler, pixPtClipSpace);
}
)";
#endif

#if C8_OPENGL_VERSION_3
static constexpr char const MVP_UV_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
in vec3 position;
in vec2 uv;
uniform mat4 mvp;
uniform int rotation;
out vec2 texUv;
void main() {
  gl_Position = mvp * vec4(position, 1.0);
  if (rotation == -90 || rotation == 270) {
    texUv = vec2(1.0 - uv.y, uv.x);
  } else if (rotation == 180 || rotation == -180) {
    texUv = vec2(1.0 - uv.x, 1.0 - uv.y);
  } else if (rotation == -270 || rotation == 90) {
    texUv = vec2(uv.y, 1.0 - uv.x);
  } else {
    texUv = uv;
  }
}
)";
static constexpr char const NOP_UV_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
in vec3 position;
in vec2 uv;
out vec2 texUv;
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = uv;
}
)";
static constexpr char NOP_UV_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
in vec2 texUv;
uniform sampler2D sampler;
out vec4 fragmentColor;
void main() {
  fragmentColor = texture(sampler, texUv);
}
)";
#else  // OpenGL 2
static constexpr char MVP_UV_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
attribute vec3 position;
attribute vec2 uv;
varying vec2 texUv;
uniform mat4 mvp;
void main() {
  gl_Position = mvp * vec4(position, 1.0);
  texUv = uv;
}
)";
static constexpr char NOP_UV_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
attribute vec3 position;
attribute vec2 uv;
varying vec2 texUv;
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = uv;
}
)";
static constexpr char NOP_UV_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
varying vec2 texUv;
uniform sampler2D sampler;
void main() {
  gl_FragColor = texture2D(sampler, texUv);
}
)";
#endif

#if C8_OPENGL_VERSION_3
static constexpr char const ROTATE_90_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
in vec3 position;
in vec2 uv;
out vec2 texUv;
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = vec2(uv.y, 1.0 - uv.x);
}
)";
#else  // OpenGL 2
static constexpr char ROTATE_90_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
attribute vec3 position;
attribute vec2 uv;
varying vec2 texUv;
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = vec2(uv.y, 1.0 - uv.x);
}
)";
#endif

#if C8_OPENGL_VERSION_3
static constexpr char const ROTATE_270_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
in vec3 position;
in vec2 uv;
out vec2 texUv;
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = vec2(1.0 - uv.y, uv.x);
}
)";
#else  // OpenGL 2
static constexpr char ROTATE_270_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
attribute vec3 position;
attribute vec2 uv;
varying vec2 texUv;
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = vec2(1.0 - uv.y, uv.x);
}
)";
#endif

#if C8_OPENGL_VERSION_3
static constexpr char const EXTERNAL_UV_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
in vec3 position;
in vec2 uv;
uniform mat4 texMatrix;
out vec2 texUv;
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = (texMatrix * vec4(uv, 0, 1)).xy;
}
)";
static constexpr char EXTERNAL_UV_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
#extension GL_OES_EGL_image_external : require
#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;
in vec2 texUv;
uniform samplerExternalOES sampler;
out vec4 fragmentColor;
void main() {
  fragmentColor = texture(sampler, texUv);
}
)";
static constexpr char EXTERNAL_UV_FRAGMENT_CODE_NO_ESSL3[] =  //
  C8_GLSL_VERSION_LINE
  R"(
#extension GL_OES_EGL_image_external : require
precision mediump float;
in vec2 texUv;
uniform samplerExternalOES sampler;
out vec4 fragmentColor;
void main() {
  fragmentColor = texture(sampler, texUv);
}
)";
#else  // OpenGL 2
static constexpr char EXTERNAL_UV_VERTEX_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
attribute vec3 position;
attribute vec2 uv;
uniform mat4 texMatrix;
varying vec2 texUv;
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = (texMatrix * vec4(uv, 0, 1)).xy;
}
)";
static constexpr char EXTERNAL_UV_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
#extension GL_OES_EGL_image_external : require
precision mediump float;
varying vec2 texUv;
uniform samplerExternalOES sampler;
void main() {
  gl_FragColor = textureExternalOES(sampler, texUv);
}
)";
static constexpr char EXTERNAL_UV_FRAGMENT_CODE_NO_ESSL3[] =  //
  C8_GLSL_VERSION_LINE
  R"(
#extension GL_OES_EGL_image_external : require
precision mediump float;
varying vec2 texUv;
uniform samplerExternalOES sampler;
void main() {
  gl_FragColor = textureExternalOES(sampler, texUv);
}
)";
#endif

#if C8_OPENGL_VERSION_3
static constexpr char RGB_TO_YUV_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
in vec2 texUv;
uniform sampler2D sampler;
out vec4 fragmentColor;
void main() {
  vec4 color = texture(sampler, texUv);
  float y = .299 * color.r + .587 * color.g + .114 * color.b;
  fragmentColor.r = y;
  fragmentColor.g = -0.169 * color.r - .331 * color.g + .5 * color.b + 0.5;
  fragmentColor.b = 0.5 * color.r - .419 * color.g - .081 * color.b + 0.5;
  fragmentColor.a = color.a;
}
)";

static constexpr char RGB_TO_YUV_FLIP_Y_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
in vec2 texUv;
uniform sampler2D sampler;
out vec4 fragmentColor;
void main() {
  vec4 color = texture(sampler, vec2(texUv.x, 1.0 - texUv.y));
  float y = .299 * color.r + .587 * color.g + .114 * color.b;
  fragmentColor.r = y;
  fragmentColor.g = -0.169 * color.r - .331 * color.g + .5 * color.b + 0.5;
  fragmentColor.b = 0.5 * color.r - .419 * color.g - .081 * color.b + 0.5;
  fragmentColor.a = color.a;
}
)";
#else  // OpenGL 2
static constexpr char RGB_TO_YUV_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
varying vec2 texUv;
uniform sampler2D sampler;
void main() {
  vec4 color = texture2D(sampler, texUv);
  float y = .299 * color.r + .587 * color.g + .114 * color.b;
  gl_FragColor.r = y;
  gl_FragColor.g = -0.169 * color.r - .331 * color.g + .5 * color.b + 0.5;
  gl_FragColor.b = 0.5 * color.r - .419 * color.g - .081 * color.b + 0.5;
  gl_FragColor.a = color.a;
}
)";
static constexpr char RGB_TO_YUV_FLIP_Y_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
varying vec2 texUv;
uniform sampler2D sampler;
void main() {
  vec4 color = texture2D(sampler, vec2(texUv.x, 1.0 - texUv.y));
  float y = .299 * color.r + .587 * color.g + .114 * color.b;
  gl_FragColor.r = y;
  gl_FragColor.g = -0.169 * color.r - .331 * color.g + .5 * color.b + 0.5;
  gl_FragColor.b = 0.5 * color.r - .419 * color.g - .081 * color.b + 0.5;
  gl_FragColor.a = color.a;
}
)";
#endif

#if C8_OPENGL_VERSION_3
static constexpr char CHANNEL_HEATMAP_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
in vec2 texUv;
uniform sampler2D sampler;
out vec4 fragmentColor;
uniform int channel;
void main() {
  float v = texture(sampler, texUv)[channel];
  if (v <= 0.299) {
    fragmentColor = vec4(v / 0.299, 0.0, 0.0, 1.0);
  } else if (v <= (0.299 + 0.587)) {
    fragmentColor = vec4(1.0, (v - 0.299) / .587, 0.0, 1.0);
  } else {
    fragmentColor = vec4(1.0, 1.0, (v - .299 - .587) / (1.0 - .299 - .587), 1.0);
  }
}
)";
#else  // OpenGL 2
static constexpr char CHANNEL_HEATMAP_FRAGMENT_CODE[] =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
varying vec2 texUv;
uniform sampler2D sampler;
uniform int channel;
void main() {
  float v = 0.0;
  if (channel == 0) {
    v = texture2D(sampler, texUv)[0];
  } else if (channel == 1) {
    v = texture2D(sampler, texUv)[1];
  } else if (channel == 2) {
    v = texture2D(sampler, texUv)[2];
  } else if (channel == 3) {
    v = texture2D(sampler, texUv)[3];
  }
  if (v <= 0.299) {
    gl_FragColor = vec4(v / 0.299, 0.0, 0.0, 1.0);
  } else if (v <= (0.299 + 0.587)) {
    gl_FragColor = vec4(1.0, (v - 0.299) / .587, 0.0, 1.0);
  } else {
    gl_FragColor = vec4(1.0, 1.0, (v - .299 - .587) / (1.0 - .299 - .587), 1.0);
  }
}
)";
#endif

}  // namespace

void readFramebufferRGBA8888Pixels(const GlFramebufferObject &fb, RGBA8888PlanePixels dest) {
  auto t = fb.tex().tex();
  fb.bind();
  glReadPixels(0, 0, t.width(), t.height(), t.format(), t.type(), dest.pixels());
  fb.unbind();
}

RGBA8888PlanePixelBuffer readFramebufferRGBA8888PixelBuffer(const GlFramebufferObject &fb) {
  auto t = fb.tex().tex();
  RGBA8888PlanePixelBuffer pixelBuffer(t.height(), t.width());
  readFramebufferRGBA8888Pixels(fb, pixelBuffer.pixels());
  return pixelBuffer;
}

#if C8_OPENGL_VERSION_3
void readFramebufferRGBAToPixelBuffer(
  const GlFramebufferObject &fb, GlRGBA8888PlanePixelBuffer *dest) {
  auto t = fb.tex().tex();
  fb.bind();
  dest->bind();
  glReadPixels(0, 0, t.width(), t.height(), t.format(), t.type(), 0);
  dest->unbind();
  fb.unbind();
}

void readFramebufferYUVAToPixelBuffer(
  const GlFramebufferObject &fb, GlYUVA8888PlanePixelBuffer *dest) {
  auto t = fb.tex().tex();
  fb.bind();
  dest->bind();
  glReadPixels(0, 0, t.width(), t.height(), t.format(), t.type(), 0);
  dest->unbind();
  fb.unbind();
}
#endif

std::function<void(GlTexture src, GlFramebufferObject *dest)> compileCopyTexture2D() {
  GlProgramObject program;
  program.initialize(
    NOP_UV_VERTEX_CODE,
    NOP_UV_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {});

  GlVertexArray rect = makeVertexArrayRect();

  // Apple OpenGL and Android API<21 are missing the typedef for glCopyImageSubDataEXT. Thus,
  // instead of relying on the verion from gl2ext.h, we redefine it here.
  typedef void (*C8_PFNGLCOPYIMAGESUBDATAEXTPROC)(
    GLuint srcName,
    GLenum srcTarget,
    GLint srcLevel,
    GLint srcX,
    GLint srcY,
    GLint srcZ,
    GLuint dstName,
    GLenum dstTarget,
    GLint dstLevel,
    GLint dstX,
    GLint dstY,
    GLint dstZ,
    GLsizei srcWidth,
    GLsizei srcHeight,
    GLsizei srcDepth);

  // Look for glCopyImageSubDataEXT.
  bool hasCopyImageEXT = hasGlExtension("GL_EXT_copy_image");
  auto glCopyImageSubData = hasCopyImageEXT
    ? (C8_PFNGLCOPYIMAGESUBDATAEXTPROC)clientGlGetProcAddress("glCopyImageSubDataEXT")
    : nullptr;

  return [glCopyImageSubData,
          program = sharedMove(std::move(program)),
          rect = sharedMove(std::move(rect))](GlTexture src, GlFramebufferObject *dest) {
    // Bind the destination framebuffer.
    dest->bind();
    if (
      glCopyImageSubData && src.width() == dest->tex().width()
      && src.height() == dest->tex().height()) {
      // Do a direct texture copy
      glCopyImageSubData(
        src.id(),
        src.target(),
        0,
        0,
        0,
        0,
        dest->tex().id(),
        dest->tex().target(),
        0,
        0,
        0,
        0,
        src.width(),
        src.height(),
        1);
    } else {
      // Copy using the render pipeline.
      glUseProgram(program->id());
      rect->bind();
      src.bind();
      glFrontFace(GL_CCW);
      glViewport(0, 0, dest->tex().width(), dest->tex().height());
      rect->drawElements();
      src.unbind();
      rect->unbind();
      dest->unbind();
    }
  };
}

std::function<void(GlTexture src, GlFramebufferObject *dest)> compileRotate90Texture2D() {
  GlProgramObject program;
  program.initialize(
    ROTATE_90_VERTEX_CODE,
    NOP_UV_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {});

  GlVertexArray rect = makeVertexArrayRect();

  return [program = sharedMove(std::move(program)), rect = sharedMove(std::move(rect))](
           GlTexture src, GlFramebufferObject *dest) {
    // Bind the destination framebuffer.
    dest->bind();
    // Copy using the render pipeline.
    glUseProgram(program->id());
    rect->bind();
    src.bind();
    glFrontFace(GL_CCW);
    glViewport(0, 0, dest->tex().width(), dest->tex().height());
    rect->drawElements();
    src.unbind();
    rect->unbind();
    dest->unbind();
  };
}

std::function<void(GlTexture src, GlFramebufferObject *dest)> compileRotate270Texture2D() {
  GlProgramObject program;
  program.initialize(
    ROTATE_270_VERTEX_CODE,
    NOP_UV_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {});

  GlVertexArray rect = makeVertexArrayRect();

  return [program = sharedMove(std::move(program)), rect = sharedMove(std::move(rect))](
           GlTexture src, GlFramebufferObject *dest) {
    // Bind the destination framebuffer.
    dest->bind();
    // Copy using the render pipeline.
    glUseProgram(program->id());
    rect->bind();
    src.bind();
    glFrontFace(GL_CCW);
    glViewport(0, 0, dest->tex().width(), dest->tex().height());
    rect->drawElements();
    src.unbind();
    rect->unbind();
    dest->unbind();
  };
}

// This function is only used in cylindrical-detect. roiAspectRatio is specified globally in
// image-roi.h, we keep roiAspectRatio parameter here so future experimentation with other aspect
// ratio for cylindrical-detect can be done.
// @param searchDims (width, height) of the search image in pixels.
std::function<void(
  CurvyImageGeometry geom,
  const HMatrix &intrinsics,
  const HMatrix &globalPose,
  int rotation,
  float roiAspectRatio,
  HPoint2 searchDims,
  GlTexture src,
  GlFramebufferObject *dest)>
compileWarpCurvyTexture2D() {
  GlProgramObject program;
  program.initialize(
    MVP_UV_VERTEX_CODE,
    CURVY_UV_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"mvp"},
     {"targetDims"},
     {"roiDims"},
     {"searchDims"},
     {"intrinsics"},
     {"globalPoseInverse"},
     {"scales"},
     {"shift"},
     {"radius"},
     {"radiusBottom"},
     {"rotation"},
     {"curvyPostRotation"}});
  GlVertexArray rect = makeVertexArrayRect();

  return [program = sharedMove(std::move(program)), rect = sharedMove(std::move(rect))](
           CurvyImageGeometry geom,
           HMatrix intrinsics,
           HMatrix globalPose,
           int rotation,
           float roiAspectRatio,
           HPoint2 searchDims,
           GlTexture src,
           GlFramebufferObject *dest) {
    // Adjust dimensions for landscape image targets (including isRotated=true).
    bool isRotated = false;
    if (geom.srcCols > geom.srcRows) {
      isRotated = true;
      std::swap(geom.srcRows, geom.srcCols);
    }

    // For non 3x4 image targets, the lifted image target won't take up the full viewport of the
    // quad. Compute the full size of the quad in pixels so that the lifted image target can mantain
    // its aspect ratio and only draw to part.
    float roiCols = (static_cast<float>(geom.srcCols) / geom.srcRows > roiAspectRatio)
      ? geom.srcCols
      : roiAspectRatio * geom.srcRows;
    float roiRows = (static_cast<float>(geom.srcCols) / geom.srcRows > roiAspectRatio)
      ? geom.srcCols / roiAspectRatio
      : geom.srcRows;

    float sx = (geom.activationRegion.right - geom.activationRegion.left) / (geom.srcCols - 1);
    float sy = 1.0f / (geom.srcRows - 1);
    float shift = geom.activationRegion.left;

    // Bind the destination framebuffer.
    dest->bind();
    glUseProgram(program->id());
    glUniformMatrix4fv(program->location("mvp"), 1, GL_FALSE, HMatrixGen::i().data().data());
    // Other layouts use a vertex shader to pre-rotate the image, but we don't pre-rotate except in
    // the case of isRotated=true images, when we need to rotate 90 degrees clockwise.
    glUniform1i(program->location("rotation"), isRotated ? 90 : 0);
    // But we still need to do a post-rotation after our geometry mapping.
    glUniform1i(program->location("curvyPostRotation"), rotation);
    glUniform2f(
      program->location("targetDims"),
      static_cast<float>(geom.srcCols),
      static_cast<float>(geom.srcRows));
    glUniform2f(program->location("roiDims"), roiCols, roiRows);
    glUniform2f(program->location("searchDims"), searchDims.x(), searchDims.y());
    glUniformMatrix4fv(program->location("intrinsics"), 1, GL_FALSE, intrinsics.data().data());
    glUniformMatrix4fv(
      program->location("globalPoseInverse"), 1, GL_FALSE, globalPose.inv().data().data());
    glUniform2f(program->location("scales"), sx, sy);
    glUniform1f(program->location("shift"), shift);
    glUniform1f(program->location("radius"), geom.radius);
    glUniform1f(program->location("radiusBottom"), geom.radiusBottom);
    rect->bind();
    src.bind();
    glFrontFace(GL_CCW);
    glViewport(0, 0, dest->tex().width(), dest->tex().height());
    rect->drawElements();
    src.unbind();
    rect->unbind();
    dest->unbind();
  };
}

std::function<void(const HMatrix &mat, GlTexture src, GlFramebufferObject *dest)>
compileWarpTexture2D() {
  GlProgramObject program;
  program.initialize(
    MVP_UV_VERTEX_CODE,
    NOP_UV_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"mvp"}});

  GlVertexArray rect = makeVertexArrayRect();

  return [program = sharedMove(std::move(program)), rect = sharedMove(std::move(rect))](
           const HMatrix &mat, GlTexture src, GlFramebufferObject *dest) {
    // Bind the destination framebuffer.
    dest->bind();
    glUseProgram(program->id());
    glUniformMatrix4fv(program->location("mvp"), 1, GL_FALSE, mat.data().data());
    rect->bind();
    src.bind();
    glFrontFace(GL_CCW);
    glViewport(0, 0, dest->tex().width(), dest->tex().height());
    // TODO(nb): make clear-color settable.
    glClearColor(45 / 255.0f, 46 / 255.0f, 67 / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    rect->drawElements();
    src.unbind();
    rect->unbind();
    dest->unbind();
  };
}
std::function<void(const HMatrix &mat, int rotation, GlTexture src, GlFramebufferObject *dest)>
compileWarpRotateTexture2D() {
  GlProgramObject program;
  program.initialize(
    MVP_UV_VERTEX_CODE,
    NOP_UV_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"mvp"}, {"rotation"}});

  GlVertexArray rect = makeVertexArrayRect();

  return [program = sharedMove(std::move(program)), rect = sharedMove(std::move(rect))](
           const HMatrix &mat, int rotation, GlTexture src, GlFramebufferObject *dest) {
    // Bind the destination framebuffer.
    dest->bind();
    glUseProgram(program->id());
    glUniformMatrix4fv(program->location("mvp"), 1, GL_FALSE, mat.data().data());
    glUniform1i(program->location("rotation"), rotation);
    rect->bind();
    src.bind();
    glFrontFace(GL_CCW);
    glViewport(0, 0, dest->tex().width(), dest->tex().height());
    // TODO(nb): make clear-color settable.
    glClearColor(45 / 255.0f, 46 / 255.0f, 67 / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    rect->drawElements();
    src.unbind();
    rect->unbind();
    dest->unbind();
  };
}

std::function<void(int channel, GlTexture src, GlFramebufferObject *dest)> compileChannelHeatmap() {
  GlProgramObject program;
  program.initialize(
    NOP_UV_VERTEX_CODE,
    CHANNEL_HEATMAP_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"channel"}});

  GlVertexArray rect = makeVertexArrayRect();

  return [program = sharedMove(std::move(program)), rect = sharedMove(std::move(rect))](
           int channel, GlTexture src, GlFramebufferObject *dest) {
    // Bind the destination framebuffer.
    dest->bind();
    glUseProgram(program->id());
    glUniform1i(program->location("channel"), channel % 4);
    rect->bind();
    src.bind();
    glFrontFace(GL_CCW);
    glViewport(0, 0, dest->tex().width(), dest->tex().height());
    rect->drawElements();
    src.unbind();
    rect->unbind();
    dest->unbind();
  };
}

std::function<void(const float mtx[16], GlTexture src, GlFramebufferObject *dest)>
compileCopyExternalOesTexture2D() {
  GlProgramObject program;
  bool isInitialized = program.initialize(
    EXTERNAL_UV_VERTEX_CODE,
    EXTERNAL_UV_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"texMatrix"}});

  if (!isInitialized) {
    program.initialize(
      EXTERNAL_UV_VERTEX_CODE,
      EXTERNAL_UV_FRAGMENT_CODE_NO_ESSL3,
      {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
      {{"texMatrix"}});
  }
  GLint texMatrixUniform = program.location("texMatrix");

  GlVertexArray rect = makeVertexArrayRect();

  return [program = sharedMove(std::move(program)),
          rect = sharedMove(std::move(rect)),
          texMatrixUniform](const float mtx[16], GlTexture src, GlFramebufferObject *dest) {
    // Bind the destination framebuffer.
    dest->bind();

    // Copy using the render pipeline.
    glUseProgram(program->id());

    // Upload the matrix.
    glUniformMatrix4fv(texMatrixUniform, 1, GL_FALSE, mtx);

    rect->bind();
    src.bind();
    glFrontFace(GL_CCW);
    glViewport(0, 0, dest->tex().width(), dest->tex().height());
    rect->drawElements();
    src.unbind();
    rect->unbind();
    dest->unbind();
  };
}

namespace {
std::function<void(GlTexture src, GlFramebufferObject *dest)> compileConvertTextureRGBToYUV(
  bool flipY) {
  GlProgramObject program;
  program.initialize(
    NOP_UV_VERTEX_CODE,
    flipY ? RGB_TO_YUV_FLIP_Y_FRAGMENT_CODE : RGB_TO_YUV_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {});

  GlVertexArray rect = makeVertexArrayRect();

  return [program = sharedMove(std::move(program)), rect = sharedMove(std::move(rect))](
           GlTexture src, GlFramebufferObject *dest) {
    // Bind the destination framebuffer.
    dest->bind();
    // Copy using the render pipeline.
    glUseProgram(program->id());
    rect->bind();
    src.bind();
    glFrontFace(GL_CCW);
    glViewport(0, 0, dest->tex().width(), dest->tex().height());
    rect->drawElements();
    src.unbind();
    rect->unbind();
    dest->unbind();
  };
}
}  // namespace

std::function<void(GlTexture src, GlFramebufferObject *dest)> compileConvertTextureRGBToYUV() {
  return compileConvertTextureRGBToYUV(false);
}

std::function<void(GlTexture src, GlFramebufferObject *dest)> compileConvertTextureRGBToYUVFlipY() {
  return compileConvertTextureRGBToYUV(true);
}

}  // namespace c8
