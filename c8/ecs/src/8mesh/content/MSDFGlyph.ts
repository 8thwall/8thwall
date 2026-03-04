// @ts-nocheck

/**
 * Job: create a plane geometry with the right UVs to map the MSDF texture on the wanted glyph.
 *
 * Knows: dimension of the plane to create, specs of the font used, glyph requireed
 */
export default class MSDFGlyph extends window.THREE.PlaneGeometry {
  constructor(inline, font) {
    const char = inline.glyph
    const {fontSize} = inline

    // super( fontSize, fontSize );
    super(inline.width, inline.height)

    // Misc glyphs
    if (font && char.match(/\s/g) === null) {
      if (!font._charset.has(char)) {
        // eslint-disable-next-line no-console
        console.error(`The character '${char}' is not included in the font characters set.`)
      }
      this.mapUVs(font, char)
      this.transformGeometry(inline)

      // White spaces (we don't want our plane geometry to have a visual width nor a height)
    } else {
      this.nullifyUVs()

      this.scale(0, 0, 1)
      this.translate(0, fontSize / 2, 0)
    }
  }

  translate(...args) {
    super.translate(...args)
  }

  /**
   * Compute the right UVs that will map the MSDF texture so that the passed character
   * will appear centered in full size
   * @private
   */
  mapUVs(font, char) {
    const charObj = font._chars[char]
    const {common} = font

    const xMin = charObj.x / common.scaleW

    const xMax = (charObj.x + charObj.width) / common.scaleW

    const yMin = 1 - ((charObj.y + charObj.height) / common.scaleH)
    const yMax = 1 - (charObj.y / common.scaleH)

    //

    const uvAttribute = this.attributes.uv

    for (let i = 0; i < uvAttribute.count; i++) {
      let u = uvAttribute.getX(i)
      let v = uvAttribute.getY(i)
      // eslint-disable-next-line consistent-return
      ;[u, v] = (() => {
        // eslint-disable-next-line default-case
        switch (i) {
          case 0:
            return [xMin, yMax]
          case 1:
            return [xMax, yMax]
          case 2:
            return [xMin, yMin]
          case 3:
            return [xMax, yMin]
        }
      })()

      uvAttribute.setXY(i, u, v)
    }
  }

  /** Set all UVs to 0, so that none of the glyphs on the texture will appear */
  nullifyUVs() {
    const uvAttribute = this.attributes.uv

    for (let i = 0; i < uvAttribute.count; i++) {
      uvAttribute.setXY(i, 0, 0)
    }
  }

  /** Gives the previously computed scale and offset to the geometry */
  transformGeometry(inline) {
    this.translate(
      inline.width / 2,
      inline.height / 2,
      0
    )
  }
}
