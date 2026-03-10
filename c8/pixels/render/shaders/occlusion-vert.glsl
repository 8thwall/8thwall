#ifndef C8_PIXELS_RENDER_SHADERS_OCCLUSION_VERT_
#define C8_PIXELS_RENDER_SHADERS_OCCLUSION_VERT_

out vec4 projPos;    // Projected position
out float fragDepth; // Fragment depth
out float outSize;   // Set to pointSize / mvz, Only used for gl_point pointclouds

void occlusion(mat4 mvp, mat4 mv, vec4 position) {
  projPos = mvp * position;
  fragDepth = (mv * position).z;
}

// Set ptSize to pointSize / mvz (Do not account for raysToPix)
void occlusionPointCloud(mat4 mvp, mat4 mv, vec4 position, float ptSize) {
  projPos = mvp * position;
  fragDepth = (mv * position).z;
  outSize = ptSize;
}

#endif
