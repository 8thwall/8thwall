import THREE from './three'
import type {Texture} from './three-types'
import type {LayoutMetrics, UiRuntimeSettings} from './ui'
import ThreeMeshUI from './8mesh'
import {DEFAULT_FONT_NAME, FontUrls, getBuiltinFontUrls} from '../shared/fonts'
import {assets} from './assets'
import type {Eid} from '../shared/schema'
import {parseNineSliceValue} from '../shared/ui-nine-slice'
import {Ui} from './components'
import type {World} from './world'
import {translateX, translateY} from './3d-ui-translate'
import {getBackgroundOpacity} from '../shared/ui-constants'
import type {RawFontSetData} from '../shared/msdf-font-type'
import {disposeObject, markAsDisposableRecursively} from './dispose'
import {THREE_LAYERS} from '../shared/three-layers'
import {addChild, notifySelfChanged} from './matrix-refresh'

type FontMtsdf = Omit<FontUrls, 'ttf'>

// note(owenmech): Since studio controls precise render order, we can completely flatten
const UI_3D_Z = 0
const UI_3D_SCALE = 0.01

const uiText: Map<ThreeMeshUI.Block, ThreeMeshUI.Text> = new Map()
const uiResourceUrl: WeakMap<ThreeMeshUI.Block, string> = new Map()
const loadedFonts: Map<string, FontMtsdf> = new Map()
type EidToLoadingPromise = Map<Eid, Promise<FontMtsdf>>
const fontPromises: Map<string, Promise<FontMtsdf>> = new Map()

const clearTexture = (texture: Texture) => {
  if (texture instanceof THREE.VideoTexture) {
    texture.image.pause()
    texture.image.removeAttribute('src')
    texture.image.load()  // load to force immediate reset
    texture.image.remove()
    texture.image = null
  }
  texture.dispose()
}

// TODO(johnny): Implement remove3dUi in ui-system.ts
const remove3dUi = (
  eid: Eid, eidToLoadingPromise: EidToLoadingPromise, object: ThreeMeshUI.Block
) => {
  eidToLoadingPromise.delete(eid)
  uiText.delete(object)
  object.removeFromParent()

  if (object.backgroundTexture) {
    clearTexture(object.backgroundTexture)
    object.backgroundTexture = null
  }

  object.clear()
  disposeObject(object)
}

const getKnownFont = (font: string): FontMtsdf | null | undefined => {
  const builtIn = getBuiltinFontUrls(font)
  if (builtIn) {
    return builtIn
  }
  return loadedFonts.get(font)
}

const resolveCustomFont = async (font: string): Promise<FontMtsdf> => {
  // NOTE(christoph): Custom fonts are asset urls/paths
  const asset = await assets.load({url: font})
  if (!asset.remoteUrl) {
    throw new Error(`Font ${font} not found`)
  }
  const font8Text = await asset.data.text()
  const font8Json = JSON.parse(font8Text) as RawFontSetData
  // NOTE(tri): Resolve asset.remoteUrl to the current origin
  const bundlePath = new URL(asset.remoteUrl, window.location.href).href

  const font8Data: FontMtsdf = {
    png: new URL(font8Json.pages[0], bundlePath).href,
    json: asset.localUrl,
  }
  loadedFonts.set(font, font8Data)
  fontPromises.delete(font)
  return font8Data
}

const applyFont = (
  font: FontMtsdf,
  object: ThreeMeshUI.Block | ThreeMeshUI.Text
) => {
  object.set({
    fontFamily: font.json,
    fontTexture: font.png,
  })
}

const loadAndApplyFont = (
  eid: Eid,
  eidToLoadingPromise: EidToLoadingPromise,
  object: ThreeMeshUI.Block | ThreeMeshUI.Text,
  font: string
) => {
  const existingFont = getKnownFont(font)
  if (existingFont) {
    applyFont(existingFont, object)
    eidToLoadingPromise.delete(eid)
    return
  }

  let fontPromise = fontPromises.get(font)
  if (!fontPromise) {
    fontPromise = resolveCustomFont(font)
    fontPromises.set(font, fontPromise)
  }

  const alreadyLoadingCurrentFont = eidToLoadingPromise.get(eid) === fontPromise
  if (alreadyLoadingCurrentFont) {
    return
  }

  eidToLoadingPromise.set(eid, fontPromise)
  fontPromise.then((result) => {
    const completedLoadIsCurrent = eidToLoadingPromise.get(eid) === fontPromise
    if (completedLoadIsCurrent) {
      applyFont(result, object)
      eidToLoadingPromise.delete(eid)
    }
  })
}

