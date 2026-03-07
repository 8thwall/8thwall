const motionPassVert = `\
/* From threejs builtins

https://threejs.org/docs/?q=mat#api/en/renderers/webgl/WebGLProgram

// = object.matrixWorld
uniform mat4 modelMatrix;

// = camera.matrixWorldInverse * object.matrixWorld
uniform mat4 modelViewMatrix;

// = camera.projectionMatrix
uniform mat4 projectionMatrix;

// = camera.matrixWorldInverse
uniform mat4 viewMatrix;

// = inverse transpose of modelViewMatrix
uniform mat3 normalMatrix;

// default vertex attributes provided by BufferGeometry
attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;
*/

/* From our own bookkeeping */

// NOTE(lreyna): This attribute is something that needs to be set by an ecs application
// https://docs.google.com/document/d/<REMOVED_BEFORE_OPEN_SOURCING>
#ifdef USE_INSTANCING
attribute mat4 prevInstanceMatrix;
#endif

uniform bool skipMotionRenderPass;
uniform mat4 prevWorldMatrix;
uniform mat4 prevCameraProjectionMatrix;
uniform mat4 prevCameraMatrixWorldInverse;

#include <skinning_pars_vertex>

/* Passing to fragment */

out highp vec4 clipPos;
out highp vec4 prevClipPos;

void main() {
  vec4 myPosition = vec4(position, 1.0);

  // NOTE(lreyna): More efficient with defines, but setting them in onBeforeRender needs more work
  if (skipMotionRenderPass) {
    return;
  }

#ifdef USE_SKINNING
  // These values are used by the skin includes
  vec3 transformed = vec3(position);
  vec3 objectNormal = vec3(normal);

  #include <skinbase_vertex>
  #include <skinnormal_vertex>
  #include <skinning_vertex>

  myPosition = vec4(transformed, 1.0);
#endif

#ifdef USE_INSTANCING
  prevClipPos = prevCameraProjectionMatrix * prevCameraMatrixWorldInverse *
    prevWorldMatrix * prevInstanceMatrix * myPosition;
#else
  prevClipPos = prevCameraProjectionMatrix * prevCameraMatrixWorldInverse *
    prevWorldMatrix * myPosition;
#endif

// mat4 projectionViewMatrix = projectionMatrix * myPosition; // Local position to camera
#ifdef USE_INSTANCING
  clipPos = projectionMatrix * modelViewMatrix * instanceMatrix * myPosition;
#else
  clipPos = projectionMatrix * modelViewMatrix * myPosition; // Local position to clip space
#endif
  gl_Position = clipPos;
}
`

const motionPassFrag = `
in highp vec4 clipPos;
in highp vec4 prevClipPos;

uniform bool skipMotionRenderPass;
uniform vec4 motionVectorScale;

void main() {

  if (skipMotionRenderPass) {
    discard;
  }

  highp vec4 motionVector = ( clipPos / clipPos.w - prevClipPos / prevClipPos.w );

  // gl_FragColor = 0.5 + 20. * motionVector; // Debug with Color
  gl_FragColor = motionVectorScale * motionVector;
}

`

export {
  motionPassFrag,
  motionPassVert,
}
