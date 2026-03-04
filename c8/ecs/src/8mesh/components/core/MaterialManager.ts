import Defaults from '../../utils/Defaults'
import {backgroundFragment} from '../shaders/background-fragment'
import {backgroundVertex} from '../shaders/background-vertex'
import {textFragment} from '../shaders/text-fragment'
import {textVertex} from '../shaders/text-vertex'
import FontLibrary from './FontLibrary'
import type {Color, Material, Plane, Texture, Vector2 as Vec2} from '../../utils/three-types'
import * as BackgroundSize from '../../utils/background-size'

const clippingPlanesAreEqual = (oldValue: Array<Plane> | null, newValue: Array<Plane> | null) => {
  if (!oldValue && !newValue) {
    return true
  } else if (!oldValue || !newValue) {
    return false
  } else if (oldValue.length !== newValue.length) {
    return false
  }
  return newValue.every((plane, idx) => {
    const {normal, constant} = oldValue[idx]
    const {x: oldX, y: oldY, z: oldZ} = normal
    const {x: newX, y: newY, z: newZ} = plane.normal

    return oldX === newX && oldY === newY && oldZ === newZ && plane.constant === constant
  })
}

// @ts-ignore
const {ShaderMaterial, Vector2} = window.THREE
// NOTE (tri) switch this to true to have named shaders
const DEBUG_SHADERS = false

/**

Job:
- Host the materials of a given component.
- Update a component's materials clipping planes.
- Update a material uniforms and such.

Knows:
- Its component materials.
- Its component ancestors clipping planes.

 */
type Constructor<T = {}> = new (...args: any[]) => T;

// NOTE(christoph): These are provided by by mix.withBase(Object3D)(...other mixins)
// TODO(christoph): Switch to composing this interface from each mixin interface
type BaseInstance = {
  id: string
  size: Vec2
  backgroundColor: number
  backgroundOpacity: number
  getBackgroundTexture: () => Texture
  getHasBackgroundTexture: () => boolean
  getBackgroundColor: () => Color
  getBackgroundOpacity: () => number
  getBackgroundSize: () => BackgroundSize.BackgroundSize
  getBorderWidth: () => number
  getBorderColor: () => Color
  getBorderRadius: () => number
  getBorderOpacity: () => number
  getNineSliceBorderTop: () => number
  getNineSliceBorderBottom: () => number
  getNineSliceBorderLeft: () => number
  getNineSliceBorderRight: () => number
  getNineSliceScaleFactor: () => number
  getUIScale: () => number
  getFontColor: () => Color
  getFontOpacity: () => number
  getFontPXRange: () => number
  getFontSupersampling: () => boolean
  getBackgroundDepthWrite: () => boolean
  getForegroundDepthWrite: () => boolean
  getClippingPlanes: () => Array<Plane> | null
  getBorderRadiusBottomLeft: () => number
  getBorderRadiusBottomRight: () => number
  getBorderRadiusTopLeft: () => number
  getBorderRadiusTopRight: () => number
  getDefines: () => Object
  clippingPlanes: Array<Plane> | null
  backgroundMaterial: Material | undefined
}