const update3dUi = (
  world: World,
  eid: Eid,
  eidToLoadingPromise: EidToLoadingPromise,
  object: ThreeMeshUI.Block,
  settings: UiRuntimeSettings,
  isHidden: boolean,
  metrics?: LayoutMetrics
) => {
  const dimensions = metrics
    ? {
      width: metrics.width * UI_3D_SCALE,
      height: metrics.height * UI_3D_SCALE,
    }
    : null

  const borderRadiusTopLeft = settings.borderRadiusTopLeft
    ? {
      borderRadiusTopLeft: Number(settings.borderRadiusTopLeft) * UI_3D_SCALE,
    }
    : null

  const borderRadiusTopRight = settings.borderRadiusTopRight
    ? {
      borderRadiusTopRight: Number(settings.borderRadiusTopRight) * UI_3D_SCALE,
    }
    : null

  const borderRadiusBottomRight = settings.borderRadiusBottomRight
    ? {
      borderRadiusBottomRight: Number(settings.borderRadiusBottomRight) * UI_3D_SCALE,
    }
    : null

  const borderRadiusBottomLeft = settings.borderRadiusBottomLeft
    ? {
      borderRadiusBottomLeft: Number(settings.borderRadiusBottomLeft) * UI_3D_SCALE,
    }
    : null

  // @ts-ignore
  object.set({
    ...dimensions,
    fontColor: new THREE.Color(settings.color),
    backgroundColor: settings.image || settings.video ? null : new THREE.Color(settings.background),
    backgroundOpacity: getBackgroundOpacity(
      settings.backgroundOpacity, !!(settings.image || settings.video)
    ),
    borderRadius: settings.borderRadius * UI_3D_SCALE,
    ...borderRadiusTopLeft,
    ...borderRadiusTopRight,
    ...borderRadiusBottomRight,
    ...borderRadiusBottomLeft,
    borderWidth: settings.borderWidth * UI_3D_SCALE,
    borderColor: settings.borderColor ? new THREE.Color(settings.borderColor) : null,
    borderOpacity: settings.borderWidth ? 1 : 0,
    fontSize: settings.fontSize * UI_3D_SCALE,
    opacity: settings.opacity,
    textAlign: settings.textAlign,
    justifyContent: settings.verticalTextAlign,
    backgroundSize: settings.backgroundSize,
    nineSliceScaleFactor: settings.nineSliceScaleFactor,
    uiScale: UI_3D_SCALE,
    defines: {},
    activeLayers: settings.ignoreRaycast
      ? [THREE_LAYERS.renderedNotRaycasted]
      : [THREE_LAYERS.default, THREE_LAYERS.uiRaycasted],
    overrideRenderOrder: 0,
  })
  object.visible = !isHidden

  if (settings.image) {
    const previousImg = uiResourceUrl.get(object)
    // only load image if it's different from the previous one
    if (previousImg !== settings.image) {
      uiResourceUrl.set(object, settings.image)
      if (uiText.has(object)) {
        object.remove(uiText.get(object)!)
        uiText.delete(object)
      }
      if (object.backgroundTexture) {
        clearTexture(object.backgroundTexture)
      }
      const {
        backgroundSize, nineSliceBorderBottom, nineSliceBorderTop,
        nineSliceBorderLeft, nineSliceBorderRight, image,
      } = settings
      assets.load({url: image}).then((asset) => {
        if (!Ui.has(world, eid)) {
          return
        }

        // Get the updated ui cursor in case it has changed to avoid a race
        // condition where this loaded asset is no longer the current ui image
        if (image === Ui.get(world, eid).image) {
          new THREE.TextureLoader().load(asset.localUrl, (texture) => {
            if (!Ui.has(world, eid)) {
              return
            }

            // Get the updated ui cursor in case it has changed to avoid a race
            // condition where this loaded asset is no longer the current ui image
            if (image === Ui.get(world, eid).image) {
              texture.colorSpace = THREE.SRGBColorSpace
              object.set({
                backgroundTexture: texture,
                backgroundSize,
                // eslint-disable-next-line max-len
                nineSliceBorderTop: parseNineSliceValue(nineSliceBorderTop, texture.image.height ?? 1.0),
                // eslint-disable-next-line max-len
                nineSliceBorderBottom: parseNineSliceValue(nineSliceBorderBottom, texture.image.height ?? 1.0),
                // eslint-disable-next-line max-len
                nineSliceBorderLeft: parseNineSliceValue(nineSliceBorderLeft, texture.image.width ?? 1.0),
                // eslint-disable-next-line max-len
                nineSliceBorderRight: parseNineSliceValue(nineSliceBorderRight, texture.image.width ?? 1.0),
              })
            }
          })
        }
      })
    }
  } else if (settings.video) {
    const previousVideo = uiResourceUrl.get(object)
    // only load video if it's different from the previous one
    if (previousVideo !== settings.video) {
      uiResourceUrl.set(object, settings.video)
      if (uiText.has(object)) {
        object.remove(uiText.get(object)!)
        uiText.delete(object)
      }
      if (object.backgroundTexture) {
        clearTexture(object.backgroundTexture)
      }
      const {
        backgroundSize, nineSliceBorderBottom, nineSliceBorderTop,
        nineSliceBorderLeft, nineSliceBorderRight, video,
      } = settings
      assets.load({url: video}).then((asset) => {
        if (!Ui.has(world, eid)) {
          return
        }

        // Get the updated ui cursor in case it has changed to avoid a race
        // condition where this loaded asset is no longer the current ui video
        if (video === Ui.get(world, eid).video) {
          const videoElement = document.createElement('video')
          videoElement.src = asset.localUrl
          videoElement.crossOrigin = 'anonymous'
          videoElement.loop = true
          videoElement.muted = true
          videoElement.playsInline = true

          const videoTexture = new THREE.VideoTexture(videoElement)
          videoTexture.colorSpace = THREE.SRGBColorSpace

          videoElement.oncanplaythrough = () => {
            if (!Ui.has(world, eid)) {
              return
            }

            videoElement?.play()

            // Get the updated ui cursor in case it has changed to avoid a race
            // condition where this loaded asset is no longer the current ui video
            if (video === Ui.get(world, eid).video && videoTexture) {
              object.set({
                backgroundTexture: videoTexture,
                backgroundSize,
                // eslint-disable-next-line max-len
                nineSliceBorderTop: parseNineSliceValue(nineSliceBorderTop, videoElement.videoHeight ?? 1.0),
                // eslint-disable-next-line max-len
                nineSliceBorderBottom: parseNineSliceValue(nineSliceBorderBottom, videoElement.videoHeight ?? 1.0),
                // eslint-disable-next-line max-len
                nineSliceBorderLeft: parseNineSliceValue(nineSliceBorderLeft, videoElement.videoWidth ?? 1.0),
                // eslint-disable-next-line max-len
                nineSliceBorderRight: parseNineSliceValue(nineSliceBorderRight, videoElement.videoWidth ?? 1.0),
                defines: {USE_VIDEO_TEXTURE: ''},
              })
            }
          }
        }
      })
    }
  } else if (settings.text) {
    const fontName = settings.font ?? DEFAULT_FONT_NAME
    if (uiText.has(object)) {
      const textObj = uiText.get(object)!
      textObj.set({
        content: settings.text,
        fontSize: settings.fontSize * UI_3D_SCALE,
      })
      loadAndApplyFont(eid, eidToLoadingPromise, textObj, fontName)
    } else {
      const textObj = new ThreeMeshUI.Text({
        content: settings.text,
        fontSize: settings.fontSize * UI_3D_SCALE,
        offset: UI_3D_Z,
        activeLayers: [THREE_LAYERS.renderedNotRaycasted],
        onTextBuilt: markAsDisposableRecursively,
        foregroundDepthWrite: false,
        overrideRenderOrder: 0,
      })
      uiText.set(object, textObj)
      addChild(object, textObj)
      loadAndApplyFont(eid, eidToLoadingPromise, textObj, fontName)
    }
  } else if (uiText.has(object)) {
    // remove text node (since the text is empty)
    const textObj = uiText.get(object)!
    object.remove(textObj)
    disposeObject(textObj)
    uiText.delete(object)
  }

  if (!settings.video) {
    delete object.defines?.USE_VIDEO_TEXTURE
  }

  if (!settings.image && !settings.video && uiResourceUrl.has(object)) {
    uiResourceUrl.delete(object)
  }

  if (!metrics) {
    return
  }
  // @ts-ignore
  object.autoLayout = false

  const x = translateX(metrics?.parentWidth, metrics?.width, metrics?.left) * UI_3D_SCALE
  const y = translateY(metrics?.parentHeight, metrics?.height, metrics?.top) * UI_3D_SCALE

  object.position.set(x, y, object.position.z)
}

const create3dUi = (
  world: World,
  eid: Eid,
  eidToLoadingPromise: EidToLoadingPromise,
  settings: UiRuntimeSettings,
  isHidden: boolean
): ThreeMeshUI.Block => {
  const obj = new ThreeMeshUI.Block({
    width: 0,
    height: 0,
    userData: {eid},
    offset: UI_3D_Z,
    backgroundDepthWrite: false,
  })
  obj.onAfterUpdate = notifySelfChanged
  update3dUi(world, eid, eidToLoadingPromise, obj, settings, isHidden)
  markAsDisposableRecursively(obj)

  return obj
}

const setUiUserData = (object: ThreeMeshUI.Block, userData: object): void => {
  object?.set({userData})
  uiText.get(object)?.set({userData})
}

export type {
  EidToLoadingPromise,
}

export {
  create3dUi,
  remove3dUi,
  update3dUi,
  setUiUserData,
}
