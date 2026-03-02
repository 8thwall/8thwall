#ifndef C8_PIXELS_RENDER_SHADERS_GEOMETRY_
#define C8_PIXELS_RENDER_SHADERS_GEOMETRY_

mat4 identity() {
  return mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));
}

mat4 trs3(vec3 t, vec4 r, vec3 s) {
  vec3 w = r.w * r.xyz;
  vec3 x = r.x * r.xyz;
  float yy = r.y * r.y;
  float yz = r.y * r.z;
  float zz = r.z * r.z;

  float m00 = 1.0 - 2.0 * (yy + zz);
  float m01 = 2.0 * (x.y - w.z);
  float m02 = 2.0 * (x.z + w.y);

  float m10 = 2.0 * (x.y + w.z);
  float m11 = 1.0 - 2.0 * (x.x + zz);
  float m12 = 2.0 * (yz - w.x);

  float m20 = 2.0 * (x.z - w.y);
  float m21 = 2.0 * (yz + w.x);
  float m22 = 1.0 - 2.0 * (x.x + yy);

  return mat4(
    s.z * vec4(m00, m10, m20, 0.0),
    s.y * vec4(m01, m11, m21, 0.0),
    s.z * vec4(m02, m12, m22, 0.0),
    vec4(t, 1.0));
}

mat4 trs(vec4 position, vec4 rotation, vec4 scale) {
  vec3 scale3 = vec3(1, 1, 1);
  if (dot(scale.xyz, scale.xyz) > 0.0) {
    scale3 = scale.xyz;
  }
  return trs3(position.xyz / position.w, rotation, scale3);
}

#endif  // C8_PIXELS_RENDER_SHADERS_GEOMETRY_
