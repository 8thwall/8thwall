import {unconify} from './unconify'

onmessage = (event) => {
  const {imgData, pixelPoints, outputWidth} = event.data
  const unconedImageData = unconify(imgData, pixelPoints, outputWidth)
  postMessage(unconedImageData)
}
