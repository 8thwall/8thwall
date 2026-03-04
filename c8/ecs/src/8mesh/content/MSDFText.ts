import type {FontSetData} from '../../shared/msdf-font-type'
import FontLibrary from '../components/core/FontLibrary'
import type {Material, Mesh as ThreeMesh} from '../utils/three-types'
import MSDFGlyph from './MSDFGlyph'

// Note (Tri) this is just a subset of MeshUiComponent.inlines
// We will need to refactor this to use MeshUiComponent.inlines or reuse Inline type here
interface GlyphInfo {
  glyph: string
  offsetX: number
  offsetY: number
}

type TextType = 'MSDF' | 'SDF' | 'MTSDF'

interface GetGlyphDimensionsOptions {
  textType: TextType
  glyph: string
  font: FontSetData
  fontSize: number
}

interface GlyphDimension {
  width: number
  height: number
  anchor: number
  xadvance: number
  xoffset: number
}

// This is expected to be set from the caller environment, see set-three.ts, if it starts to cause
// issues, we can inline BufferGeometryUtils.
// @ts-ignore
const {Mesh, BufferGeometryUtils} = window.THREE

const {mergeGeometries} = BufferGeometryUtils

/**

Job:
- Computing glyphs dimensions according to this component's font and content
- Create the text Mesh (call MSDFGlyph for each letter)

Knows:
- The Text component for which it creates Meshes
- The parameters of the text mesh it must return

 */

function getGlyphDimensions(options: GetGlyphDimensionsOptions): GlyphDimension {
  const {font, fontSize, glyph} = options

  const fontScale = fontSize / font.info.size

  //

  const charObj = font._chars[glyph]
  let width = charObj ? charObj.width * fontScale : fontSize / 3
  let height = charObj ? charObj.height * fontScale : 0

  // handle exported whitespaces
  if (width === 0) {
    // if this whitespaces in is the charset, use its xadvance value
    // or fallback to fontSize
    width = charObj ? charObj.xadvance * fontScale : fontSize
  }

  if (height === 0) height = fontSize * 0.7

  if (glyph === '\n') width = 0

  const xadvance = charObj ? charObj.xadvance * fontScale : width
  const xoffset = charObj ? charObj.xoffset * fontScale : 0

  // world-space length between lowest point and the text cursor position
  // eslint-disable-next-line max-len
  // const anchor = charObj ? ( ( charObj.yoffset + charObj.height - FONT.common.base ) * FONT_SIZE ) / FONT.common.lineHeight : 0;

  // some fonts have higher baseline heights due to having more space for ascenders, resulting
  // in larger yoffsets, so we need to adjust the anchor point to account for the higher baseline
  // making display across different fonts more consistent
  // additionally, padding is added to account for the distance range, so the anchor point needs to
  // be adjusted accordingly
  const additionalHeight = font.common.base + font.distanceField.distanceRange
  const anchor = charObj ? (charObj.yoffset - additionalHeight) * fontScale : 0

  return {
    // lineHeight,
    width,
    height,
    anchor,
    xadvance,
    xoffset,
  }
}

/**
 * Try to find the kerning amount of a
 * @param font
 * @param {string} glyphPair
 * @returns {number}
 */
function getGlyphPairKerning(font: FontSetData, glyphPair: string): number {
  const kernings = font._kernings
  return kernings[glyphPair] ? kernings[glyphPair] : 0
}

//

const EMPTY_MESH = new Mesh()
const VERTICES_PER_CHARACTER = 6
/**
 * Creates a THREE.Plane geometry, with UVs carefully positioned to map a particular
 * glyph on the MSDF texture. Then creates a shaderMaterial with the MSDF shaders,
 * creates a THREE.Mesh, returns it.
 */
function buildText(
  glyphInfos: GlyphInfo[],
  font: FontSetData,
  materials: Record<string, Material>,
  baseTextureUrl: string
): ThreeMesh {
  if (!font) {
    // font is not loaded yet, return empty mesh
    return EMPTY_MESH
  }
  const materialArray: Material[] = []
  const textureUrlMaterialIdx: Record<string, number> = {}
  Object.entries(materials).forEach(([url, value]) => {
    textureUrlMaterialIdx[url] = materialArray.length
    materialArray.push(value)
  })

  // Create a geometry for each glyph, and translate it to its position, bucketed by character page
  const translatedGeom = glyphInfos.reduce((acc, inline) => {
    const {glyph} = inline
    if (!font._chars[glyph]) {
      // in case the glyph is not in the font, we skip rendering it
      return acc
    }
    const glyphGeometry = new MSDFGlyph(inline, font)
    glyphGeometry.translate(inline.offsetX, inline.offsetY, 0)
    if (font.pages.length === 1) {
      if (acc[baseTextureUrl]) {
        acc[baseTextureUrl].push(glyphGeometry)
      } else {
        acc[baseTextureUrl] = [glyphGeometry]
      }
      return acc
    }
    const pageIdx = font._chars[glyph].page
    const pageName = font.pages[pageIdx]
    const pageUrl = FontLibrary.getPageUrl(baseTextureUrl, pageName)
    if (acc[pageUrl]) {
      acc[pageUrl].push(glyphGeometry)
    } else {
      acc[pageUrl] = [glyphGeometry]
    }
    return acc
  }, {} as Record<string, MSDFGlyph[]>)

  // create merging group for each character page
  const groupings: Array<{
    page: string
    count: number
    geometries: MSDFGlyph[]
  }> = Object.entries(translatedGeom).map(([key, value]) => ({
    page: key, count: value.length, geometries: value,
  }))

  // merge all geometries
  const mergedGeom = mergeGeometries(groupings.map(group => group.geometries).flat(), false)

  // create groups for merged geometry
  mergedGeom.groups = []
  groupings.reduce((start, {page, count}) => {
    const verticesNum = count * VERTICES_PER_CHARACTER
    const end = start + verticesNum
    mergedGeom.addGroup(start, verticesNum, textureUrlMaterialIdx[page])
    return end
  }, 0)

  const mesh = new Mesh(mergedGeom, materialArray)
  return mesh
}

//

export {
  getGlyphDimensions,
  getGlyphPairKerning,
  buildText,
}

export type {
  GlyphInfo,
  GetGlyphDimensionsOptions,
  GlyphDimension,
  TextType,
}
