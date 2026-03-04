// @ts-nocheck
import FontLibrary from './FontLibrary'
import UpdateManager from './UpdateManager'

import DEFAULTS from '../../utils/Defaults'
import {warnAboutDeprecatedAlignItems} from '../../utils/block-layout/AlignItems'
import * as Whitespace from '../../utils/inline-layout/Whitespace'
import {setLayersOnObject} from '../../utils/set-layers-on-object'

const {Plane, Vector3, Object3D} = window.THREE

type ComparableProp = boolean | number | string | Array | Object | undefined

// NOTE(johnny): This function is used to compare properties of a MeshUIComponent.
// booleans, strings, numbers, primitive arrays, color objects, and texture objects are accounted
// for.
const isPropNotEqual = (prop1: ComparableProp, prop2: ComparableProp): boolean => {
  if (prop1 === prop2) {
    return false
  }

  if (!prop1 || !prop2) {
    return prop1 !== prop2
  }

  if (Array.isArray(prop1)) {
    if (!Array.isArray(prop2)) {
      return true
    }
    return prop1.length !== prop2.length || prop1.some((val, index) => val !== prop2[index])
  }

  if (typeof prop1 === 'object') {
    if (typeof prop2 !== 'object') {
      return true
    }
    if (prop1.isColor && prop2.isColor) {
      return prop1.r !== prop2.r || prop1.g !== prop2.g || prop1.b !== prop2.b
    }
    if (prop1.isTexture && prop2.isTexture) {
      return prop1.uuid !== prop2.uuid
    }
  }

  return true
}

