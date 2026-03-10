#ifndef C8_PIXELS_RENDER_PHYSICAL_LIGHTING_FRAG_
#define C8_PIXELS_RENDER_PHYSICAL_LIGHTING_FRAG_

// TODO(nb): Figure out how to make number of lights larger without crashing webgl. On Android
// Chrome I noticed that this shader could not be linked. It appeared the reason was because the
// majority of work in this shader was inlined to a single line, and the gpu didn't have enough
// registers to hold all of the temporary values in memory at once. Reducing the number of lights
// temporarily fixes this problem, but a better solution would be to write the code in a way that
// SPIRV compiler doesn't generate single mega statements.
#define NUM_POINT_LIGHTS 5  // NOTE: Keep this in sync with renderer.cc.
#define NUM_DIR_LIGHTS 5    // NOTE: Keep this in sync with renderer.cc.

struct PointLight {
  vec3 posInView;
  vec3 color;
  float intensity;
};

struct DirectionalLight {
  vec3 dirInView;
  vec3 color;
  float intensity;
};

in vec3 normalInView;
in vec3 fragPosInView;

uniform vec3 ambientLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform DirectionalLight directionalLights[NUM_DIR_LIGHTS];

vec3 pointLight(PointLight light, vec3 normal, vec3 fragPosInView, vec3 viewDir) {
  vec3 lightDir = normalize(light.posInView - fragPosInView);
  // Diffuse light.
  float diff = max(dot(normal, lightDir), 0.0);

  // Specular light.
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);

  // Attenuation.
  float distance = length(light.posInView - fragPosInView);
  float attenuation = 1.0 / (1.0 + 0.045 * distance + 0.0075 * (distance * distance));

  vec3 diffuse = vec3(0.8, 0.8, 0.8) * diff;
  vec3 specular = vec3(1.0, 1.0, 1.0) * spec;
  return (diffuse + specular) * attenuation * light.intensity * light.color;
}

vec3 directionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.dirInView);
  // Diffuse light.
  float diff = max(dot(normal, lightDir), 0.0);

  // Specular light.
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);

  vec3 diffuse = vec3(0.8, 0.8, 0.8) * diff;
  vec3 specular = vec3(1.0, 1.0, 1.0) * spec;
  return (diffuse + specular) * light.intensity * light.color;
}

vec4 physicalLightForNormal(vec3 norm) {
  vec3 light = ambientLight;

  // Calculations are done in view space, so the camera is always at (0, 0, 0).
  vec3 viewDir = normalize(-fragPosInView);

  // TODO(paris): Find why for loop causes compiler errors with WebGL1.
  light += pointLight(pointLights[0], norm, fragPosInView, viewDir);
  light += pointLight(pointLights[1], norm, fragPosInView, viewDir);
  light += pointLight(pointLights[2], norm, fragPosInView, viewDir);
  light += pointLight(pointLights[3], norm, fragPosInView, viewDir);
  light += pointLight(pointLights[4], norm, fragPosInView, viewDir);

  light += directionalLight(directionalLights[0], norm, viewDir);
  light += directionalLight(directionalLights[1], norm, viewDir);
  light += directionalLight(directionalLights[2], norm, viewDir);
  light += directionalLight(directionalLights[3], norm, viewDir);
  light += directionalLight(directionalLights[4], norm, viewDir);

  return vec4(light, 1.0);
}

vec4 physicalLight() {
  vec3 norm = normalize(normalInView);
  return physicalLightForNormal(norm);
}

#endif
