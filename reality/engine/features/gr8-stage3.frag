#version 320 es

precision mediump float;

in vec2 texUv;
uniform sampler2D sampler;
uniform vec2 scale;
uniform float nmsTolerance;   // 255 is current; 0 is default; check 0-255;
uniform int skipSubpixel;
uniform float subpixelQ[6];
uniform float scoreThresh;
uniform float gain;
float texValueG(vec2 p) {
  return texture(sampler, texUv + p * scale).g;
}
vec4 texValueRGBA(vec2 p) {
  return texture(sampler, texUv + p * scale);
}
vec3 texValueGBA(vec2 p) {
  return texture(sampler, texUv + p * scale).gba;
}
vec2 texValueBA(vec2 p) {
  return texture(sampler, texUv + p * scale).ba;
}

float unpackTwoByteFloat(vec2 p) {
  const vec2 decode = vec2(1.0, 1.0/255.0);
  return dot(decode, p);
}
float unpackBA(vec2 p) {
  return unpackTwoByteFloat(texValueBA(p));
}
vec2 unpackGBA(vec2 p) {
  vec3 s = texValueGBA(p);
  return vec2(s.x, unpackTwoByteFloat(s.yz));
}
float maxOfThree(float c,
  float v00, float v01, float v02,
  float v10,            float v12,
  float v20, float v21, float v22) {
  if (nmsTolerance == 0.0) {
    // True NMS: supress all non-max including ties.
    return (
         c  > v00 && c  > v01 && c  > v02
      && c  > v10 &&             c  > v12
      && c  > v20 && c  > v21 && c  > v22)
      ? 1.0 : 0.0;
  } else if (nmsTolerance == 1.0) {
    // NMS with spatial tie breaker.
    return (
         c  > v00 && c  > v01 && c  > v02
      && c  > v10 &&             c >= v12
      && c >= v20 && c >= v21 && c >= v22)
      ? 1.0 : 0.0;
  } else {
    // No NMS.
    return 1.0;
  }
}
float gauss33(
  float v00, float v01, float v02,
  float v10, float v11, float v12,
  float v20, float v21, float v22) {
  return
    + 0.101868 * v00 + 0.115432 * v01 + 0.101868 * v02
    + 0.115432 * v10 + 0.130801 * v11 + 0.115432 * v12
    + 0.101868 * v20 + 0.115432 * v21 + 0.101868 * v22;
}
float maxOfFive(float c,
  float v00, float v01, float v02, float v03, float v04,
  float v10, float v11, float v12, float v13, float v14,
  float v20, float v21,            float v23, float v24,
  float v30, float v31, float v32, float v33, float v34,
  float v40, float v41, float v42, float v43, float v44) {
  if (nmsTolerance == 0.0) {
    // True NMS: supress all non-max including ties.
    return (
         c  > v00 && c  > v01 && c  > v02 && c  > v03 && c  > v04
      && c  > v10 && c  > v11 && c  > v12 && c  > v13 && c  > v14
      && c  > v20 && c  > v21 &&             c  > v23 && c  > v24
      && c  > v30 && c  > v31 && c  > v32 && c  > v33 && c  > v34
      && c  > v40 && c  > v41 && c  > v42 && c  > v43 && c  > v44)
      ? 1.0 : 0.0;
  } else if (nmsTolerance == 1.0) {
    // NMS with spatial tie breaker.
    return (
         c  > v00 && c  > v01 && c  > v02 && c  > v03 && c  > v04
      && c  > v10 && c  > v11 && c  > v12 && c  > v13 && c  > v14
      && c  > v20 && c  > v21 &&             c >= v23 && c >= v24
      && c >= v30 && c >= v31 && c >= v32 && c >= v33 && c >= v34
      && c >= v40 && c >= v41 && c >= v42 && c >= v43 && c >= v44)
      ? 1.0 : 0.0;
  } else {
    // No NMS.
    return 1.0;
  }
}
float maxOfSeven(float c,
  float v00, float v01, float v02, float v03, float v04, float v05, float v06,
  float v10, float v11, float v12, float v13, float v14, float v15, float v16,
  float v20, float v21, float v22, float v23, float v24, float v25, float v26,
  float v30, float v31, float v32,            float v34, float v35, float v36,
  float v40, float v41, float v42, float v43, float v44, float v45, float v46,
  float v50, float v51, float v52, float v53, float v54, float v55, float v56,
  float v60, float v61, float v62, float v63, float v64, float v65, float v66) {
  if (nmsTolerance == 0.0) {
    // True NMS: supress all non-max including ties.
    return (
         c  > v00 && c  > v01 && c  > v02 && c  > v03 && c  > v04 && c  > v05 && c  > v06
      && c  > v10 && c  > v11 && c  > v12 && c  > v13 && c  > v14 && c  > v15 && c  > v16
      && c  > v20 && c  > v21 && c  > v22 && c  > v23 && c  > v24 && c  > v25 && c  > v26
      && c  > v30 && c  > v31 && c  > v32             && c  > v34 && c  > v35 && c  > v36
      && c  > v40 && c  > v41 && c  > v42 && c  > v43 && c  > v44 && c  > v45 && c  > v46
      && c  > v50 && c  > v51 && c  > v52 && c  > v53 && c  > v54 && c  > v55 && c  > v56
      && c  > v60 && c  > v61 && c  > v62 && c  > v63 && c  > v64 && c  > v65 && c  > v66)
      ? 1.0 : 0.0;
  } else if (nmsTolerance == 1.0) {
    // NMS with spatial tie breaker.
    return (
         c  > v00 && c  > v01 && c  > v02 && c  > v03 && c  > v04 && c  > v05 && c  > v06
      && c  > v10 && c  > v11 && c  > v12 && c  > v13 && c  > v14 && c  > v15 && c  > v16
      && c  > v20 && c  > v21 && c  > v22 && c  > v23 && c  > v24 && c  > v25 && c  > v26
      && c  > v30 && c  > v31 && c  > v32             && c >= v34 && c >= v35 && c >= v36
      && c >= v40 && c >= v41 && c >= v42 && c >= v43 && c >= v44 && c >= v45 && c >= v46
      && c >= v50 && c >= v51 && c >= v52 && c >= v53 && c >= v54 && c >= v55 && c >= v56
      && c >= v60 && c >= v61 && c >= v62 && c >= v63 && c >= v64 && c >= v65 && c >= v66)
      ? 1.0 : 0.0;
  } else {
    // No NMS.
    return 1.0;
  }
}
float nmsScore(float c,
  float v00, float v01, float v02, float v03, float v04, float v05, float v06,
  float v10, float v11, float v12, float v13, float v14, float v15, float v16,
  float v20, float v21, float v22, float v23, float v24, float v25, float v26,
  float v30, float v31, float v32,            float v34, float v35, float v36,
  float v40, float v41, float v42, float v43, float v44, float v45, float v46,
  float v50, float v51, float v52, float v53, float v54, float v55, float v56,
  float v60, float v61, float v62, float v63, float v64, float v65, float v66) {
  return c * maxOfSeven(c,
    v00, v01, v02, v03, v04, v05, v06,
    v10, v11, v12, v13, v14, v15, v16,
    v20, v21, v22, v23, v24, v25, v26,
    v30, v31, v32,      v34, v35, v36,
    v40, v41, v42, v43, v44, v45, v46,
    v50, v51, v52, v53, v54, v55, v56,
    v60, v61, v62, v63, v64, v65, v66);
  /*
  float g00 = gauss33(      v00, v01, v02,      v10, v11, v12,       v20, v21, v22      );
  float g01 = gauss33(      v01, v02, v03,      v11, v12, v13,       v21, v22, v23      );
  float g02 = gauss33(      v02, v03, v04,      v12, v13, v14,       v22, v23, v24      );
  float g03 = gauss33(      v03, v04, v05,      v13, v14, v15,       v23, v24, v25      );
  float g04 = gauss33(      v04, v05, v06,      v14, v15, v16,       v24, v25, v26      );

  float g10 = gauss33(      v10, v11, v12,      v20, v21, v22,       v30, v31, v32      );
  float g11 = gauss33(      v11, v12, v13,      v21, v22, v23,       v31, v32,   c      );
  float g12 = gauss33(      v12, v13, v14,      v22, v23, v24,       v32,   c, v34      );
  float g13 = gauss33(      v13, v14, v15,      v23, v24, v25,         c, v34, v35      );
  float g14 = gauss33(      v14, v15, v16,      v24, v25, v26,       v34, v35, v36      );

  float g20 = gauss33(      v20, v21, v22,      v30, v31, v32,       v40, v41, v42      );
  float g21 = gauss33(      v21, v22, v23,      v31, v32,   c,       v41, v42, v43      );
  float  gc = gauss33(      v22, v23, v24,      v32,   c, v34,       v42, v43, v44      );
  float g23 = gauss33(      v23, v24, v25,        c, v34, v35,       v43, v44, v45      );
  float g24 = gauss33(      v24, v25, v26,      v34, v35, v36,       v44, v45, v46      );

  float g30 = gauss33(      v30, v31, v32,      v40, v41, v42,       v50, v51, v52      );
  float g31 = gauss33(      v31, v32,   c,      v41, v42, v43,       v51, v52, v53      );
  float g32 = gauss33(      v32,   c, v34,      v42, v43, v44,       v52, v53, v54      );
  float g33 = gauss33(        c, v34, v35,      v43, v44, v45,       v53, v54, v55      );
  float g34 = gauss33(      v34, v35, v36,      v44, v45, v46,       v54, v55, v56      );

  float g40 = gauss33(      v40, v41, v42,      v50, v51, v52,       v60, v61, v62      );
  float g41 = gauss33(      v41, v42, v43,      v51, v52, v53,       v61, v62, v63      );
  float g42 = gauss33(      v42, v43, v44,      v52, v53, v54,       v62, v63, v64      );
  float g43 = gauss33(      v43, v44, v45,      v53, v54, v55,       v63, v64, v65      );
  float g44 = gauss33(      v44, v45, v46,      v54, v55, v56,       v64, v65, v66      );

  return gc * maxOfFive(gc,
    g00, g01, g02, g03, g04,
    g10, g11, g12, g13, g14,
    g20, g21,      g23, g24,
    g30, g31, g32, g33, g34,
    g40, g41, g42, g43, g44);
  */
}

