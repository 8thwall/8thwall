/*

Job:
Keeping record of all the loaded fonts, which component use which font,
and load new fonts if necessary

Knows: Which component use which font, loaded fonts

This is one of the only modules in the 'component' folder that is not used
for composition (Object.assign). MeshUIComponent is the only module with
a reference to it, it uses FontLibrary for recording fonts accross components.
This way, if a component uses the same font as another, FontLibrary will skip
loading it twice, even if the two component are not in the same parent/child hierarchy

*/

import DEFAULTS from '../../utils/Defaults'
import type {Texture} from '../../utils/three-types'
import {
  isProcessedFontSetData, type FontSetData, type RawFontSetData, type CharData,
} from '../../../shared/msdf-font-type'

// @ts-ignore
const {FileLoader, TextureLoader, LinearFilter} = window.THREE

// TODO (Tri) Remove this Component type when we have full MeshUIComponent type
type BaseComponent = {
  id: string
  parentUI?: Component
}

// TODO (Tri) Remove this Component type when we have full MeshUIComponent type
type Component = BaseComponent & {
  _updateFontFamily: (font: FontSetData) => void
  _updateFontTextures: () => void
}

type ComponentRecord = {
  component: Component
  textureURLs: Set<string>
  jsonURL?: string
}

const DEFAULT_TEXTURE_URL = ''

const fileLoader = new FileLoader()
const requiredFontFamilies = new Set()
const fontFamilies: Record<string, FontSetData> = {}

const textureLoader = new TextureLoader()
const requiredFontTextures = new Set()
const fontTextures: Record<string, Texture> = {}

const records: Record<string, ComponentRecord> = {}

function updateComponentFontTextures(url: string): void {
  // eslint-disable-next-line no-restricted-syntax
  for (const recordID of Object.keys(records)) {
    if (records[recordID].textureURLs && records[recordID].textureURLs.has(url)) {
      // update all the components that were waiting for this font for an update
      records[recordID].component._updateFontTextures()
    }
  }
}

function loadNewFontTexture(url: string): void {
  requiredFontTextures.add(url)
  textureLoader.load(url, (texture: Texture) => {
    texture.generateMipmaps = false
    texture.minFilter = LinearFilter
    texture.magFilter = LinearFilter

    fontTextures[url] = texture

    updateComponentFontTextures(url)
  }, undefined, () => {
    // eslint-disable-next-line no-console
    console.error(`Failed to load font texture: ${url}`)
  })
}

/**

Called by MeshUIComponent after fontTexture was set
When done, it calls MeshUIComponent.update, to actually display
the text with the loaded font.

 */
function setFontTexture(component: Component, url: string) {
  // if this font was never asked for, we load it
  if (!requiredFontTextures.has(url)) {
    loadNewFontTexture(url)
  }

  // keep record of the font that this component use
  if (!records[component.id]) {
    records[component.id] = {component, textureURLs: new Set()}
  }
  records[component.id].textureURLs.add(url)

  // update the component, only if the font is already requested and loaded
  if (fontTextures[url]) {
    component._updateFontTextures()
  }
}

function setFontTextures(component: Component, urls: Set<string>) {
  // if this font was never asked for, we load it
  for (const url of urls) {
    if (!requiredFontTextures.has(url)) {
      loadNewFontTexture(url)
    }
  }

  if (!records[component.id]) {
    records[component.id] = {component, textureURLs: urls}
  } else {
    records[component.id].textureURLs = urls
  }

  component._updateFontTextures()
}

function getFontTextureFromUrl(url: string): Texture {
  if (url === DEFAULT_TEXTURE_URL) {
    return DEFAULTS.getDefaultTexture()
  }
  if (!requiredFontTextures.has(url)) {
    loadNewFontTexture(url)
  }
  return fontTextures[url]
}

function getFontTexturesFromUrls(urls: Set<string>): Map<string, Texture> {
  const textures = new Map<string, Texture>()
  for (const url of urls) {
    textures.set(url, getFontTextureFromUrl(url))
  }
  return textures
}

/** used by Text to know if a warning must be thrown */
function getFontOf(component: BaseComponent) {
  const record = records[component.id]
  if (!record && component.parentUI) {
    return getFontOf(component.parentUI)
  }

  return record
}

/**
 * From the original json font kernings array
 * First  : Reduce the number of values by ignoring any kerning defining an amount of 0
 * Second : Update the data structure of kernings from
 *       {Array} : [{first: 97, second: 121, amount: 0},{first: 97, second: 122, amount: -1},...]
 *       to
 *       {Object}: {"ij":-2,"WA":-3,...}}
 *
 * @private
 */
function _buildFriendlyKerningValues(
  kernings: RawFontSetData['kernings']
): FontSetData['_kernings'] {
  return kernings.reduce((acc, kerning) => {
    // ignore zero kerned glyph pair
    if (kerning.amount === 0) {
      return acc
    }
    // Build and store the glyph paired characters "ij","WA", ... as keys, referencing their kerning
    // amount
    const glyphPair = String.fromCharCode(kerning.first, kerning.second)
    acc[glyphPair] = kerning.amount
    return acc
  }, {} as FontSetData['_kernings'])
}

