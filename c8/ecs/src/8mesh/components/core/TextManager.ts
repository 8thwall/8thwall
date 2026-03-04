// @ts-nocheck
import * as MSDFText from '../../content/MSDFText'
import {setLayersOnObject} from '../../utils/set-layers-on-object'

/**

Job:
- Routing the request for Text dimensions and Text creation depending on Text type.

Knows:
- this component's textType attribute

Note:
Only one Text type is natively supported by the library at the moment,
but the architecture allows you to easily stick in your custom Text type.
More information here :
https://github.com/felixmariotto/three-mesh-ui/wiki/Using-a-custom-text-type

 */
const createText = (component) => {
  // eslint-disable-next-line consistent-return
  const mesh = (() => {
    switch (component.getTextType()) {
      case 'MSDF':
        return MSDFText.buildText(
          component.inlines,
          component.getFontFamily(),
          component.getFontMaterials(),
          component.getTextureBaseUrl()
        )
      default:
        // eslint-disable-next-line no-console
        console.warn(
          `'${component.getTextType()}' is not a supported text type.\n` +
          'See https://github.com/felixmariotto/three-mesh-ui/wiki/Using-a-custom-text-type'
        )
        break
    }
  })()

  mesh.renderOrder = component.overrideRenderOrder ?? Infinity
  setLayersOnObject(mesh, component.getActiveLayers())

  // This is for hiddenOverflow to work
  mesh.onBeforeRender = () => {
    if (component.updateClippingPlanes) {
      component.updateClippingPlanes()
    }
  }
  return mesh
}

/**
 * Called by Text to get the dimensions of a particular glyph,
 * in order for InlineManager to compute its position
 */
// eslint-disable-next-line consistent-return, class-methods-use-this
const getGlyphDimensions = (options) => {
  switch (options.textType) {
    case 'MSDF':

      return MSDFText.getGlyphDimensions(options)

    default:
      // eslint-disable-next-line no-console
      console.warn(
        `'${options.textType}' is not a supported text type.\n` +
            'See https://github.com/felixmariotto/three-mesh-ui/wiki/Using-a-custom-text-type'
      )
      break
  }
}

/**
 * Called by Text to get the amount of kerning for pair of glyph
 * @param textType
 * @param font
 * @param glyphPair
 * @returns {number}
 */
// eslint-disable-next-line consistent-return, class-methods-use-this
const getGlyphPairKerning = (textType, font, glyphPair) => {
  switch (textType) {
    case 'MSDF':

      return MSDFText.getGlyphPairKerning(font, glyphPair)

    default:
      // eslint-disable-next-line no-console
      console.warn(
        `'${textType}' is not a supported text type.\n` +
            'See https://github.com/felixmariotto/three-mesh-ui/wiki/Using-a-custom-text-type'
      )
      break
  }
}

export {
  createText,
  getGlyphDimensions,
  getGlyphPairKerning,
}
