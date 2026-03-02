// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Akul Gupta (akulgupta@nianticlabs.com)

// Creates a canvas with the 8w tag along with canvasId tag in the classlist.
// This allows us and end users to better debug canvas elements.
const create8wCanvas = (canvasId: string) => {
  const canvas = document.createElement('canvas')
  canvas.classList.add('8w')
  canvas.classList.add(canvasId)
  return canvas
}

export {
  create8wCanvas,
}
