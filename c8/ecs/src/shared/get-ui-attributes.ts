import type {UiGraphSettings} from './scene-graph'
import type {UiRuntimeSettings} from '../runtime/ui'
import {extractResourceUrl, extractFontResourceUrl} from './resource'
import {UI_DEFAULTS} from './ui-constants'

// NOTE(johnny): All properties need to be returned to ensure that runtime values get reset
// if you remove them from the scene graph.
const getUiAttributes = (ui: UiGraphSettings): UiRuntimeSettings => ({
  ...UI_DEFAULTS,
  ...ui,
  font: ui.font ? extractFontResourceUrl(ui.font) : UI_DEFAULTS.font.font,
  background: ui.background ? extractResourceUrl(ui.background) : UI_DEFAULTS.background,
  image: ui.image ? extractResourceUrl(ui.image) : UI_DEFAULTS.image,
  video: ui.video ? extractResourceUrl(ui.video) : UI_DEFAULTS.video,
  width: typeof ui.width === 'number' ? ui.width.toString() : ui.width ?? UI_DEFAULTS.width,
  height: typeof ui.height === 'number' ? ui.height.toString() : ui.height ?? UI_DEFAULTS.height,
})

export {getUiAttributes}
