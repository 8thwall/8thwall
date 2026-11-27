/* globals AFRAME */

// Restores the defaults of the AR ground shadow from 8-Frame on A-Frame's `shadow` shader, so that
// `material="shader: shadow"` looks the same as with 8-Frame. This differs from stock A-Frame:
// - alphaToCoverage: false (A-Frame: true). With antialiasing (on by default on desktop),
//   alpha to coverage gives a dithered shadow instead of a smooth one.
// - opacity: 0.4 (A-Frame: 0.5), 8-Frame's default.
// Values set on the material, e.g. `material="shader: shadow; opacity: 0.6"`, are kept.
const SHADOW_SHADER_DEFAULTS = {
  alphaToCoverage: false,
  opacity: 0.4,
}

const applyShadowShaderDefaults = () => {
  const shadow = AFRAME.shaders && AFRAME.shaders.shadow
  if (!shadow || !shadow.schema) {
    return
  }
  Object.keys(SHADOW_SHADER_DEFAULTS).forEach((name) => {
    // 8-Frame's shadow shader has no alphaToCoverage property.
    if (shadow.schema[name]) {
      shadow.schema[name].default = SHADOW_SHADER_DEFAULTS[name]
    }
  })
}

export {
  applyShadowShaderDefaults,
}
