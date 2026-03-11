import {ShaderMaterial, Texture, Vector2, Color} from 'three'

interface OutlineUniforms {
  thickness: number
  color: Color
  texture: Texture
  canvasWidth: number
  canvasHeight: number
}

const makeOutlineMaterial = (uniforms: OutlineUniforms) => new ShaderMaterial({
  uniforms: {
    tDiffuse: {value: uniforms.texture},
    resolution: {value: new Vector2(uniforms.canvasWidth, uniforms.canvasHeight)},
    outlineColor: {value: uniforms.color},
    thickness: {value: uniforms.thickness},
  },
  // eslint-disable-next-line local-rules/hardcoded-copy
  vertexShader: `
    varying vec2 vUv;
    void main() {
      vUv = uv;
      gl_Position = vec4(position, 1.0);
    }
  `,
  // eslint-disable-next-line local-rules/hardcoded-copy
  fragmentShader: `
    varying vec2 vUv;
    uniform sampler2D tDiffuse;
    uniform vec2 resolution;
    uniform vec3 outlineColor;
    uniform float thickness;

    void main() {
      vec2 offset = thickness / resolution;

      // Sample red channel values in a 3x3 grid surrounding the current fragment
      float tl = texture2D(tDiffuse, vUv + vec2(-offset.x, -offset.y)).r;
      float tm = texture2D(tDiffuse, vUv + vec2(0.0, -offset.y)).r;
      float tr = texture2D(tDiffuse, vUv + vec2(offset.x, -offset.y)).r;
      float ml = texture2D(tDiffuse, vUv + vec2(-offset.x, 0.0)).r;
      float mm = texture2D(tDiffuse, vUv).r;
      float mr = texture2D(tDiffuse, vUv + vec2(offset.x, 0.0)).r;
      float bl = texture2D(tDiffuse, vUv + vec2(-offset.x, offset.y)).r;
      float bm = texture2D(tDiffuse, vUv + vec2(0.0, offset.y)).r;
      float br = texture2D(tDiffuse, vUv + vec2(offset.x, offset.y)).r;

      // Calculate gradient magnitude using Sobel operator
      // See: https://homepages.inf.ed.ac.uk/rbf/HIPR2/sobel.htm
      float sobelX = -tl + tr - 2.0 * ml + 2.0 * mr - bl + br;
      float sobelY = -tl - 2.0 * tm - tr + bl + 2.0 * bm + br;
      float gradientMagnitude = sqrt(sobelX * sobelX + sobelY * sobelY);

      // Use threshold to eliminate noise
      const float threshold = 0.1;
      if (gradientMagnitude < threshold) {
        discard;
      }

      gl_FragColor = vec4(outlineColor, 1.0);
    }
  `,
  depthWrite: false,
  depthTest: false,
})

export {
  makeOutlineMaterial,
}
