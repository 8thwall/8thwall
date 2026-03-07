const textVertex = `
varying vec2 vUv;

#include <clipping_planes_pars_vertex>

void main() {

  vUv = uv;
  vec4 mvPosition = modelViewMatrix * vec4( position, 1.0 );
  gl_Position = projectionMatrix * mvPosition;
  gl_Position.z -= 0.00001;

  #include <clipping_planes_vertex>

}
`
export {textVertex}
