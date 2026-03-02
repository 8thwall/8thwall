#ifndef C8_PIXELS_RENDER_SHADERS_SPLAT_VERT_
#define C8_PIXELS_RENDER_SHADERS_SPLAT_VERT_

const vec4 BEHIND_CAM = vec4(0.0, 0.0, 2.0, 1.0);

vec3 shOffset(
  vec3 camPos,
  vec3 instancePosition,
  vec3[3] sh1,
  vec3[5] sh2,
  vec3[7] sh3
) {
  vec3 toCam = instancePosition.xyz - camPos;
  float dist = length(toCam);
  if (dist < 1e-6f) {
    toCam = vec3(0.0, 0.0, 0.0);
  } else {
    toCam /= dist;
  }

  float x = toCam.x, y = toCam.y, z = toCam.z;
  float x2 = x * x, y2 = y * y, z2 = z * z;
  float xy = x * y, xz = x * z, yz = y * z;

  return sh1[0] * (-0.488603 * y)                  // 0  - y
    + sh1[1] * (0.488603 * z)                      // 1  - z
    + sh1[2] * (-0.488603 * x)                     // 2  - x
    + sh2[0] * (1.092548 * xy)                     // 3  - x, y
    + sh2[1] * (-1.092548 * yz)                    // 4  - y, z
    + sh2[2] * (0.946175 * z2 - 0.315392)          // 5
    + sh2[3] * (-1.092548 * xz)                    // 6  - x, z
    + sh2[4] * (0.546274 * (x2 - y2))              // 7
    + sh3[0] * (-0.590044 * y * (3.0 * x2 - y2))   // 8  - y
    + sh3[1] * (2.890611 * xy * z)                 // 9  - x, y, z
    + sh3[2] * (0.457046 * y * (1.0 - 5.0 * z2))   // 10 - y
    + sh3[3] * (0.373176 * z * (5.0 * z2 - 3.0))   // 11 - z
    + sh3[4] * (0.457046 * x * (1.0 - 5.0 * z2))   // 12 - x
    + sh3[5] * (1.445306 * z * (x2 - y2))          // 13 - z
    + sh3[6] * (-0.590044 * x * (x2 - 3.0 * y2));  // 14 - x
}

