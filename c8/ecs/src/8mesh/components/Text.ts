// @ts-nocheck
import InlineComponent from './core/InlineComponent'
import MeshUIComponent from './core/MeshUIComponent'
import FontLibrary from './core/FontLibrary'
import * as TextManager from './core/TextManager'
import MaterialManager from './core/MaterialManager'

import deepDelete from '../utils/deepDelete'
import {mix} from '../utils/mix'
import * as Whitespace from '../utils/inline-layout/Whitespace'

const {Object3D} = window.THREE

/**

Job:
- computing its own size according to user measurements or content measurement
- creating 'inlines' objects with info, so that the parent component can organize them in lines

Knows:
- Its text content (string)
- Font attributes ('font', 'fontSize'.. etc..)
- Parent block

 */
export default class Text extends mix.withBase(Object3D)(
  InlineComponent,
  MaterialManager,
  MeshUIComponent
) {
  constructor(options) {
    super(options)
    this.isText = true
    this.set(options)
  }

  /// ////////
  // UPDATES
  /// ////////

  /**
   * Here we compute each glyph dimension, and we store it in this
   * component's inlines parameter. This way the parent Block will
   * compute each glyph position on updateLayout.
   */
  parseParams() {
    if (this.rebuildText) {
      this.calculateInlines(this._fitFontSize || this.getFontSize())
    }
  }

  /**
   * Create text content
   *
   * At this point, text.inlines should have been modified by the parent
   * component, to add xOffset and yOffset properties to each inlines.
   * This way, TextContent knows were to position each character.
   */
  updateLayout() {
    this.position.z = this.getOffset()
    if (this.rebuildText) {
      deepDelete(this)
      if (this.inlines) {
      // happening in TextManager
        this.textContent = TextManager.createText(this)

        Object.assign(this.textContent.userData, this.userData)
        this.textContent.userData.isText = true

        this.updateTextMaterials()

        this.onTextBuilt?.(this.textContent)
        this.add(this.textContent)
      }
    }
  }

  updateInner() {
    this.position.z = this.getOffset()

    if (this.inlines && this.textContent && this.rebuildText) {
      this.updateTextMaterials()
      this.rebuildText = false
    }
  }

  calculateInlines(fontSize) {
    const {content} = this
    const font = this.getFontFamily()
    const breakChars = this.getBreakOn()
    const textType = this.getTextType()
    const whiteSpace = this.getWhiteSpace()

    // Abort conditions

    if (!font || typeof font === 'string') {
      // eslint-disable-next-line no-console
      if (!FontLibrary.getFontOf(this)) console.warn('no font was found')
      return
    }

    if (!this.content) {
      this.inlines = null
      return
    }

    for (const char of content) {
      if (!font._charset.has(char) && !breakChars.includes(char)) {
        this.inlines = null
        // eslint-disable-next-line no-console
        console.warn(`Character '${char}' of '${content}' is not in the font charset`)
        return
      }
    }

    if (!textType) {
      // eslint-disable-next-line no-console
      console.error(
        'You must provide a \'textType\' attribute so three-mesh-ui knows how to render your' +
        'text.\n See https://github.com/felixmariotto/three-mesh-ui/wiki/Using-a-custom-text-type'
      )
      return
    }

    // collapse whitespace for white-space normal
    const whitespaceProcessedContent = Whitespace.collapseWhitespaceOnString(content, whiteSpace)
    const chars =
      Array.from
        ? Array.from(whitespaceProcessedContent)
        : String(whitespaceProcessedContent).split('')

    // Compute glyphs sizes

    const fontScale = fontSize / font.info.size
    const lineHeight = font.common.lineHeight * fontScale
    const lineBase = font.common.base * fontScale

    const glyphInfos = chars.map((glyph) => {
      // Get height, width, and anchor point of this glyph
      const dimensions = TextManager.getGlyphDimensions({
        textType,
        glyph,
        font,
        fontSize,
      })

      //

      let lineBreak = null

      if (whiteSpace !== Whitespace.NOWRAP) {
        if (breakChars.includes(glyph) || glyph.match(/\s/g)) lineBreak = 'possible'
      }

      if (glyph.match(/\n/g)) {
        lineBreak = Whitespace.newlineBreakability(whiteSpace)
      }

      //

      return {
        height: dimensions.height,
        width: dimensions.width,
        anchor: dimensions.anchor,
        xadvance: dimensions.xadvance,
        xoffset: dimensions.xoffset,
        lineBreak,
        glyph,
        fontSize,
        lineHeight,
        lineBase,
      }
    })

    // apply kerning
    if (this.getFontKerning() !== 'none') {
      // First character won't be kerned with its void lefthanded peer
      for (let i = 1; i < glyphInfos.length; i++) {
        const glyphInfo = glyphInfos[i]
        const glyphPair = glyphInfos[i - 1].glyph + glyphInfos[i].glyph

        // retrieve the kerning from the font
        const kerning = TextManager.getGlyphPairKerning(textType, font, glyphPair)

        // compute the final kerning value according to requested fontSize
        glyphInfo.kerning = kerning * (fontSize / font.info.size)
      }
    }

    // Update 'inlines' property, so that the parent can compute each glyph position
    this.inlines = glyphInfos
  }
}
