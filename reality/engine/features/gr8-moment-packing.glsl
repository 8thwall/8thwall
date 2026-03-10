#ifndef REALITY_ENGINE_FEATURES_GR8_MOMENT_PACKING_
#define REALITY_ENGINE_FEATURES_GR8_MOMENT_PACKING_

vec3 packMoment(vec3 unpacked) {
  vec3 sgn = sign(unpacked);
  vec3 transform = sgn * sqrt(abs(unpacked));
  return transform * vec3(1.0, 1.0, 0.5) + vec3(0.0, 0.0, 0.5);
}

vec3 unpackMoment(vec3 pcked) {
  vec3 transform = pcked * vec3(1.0, 1.0, 2.0) - vec3(0.0, 0.0, 1.0);
  vec3 sgn = sign(transform);
  return sgn * transform * transform;
}

#endif  // REALITY_ENGINE_FEATURES_GR8_MOMENT_PACKING_