void projectSplat(
  // Inputs
  in mat4 mv,                // Model-view matrix for the splat.
  in mat4 p,                 // Projection matrix for the camera
  in float raysToPix,        // Mapping from rays to pixels.
  in vec4 vertexPosition,    // Quad corner of this vertex.
  in vec4 instancePosition,  // Position of the splat.
  in vec4 instanceColor,     // Color of the splats.
  in float[6] rssrCoeffs,    // RSS'R' coefficients for the splat.
  in float zDir,             // Whether to flip the Z axis, either Right Up Back or Right Up Front.
  in float antialiased,      // Whether to use 2D mip filter for antialiasing
  // Outputs
  out vec4 outVertexPosition,  // Output vertex position for the input quad corner.
  out vec4 splatColor,         // Color of the splat.
  out vec2 splatEccentricity,  // Gaussian eccentricity of the splat.
  out float maxSqEccentricity  // Maximum squared eccentricity for this splat to add color.
) {
  // minAlpha controls how the maxSigma and the size of the fragments that need to be drawn, so it
  // is an important performance / quality knob. The equation is maxSigma = sqrt(-2 * ln(10/255))
  //
  // minAlpha | maxSigma | fragment area
  //    1/255 |   3.33   | 100%
  //    5/255 |   2.80   |  70%
  //   10/255 |   2.55   |  58%
  //   16/255 |   2.35   |  50%
  //   25/255 |   2.15   |  41%
  //   35/255 |   1.99   |  35%
  float minAlpha = 10.0 / 255.0;
  splatColor = instanceColor / 255.0;

  if (instanceColor.a < minAlpha) {
    outVertexPosition = BEHIND_CAM;
    return;
  }

  vec4 splat3d = mv * instancePosition;
  splat3d /= splat3d.w;
  vec4 splat2d = p * splat3d;
  splat2d /= splat2d.w;

  // cull behind camera
  if (splat2d.z < -1.0 || splat2d.z > 1.0) {
    outVertexPosition = BEHIND_CAM;
    return;
  }

  float c0 = rssrCoeffs[0];
  float c1 = rssrCoeffs[1];
  float c2 = rssrCoeffs[2];
  float c3 = rssrCoeffs[3];
  float c4 = rssrCoeffs[4];
  float c5 = rssrCoeffs[5];

  mat3 rssr = mat3(
    vec3(c0, c1, c2),  // col 0
    vec3(c1, c3, c4),  // col 1
    vec3(c2, c4, c5)   // col 2
  );

  // This needs to match the space of the projection matrix (e.g. RDF or RUB or RUF).
  // Currently this is RUB.
  mat3 J = mat3(
    vec3(1.0, 0., -splat3d.x / splat3d.z),  // col 0
    vec3(0., 1.0, -splat3d.y / splat3d.z),  // col 1
    vec3(0., 0., 0.)                        // col 2
  );

  mat3 T = transpose(mat3(mv)) * J;
  mat3 cov2d = transpose(T) * rssr * T;

  // Low-pass filter the covariance matrix to prevent pixel-level gaps in the splats.
  // We increase the standard deviation of the splats by approximately 1 pixel per 3
  // standard deviations.
  float lowPassPaddingStd = splat3d.z / (1.7320508 * raysToPix);
  float lowPassPaddingVar = lowPassPaddingStd * lowPassPaddingStd;
  float diagonal1 = cov2d[0][0] + lowPassPaddingVar;
  float offDiagonal = cov2d[0][1];
  float diagonal2 = cov2d[1][1] + lowPassPaddingVar;

  float mid = 0.5 * (diagonal1 + diagonal2);
  float radius = length(vec2((diagonal1 - diagonal2) / 2.0, offDiagonal));
  float lambda1 = mid + radius;
  float lambda2 = mid - radius;

  if (lambda2 <= 0.0) {
    outVertexPosition = BEHIND_CAM;
    return;
  }

  vec2 diagonalVector = vec2(offDiagonal, lambda1 - diagonal1);

  float dn = length(diagonalVector);
  if (dn < 1e-12f) {
    // Splats that are too long and thin are culled.
    outVertexPosition = BEHIND_CAM;
    return;
  } else {
    diagonalVector /= dn;
  }

  float std1 = sqrt(lambda1);
  float std2 = sqrt(lambda2);

  // cull splats that are too close to the near clip plane and also too large.
  float projectedStd1 = std1 / (zDir * splat3d.z);
  if (projectedStd1 > 1.0 / min(p[0][0], p[1][1])) {
    outVertexPosition = BEHIND_CAM;
    return;
  }

  // cull splats that are too far from the camera and also too small.
  float projectedStd2 = std2 / (zDir * splat3d.z);
  if (projectedStd2 < 3e-4 / max(p[0][0], p[1][1])) {
    outVertexPosition = BEHIND_CAM;
    return;
  }

  if (antialiased != 0.0) {
    // Apply alpha scale factor for 2D mip filter.
    // TODO: Figure out how to do this correction in terms of majorAxis and minorAxis below.
    float detPostLowPass = diagonal1 * diagonal2 - offDiagonal * offDiagonal;
    float detPreLowPass =
      (diagonal1 - lowPassPaddingVar) * (diagonal2 - lowPassPaddingVar) - offDiagonal * offDiagonal;
    splatColor.a *= sqrt(max(0.0, detPreLowPass / detPostLowPass));
  }

  // We want:
  //   exp(-0.5 * sqEccentricity) * alpha > minAlpha
  //                -0.5 * sqEccentricity > log(minALpha / alpha)
  //                       sqEccentricity < -2.0 * log(minAlpha / alpha)
  maxSqEccentricity = -2.0 * log(minAlpha / splatColor.a);

  float maxSigma = sqrt(maxSqEccentricity);
  vec2 majorAxis = maxSigma * std1 * diagonalVector;
  vec2 minorAxis = maxSigma * std2 * vec2(diagonalVector.y, -diagonalVector.x);

  mat4 transform = mat4(
    vec4(majorAxis, 0.0, 0.0),
    vec4(minorAxis, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(splat3d.xyz, 1.0));

  outVertexPosition = p * transform * vertexPosition;
  splatEccentricity = maxSigma * vertexPosition.xy;
}

// Convert 16-bit float to 32-bit float
// There are faster implementations out there but the one below supports denormals, infinity, and nan's
float unpackF16(uint h) {
  uint s = (h >> 15u) & 0x1u;       // Sign bit
  uint e = (h >> 10u) & 0x1Fu;      // Exponent
  uint m = h & 0x3FFu;              // Mantissa
  float sign = s == 1u ? -1.0 : 1.0;
  float exponent;
  float mantissa;

  if (e == 0u) {
    if (m == 0u) {
        return sign * 0.0;         // Zero
    } else {
        // Subnormal number
        exponent = -14.0;
        mantissa = float(m) / 1024.0;
    }
  } else if (e == 31u) {
    if (m == 0u) {
      return sign * 1.0 / 0.0;   // Infinity
    } else {
      return sign * (1.0 / 0.0 - 1.0 / 0.0); // NaN
    }
  } else {
    exponent = float(e) - 15.0;
    mantissa = 1.0 + float(m) / 1024.0;
  }
  return sign * mantissa * exp2(exponent);
}

// unpackHalf2x16 isn't available on all devices, use this manual floating point hack if
// not supported (such as on MacOS)
vec2 unpackHalf2x16_OSX(uint packed) {
  uint x = packed & 0xFFFFu;           // Extract lower 16 bits
  uint y = (packed >> 16u) & 0xFFFFu;  // Extract upper 16 bits
  float fx = unpackF16(x);
  float fy = unpackF16(y);
  return vec2(fx, fy);
}

float[6] rssrCoeffs(uvec4 c) {
  #ifdef OSX
  vec2 c1 = unpackHalf2x16_OSX(c.x);
  vec2 c2 = unpackHalf2x16_OSX(c.y);
  vec2 c3 = unpackHalf2x16_OSX(c.z);
  #else
  vec2 c1 = unpackHalf2x16(c.x);
  vec2 c2 = unpackHalf2x16(c.y);
  vec2 c3 = unpackHalf2x16(c.z);
  #endif

  return float[6](0.25 * c1.x, 0.25 * c1.y, 0.25 * c2.x, 0.25 * c2.y, 0.25 * c3.x, 0.25 * c3.y);
}


vec3 b2f(int bits, vec3 b) { return b * float(1 << (8 - bits)) / 128.0f - 1.0f; }

vec4 unpackPosition(uvec4 positionColor) {
  return vec4(uintBitsToFloat(positionColor.xyz), 1);
}

vec4 unpackBaseColor(uvec4 positionColor) {
  uint packedColor = uint(positionColor.w);
  return vec4(
    (packedColor) & 0xffu,
    (packedColor >> 8) & 0xffu,
    (packedColor >> 16) & 0xffu,
    (packedColor >> 24) & 0xffu);
}

vec3[3] unpackSh1(uvec4 shRG, uvec4 shB) {
  // The order of the indexes here is accounting for endianness of encoding uint32 pixels.
  return vec3[3](
    b2f(5, vec3((shRG.x >> 27) & 0x1fu, (shRG.z >> 27) & 0x1fu, (shB.x >> 27) & 0x1fu)),
    b2f(5, vec3((shRG.x >> 22) & 0x1fu, (shRG.z >> 22) & 0x1fu, (shB.x >> 22) & 0x1fu)),
    b2f(5, vec3((shRG.x >> 17) & 0x1fu, (shRG.z >> 17) & 0x1fu, (shB.x >> 17) & 0x1fu)));
}

vec3[5] unpackSh2(uvec4 shRG, uvec4 shB) {
  // The order of the indexes here is accounting for endianness of encoding uint32 pixels.
  return vec3[5](
    b2f(4, vec3((shRG.x >> 13) & 0x0fu, (shRG.z >> 13) & 0x0fu, (shB.x >> 13) & 0x0fu)),
    b2f(4, vec3((shRG.x >> 9) & 0x0fu, (shRG.z >> 9) & 0x0fu, (shB.x >> 9) & 0x0fu)),
    b2f(4, vec3((shRG.x >> 5) & 0x0fu, (shRG.z >> 5) & 0x0fu, (shB.x >> 5) & 0x0fu)),
    b2f(4, vec3((shRG.x >> 1) & 0x0fu, (shRG.z >> 1) & 0x0fu, (shB.x >> 1) & 0x0fu)),
    b2f(4, vec3((shRG.y >> 28) & 0x0fu, (shRG.w >> 28) & 0x0fu, (shB.y >> 28) & 0x0fu)));
}

vec3[7] unpackSh3(uvec4 shRG, uvec4 shB) {
  // The order of the indexes here is accounting for endianness of encoding uint32 pixels.
  return vec3[7](
    b2f(4, vec3((shRG.y >> 24) & 0x0fu, (shRG.w >> 24) & 0x0fu, (shB.y >> 24) & 0x0fu)),
    b2f(4, vec3((shRG.y >> 20) & 0x0fu, (shRG.w >> 20) & 0x0fu, (shB.y >> 20) & 0x0fu)),
    b2f(4, vec3((shRG.y >> 16) & 0x0fu, (shRG.w >> 16) & 0x0fu, (shB.y >> 16) & 0x0fu)),
    b2f(4, vec3((shRG.y >> 12) & 0x0fu, (shRG.w >> 12) & 0x0fu, (shB.y >> 12) & 0x0fu)),
    b2f(4, vec3((shRG.y >> 8) & 0x0fu, (shRG.w >> 8) & 0x0fu, (shB.y >> 8) & 0x0fu)),
    b2f(4, vec3((shRG.y >> 4) & 0x0fu, (shRG.w >> 4) & 0x0fu, (shB.y >> 4) & 0x0fu)),
    b2f(4, vec3((shRG.y >> 0) & 0x0fu, (shRG.w >> 0) & 0x0fu, (shB.y >> 0) & 0x0fu)));
}

void unpackAndProjectSplatMultiTex(
  // Inputs
  in mat4 mv,                            // Model-view matrix for the splat.
  in mat4 p,                             // Projection matrix for the camera
  in float raysToPix,                    // Mapping from rays to pixels.
  in vec4 vertexPosition,                // Quad corner of this vertex.
  in uint instanceId,                    // Index of the splat instance.
  in highp usampler2D positionColorTexture, // Texture containing position and color.
  in highp usampler2D rotationScaleTexture, // Texture containing rotation and scale.
  in highp usampler2D shRGTexture, // Texture containing SH coefficients for the red and green channels.
  in highp usampler2D shBTexture, // Texture containing SH coefficients for the blue channel.
  in float zDir,  // Whether to flip the Z axis, either Right Up Back or Right Up Front.
  in float antialiased,                  // Whether to use 2D mip filter for antialiasing
  // Outputs
  out vec4 outVertexPosition,  // Output vertex position for the input quad corner.
  out vec4 splatColor,         // Color of the splat.
  out vec2 splatEccentricity,  // Gaussian eccentricity of the splat.
  out float maxSqEccentricity  // Maximum squared eccentricity for this splat to add color.
) {

  int width = textureSize(positionColorTexture, 0).x;
  ivec2 texelCoords = ivec2(int(instanceId) % width, int(instanceId) / width);

  uvec4 pix0 = texelFetch(positionColorTexture, texelCoords, 0);
  uvec4 pix1 = texelFetch(rotationScaleTexture, texelCoords, 0);
  uvec4 pix2 = texelFetch(shRGTexture, texelCoords, 0);
  uvec4 pix3 = texelFetch(shBTexture, texelCoords, 0);

  vec4 instancePosition = unpackPosition(pix0);
  vec4 instanceColor = unpackBaseColor(pix0);
  float coeffs[6] = rssrCoeffs(pix1);
  vec3 sh1[3] = unpackSh1(pix2, pix3);
  vec3 sh2[5] = unpackSh2(pix2, pix3);
  vec3 sh3[7] = unpackSh3(pix2, pix3);

  projectSplat(
    // Inputs
    mv,
    p,
    raysToPix,
    vertexPosition,
    instancePosition,
    instanceColor,
    coeffs,
    zDir,
    antialiased,
    // Outputs
    outVertexPosition,
    splatColor,
    splatEccentricity,
    maxSqEccentricity);

  // inverse(mv)[3].xyz is the camera position in model space, should probably just pass this into
  // the shader.
  splatColor.rgb += shOffset(inverse(mv)[3].xyz, instancePosition.xyz, sh1, sh2, sh3);
}

void unpackAndProjectSplat(
  // Inputs
  in mat4 mv,                            // Model-view matrix for the splat.
  in mat4 p,                             // Projection matrix for the camera
  in float raysToPix,                    // Mapping from rays to pixels.
  in vec4 vertexPosition,                // Quad corner of this vertex.
  in uvec4[5] instanceAttributes,        // Attributes associated with splat instance.
  in uint instanceId,                    // Index of the splat instance.
  in highp usampler2D splatDataTexture,  // Texture containing splat data.
  in float sortTexture,                  // Whether to sort the splats.
  in float zDir,  // Whether to flip the Z axis, either Right Up Back or Right Up Front.
  in float antialiased,                  // Whether to use 2D mip filter for antialiasing
  // Outputs
  out vec4 outVertexPosition,  // Output vertex position for the input quad corner.
  out vec4 splatColor,         // Color of the splat.
  out vec2 splatEccentricity,  // Gaussian eccentricity of the splat.
  out float maxSqEccentricity  // Maximum squared eccentricity for this splat to add color.
) {
  uvec4 pix0 = instanceAttributes[0];  // positionColor
  uvec4 pix1 = instanceAttributes[1];  // scaleRotation
  uvec4 pix2 = instanceAttributes[2];  // shRG
  uvec4 pix3 = instanceAttributes[3];  // shB

  if (sortTexture == 0.0) {
    // Fetch the splat data from the texture.
    int pixelsPerSplat = 4;
    int splatsPerRow = textureSize(splatDataTexture, 0).x / pixelsPerSplat;
    int col = (int(instanceId) % splatsPerRow) * pixelsPerSplat;
    int row = int(instanceId) / splatsPerRow;

    pix0 = texelFetch(splatDataTexture, ivec2(col, row), 0);
    pix1 = texelFetch(splatDataTexture, ivec2(col + 1, row), 0);
    // Not texelFetch these SH coordinates. (set to zero SH, then do no SH computation at all)
    // improve fps of Kate Sessions from 24 to 30fps.
    pix2 = texelFetch(splatDataTexture, ivec2(col + 2, row), 0);
    pix3 = texelFetch(splatDataTexture, ivec2(col + 3, row), 0);
  }

  vec4 instancePosition = unpackPosition(pix0);
  vec4 instanceColor = unpackBaseColor(pix0);

  float coeffs[6] = rssrCoeffs(pix1);

  vec3 sh1[3] = unpackSh1(pix2, pix3);
  vec3 sh2[5] = unpackSh2(pix2, pix3);
  vec3 sh3[7] = unpackSh3(pix2, pix3);

  projectSplat(
    // Inputs
    mv,
    p,
    raysToPix,
    vertexPosition,
    instancePosition,
    instanceColor,
    coeffs,
    zDir,
    antialiased,
    // Outputs
    outVertexPosition,
    splatColor,
    splatEccentricity,
    maxSqEccentricity);

  // inverse(mv)[3].xyz is the camera position in model space, should probably just pass this into
  // the shader.
  splatColor.rgb += shOffset(inverse(mv)[3].xyz, instancePosition.xyz, sh1, sh2, sh3);
}

#endif  // C8_PIXELS_RENDER_SHADERS_SPLAT_VERT_
