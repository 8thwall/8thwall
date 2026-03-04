const STRETCH = 'stretch'
const CONTAIN = 'contain'
const COVER = 'cover'
const NINESLICE = 'nineslice'

type BackgroundSize = typeof STRETCH | typeof CONTAIN | typeof COVER | typeof NINESLICE

const convertBackgroundSizeToUniform = (size: BackgroundSize) => {
  switch (size) {
    case STRETCH:
      return 0
    case CONTAIN:
      return 1
    case COVER:
      return 2
    case NINESLICE:
      return 3
    default:
      throw new Error(`Unknown background size: ${size}`)
  }
}

export {
  STRETCH,
  CONTAIN,
  COVER,
  NINESLICE,
  convertBackgroundSizeToUniform,
}

export type {
  BackgroundSize,
}