out vec4 color;
void main() {
  vec4 v33 =  texValueRGBA(vec2(0.0, 0.0));

  const float UNIT = 1.0 / 255.0;
  float threshold = UNIT * scoreThresh;
  float center = unpackTwoByteFloat(v33.ba);

  vec2 subpixelOffset = vec2(0.0, 0.0);

  // If this pixel has a corner score, it is a potential feature candidate and we need to run NMS
  // to see if it should be suppressed or kept.
  if (center > threshold) {
    // 7x1 Vertical Strip is used for gaussian blur (green channel).
    vec2 v03 =  unpackGBA( vec2(  0.0, -3.0));
    vec2 v13 =  unpackGBA( vec2(  0.0, -2.0));
    vec2 v23 =  unpackGBA( vec2(  0.0, -1.0));
    vec2 v43 =  unpackGBA( vec2(  0.0,  1.0));
    vec2 v53 =  unpackGBA( vec2(  0.0,  2.0));
    vec2 v63 =  unpackGBA( vec2(  0.0,  3.0));

    // 7x7 central block is used for robust NMS.
    float v00 =  unpackBA(  vec2( -3.0, -3.0));
    float v10 =  unpackBA(  vec2( -3.0, -2.0));
    float v20 =  unpackBA(  vec2( -3.0, -1.0));
    float v30 =  unpackBA(  vec2( -3.0,  0.0));
    float v40 =  unpackBA(  vec2( -3.0,  1.0));
    float v50 =  unpackBA(  vec2( -3.0,  2.0));
    float v60 =  unpackBA(  vec2( -3.0,  3.0));

    float v01 =  unpackBA(  vec2( -2.0, -3.0));
    float v11 =  unpackBA(  vec2( -2.0, -2.0));
    float v21 =  unpackBA(  vec2( -2.0, -1.0));
    float v31 =  unpackBA(  vec2( -2.0,  0.0));
    float v41 =  unpackBA(  vec2( -2.0,  1.0));
    float v51 =  unpackBA(  vec2( -2.0,  2.0));
    float v61 =  unpackBA(  vec2( -2.0,  3.0));

    float v02 =  unpackBA(  vec2( -1.0, -3.0));
    float v12 =  unpackBA(  vec2( -1.0, -2.0));
    float v22 =  unpackBA(  vec2( -1.0, -1.0));
    float v32 =  unpackBA(  vec2( -1.0,  0.0));
    float v42 =  unpackBA(  vec2( -1.0,  1.0));
    float v52 =  unpackBA(  vec2( -1.0,  2.0));
    float v62 =  unpackBA(  vec2( -1.0,  3.0));

    float v04 =  unpackBA(  vec2(  1.0, -3.0));
    float v14 =  unpackBA(  vec2(  1.0, -2.0));
    float v24 =  unpackBA(  vec2(  1.0, -1.0));
    float v34 =  unpackBA(  vec2(  1.0,  0.0));
    float v44 =  unpackBA(  vec2(  1.0,  1.0));
    float v54 =  unpackBA(  vec2(  1.0,  2.0));
    float v64 =  unpackBA(  vec2(  1.0,  3.0));

    float v05 =  unpackBA(  vec2(  2.0, -3.0));
    float v15 =  unpackBA(  vec2(  2.0, -2.0));
    float v25 =  unpackBA(  vec2(  2.0, -1.0));
    float v35 =  unpackBA(  vec2(  2.0,  0.0));
    float v45 =  unpackBA(  vec2(  2.0,  1.0));
    float v55 =  unpackBA(  vec2(  2.0,  2.0));
    float v65 =  unpackBA(  vec2(  2.0,  3.0));

    float v06 =  unpackBA(  vec2(  3.0, -3.0));
    float v16 =  unpackBA(  vec2(  3.0, -2.0));
    float v26 =  unpackBA(  vec2(  3.0, -1.0));
    float v36 =  unpackBA(  vec2(  3.0,  0.0));
    float v46 =  unpackBA(  vec2(  3.0,  1.0));
    float v56 =  unpackBA(  vec2(  3.0,  2.0));
    float v66 =  unpackBA(  vec2(  3.0,  3.0));

    // Set the r-channel to the original image pixel.
    color.r = v33.r;

    // Set the g-channel to the 7x7 gaussian blurred pixel by convolving the 7x1 vertical stripe
    // with the previously-convolved 1x7 horizontal stripe.
    color.g =
      dot(vec4(0.070159, 0.131075, 0.190713, 0.216106), vec4(v03.r, v13.r, v23.r, v33.g))
      + dot(vec3( 0.190713, 0.131075, 0.070159), vec3(v43.r, v53.r, v63.r));

    // Suppress the pixel if it (with contribution from its neighbors) is greater than all of its
    // neighbors (with contribution from their neighbors), breaking ties.
    float finalScore = gain * nmsScore(center,
      v00, v01, v02, v03.g, v04, v05, v06,
      v10, v11, v12, v13.g, v14, v15, v16,
      v20, v21, v22, v23.g, v24, v25, v26,
      v30, v31, v32,        v34, v35, v36,
      v40, v41, v42, v43.g, v44, v45, v46,
      v50, v51, v52, v53.g, v54, v55, v56,
      v60, v61, v62, v63.g, v64, v65, v66);

    if (finalScore > 0.0) {
      if (skipSubpixel == 0) {
        // See go/gr8sp for details.
        float z[9];
        z[0] = v22;
        z[1] = v23.g;
        z[2] = v24;
        z[3] = v32;
        z[4] = center;
        z[5] = v34;
        z[6] = v42;
        z[7] = v43.g;
        z[8] = v44;

        float u0[6];
        u0[0] = 0.5 * (z[3] + z[5]) - z[4];
        u0[1] = 0.5 * (z[1] + z[7]) - z[4];
        u0[2] = z[0] - z[1] - z[3] + z[4];
        u0[3] = 0.5 * (z[5] - z[3]);
        u0[4] = 0.5 * (z[7] - z[1]);
        u0[5] = z[4];

        float w[9];
        w[0] = 0.0;
        w[1] = 0.0;
        w[2] = z[2] - (u0[0] + u0[1] - u0[2] + u0[3] - u0[4] + u0[5]);
        w[3] = 0.0;
        w[4] = 0.0;
        w[5] = 0.0;
        w[6] = z[6] - (u0[0] + u0[1] - u0[2] - u0[3] + u0[4] + u0[5]);
        w[7] = 0.0;
        w[8] = z[8] - (u0[0] + u0[1] + u0[2] + u0[3] + u0[4] + u0[5]);

        float u[6];
        u[0] = u0[0] +
          subpixelQ[0] * (w[0] - 2.0 * w[1] + w[2] + w[6] - 2.0 * w[7] + w[8]) +
          subpixelQ[1] * (w[3] - 2.0 * w[4] + w[5]);
        u[1] = u0[1] +
          subpixelQ[0] * (w[0] + w[2] - 2.0 * w[3] - 2.0 * w[5] + w[6] + w[8]) +
          subpixelQ[1] * (w[1] - 2.0 * w[4] + w[7]);
        u[2] = u0[2] +
          0.25 * (w[0] - w[2] - w[6] + w[8]);
        u[3] = u0[3] +
          subpixelQ[2] * (w[5] - w[3]) + subpixelQ[3] * (w[2] - w[0] - w[6] + w[8]);
        u[4] = u0[4] +
          subpixelQ[2] * (w[7] - w[1]) + subpixelQ[3] * (w[6] - w[0] - w[2] + w[8]);
        u[5] = u0[5] +
          subpixelQ[4] * (2.0 * w[1] - w[0] - w[2] + 2.0 * w[3] + 2.0 * w[5] - w[6] + 2.0 * w[7] - w[8]) +
          subpixelQ[5] * w[4];

        float curvature = u[2] * u[2] - 4.0 * u[1] * u[0];

        if (u[0] < 0.0 && u[1] < 0.0 && curvature < 0.0) {
          subpixelOffset = vec2(
            2.0 * u[1] * u[3] - u[2] * u[4],
            2.0 * u[0] * u[4] - u[2] * u[3]) / curvature;

          /* Uncomment if score refinement is desired.
          finalScore = min(
            subpixelOffset.x * subpixelOffset.x * u[0] +
            subpixelOffset.y * subpixelOffset.y * u[1] +
            subpixelOffset.x * subpixelOffset.y * u[2] +
            subpixelOffset.x * u[3] +
            subpixelOffset.y * u[4] +
            u[5]);
          */
        }
      }

      color.b = finalScore;
    } else {
      // Pixel is suppressed from NMS.
      color.b = 0.0;
    }
  } else {
    // This is not a candidate feature point so we don't need to run NMS. Just compute the gaussian
    // blur. 7x1 Vertical Strip is used for gaussian blur (green channel).
    float v03 =  texValueG( vec2(0.0, -3.0));
    float v13 =  texValueG( vec2(0.0, -2.0));
    float v23 =  texValueG( vec2(0.0, -1.0));
    float v43 =  texValueG( vec2(0.0,  1.0));
    float v53 =  texValueG( vec2(0.0,  2.0));
    float v63 =  texValueG( vec2(0.0,  3.0));

    // Set the r-channel to the original image pixel.
    color.r = v33.r;

    // Set the g-channel to the 7x7 gaussian blurred pixel by convolving the 7x1 vertical stripe
    // with the previously-convolved 1x7 horizontal stripe.
    color.g =
      dot(vec4(0.070159, 0.131075, 0.190713, 0.216106), vec4(v03, v13, v23, v33.g))
      + dot(vec3( 0.190713, 0.131075, 0.070159), vec3(v43, v53, v63));

    // This is not a feature point.
    color.b = 0.0;
  }

  // Encode (-0.5, 0.5) symmetric, using 15 of 16 levels.
  vec2 scaled = clamp((subpixelOffset + 0.5) / 1.1, 0.0, 0.9375);
  vec2 quantOffset;
  quantOffset.x = floor(scaled.x * 16.0) / 255.0;
  quantOffset.y = 16.0 * floor(scaled.y * 16.0) / 255.0;
  color.a = quantOffset.x + quantOffset.y;
}
