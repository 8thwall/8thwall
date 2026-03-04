// @attr[](visibility = "//reality/app/nae/packager:__subpackages__")
// @attr[](visibility = "//c8/ecs:__subpackages__")
// @attr[](visibility = "//reality/cloud:shared")

import {getResourceBase} from './resources'

type Font = {
  key: string
}

const FONTS: Map<string, Font> = new Map([
  [
    'Nunito', {
      key: 'nunito',
    },
  ],
  [
    'Baskerville', {
      key: 'baskerville',
    },
  ],
  [
    'Futura', {
      key: 'futura',
    },
  ],
  [
    'Nanum Pen Script', {
      key: 'nanum_pen_script',
    },
  ],
  [
    'Press Start 2p', {
      key: 'press_start_2p',
    },
  ],
  [
    'Inconsolata', {
      key: 'inconsolata',
    },
  ],
  [
    'Roboto', {
      key: 'roboto',
    },
  ],
])

type FontUrls = {
  json: string
  png: string
  ttf: string
}

const getBuiltinFontUrls = (name: string): FontUrls | null => {
  if (FONTS.has(name)) {
    const {key} = FONTS.get(name)!
    const base = getResourceBase()
    return {
      json: `${base}fonts/${key}/${key}.json`,
      png: `${base}fonts/${key}/${key}.png`,
      ttf: `${base}fonts/${key}/${key}.ttf`,
    }
  }
  return null
}

const DEFAULT_FONT_NAME = 'Nunito'

export type {
  Font,
  FontUrls,
}

export {
  FONTS,
  DEFAULT_FONT_NAME,
  getBuiltinFontUrls,
}
