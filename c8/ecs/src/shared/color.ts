// Parse a color string in the format '#RRGGBB' to an object with the components r, g, b
const parseColor = (color: string) => {
  if (color.length !== 7 || color[0] !== '#') {
    return undefined
  }

  const parseColorComponent = (index: number) => Number(`0x${color.slice(index, index + 2)}`)
  const r = parseColorComponent(1)
  const g = parseColorComponent(3)
  const b = parseColorComponent(5)

  if (Number.isNaN(r) || Number.isNaN(g) || Number.isNaN(b)) {
    return undefined
  }

  return {r, g, b}
}

// Convert common hex color string formats (e.g. '#rgb', 'RRGGBB') to the standard '#rrggbb'
const formatColor = (color: string) => {
  const colorWithHash = (color.startsWith('#') ? color : `#${color}`).toLowerCase()
  // convert short format to long format
  if (colorWithHash.length === 4) {
    return colorWithHash.replace(/[^#]/g, (c: string) => c + c)
  }

  return colorWithHash
}

export {
  parseColor,
  formatColor,
}
