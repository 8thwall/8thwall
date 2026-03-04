// @attr[](visibility = "//c8/ecs:__subpackages__")
// @attr[](visibility = "//reality/app/nae/packager:__subpackages__")
// @attr[](visibility = "//reality/cloud:shared")

const BASIC = 'https://cdn.8thwall.com/web/assets/envmap/basic_env_map-m9hqpneh.jpg'
const STUDIO = 'https://cdn.8thwall.com/web/assets/envmap/studio_small_08_1k-m89fuwlx.hdr'
const CAPE_HILL = 'https://cdn.8thwall.com/web/assets/envmap/cape_hill_1k-m89fuwlx.hdr'
const GOLDEN_BAY = 'https://cdn.8thwall.com/web/assets/envmap/golden_bay_1k-m89fuwlx.hdr'
const HILLS = 'https://cdn.8thwall.com/web/assets/envmap/hilly_terrain_01_1k-m89fuwlx.hdr'
const ESPLANADE = 'https://cdn.8thwall.com/web/assets/envmap/royal_esplanade_1k-m89fuwlx.hdr'
const CATHEDRAL = 'https://cdn.8thwall.com/web/assets/envmap/small_cathedral_02_1k-m89fuwlx.hdr'
const WINTER_LAKE = 'https://cdn.8thwall.com/web/assets/envmap/winter_lake_01_1k-m89fuwlx.hdr'

const ENV_MAP_PRESETS = {
  [BASIC]: 'reflection.preset.option.basic',
  [STUDIO]: 'reflection.preset.option.studio',
  [CAPE_HILL]: 'reflection.preset.option.cape_hill',
  [GOLDEN_BAY]: 'reflection.preset.option.golden_bay',
  [HILLS]: 'reflection.preset.option.hills',
  [ESPLANADE]: 'reflection.preset.option.esplanade',
  [CATHEDRAL]: 'reflection.preset.option.cathedral',
  [WINTER_LAKE]: 'reflection.preset.option.winter_lake',
}

export {
  ENV_MAP_PRESETS,
  BASIC,
}