function _maybeProcessFontSetData(font: FontSetData | RawFontSetData): FontSetData {
  // As "font registering" can comes from different paths : addFont, loadFontJSON, setFontFamily
  // Be sure we don't repeat this operation
  if (isProcessedFontSetData(font)) {
    return font
  }

  // update the font to keep it
  return {
    ...font,
    _kernings: _buildFriendlyKerningValues(font.kernings),
    _chars: font.chars.reduce((acc, obj) => {
      acc[obj.char] = obj
      return acc
    }, {} as Record<string, CharData>),
    _charset: new Set(font.info.charset),
  }
}

/*

This method is intended for adding manually loaded fonts. Method assumes font
hasn't been loaded or requested yet. If it was, font with specified name will be
overwritten, but components using it won't be updated.

*/
function addFont(name: string, json: FontSetData, texture: Texture) {
  texture.generateMipmaps = false
  texture.minFilter = LinearFilter
  texture.magFilter = LinearFilter

  requiredFontFamilies.add(name)

  // Ensure the font json is processed
  fontFamilies[name] = _maybeProcessFontSetData(json)
  if (texture) {
    requiredFontTextures.add(name)
    fontTextures[name] = texture
  }
}

function updateComponentFontFamilies(url: string, font: FontSetData) {
  // eslint-disable-next-line no-restricted-syntax
  for (const recordID of Object.keys(records)) {
    if (url === records[recordID].jsonURL) {
      // update all the components that were waiting for this font for an update
      records[recordID].component._updateFontFamily(font)
    }
  }
}

function loadNewFontJSON(url: string) {
  requiredFontFamilies.add(url)
  fileLoader.load(url, (text: ArrayBuffer | string) => {
    const textStr = typeof text !== 'string' ? new TextDecoder().decode(text) : text
    // FileLoader import as  a JSON string
    const font = JSON.parse(textStr) as FontSetData

    // Ensure the font json is processed
    const processedFont = _maybeProcessFontSetData(font)
    fontFamilies[url] = processedFont

    updateComponentFontFamilies(url, processedFont)
  })
}

const maybeLoadFontJSON = (url: string) => {
  if (!requiredFontFamilies.has(url)) {
    loadNewFontJSON(url)
  } else {
    updateComponentFontFamilies(url, fontFamilies[url])
  }
}

/**

Called by MeshUIComponent after fontFamily was set
When done, it calls MeshUIComponent.update, to actually display
the text with the loaded font.

 */
const setFontFamily = (component: Component, fontFamily: string | FontSetData) => {
  // keep record of the font that this component use
  if (!records[component.id]) {
    records[component.id] = {component, textureURLs: new Set()}
  }

  if (typeof fontFamily === 'string') {
    records[component.id].jsonURL = fontFamily
    maybeLoadFontJSON(fontFamily)
  } else {
    const fontId = fontFamily.info.face
    // Ensure the font json is processed
    const processedFont = _maybeProcessFontSetData(fontFamily)
    records[component.id].jsonURL = fontId
    fontFamilies[fontId] = processedFont
    updateComponentFontFamilies(fontId, processedFont)
  }
}

/**
 * Adds a font to the library from a JSON file and a texture file and updates all the components
 * that use it, typically used for preloading fonts that may have longer fetch times
 * Will overwrite existing fonts if the existing font name is the same as the url
 * @param {*} jsonURL
 * @param {*} textureURL
 */
const addFontFromUrl = (jsonURL: string, textureURL: string) => {
  loadNewFontTexture(textureURL)
  loadNewFontJSON(jsonURL)
}

const getFontTextureUrls = (component: BaseComponent): Set<string> => {
  const record = records[component.id]
  if (!record && component.parentUI) {
    return getFontTextureUrls(component.parentUI)
  }
  return record.textureURLs
}

const getFontTextures = (component: BaseComponent): Map<string, Texture> => {
  const record = records[component.id]
  if (!record && component.parentUI) {
    return getFontTextures(component.parentUI)
  } else if (!record.textureURLs) {
    return new Map<string, Texture>()
  }
  return getFontTexturesFromUrls(record.textureURLs)
}

const getFontTexture = (component: BaseComponent, url: string) => {
  if (url === DEFAULT_TEXTURE_URL) {
    return DEFAULTS.getDefaultTexture()
  }
  const textures = getFontTextures(component)
  if (textures && url in textures) {
    return textures.get(url)
  }

  return DEFAULTS.getDefaultTexture()
}

const getPageUrl = (baseUrl: string | undefined, pageFileName: string) => (
  baseUrl
    ? new URL(pageFileName, baseUrl).href
    : DEFAULT_TEXTURE_URL
)

const isPageLoaded = (pageUrl: string) => (
  fontTextures[pageUrl] !== undefined
)

const isFontJsonLoaded = (jsonUrl: string) => (
  fontFamilies[jsonUrl] !== undefined
)

const removeComponent = (component: BaseComponent) => {
  delete records[component.id]
}

const FontLibrary = {
  setFontFamily,
  setFontTexture,
  setFontTextures,
  getFontOf,
  addFont,
  addFontFromUrl,
  getFontTextureFromUrl,
  getFontTexture,
  getFontTextureUrls,
  getFontTextures,
  getPageUrl,
  isPageLoaded,
  isFontJsonLoaded,
  removeComponent,
  DEFAULT_TEXTURE_URL,
}

export default FontLibrary
