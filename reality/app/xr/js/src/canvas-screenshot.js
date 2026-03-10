/**
 * Creates an object that can take a screenshot of the XR canvas.
 */

import {create8wCanvas} from './canvas'
import {singleton} from './factory'

const CanvasScreenshotFactory = singleton(() => {
  let canvas_ = null
  let fgCanvas_ = null
  let repeatTries = 0
  let captureCanvas_ = null
  let captureContext_ = null

  const MAX_REPEAT_TRIES = 10
  const config_ = {
    maxDimension: 1280,
    jpgCompression: 75,
  }

  const onAttach = ({canvas}) => {
    canvas_ = canvas
    captureCanvas_ = create8wCanvas('canvas-screenshot')
    captureContext_ = captureCanvas_.getContext('2d')
  }

  const onDetach = () => {
    canvas_ = null
    captureCanvas_.width = 0
    captureCanvas_.height = 0
    captureCanvas_ = null
    captureContext_ = null
  }

  const pipelineModule = () => ({
    name: 'canvasscreenshot',
    onAttach,
    onDetach,
  })

  const setForegroundCanvas = (canvas) => {
    fgCanvas_ = canvas
  }

  const configure = ({maxDimension, jpgCompression}) => {
    config_.maxDimension = maxDimension || config_.maxDimension
    config_.jpgCompression = jpgCompression || config_.jpgCompression
  }

  // Returns base64 string of image data upon success, null upon failure
  const getCanvasData = (options) => {
    const {maxDimension, jpgCompression} = config_

    let captureWidth = canvas_.width
    let captureHeight = canvas_.height

    if (captureWidth > captureHeight && captureWidth > maxDimension) {
      captureHeight = Math.round(maxDimension / captureWidth * captureHeight)
      captureWidth = maxDimension
    } else if (captureHeight > captureWidth && captureHeight > maxDimension) {
      captureWidth = Math.round(maxDimension / captureHeight * captureWidth)
      captureHeight = maxDimension
    }

    captureCanvas_.width = captureWidth
    captureCanvas_.height = captureHeight

    captureContext_.drawImage(canvas_, 0, 0, captureWidth, captureHeight)

    // drawImage can fail on iOS which leaves the canvas transparent
    // We return null so we can retry later.
    const firstPixel = captureContext_.getImageData(0, 0, 1, 1)
    if (firstPixel.data[3] === 0) {
      return null
    }

    if (fgCanvas_) {
      captureContext_.drawImage(fgCanvas_, 0, 0, captureWidth, captureHeight)
    }

    if (options && options.onProcessFrame) {
      captureContext_.save()
      options.onProcessFrame({
        ctx: captureContext_,
        canvas: captureCanvas_,
      })
      captureContext_.restore()
    }

    const dataWithHeader = captureCanvas_.toDataURL('image/jpeg', jpgCompression / 100)

    const data = dataWithHeader.substring('data:image/jpeg;base64,'.length)

    return data
  }

  const takeScreenshot = options => new Promise((resolve, reject) => {
    if (!canvas_) {
      reject(new Error('Not attached to a running camera feed'))
      return
    }
    window.requestAnimationFrame(() => {
      const imageData = getCanvasData(options)

      if (repeatTries < MAX_REPEAT_TRIES && !imageData) {
        repeatTries++
        setTimeout(() => {
          takeScreenshot(options).then(resolve, reject)
        }, 60)
      } else {
        repeatTries = 0
        if (imageData) {
          resolve(imageData)
        } else {
          reject(new Error('Unable to read pixels from canvas.'))
        }
      }
    })
  })

  const displayScreenshot = (imData) => {
    // This is provided for convenience and debugging. Apps will probably want to create their own
    // custom version of this.

    const b = document.getElementsByTagName('body')[0]
    const imdiv = document.createElement('div')
    imdiv.id = 'screenshot'

    imdiv.style = `
      position: fixed;
      z-index: 20;
      left: 10%;
      top: 10%;
      width: 80%;
      height: 80%;
      overflow: none;
      padding: 0px;
      background-color: rgb(255, 255, 255);
      box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);`
    b.appendChild(imdiv)

    const drawImg = document.createElement('img')
    drawImg.id = 'screenshot-img'
    drawImg.onload = () => {
      const nh = drawImg.naturalHeight
      const nw = drawImg.naturalWidth
      const dw = imdiv.clientWidth
      const dh = imdiv.clientHeight
      const sw = dw / drawImg.naturalWidth
      const sh = dh / drawImg.naturalHeight
      const s = Math.max(sw, sh)

      // eslint-disable-next-line no-console
      console.log(`[canvas-screenshot(js)] Scaling by ${s} to make ${nw}x${nh} cover ${dw}x${dh}`)

      drawImg.width = drawImg.naturalWidth * s
      drawImg.height = drawImg.naturalHeight * s
      imdiv.appendChild(drawImg)
    }

    drawImg.src = `data:image/jpeg;base64,${imData}`
    drawImg.setAttribute('download', 'screenshot.jpg')

    /*
    // Something like this might be needed to enable copying of the image on iOS.
    const download = document.createElement('a')
    download.download = 'screenshot.jpg'
    download.href = 'data:image/jpeg;base64,' + imData
    download.hidden = true
    imdiv.appendChild(download)
    */

    imdiv.onclick = () => {
      // download.click()
      imdiv.parentNode.removeChild(imdiv)
    }
  }

  return {

    // Creates a camera pipeline module that, when installed, receives callbacks on when
    // the camera has started and when the canvas size has changed.
    pipelineModule,

    // Configures the expected result of canvas screenshots.
    //
    // maxDimension: [Optional] The value of the largest expected dimension
    // jpgCompression: [Optional] 1-100 value representing the JPEG compression quality. 100 is
    //   little to no loss, and 1 is a very low quality image.
    configure,

    // Returns a Promise that when resolved, provides a buffer containing the JPEG compressed image.
    // When rejected, an error message is provided.
    takeScreenshot,

    // Sets a foreground canvas to be displayed on top of the camera canvas.
    // This must be the same dimensions as the camera canvas.
    //
    // canvas: The canvas to use as a foreground in the screenshot
    setForegroundCanvas,

    // Deprecated in R9.1
    canvasScreenshot: () => ({
      cameraPipelineModule: pipelineModule,
      configure,
      takeScreenshot,
      setForegroundCanvas,
      displayScreenshot: () => takeScreenshot().then(displayScreenshot),
    }),
  }
})

export {
  CanvasScreenshotFactory,
}
