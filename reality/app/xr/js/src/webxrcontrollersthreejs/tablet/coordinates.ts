const worldToPlane = (worldPosition, plane) => {
  plane.worldToLocal(worldPosition)
  return {
    x: worldPosition.x,
    y: worldPosition.y,
  }
}

// Plane boundaries are -0.5 to 0.5, with the origin at the bottom left
const planeToElement = (planePosition, domWidth, domHeight) => ({
  x: Math.min(1, Math.max(0, planePosition.x + 0.5)) * domWidth,
  y: Math.min(1, Math.max(0, -planePosition.y + 0.5)) * domHeight,
})

const elementToScreen = (domPosition, domElement) => {
  const clientRect = domElement.getBoundingClientRect()
  return {
    x: domPosition.x + clientRect.x,
    y: domPosition.y + clientRect.y,
  }
}

const intersectionToClient = (intersection, plane, domWidth, domHeight, domElement) => {
  const planePosition = worldToPlane(intersection.point, plane)
  const domElementPosition = planeToElement(planePosition, domWidth, domHeight)
  return elementToScreen(domElementPosition, domElement)
}

export {
  intersectionToClient,
}
