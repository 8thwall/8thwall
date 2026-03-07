const textFragment = `

uniform sampler2D uTexture;
uniform vec3 uColor;
uniform float uOpacity;
uniform float uPxRange;
uniform bool uUseRGSS;

varying vec2 vUv;

#include <clipping_planes_pars_fragment>

// functions from the original msdf repo:
// https://github.com/Chlumsky/msdfgen#using-a-multi-channel-distance-field

float median(float r, float g, float b) {
  return max(min(r, g), min(max(r, g), b));
}

float screenPxRange() {
  vec2 unitRange = vec2(uPxRange)/vec2(textureSize(uTexture, 0));
  vec2 screenTexSize = vec2(1.0)/fwidth(vUv);
  return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float tap(vec2 offsetUV) {
  vec3 msd = texture( uTexture, offsetUV ).rgb;
  float sd = median(msd.r, msd.g, msd.b);
  float screenPxDistance = screenPxRange() * (sd - 0.5);
  float alpha = clamp(screenPxDistance + 0.5, 0.0, 1.0);
  return alpha;
}

void main() {

  float alpha;

  if ( uUseRGSS ) {

    // shader-based supersampling based on 
    // https://bgolus.medium.com/sharper-mipmapping-using-shader-based-supersampling-ed7aadb47bec
    // per pixel partial derivatives
    vec2 dx = dFdx(vUv);
    vec2 dy = dFdy(vUv);

    // rotated grid uv offsets
    vec2 uvOffsets = vec2(0.125, 0.375);
    vec2 offsetUV = vec2(0.0, 0.0);

    // supersampled using 2x2 rotated grid
    alpha = 0.0;
    offsetUV.xy = vUv + uvOffsets.x * dx + uvOffsets.y * dy;
    alpha += tap(offsetUV);
    offsetUV.xy = vUv - uvOffsets.x * dx - uvOffsets.y * dy;
    alpha += tap(offsetUV);
    offsetUV.xy = vUv + uvOffsets.y * dx - uvOffsets.x * dy;
    alpha += tap(offsetUV);
    offsetUV.xy = vUv - uvOffsets.y * dx + uvOffsets.x * dy;
    alpha += tap(offsetUV);
    alpha *= 0.25;

  } else {

    alpha = tap( vUv );

  }


  // apply the opacity
  alpha *= uOpacity;

  // this is useful to avoid z-fighting when quads overlap because of kerning
  if ( alpha < 0.02) discard;


  gl_FragColor = vec4( uColor, alpha );

  #include <clipping_planes_fragment>
  #include <colorspace_fragment>

}
`

export {textFragment}