/**
Job:
- Set this component attributes and call updates accordingly
- Getting this component attribute, from itself or from its parents

This is the core module of three-mesh-ui. Every component is composed with it.
*/
export default function MeshUIComponent(Base) {
  // eslint-disable-next-line @typescript-eslint/no-shadow
  return class MeshUIComponent extends Base {
    borderRadiusTopLeft?: number

    borderRadiusTopRight?: number

    borderRadiusBottomRight?: number

    borderRadiusBottomLeft?: number

    constructor(options) {
      super(options)
      this.isUI = true
      this.autoLayout = true
      this.backgroundDepthWrite = options.backgroundDepthWrite
      this.foregroundDepthWrite = options.foregroundDepthWrite
      this.defines = options.defines || {}

      // children
      this.childrenUIs = []
      this.childrenBoxes = []
      this.childrenTexts = []
      this.childrenInlines = []
      this.userData = options.userData || {}

      // parents
      this.parentUI = null
      this.userData.rootUi = this
      // update parentUI when this component will be added or removed
      this.addEventListener('added', this._rebuildParentUI)
      this.addEventListener('removed', this._rebuildParentUI)
    }

    /// //////////
    /// GETTERS
    /// //////////

    getOpacity() {
      if (!this.parentUI) {
        return this.opacity ?? DEFAULTS.opacity
      }
      return this.parentUI.getOpacity() * (this.opacity ?? DEFAULTS.opacity)
    }

    getClippingPlanes() {
      const planes = []

      if (this.parentUI) {
        if (this.isBlock && this.parentUI.getHiddenOverflow()) {
          const yLimit = (this.parentUI.getHeight() / 2) - (this.parentUI.padding || 0)
          const xLimit = (this.parentUI.getWidth() / 2) - (this.parentUI.padding || 0)
          const newPlanes = [
            new Plane(new Vector3(0, 1, 0), yLimit),
            new Plane(new Vector3(0, -1, 0), yLimit),
            new Plane(new Vector3(1, 0, 0), xLimit),
            new Plane(new Vector3(-1, 0, 0), xLimit),
          ]
          newPlanes.forEach((plane) => {
            plane.applyMatrix4(this.parent.matrixWorld)
          })
          planes.push(...newPlanes)
        }

        if (this.parentUI.parentUI) {
          planes.push(...this.parentUI.getClippingPlanes())
        }
      }

      return planes
    }

    /** Get the highest parent of this component (the parent that has no parent on top of it) */
    getHighestParent() {
      if (!this.parentUI) {
        return this
      }

      return this.parent.getHighestParent()
    }

    /**
     * look for a property in this object, and if does not find it, find in
     * parents or return default value
     * @private
     */
    _getProperty(propName) {
      if (this[propName] === undefined && this.parentUI) {
        return this.parent._getProperty(propName)
      } else if (this[propName] !== undefined) {
        return this[propName]
      }

      return DEFAULTS[propName]
    }

    //

    getFontSize() {
      return this._getProperty('fontSize')
    }

    getFontKerning() {
      return this._getProperty('fontKerning')
    }

    getLetterSpacing() {
      return this._getProperty('letterSpacing')
    }

    getTextureBaseUrl() {
      return this._getProperty('textureBaseUrl')
    }

    getFontFamilyUrl() {
      return this._getProperty('fontFamilyUrl')
    }

    getFontFamily() {
      return this._getProperty('fontFamily')
    }

    getBreakOn() {
      return this._getProperty('breakOn')
    }

    getWhiteSpace() {
      return this._getProperty('whiteSpace')
    }

    getTextAlign() {
      return this._getProperty('textAlign')
    }

    getTextType() {
      return this._getProperty('textType')
    }

    getFontColor() {
      return this._getProperty('fontColor')
    }

    getFontSupersampling() {
      return this._getProperty('fontSupersampling')
    }

    getFontOpacity() {
      return this._getProperty('fontOpacity') * this.getOpacity()
    }

    getFontPXRange() {
      return this._getProperty('fontPXRange')
    }

    getActiveLayers() {
      return this._getProperty('activeLayers')
    }

    getBorderRadius() {
      return this._getProperty('borderRadius')
    }

    getBorderRadiusTopLeft() {
      return this.borderRadiusTopLeft ? this.borderRadiusTopLeft : this.borderRadius
    }

    getBorderRadiusTopRight() {
      return this.borderRadiusTopRight ? this.borderRadiusTopRight : this.borderRadius
    }

    getBorderRadiusBottomLeft() {
      return this.borderRadiusBottomLeft ? this.borderRadiusBottomLeft : this.borderRadius
    }

    getBorderRadiusBottomRight() {
      return this.borderRadiusBottomRight ? this.borderRadiusBottomRight : this.borderRadius
    }

    getBorderWidth() {
      return this._getProperty('borderWidth')
    }

    getBorderColor() {
      return this._getProperty('borderColor')
    }

    getBorderOpacity() {
      return this._getProperty('borderOpacity') * this.getOpacity()
    }

    getBackgroundDepthWrite() {
      return this._getProperty('backgroundDepthWrite')
    }

    getForegroundDepthWrite() {
      return this._getProperty('foregroundDepthWrite')
    }

    /// SPECIALS

    /** return the first parent with a 'threeOBJ' property */
    getContainer() {
      if (!this.threeOBJ && this.parent) {
        return this.parent.getContainer()
      } else if (this.threeOBJ) {
        return this
      }

      return DEFAULTS.container
    }

    /** Get the number of UI parents above this elements (0 if no parent) */
    getParentsNumber(num) {
      const i = num || 0

      if (this.parentUI) {
        return this.parentUI.getParentsNumber(i + 1)
      }

      return i
    }

    /// /////////////////////////////////
    /// GETTERS WITH NO PARENTS LOOKUP
    /// /////////////////////////////////

    getBackgroundOpacity() {
      const backgroundOpacity = (!this.backgroundOpacity && this.backgroundOpacity !== 0)
        ? DEFAULTS.backgroundOpacity
        : this.backgroundOpacity
      return backgroundOpacity * this.getOpacity()
    }

    getBackgroundColor() {
      return this.backgroundColor || DEFAULTS.backgroundColor
    }

    getBackgroundTexture() {
      return this.backgroundTexture || DEFAULTS.getDefaultTexture()
    }

    getHasBackgroundTexture() {
      return !!this.backgroundTexture
    }

    /**
     * @deprecated
     * @returns {string}
     */
    getAlignContent() {
      return this.alignContent || DEFAULTS.alignContent
    }

    getAlignItems() {
      return this.alignItems || DEFAULTS.alignItems
    }

    getContentDirection() {
      return this.contentDirection || DEFAULTS.contentDirection
    }

    getJustifyContent() {
      return this.justifyContent || DEFAULTS.justifyContent
    }

    getInterLine() {
      return (this.interLine === undefined) ? DEFAULTS.interLine : this.interLine
    }

    getOffset() {
      return (this.offset === undefined) ? DEFAULTS.offset : this.offset
    }

    getBackgroundSize() {
      return (this.backgroundSize === undefined) ? DEFAULTS.backgroundSize : this.backgroundSize
    }

    getHiddenOverflow() {
      return (this.hiddenOverflow === undefined) ? DEFAULTS.hiddenOverflow : this.hiddenOverflow
    }

    getBestFit() {
      return (this.bestFit === undefined) ? DEFAULTS.bestFit : this.bestFit
    }

    getNineSliceBorderTop() {
      return this.nineSliceBorderTop || DEFAULTS.nineSliceBorderTop
    }

    getNineSliceBorderBottom() {
      return this.nineSliceBorderBottom || DEFAULTS.nineSliceBorderBottom
    }

    getNineSliceBorderLeft() {
      return this.nineSliceBorderLeft || DEFAULTS.nineSliceBorderLeft
    }

    getNineSliceBorderRight() {
      return this.nineSliceBorderRight || DEFAULTS.nineSliceBorderRight
    }

    getNineSliceScaleFactor() {
      return this.nineSliceScaleFactor || DEFAULTS.nineSliceScaleFactor
    }

    getUIScale() {
      return this.uiScale || DEFAULTS.uiScale
    }

    getDefines() {
      return this.defines || DEFAULTS.defines
    }

    /// ////////////
    ///  UPDATE
    /// ////////////

    /**
     * Filters children in order to compute only one times children lists
     * @private
     */
    _rebuildChildrenLists() {
      // Stores all children that are ui
      this.childrenUIs = this.children.filter(child => child.isUI)

      // Stores all children that are box
      this.childrenBoxes = this.children.filter(child => child.isBoxComponent)

      // Stores all children that are inline
      this.childrenInlines = this.children.filter(child => child.isInline)

      // Stores all children that are text
      this.childrenTexts = this.children.filter(child => child.isText)

      // Stores all children that are not distinct UI elements
      this.childrenComponents = this.children.filter(child => !child.isUI)
    }

    /**
     * Try to retrieve parentUI after each structural change
     * @private
     */
    _rebuildParentUI = () => {
      if (this.parent && this.parent.isUI) {
        this.parentUI = this.parent
        this.traverse((child) => {
          child.userData.rootUi = this.parent.userData.rootUi
        })
      } else {
        this.parentUI = null
        this.traverse((child) => {
          child.userData.rootUi = this
        })
      }
    };

    /**
     * When the user calls component.add, it registers for updates,
     * then call THREE.Object3D.add.
     */
    add(...args) {
      // NOTE(johnny): Need to rebuild the children lists before calling
      // _updateChildren() for text and alignment.
      const result = super.add(...args)
      this._rebuildChildrenLists()

      // eslint-disable-next-line no-restricted-syntax
      for (const id of Object.keys(args)) {
        // An inline component relies on its parent for positioning
        if (args[id].isInline) this.update(null, true)
        if (args[id].isText) {
          this._updateChildren()
        }
      }

      return result
    }

    /**
     * When the user calls component.remove, it registers for updates,
     * then call THREE.Object3D.remove.
     */
    remove(...args) {
      // eslint-disable-next-line no-restricted-syntax
      for (const id of Object.keys(args)) {
        // An inline component relies on its parent for positioning
        if (args[id].isInline) this.update(null, true)
      }

      const result = super.remove(...args)
      this._rebuildChildrenLists()

      return result
    }

    //

    update(updateParsing, updateLayout, updateInner, rebuildText) {
      UpdateManager.requestUpdate(this, updateParsing, updateLayout, updateInner, rebuildText)
    }

    // eslint-disable-next-line class-methods-use-this
    onAfterUpdate() {

    }

    _updateChildren() {
      this.traverse((child) => {
        // if this component/its children are Text, update their font texture urls
        if (child.isText) child._updateFontTextureUrls()
        if (child.isUI) child.update(true, true, false, true)
      })
      this.getHighestParent().update(false, true, false, true)
    }

    _updateLayers() {
      const layers = this.getActiveLayers()
      setLayersOnObject(this, layers)

      const updateChildLayers = (obj: typeof Object3D) => {
        obj.children?.forEach((child) => {
          if (!child.activeLayers) {
            setLayersOnObject(child, layers)
            updateChildLayers(child)
          }
        })
      }

      updateChildLayers(this)
    }

    /**
     * Called by FontLibrary when the font requested for the current component is ready.
     * Trigger an update for the component whose font is now available.
     * @private - "package protected"
     */
    _updateFontFamily(font) {
      // set font family data to the component
      this.fontFamily = font

      this._updateChildren()
    }

    _updateFontTextureUrls() {
      const font = this.getFontFamily()
      const baseUrl = this.getTextureBaseUrl()
      if (!font || !this.content) {
        return
      }
      const breakChars = this.getBreakOn()
      // NOTE(johnny): This is a temporary fix to get Solstice unstuck. Ideally we should
      // create a fallback blank glyph when generating the content.
      this.content = this.unfilteredContent.split('').reduce(
        (acc, char) => {
          const isBreakChar = breakChars.includes(char)
          return acc + (font._chars[char] || isBreakChar ? char : '[]')
        }, ''
      )

      const fontFamilyUrl = this.getFontFamilyUrl()
      if (!FontLibrary.isFontJsonLoaded(fontFamilyUrl)) {
        return
      }

      const whitespaceProcessedContent =
        Whitespace.collapseWhitespaceOnString(this.content, this.getWhiteSpace())

      if (font.pages.length === 1) {
        // if there is only one page of text in the font, we can use the same texture for all
        FontLibrary.setFontTextures(this, new Set([this.getTextureBaseUrl()]))
        return
      }

      const chars =
        Array.from
          ? Array.from(whitespaceProcessedContent)
          : String(whitespaceProcessedContent).split('')

      const fontTextureUrls = new Set()

      for (const glyph of chars) {
        if (glyph === '\n') {
          // eslint-disable-next-line no-continue
          continue
        }
        if (!font._chars[glyph]) {
          // eslint-disable-next-line no-console
          console.warn(`Character '${glyph}' of '${this.content}' is not in the font charset`)
          break
        }

        const pageIdx = font._chars[glyph].page
        const {pages} = font
        const url = FontLibrary.getPageUrl(baseUrl, pages[pageIdx])
        fontTextureUrls.add(url)
      }
      FontLibrary.setFontTextures(this, fontTextureUrls)
    }

    /** @private - "package protected" */
    _updateFontTextures() {
      this.getHighestParent().update(false, true, false, true)
    }

    /**
     * Set this component's passed parameters.
     * If necessary, take special actions.
     * Update this component unless otherwise specified.
     */
    set(options) {
      let parsingNeedsUpdate, layoutNeedsUpdate, innerNeedsUpdate, rebuildText

      // Register to the update manager, so that it knows when to update

      UpdateManager.register(this)

      // Abort if no option passed

      if (!options || Object.keys(options).length === 0) return

      // DEPRECATION Warnings until
      // -------------------------------------- 7.x.x ---------------------------------------

      // Align content has been removed
      if (options.alignContent) {
        options.alignItems = options.alignContent

        if (!options.textAlign) {
          options.textAlign = options.alignContent
        }

        // eslint-disable-next-line no-console
        console.warn(
          '`alignContent` property has been deprecated, please rely on ' +
          '`alignItems` and `textAlign` instead.'
        )

        delete options.alignContent
      }

      // Align items left top bottom right will be removed
      if (options.alignItems) {
        warnAboutDeprecatedAlignItems(options.alignItems)
      }

      if (!options.borderRadiusTopLeft) {
        delete this.borderRadiusTopLeft
      }

      if (!options.borderRadiusTopRight) {
        delete this.borderRadiusTopRight
      }

      if (!options.borderRadiusBottomRight) {
        delete this.borderRadiusBottomRight
      }

      if (!options.borderRadiusBottomLeft) {
        delete this.borderRadiusBottomLeft
      }

      // Set this component parameters according to options, and trigger updates accordingly
      // The benefit of having two types of updates, is to put everything that takes time
      // in one batch, and the rest in the other. This way, efficient animation is possible with
      // attribute from the light batch.

      // eslint-disable-next-line no-restricted-syntax
      for (const prop of Object.keys(options)) {
        if (prop !== 'fontFamily' && prop !== 'fontTexture' &&
          isPropNotEqual(this[prop], options[prop])) {
          // eslint-disable-next-line default-case
          switch (prop) {
            case 'content':
              this.unfilteredContent = options[prop]
              this[prop] = options[prop]
              this._updateFontTextureUrls()
              this._updateChildren()
              rebuildText = true
              break

            case 'fontSize':
            case 'fontKerning':
            case 'breakOn':
            case 'whiteSpace':
              this[prop] = options[prop]
              if (this.isText) {
                parsingNeedsUpdate = true
                this._updateFontTextureUrls()
              }
              layoutNeedsUpdate = true
              rebuildText = true
              break

            case 'bestFit':
              if (this.isBlock) {
                parsingNeedsUpdate = true
                layoutNeedsUpdate = true
                rebuildText = true
              }
              this[prop] = options[prop]
              break

            case 'width':
            case 'height':
            case 'padding':
              if (this.isInlineBlock || (this.isBlock && this.getBestFit() !== 'none')) {
                parsingNeedsUpdate = true
              }
              layoutNeedsUpdate = true
              rebuildText = true
              this[prop] = options[prop]
              break

            case 'letterSpacing':
            case 'interLine':
              if (this.isBlock && this.getBestFit() !== 'none') parsingNeedsUpdate = true
              layoutNeedsUpdate = true
              rebuildText = true
              this[prop] = options[prop]
              break

            case 'margin':
            case 'contentDirection':
            case 'alignContent':
            case 'alignItems':
              layoutNeedsUpdate = true
              this[prop] = options[prop]
              break

            case 'justifyContent':
            case 'textAlign':
            case 'textType':
              layoutNeedsUpdate = true
              rebuildText = true
              this[prop] = options[prop]
              break

            case 'backgroundColor':
            case 'backgroundOpacity':
            case 'backgroundTexture':
            case 'backgroundSize':
            case 'backgroundDepthWrite':
            case 'borderRadius':
            case 'borderRadiusTopLeft':
            case 'borderRadiusTopRight':
            case 'borderRadiusBottomLeft':
            case 'borderRadiusBottomRight':
            case 'borderWidth':
            case 'borderColor':
            case 'borderOpacity':
            case 'defines':
            case 'foregroundDepthWrite':
            case 'nineSliceBorderTop':
            case 'nineSliceBorderBottom':
            case 'nineSliceBorderLeft':
            case 'nineSliceBorderRight':
            case 'nineSliceScaleFactor':
              innerNeedsUpdate = true
              this[prop] = options[prop]
              break

            case 'fontColor':
            case 'fontOpacity':
            case 'fontSupersampling':
            case 'offset':
            case 'opacity':
            case 'uiScale':
              rebuildText = true
              innerNeedsUpdate = true
              this[prop] = options[prop]
              break

            case 'hiddenOverflow':
              this[prop] = options[prop]
              break
            case 'activeLayers':
              this[prop] = options[prop]
              this._updateLayers()
              break
            case 'userData':
              Object.assign(this.userData, options[prop])
              this.childrenComponents?.forEach((child) => {
                Object.assign(child.userData, options[prop])
              })
              break
            default:
              this[prop] = options[prop]
          }
        }
      }

      // special cases, this.update() must be called only when some files finished loading

      // TODO (tri) font texture parameter should be able to be a set of urls (for multi-page fonts)
      // we will need to add a check here for font family data to ensure there are matching pages
      if (options.fontTexture && this.textureBaseUrl !== options.fontTexture) {
        rebuildText = true
        innerNeedsUpdate = true
        this.textureBaseUrl = options.fontTexture
      }

      if (options.fontFamily && this.fontFamilyUrl !== options.fontFamily) {
        rebuildText = true
        innerNeedsUpdate = true
        this.fontFamilyUrl = options.fontFamily
        FontLibrary.setFontFamily(this, options.fontFamily)
      }

      // if font kerning changes for a child of a block with Best Fit enabled,
      // we need to trigger parsing for the parent as well.
      if (this.parentUI && this.parentUI.getBestFit() !== 'none') {
        this.parentUI.update(true, true, false, rebuildText)
      }

      // Call component update

      this.update(parsingNeedsUpdate, layoutNeedsUpdate, innerNeedsUpdate, rebuildText)
      if (layoutNeedsUpdate) this.getHighestParent().update(false, true, false, rebuildText)
    }

    /** Get completely rid of this component and its children, also unregister it for updates */
    // NOTE(johnny): We will dispose of the material and geometry externally.
    clear() {
      this.traverse((obj) => {
        UpdateManager.disposeOf(obj)
        FontLibrary.removeComponent(obj)
      })
    }
  }
}
