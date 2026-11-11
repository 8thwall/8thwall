import FOOTER_IMAGE_URL from '../almosttheremodule/poweredby-horiz-white.svg'

declare const XR8: any

interface CaptureAudioNodes {
  audioProcessor: AudioNode
  microphoneInput: AudioNode
}

interface MediaRecorderFrameInfo {
  ctx: CanvasRenderingContext2D
  [key: string]: unknown
}

type ConfigureAudioOutput = (nodes: CaptureAudioNodes) => AudioNode
type WatermarkLocation = 'topLeft' | 'topMiddle' | 'topRight' |
  'bottomLeft' | 'bottomMiddle' | 'bottomRight'

interface MediaRecorderConfigOptions {
  audioContext?: AudioContext
  configureAudioOutput?: ConfigureAudioOutput | null
  coverImageUrl?: string
  enableEndCard?: boolean
  endCardCallToAction?: string
  fileNamePrefix?: string
  footerImageUrl?: string
  maxDimension?: number
  maxDurationMs?: number
  onProcessFrame?: (frameInfo: MediaRecorderFrameInfo) => void
  requestMic?: string
  shortLink?: string
  watermarkImageUrl?: string
  watermarkLocation?: WatermarkLocation
  watermarkMaxHeight?: number
  watermarkMaxWidth?: number
}

interface InternalMediaRecorderConfig extends MediaRecorderConfigOptions {
  watermarkImage?: HTMLImageElement | null
}

const currentConfig: MediaRecorderConfigOptions = {
  enableEndCard: true,
  footerImageUrl: FOOTER_IMAGE_URL,
}

const internalConfig: InternalMediaRecorderConfig = {}

let waitingOnXr = false

let loadedWatermarkUrl: string | undefined

const updateWatermarkImage = () => {
  const newImageUrl = internalConfig.watermarkImageUrl
  if (newImageUrl === loadedWatermarkUrl) {
    return
  }

  loadedWatermarkUrl = newImageUrl

  if (!loadedWatermarkUrl) {
    internalConfig.watermarkImage = null
    return
  }

  const img = document.createElement('img') as unknown as HTMLImageElement

  img.onload = () => {
    internalConfig.watermarkImage = img
  }

  img.onerror = () => {
    // eslint-disable-next-line no-console
    console.error(`Failed to load image from ${img.src}`)
    internalConfig.watermarkImage = null
  }

  img.setAttribute('crossorigin', 'anonymous')
  img.setAttribute('src', loadedWatermarkUrl)
}

const updateConfig = () => {
  XR8.MediaRecorder.configure(currentConfig)
}

const internalKeys = new Set<keyof MediaRecorderConfigOptions>([
  'watermarkImageUrl', 'watermarkMaxWidth', 'watermarkMaxHeight', 'watermarkLocation',
  'fileNamePrefix', 'onProcessFrame',
])

const configure = (config: MediaRecorderConfigOptions = {}) => {
  (Object.keys(config) as Array<keyof MediaRecorderConfigOptions>).forEach((key) => {
    const value = config[key]
    if (value === undefined) {
      return
    }

    if (internalKeys.has(key)) {
      Object.assign(internalConfig, {[key]: value})
    } else {
      Object.assign(currentConfig, {[key]: value})
    }
  })

  updateWatermarkImage()

  if (window.XR8) {
    updateConfig()
  } else if (!waitingOnXr) {
    waitingOnXr = true
    window.addEventListener('xrloaded', updateConfig, {once: true})
  }
}

const getConfig = () => internalConfig

export {
  configure,
  getConfig,
}
export type {
  CaptureAudioNodes,
  ConfigureAudioOutput,
  MediaRecorderConfigOptions,
  MediaRecorderFrameInfo,
  WatermarkLocation,
}
