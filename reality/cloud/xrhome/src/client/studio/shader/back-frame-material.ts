import {ShaderMaterial, BackSide, Color} from 'three'

import {gray2} from '../../static/styles/settings'

const BACK_FRAME_MATERIAL = new ShaderMaterial({
  // eslint-disable-next-line local-rules/hardcoded-copy
  vertexShader: `
varying vec2 vUv;
varying vec3 vPosition;
varying vec3 vNormal;

void main() {
  vUv = uv;
  vPosition = position;
  vNormal = normal;
  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}
  `,
  // eslint-disable-next-line local-rules/hardcoded-copy
  fragmentShader: `
uniform vec3 color;
uniform float fillAlpha;
uniform float lineAlpha;
uniform float borderWidth;
varying vec2 vUv;
varying vec3 vPosition;
varying vec3 vNormal;

void main() {
  // calculate screen-space derivatives for consistent border width
  vec2 fw = fwidth(vUv);

  // calculate normalized distance from edges, accounting for aspect ratio
  vec2 distFromEdge = min(vUv, 1.0 - vUv);
  vec2 normalizedDist = distFromEdge / fw;

  // use the minimum normalized distance for consistent border width
  float minNormalizedDist = min(normalizedDist.x, normalizedDist.y);

  // calculate the alpha value for border or fill (depending on where we are)
  float border = step(minNormalizedDist, borderWidth);
  float alpha = border * lineAlpha + (1.0 - border) * fillAlpha;

  gl_FragColor = vec4(color, alpha);
}
  `,
  side: BackSide,
  transparent: true,
  uniforms: {
    color: {value: new Color(gray2)},
    fillAlpha: {value: 0.4},
    lineAlpha: {value: 0.6},
    borderWidth: {value: 5},
  },
  depthWrite: false,
})

export {
  BACK_FRAME_MATERIAL,
}
