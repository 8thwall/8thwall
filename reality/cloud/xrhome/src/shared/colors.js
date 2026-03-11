const RED_MASK = 0xFF0000
const GREEN_MASK = 0x00FF00
const BLUE_MASK = 0x0000FF

/**
 * Returns either #000000 or #FFFFFF depending on the background color value provided.
 * The backgroundColor value provided can either be a string representing the RGB color,
 * or the raw int representing the RGB value.
 * @param {*} backgroundColor - string or int representing the background color text will be
 *  displayed on.
 */
function getContrastFontColor(backgroundColor) {
  let backgroundColorInt = backgroundColor
  if (typeof backgroundColor === 'string') {
    const colorHexStr = backgroundColor.charAt(0) === '#'
      ? backgroundColor.substring(1)
      : backgroundColor
    backgroundColorInt = parseInt(colorHexStr, 16)
  }

  const red = (backgroundColorInt & RED_MASK) >> 16
  const green = (backgroundColorInt & GREEN_MASK) >> 8
  const blue = backgroundColorInt & BLUE_MASK
  const value = ((red * 0.299) + (green * 0.587) + (blue * 0.114))
  return ((red * 0.299) + (green * 0.587) + (blue * 0.114)) > 186
    ? '#000000'
    : '#FFFFFF'
}

/**
 * Many brand colors are provided in hex. Changing the opacity for a hex number requires
 * adding two hex digits to the end of the hex color. Most designs provide the brand color
 * with the opacity as a percentage, not a hex value. This function will take a hex color,
 * and a percentage (0.0-1.0), and generate the appropriate hex color string.
 * e.g.
 *     Mango 33% -> #ffc828 33% -> hexColorWithAlpha('#ffc828', 0.33) -> '#ffc82854'
 * @param {*} hexColorString the hex color string without alpha. e.g "#7611b6"
 * @param {*} alpha the alpha percentage
 */
const hexColorWithAlpha = (hexColorString, alpha) => {
  if (typeof alpha !== 'number' || alpha < 0.0 || alpha > 1.0) {
    throw new Error('Alpha must be a number between 0.0 and 1.0')
  }
  const alphaValue = Math.floor(0xFF * alpha)
  const alphaHex = alphaValue.toString(16).padStart(2, '0')
  return `${hexColorString}${alphaHex}`
}

/**
 * Generates a color determined by an arbitrary hash function based on an inputted string.
 * Primarily used for generating color for monogram icons for default profile pictures.
 * @param {*} str - string that will be hashed into generating a color in hex
 */
const hexColorByString = (str) => {
  if (str.length === 0) {
    return '#ffffff'
  }

  /* eslint-disable no-bitwise */
  let hash = 0
  for (let i = 0; i < str.length; i++) {
    hash = str.charCodeAt(i) + ((hash << 5) - hash)
    hash &= hash
  }
  let color = '#'
  for (let i = 0; i < 3; i++) {
    const value = (hash >> (i * 8)) & 255
    color += value.toString(16).padStart(2, '0')
  }
  /* eslint-enable no-bitwise */
  return color
}

module.exports = {
  getContrastFontColor,
  hexColorWithAlpha,
  hexColorByString,
}
