const scaleToFit = (width, height, maxWidth, maxHeight) => {
  const scale = Math.min(maxWidth / width, maxHeight / height)

  return {width: width * scale, height: height * scale}
}

// Fit to max dimensions, keeping the dimensions divisible by 2
const scaleCanvas = (width, height, maxWidth, maxHeight) => {
  const scale = Math.min(maxWidth / width, maxHeight / height, 1)

  return {width: Math.round((width * scale) / 2) * 2, height: Math.round((height * scale) / 2) * 2}
}

// If there are two canvases, foregroundCanvas is overlaid over the sourceCanvas
const copyCanvas = (canvas, ctx, sourceCanvas, foregroundCanvas) => {
  const {width, height} = scaleCanvas(
    sourceCanvas.width, sourceCanvas.height, canvas.width, canvas.height
  )

  const x = (canvas.width - width) / 2
  const y = (canvas.height - height) / 2

  if (width !== canvas.width || height !== canvas.height) {
    ctx.fillStyle = 'black'
    ctx.fillRect(0, 0, canvas.width, canvas.height)
  }

  ctx.drawImage(sourceCanvas, x, y, width, height)

  if (foregroundCanvas) {
    ctx.drawImage(foregroundCanvas, x, y, width, height)
  }
}

const drawCoverImage = (canvas, ctx, image) => {
  const {naturalWidth, naturalHeight} = image
  const {width, height} = scaleToFit(naturalWidth, naturalHeight, canvas.width, canvas.height / 2)
  const x = (canvas.width - width) / 2
  const y = canvas.height / 2 - height

  ctx.drawImage(image, x, y, width, height)
}

const drawFooterImage = (canvas, ctx, image) => {
  const {width, height} = scaleToFit(
    image.naturalWidth, image.naturalHeight, canvas.width * 0.33, canvas.height * 0.07
  )
  const x = (canvas.width - width) / 2
  const y = canvas.height * 0.97 - height

  ctx.drawImage(image, x, y, width, height)
}

const drawEndCard = ({
  canvas, ctx, coverImage, footerImage, callToAction, shortLink, endCardOpacity,
}) => {
  // Applies fade to all end card elements
  ctx.globalAlpha = endCardOpacity
  ctx.fillStyle = 'black'
  ctx.fillRect(0, 0, canvas.width, canvas.height)

  if (coverImage) {
    drawCoverImage(canvas, ctx, coverImage)
  }

  if (shortLink) {
    const textStartY = canvas.height * (coverImage ? 0.55 : 0.40)
    const fontSize = Math.ceil(Math.min(canvas.width, canvas.height) * 0.06)

    ctx.fillStyle = 'white'
    ctx.textAlign = 'center'
    ctx.textBaseline = 'top'
    ctx.font = `normal ${fontSize}px "Nunito", Arial, sans-serif`
    ctx.fillText(callToAction, canvas.width / 2, textStartY)
    ctx.font = `bold ${fontSize}px "Nunito", Arial, sans-serif`
    ctx.fillText(shortLink, canvas.width / 2, textStartY + 2 * fontSize)
  }

  if (footerImage) {
    drawFooterImage(canvas, ctx, footerImage)
  }

  ctx.globalAlpha = 1
}

const CanvasOperations = {
  scaleToFit,
  scaleCanvas,
  copyCanvas,
  drawEndCard,
}

export {
  CanvasOperations,
}
