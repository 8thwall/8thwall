#ifndef C8_PIXELS_RENDER_PHYSICAL_LIGHTING_VERT_
#define C8_PIXELS_RENDER_PHYSICAL_LIGHTING_VERT_

out vec3 normalInView;
out vec3 fragPosInView;

void physicalLighting(vec4 position, vec4 normal, mat4 mv, mat4 normalMatrix) {
  fragPosInView = vec3(mv * position);
  normalInView = mat3(normalMatrix) * vec3(normal);
}

#endif
