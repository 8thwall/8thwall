import React from 'react'
import * as THREE from 'three'

import type {LayoutMetrics} from '@ecs/runtime/ui-types'
import {translateX, translateY} from '@ecs/runtime/3d-ui-translate'

import type {UiGraphSettings} from '@ecs/shared/scene-graph'

import * as ThreeMeshUI from '@ecs/8mesh/three-mesh-ui'
import {parseNineSliceValue} from '@ecs/shared/ui-nine-slice'
import {getUiAttributes} from '@ecs/shared/get-ui-attributes'
import type {RawFontSetData} from '@ecs/shared/msdf-font-type'
import {getBackgroundOpacity} from '@ecs/shared/ui-constants'
import {THREE_LAYERS} from '@ecs/shared/three-layers'

import {BACK_FRAME_MATERIAL} from './shader/back-frame-material'

// note(owenmech): Since studio controls precise render order, we can completely flatten
const UI_3D_Z = 0

const updateBlock = (
  ui: UiGraphSettings,
  layout: LayoutMetrics,
  fontData: RawFontSetData | string,
  fontTextureUrl: string,
  texture: THREE.Texture,
  obj: ThreeMeshUI.Block,
  textObj: ThreeMeshUI.Text | null,
  uiOrder: number,
  stackingContext: number,
  outlined: boolean
) => {
  const settings = getUiAttributes(ui)

  if (texture && settings.image) {
    texture.colorSpace = THREE.SRGBColorSpace
  }

  const textureWidth = texture?.image?.width ?? 1.0
  const textureHeight = texture?.image?.height ?? 1.0

  const renderLayers = settings.ignoreRaycast
    ? [THREE_LAYERS.renderedNotRaycasted]
    : [THREE_LAYERS.default]
  const outlineLayers = outlined ? [THREE_LAYERS.outline] : []

  if (textObj && !settings.image && !settings.video) {
    textObj.set({
      content: settings.text,
      fontSize: settings.fontSize,
      userData: {uiOrder, stackingContext},
      // note(owen): we do not want to outline or raycast text
      activeLayers: [THREE_LAYERS.renderedNotRaycasted],
    })
    if (textObj.parent !== obj) {
      obj.add(textObj)
    }
  }

  obj.set({
    width: (layout.width ?? 0),
    height: (layout.height ?? 0),
    backgroundColor: settings.image || settings.video ? null : new THREE.Color(settings.background),
    backgroundOpacity: getBackgroundOpacity(
      settings.backgroundOpacity, !!(settings.image || settings.video)
    ),
    fontFamily: fontData,
    fontTexture: fontTextureUrl,
    fontColor: new THREE.Color(settings.color),
    borderRadius: settings.borderRadius,
    borderRadiusTopLeft: Number(settings.borderRadiusTopLeft || 0),
    borderRadiusTopRight: Number(settings.borderRadiusTopRight || 0),
    borderRadiusBottomRight: Number(settings.borderRadiusBottomRight || 0),
    borderRadiusBottomLeft: Number(settings.borderRadiusBottomLeft || 0),
    borderWidth: settings.borderWidth,
    borderColor: settings.borderColor ? new THREE.Color(settings.borderColor) : null,
    borderOpacity: settings.borderWidth ? 1 : 0,
    opacity: settings.opacity,
    textAlign: settings.textAlign,
    justifyContent: settings.verticalTextAlign,
    backgroundSize: settings.backgroundSize,
    nineSliceScaleFactor: settings.nineSliceScaleFactor,
    backgroundTexture: texture,
    nineSliceBorderTop: parseNineSliceValue(settings.nineSliceBorderTop, textureHeight),
    nineSliceBorderBottom: parseNineSliceValue(settings.nineSliceBorderBottom, textureHeight),
    nineSliceBorderLeft: parseNineSliceValue(settings.nineSliceBorderLeft, textureWidth),
    nineSliceBorderRight: parseNineSliceValue(settings.nineSliceBorderRight, textureWidth),
    defines: settings.video ? {USE_VIDEO_TEXTURE: ''} : {},
    userData: {uiOrder, stackingContext},
    activeLayers: [...renderLayers, ...outlineLayers],
  })

  // @ts-ignore
  obj.autoLayout = false
  const x = translateX(layout.parentWidth, layout.width, layout.left)
  const y = translateY(layout.parentHeight, layout.height, layout.top)
  obj.position.set(x, y, obj.position.z)
}

const useUiBlock = (
  ui: UiGraphSettings | undefined,
  layout: LayoutMetrics | undefined,
  fontData: RawFontSetData | string,
  fontTextureUrl: string,
  texture: THREE.Texture | undefined,
  backside: boolean,
  id: string,
  uiOrder: number,
  stackingContext: number,
  outlined: boolean
): ThreeMeshUI.Block => {
  const hasUi = !!ui && !!layout

  const block = React.useMemo(() => {
    if (!hasUi) {
      return null
    }
    return new ThreeMeshUI.Block({
      width: 0,
      height: 0,
      offset: UI_3D_Z,
      userData: {id},
      overrideRenderOrder: 0,
      backgroundDepthWrite: false,
    })
  }, [hasUi, id])

  const hasText = hasUi && !!ui.text

  const text = React.useMemo(() => {
    if (!hasText) {
      return null
    }
    return new ThreeMeshUI.Text({
      content: '',
      offset: UI_3D_Z,
      overrideRenderOrder: 0,
      foregroundDepthWrite: false,
    })
  }, [hasText])

  React.useLayoutEffect(() => {
    if (!block) {
      return undefined
    }

    updateBlock(
      ui, layout, fontData, fontTextureUrl, texture, block, text, uiOrder, stackingContext, outlined
    )

    return () => {
      if (text) {
        text.removeFromParent()
      }
    }
  }, [
    ui, layout, fontData, fontTextureUrl, texture, block, text, uiOrder, stackingContext, outlined,
  ])

  React.useLayoutEffect(() => {
    if (!block || !layout || !backside) {
      return undefined
    }

    const wireframeMesh = new THREE.Mesh(
      new THREE.PlaneGeometry(layout.width, layout.height), BACK_FRAME_MATERIAL
    )
    block.add(wireframeMesh)

    return () => {
      block.remove(wireframeMesh)
      wireframeMesh.geometry.dispose()
    }
  }, [block, layout, backside])

  return block
}

export {
  useUiBlock,
}
