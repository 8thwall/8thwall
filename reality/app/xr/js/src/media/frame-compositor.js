import {CanvasOperations} from './canvas-operations'
import {create8wCanvas} from '../canvas'

const DIRECT_CONFIG_PARAMS = [
  'endCardCallToAction',
  'maxDimension',
  'foregroundCanvas',
  'shortLink',
]

const createFrameCompositor = (sourceCanvas, initialConfig) => {
  const canvas = create8wCanvas('frame-compositor')
  const ctx = canvas.getContext('2d')

  // Save the canvas configuration on init so we can restore to the original state before sending
  // it to the user in onProcessFrame.
  ctx.save()

  const config = {...initialConfig}

  const start = () => {
    const {width, height} = CanvasOperations.scaleCanvas(sourceCanvas.width, sourceCanvas.height,
      config.maxDimension, config.maxDimension)

    canvas.width = width
    canvas.height = height

    return {width, height}
  }

  const configure = (newConfig) => {
    DIRECT_CONFIG_PARAMS.forEach((param) => {
      if (newConfig[param] !== undefined) {
        config[param] = newConfig[param]
      }
    })
  }

  const drawEndCard = ({coverImage, footerImage, endCardOpacity = 1}) => {
    CanvasOperations.drawEndCard({
      canvas,
      ctx,
      coverImage,
      footerImage,
      callToAction: config.endCardCallToAction,
      shortLink: config.shortLink,
      endCardOpacity,
    })
  }

  const drawFrame = ({
    elapsedTimeMs, maxRecordingMs, endCardOpacity, footerImage, coverImage, onProcessFrame,
  }) => {
    CanvasOperations.copyCanvas(canvas, ctx, sourceCanvas, config.foregroundCanvas)

    if (onProcessFrame) {
      ctx.restore()
      ctx.save()
      onProcessFrame({elapsedTimeMs, maxRecordingMs, ctx, canvas})
      ctx.restore()
      ctx.save()
    }

    if (endCardOpacity > 0) {
      drawEndCard({footerImage, coverImage, endCardOpacity})
    }
  }

  const getCanvasData = () => (
    ctx.getImageData(0, 0, canvas.width, canvas.height).data
  )

  // Get a media track that can be used by the native MediaRecorder
  const getVideoTrack = () => (
    canvas.captureStream().getVideoTracks()[0]
  )

  // This is a workaround because the native recorder won't use the last frame of the end card
  // unless we make a tiny change before ending the recording.
  const flush = () => {
    ctx.fillStyle = '#00000001'
    ctx.fillRect(0, 0, 1, 1)
  }

  return ({
    configure,
    start,
    drawFrame,
    getCanvasData,
    drawEndCard,
    getVideoTrack,
    flush,
  })
}

export {
  createFrameCompositor,
}
