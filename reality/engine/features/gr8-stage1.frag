#version 320 es

precision mediump float;

in vec2 texUv;
uniform sampler2D sampler;
uniform int clear;
uniform vec2 scale;
uniform vec2 pixScale;
// For curvy geometry ROI lifting
uniform int isCurvy;
uniform vec2 targetDims;
uniform vec2 roiDims;
uniform vec2 searchDims;
uniform mat4 intrinsics;
uniform mat4 globalPoseInverse;
uniform vec2 scales;
uniform float shift;
uniform float radius;
uniform float radiusBottom;
uniform mediump int rotation;
uniform int curvyPostRotation;
const float PI = 3.1415926535897932384626433832795;
vec3 texValueRGB(vec2 offsetPixels) {
  if (clear == 0) {
    return texture(sampler, scale * (offsetPixels + pixScale * texUv)).rgb;
  } else {
    return vec3(0.0, 0.0, 0.0);
  }
}

vec2 getCurvyLiftedUv() {
  vec2 pointInPixelSpace = vec2(texUv.x * roiDims.x, texUv.y * roiDims.y);

  if (pointInPixelSpace.x > targetDims.x || pointInPixelSpace.y > targetDims.y) {
    return vec2(-1.0, -1.0);
  }

  float radiusAtY = texUv.y * radiusBottom + radius * (1.0 - texUv.y);
  float v_x = 2.0 * PI * (shift + (scales.x * pointInPixelSpace.x));
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
    return vec2(-1.0, -1.0);
  }

  // TODO(paris): Investigate whether we could pre-process inputs to save this step.
  if (curvyPostRotation == -90 || curvyPostRotation == 270) {
    return vec2(1.0 - (pixPt.y / searchDims.y), pixPt.x / searchDims.x);
  } else if (curvyPostRotation == 180 || curvyPostRotation == -180) {
    return vec2(1.0 - (pixPt.x / searchDims.x), 1.0 - (pixPt.y / searchDims.y));
  } else if (curvyPostRotation == -270 || curvyPostRotation == 90) {
    return vec2(pixPt.y / searchDims.y, 1.0 - (pixPt.x / searchDims.x));
  } else {
    return vec2(pixPt.x / searchDims.x, pixPt.y / searchDims.y);
  }
}

float texGray(vec2 p) {
  const vec3 cvt = vec3(.299, .587, .114);
  return dot(cvt, texValueRGB(p));
}

out vec4 fragColor;
void main() {
  vec2 offsetPixels = vec2(0.0, 0.0);
  if (isCurvy == 1 && clear == 0) {
    vec2 liftedUv = getCurvyLiftedUv();
    offsetPixels = (pixScale * liftedUv) - (pixScale * texUv);
    if ((liftedUv.x == -1.0 && liftedUv.y == -1.0) ||
        (liftedUv.x < 0.0 || liftedUv.x > 1.0 || liftedUv.y < 0.0 || liftedUv.y > 1.0)) {
      fragColor = vec4(0.0, 0.0, 0.0, 0.0);
      return;
    }
  }
  fragColor = vec4(
    texGray(offsetPixels),
    0.0,
    0.0,
    1.0
  );
}
