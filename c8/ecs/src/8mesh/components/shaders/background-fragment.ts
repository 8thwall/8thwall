/* eslint-disable max-len */
const backgroundFragment = `

uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform vec3 uColor;
uniform float uOpacity;

uniform float uBorderRadiusTopLeft;
uniform float uBorderRadiusTopRight;
uniform float uBorderRadiusBottomLeft;
uniform float uBorderRadiusBottomRight;
uniform float uBorderWidth;
uniform vec3 uBorderColor;
uniform float uBorderOpacity;
uniform vec2 uSize;
uniform vec2 uTexSize;
uniform int uBackgroundMapping;
uniform float uNineSliceBorderTop;
uniform float uNineSliceBorderBottom;
uniform float uNineSliceBorderLeft;
uniform float uNineSliceBorderRight;
uniform float uNineSliceScaleFactor;
uniform float uUiScale;

varying vec2 vUv;

#include <clipping_planes_pars_fragment>

float getEdgeDist() {
  vec2 ndc = vec2( vUv.x * 2.0 - 1.0, vUv.y * 2.0 - 1.0 );
  vec2 planeSpaceCoord = vec2( uSize.x * 0.5 * ndc.x, uSize.y * 0.5 * ndc.y );
  vec2 corner = uSize * 0.5;
  vec2 offsetCorner = corner - abs( planeSpaceCoord );
  float innerRadDist = min( offsetCorner.x, offsetCorner.y ) * -1.0;
  if (vUv.x < 0.5 && vUv.y >= 0.5) {
    float roundedDist = length( max( abs( planeSpaceCoord ) - uSize * 0.5 + uBorderRadiusTopLeft, 0.0 ) ) - uBorderRadiusTopLeft;
    float s = step( innerRadDist * -1.0, uBorderRadiusTopLeft );
    return mix( innerRadDist, roundedDist, s );
  }
  if (vUv.x >= 0.5 && vUv.y >= 0.5) {
    float roundedDist = length( max( abs( planeSpaceCoord ) - uSize * 0.5 + uBorderRadiusTopRight, 0.0 ) ) - uBorderRadiusTopRight;
    float s = step( innerRadDist * -1.0, uBorderRadiusTopRight );
    return mix( innerRadDist, roundedDist, s );
  }
  if (vUv.x >= 0.5 && vUv.y < 0.5) {
    float roundedDist = length( max( abs( planeSpaceCoord ) - uSize * 0.5 + uBorderRadiusBottomRight, 0.0 ) ) - uBorderRadiusBottomRight;
    float s = step( innerRadDist * -1.0, uBorderRadiusBottomRight );
    return mix( innerRadDist, roundedDist, s );
  }
  if (vUv.x < 0.5 && vUv.y < 0.5) {
    float roundedDist = length( max( abs( planeSpaceCoord ) - uSize * 0.5 + uBorderRadiusBottomLeft, 0.0 ) ) - uBorderRadiusBottomLeft;
    float s = step( innerRadDist * -1.0, uBorderRadiusBottomLeft );
    return mix( innerRadDist, roundedDist, s );
  }
}

float calculateNineSliceTexturePosition(float _uv, float _size, float _tSize, float _min, float _max) {
  float new = _uv;
  float size = _size / uUiScale;
  float tSize = _tSize * uNineSliceScaleFactor;

  float min = _min * uNineSliceScaleFactor;
  float max = _max * uNineSliceScaleFactor;

  float firstSlice = min;
  float secondSlice = size - max;

  float raw = _uv * size;

  float factor = (tSize - (min + max)) / (size - (min + max));
  factor /= tSize / (size);

  if(raw <= firstSlice)
  {
    new = raw / tSize;
  }
  else if (raw >= secondSlice)
  {
    new = 1.0 - ((size - raw) / tSize);
  }
  else
  {
    new *= factor;
    new += 0.5 - 0.5 * factor;
  }

  return new;
}

vec4 sampleTexture() {
  float textureRatio = uTexSize.x / uTexSize.y;
  float panelRatio = uSize.x / uSize.y;
  vec2 uv = vUv;
  if ( uBackgroundMapping == 1 ) { // contain
    if ( textureRatio < panelRatio ) { // repeat on X
      float newX = uv.x * ( panelRatio / textureRatio );
      newX += 0.5 - 0.5 * ( panelRatio / textureRatio );
      uv.x = newX;
    } else { // repeat on Y
      float newY = uv.y * ( textureRatio / panelRatio );
      newY += 0.5 - 0.5 * ( textureRatio / panelRatio );
      uv.y = newY;
    }
  } else if ( uBackgroundMapping == 2 ) { // cover
    if ( textureRatio < panelRatio ) { // stretch on Y
      float newY = uv.y * ( textureRatio / panelRatio );
      newY += 0.5 - 0.5 * ( textureRatio / panelRatio );
      uv.y = newY;
    } else { // stretch on X
      float newX = uv.x * ( panelRatio / textureRatio );
      newX += 0.5 - 0.5 * ( panelRatio / textureRatio );
      uv.x = newX;
    }
  } else if (uBackgroundMapping == 3) { // nine-slice
    uv.x = calculateNineSliceTexturePosition(uv.x, uSize.x, uTexSize.x, uNineSliceBorderLeft, uNineSliceBorderRight);
    uv.y = calculateNineSliceTexturePosition(uv.y, uSize.y, uTexSize.y, uNineSliceBorderBottom, uNineSliceBorderTop);
  }


  float mask = float(uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0);
  return texture2D(uTexture, uv).rgba * mask;
}

void main() {
  float edgeDist = getEdgeDist();
  float change = fwidth( edgeDist );

  vec4 textureSample = uHasTexture ? sampleTexture() : vec4(1.0);
  vec3 blendedColor = textureSample.rgb * uColor;

  float alpha = smoothstep( change, 0.0, edgeDist );
  float blendedOpacity = uOpacity * textureSample.a * alpha;

  vec4 frameColor = vec4( blendedColor, blendedOpacity );

  if ( uBorderWidth <= 0.0 ) {
    gl_FragColor = frameColor;
  } else {
    vec4 borderColor = vec4( uBorderColor, uBorderOpacity * alpha );
    float stp = smoothstep( edgeDist + change, edgeDist, uBorderWidth * -1.0 );
    gl_FragColor = mix( frameColor, borderColor, stp );
  }

  #include <clipping_planes_fragment>

  #ifndef USE_VIDEO_TEXTURE
    #include <colorspace_fragment>
  #endif
}
`

export {backgroundFragment}
