import React from 'react'
import {extend, MaterialNode} from '@react-three/fiber'
import type * as THREE from 'three'
import {shaderMaterial} from '@react-three/drei'

const UNIFORM_DEFAULTS = {
  displayScale: 1,
}

const HiderMaterial = shaderMaterial(UNIFORM_DEFAULTS,
  // eslint-disable-next-line local-rules/hardcoded-copy
  `
#include <skinning_pars_vertex>
varying vec2 vUv;
void main() {
  #include <skinbase_vertex>
  #include <begin_vertex>
  #include <skinning_vertex>
  #include <project_vertex>

  vUv = uv;
  gl_Position = projectionMatrix * mvPosition;
}
  `,
  // eslint-disable-next-line local-rules/hardcoded-copy
  `
uniform vec3 color;
uniform float displayScale;
varying vec2 vUv;

float stippleSpacing = 16.;
float stippleSize = 4.;

void main() {
  bool xIntersects = mod(gl_FragCoord.x / displayScale, stippleSpacing) < stippleSize;
  bool yIntersects = mod(gl_FragCoord.y / displayScale, stippleSpacing) < stippleSize;
  if (xIntersects && yIntersects) {
    gl_FragColor.rgba = vec4(0.);
  } else {
    gl_FragColor.rgba = vec4(0.75, 0.75, 0.75, 0.25);
  }
}
  `)

// https://r3f.docs.pmnd.rs/api/objects#using-3rd-party-objects-declaratively
extend({HiderMaterial})

declare module '@react-three/fiber' {
  interface ThreeElements {
    hiderMaterial: (
      MaterialNode<THREE.ShaderMaterial & typeof UNIFORM_DEFAULTS, typeof HiderMaterial>
    )
  }
}

const MakeMeshHiderMaterial: React.FC = () => (
  <hiderMaterial
    key={HiderMaterial.key}
    transparent
    displayScale={window.devicePixelRatio ?? 1}
  />
)

export {
  HiderMaterial,
  MakeMeshHiderMaterial,
}