function MaterialManager<T extends Constructor<BaseInstance>>(Base: T) {
  // eslint-disable-next-line @typescript-eslint/no-shadow
  return class MaterialManager extends Base {
    backgroundUniforms: {
      uTexture: { value: Texture }
      uHasTexture: { value: boolean }
      uColor: { value: Color }
      uOpacity: { value: number }
      uBackgroundMapping: { value: number }
      uBorderWidth: { value: number }
      uBorderColor: { value: Color }
      uBorderRadiusTopLeft: { value: number }
      uBorderRadiusTopRight: { value: number }
      uBorderRadiusBottomRight: { value: number }
      uBorderRadiusBottomLeft: { value: number }
      uBorderOpacity: { value: number }
      uSize: { value: Vec2 }
      uTexSize: { value: Vec2 }
      uNineSliceBorderTop: {value: number}
      uNineSliceBorderBottom: {value: number}
      uNineSliceBorderLeft: {value: number}
      uNineSliceBorderRight: {value: number}
      uNineSliceScaleFactor: {value: number}
      uUiScale: {value: number}
    }

    // Map of font materials from font urls
    fontMaterials?: Record<string, Material>

    // Map of material uniforms from font urls
    textUniforms?: Record<string, {
      uTexture: { value: Texture }
      uColor: { value: Color }
      uOpacity: { value: number }
      uPxRange: { value: number }
      uUseRGSS: { value: boolean }
    }>

    defines?: Object

    constructor(...args: any[]) {
      super(...args)
      this.backgroundUniforms = {
        uTexture: {value: this.getBackgroundTexture()},
        uHasTexture: {value: this.getHasBackgroundTexture()},
        uColor: {value: this.getBackgroundColor()},
        uOpacity: {value: this.getBackgroundOpacity()},
        uBackgroundMapping: {
          value: BackgroundSize.convertBackgroundSizeToUniform(this.getBackgroundSize()),
        },
        uBorderWidth: {value: this.getBorderWidth()},
        uBorderColor: {value: this.getBorderColor()},
        uBorderRadiusTopLeft: {value: this.getBorderRadius()},
        uBorderRadiusTopRight: {value: this.getBorderRadius()},
        uBorderRadiusBottomRight: {value: this.getBorderRadius()},
        uBorderRadiusBottomLeft: {value: this.getBorderRadius()},
        uBorderOpacity: {value: this.getBorderOpacity()},
        uSize: {value: new Vector2(1, 1)},
        uTexSize: {value: new Vector2(1, 1)},
        uNineSliceBorderTop: {value: this.getNineSliceBorderTop()},
        uNineSliceBorderBottom: {value: this.getNineSliceBorderBottom()},
        uNineSliceBorderLeft: {value: this.getNineSliceBorderLeft()},
        uNineSliceBorderRight: {value: this.getNineSliceBorderRight()},
        uNineSliceScaleFactor: {value: this.getNineSliceScaleFactor()},
        uUiScale: {value: this.getUIScale()},
      }
    }

    /**
     * Update backgroundMaterial uniforms.
     * Used within MaterialManager and in Block and InlineBlock innerUpdates.
     */
    updateBackgroundMaterial() {
      this.backgroundUniforms.uTexture.value = this.getBackgroundTexture()
      this.backgroundUniforms.uHasTexture.value = this.getHasBackgroundTexture()

      const texWidth = this.backgroundUniforms.uTexture.value.image instanceof HTMLVideoElement
        ? this.backgroundUniforms.uTexture.value.image.videoWidth
        : this.backgroundUniforms.uTexture.value.image?.width ?? 1

      const texHeight = this.backgroundUniforms.uTexture.value.image instanceof HTMLVideoElement
        ? this.backgroundUniforms.uTexture.value.image.videoHeight
        : this.backgroundUniforms.uTexture.value.image?.height ?? 1

      this.backgroundUniforms.uTexSize.value.set(texWidth, texHeight)

      if (this.size) this.backgroundUniforms.uSize.value.copy(this.size)

      if (!this.backgroundUniforms.uHasTexture.value) {
        this.backgroundUniforms.uColor.value = this.getBackgroundColor()

        this.backgroundUniforms.uOpacity.value = this.getBackgroundOpacity()
      } else {
        this.backgroundUniforms.uColor.value =
          this.backgroundColor || Defaults.backgroundWhiteColor
        this.backgroundUniforms.uOpacity.value = this.getBackgroundOpacity()
      }

      this.backgroundUniforms.uBackgroundMapping.value =
        BackgroundSize.convertBackgroundSizeToUniform(this.getBackgroundSize())

      this.backgroundUniforms.uNineSliceBorderTop.value = this.getNineSliceBorderTop()
      this.backgroundUniforms.uNineSliceBorderBottom.value = this.getNineSliceBorderBottom()
      this.backgroundUniforms.uNineSliceBorderLeft.value = this.getNineSliceBorderLeft()
      this.backgroundUniforms.uNineSliceBorderRight.value = this.getNineSliceBorderRight()

      this.backgroundUniforms.uNineSliceScaleFactor.value = this.getNineSliceScaleFactor()

      this.backgroundUniforms.uUiScale.value = this.getUIScale()

      this.backgroundUniforms.uBorderWidth.value = this.getBorderWidth()
      this.backgroundUniforms.uBorderColor.value = this.getBorderColor()
      this.backgroundUniforms.uBorderOpacity.value = this.getBorderOpacity()

      this.backgroundUniforms.uBorderRadiusTopLeft.value = this.getBorderRadiusTopLeft()
      this.backgroundUniforms.uBorderRadiusTopRight.value = this.getBorderRadiusTopRight()
      this.backgroundUniforms.uBorderRadiusBottomRight.value = this.getBorderRadiusBottomRight()
      this.backgroundUniforms.uBorderRadiusBottomLeft.value = this.getBorderRadiusBottomLeft()

      if (this.backgroundMaterial && this.backgroundMaterial.defines !== this.getDefines()) {
        this.backgroundMaterial.defines = this.getDefines()
        this.backgroundMaterial.needsUpdate = true
      }
    }

    /**
     * Update fontMaterials uniforms.
     * Used within MaterialManager and in Text innerUpdates.
     */
    updateTextMaterials() {
      // Remake the materials if the number of textures changed
      // This is necessary for some applications that recycle meshes and pass new parameters
      const urls = FontLibrary.getFontTextureUrls(this)
      const textUniformsKeys = Object.keys(this.textUniforms ?? {})
      const {textUniforms} = this
      if (
        textUniforms === undefined ||
        urls.size !== textUniformsKeys.length ||
        textUniformsKeys.some(url => !urls.has(url))
      ) {
        this.fontMaterials = this._makeTextMaterials()
      } else {
        urls.forEach((url: string) => {
          textUniforms[url].uTexture.value = FontLibrary.getFontTextureFromUrl(url)
          textUniforms[url].uColor.value = this.getFontColor()
          textUniforms[url].uOpacity.value = this.getFontOpacity()
          textUniforms[url].uPxRange.value = this.getFontPXRange()
          textUniforms[url].uUseRGSS.value = this.getFontSupersampling()
        })
      }
    }

    /** Called by Block, which needs the background material to create a mesh */
    getBackgroundMaterial() {
      if (!this.backgroundMaterial || !this.backgroundUniforms) {
        this.backgroundMaterial = this._makeBackgroundMaterial()
      }

      return this.backgroundMaterial
    }

    /** Called by Text to get the font material */
    getFontMaterials() {
      const urls = FontLibrary.getFontTextureUrls(this)
      const textUniformsKeys = Object.keys(this.textUniforms ?? {})
      if (
        !this.fontMaterials || !this.textUniforms || urls.size !== textUniformsKeys.length ||
        textUniformsKeys.some(url => !urls.has(url))
      ) {
        this.fontMaterials = this._makeTextMaterials()
      }

      return this.fontMaterials
    }

    /** @private */
    _makeTextMaterials() {
      const fontTextureUrls = Array.from(FontLibrary.getFontTextureUrls(this))
      this.textUniforms = fontTextureUrls.reduce((acc, url) => {
        acc[url] = {
          uTexture: {value: FontLibrary.getFontTextureFromUrl(url)},
          uColor: {value: this.getFontColor()},
          uOpacity: {value: this.getFontOpacity()},
          uPxRange: {value: this.getFontPXRange()},
          uUseRGSS: {value: this.getFontSupersampling()},
        }
        return acc
      }, {} as Record<string, any>)
      return fontTextureUrls.reduce((acc, url) => {
        acc[url] = new ShaderMaterial({
          uniforms: this.textUniforms![url],
          ...(DEBUG_SHADERS ? {name: url} : {}),
          transparent: true,
          clipping: true,
          vertexShader: textVertex,
          fragmentShader: textFragment,
          extensions: {
            derivatives: true,
          },
          depthWrite: this.getForegroundDepthWrite(),
        })
        return acc
      }, {} as Record<string, Material>)
    }

    /** @private */
    _makeBackgroundMaterial() {
      return new ShaderMaterial({
        uniforms: this.backgroundUniforms,
        transparent: true,
        clipping: true,
        vertexShader: backgroundVertex,
        fragmentShader: backgroundFragment,
        extensions: {
          derivatives: true,
        },
        // Turning off z-buffer write prevents z-fighting with objects rendered before this
        // This is sometimes necessary because the z-buffer doesn't work as expected, maybe because
        // the material is transparent
        depthWrite: this.getBackgroundDepthWrite(),
        defines: this.getDefines(),
      })
    }

    /**
     * Update a component's materials clipping planes.
     * Called every frame.
     */
    updateClippingPlanes(value: Array<Plane> | undefined) {
      const newClippingPlanes = value !== undefined ? value : this.getClippingPlanes()

      if (!clippingPlanesAreEqual(newClippingPlanes, this.clippingPlanes)) {
        this.clippingPlanes = newClippingPlanes

        if (this.fontMaterials) {
          Object.values(this.fontMaterials).forEach((material: Material) => {
            material.clippingPlanes = this.clippingPlanes
          })
        }

        if (this.backgroundMaterial) this.backgroundMaterial.clippingPlanes = this.clippingPlanes
      }
    }
  }
}

export default MaterialManager
