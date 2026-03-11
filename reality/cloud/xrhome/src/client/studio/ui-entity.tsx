/* eslint-disable camelcase */
import React, {useEffect, useRef} from 'react'
import {Texture, VideoTexture, Object3D} from 'three'
import type {ThreeEvent} from '@react-three/fiber'
import {customRaycastSort} from '@ecs/shared/custom-sorting'

import {useSceneContext} from './scene-context'
import {useYogaParentContext} from './yoga-parent-context'
import {useTexture} from './hooks/use-texture'
import {useVideo} from './hooks/use-video'
import {useUiBlock} from './use-ui'
import {getObjectSelection, useObjectSelection} from './hooks/selected-objects'
import {useUiFont} from './hooks/use-ui-font'
import {useDerivedScene} from './derived-scene-context'
import {useStudioStateContext} from './studio-state-context'
import {useActiveSpace} from './hooks/active-space'

type UiEntityProps = {
  id: string
  outlined?: boolean
  parentBlock?: Object3D
}

const findIdForObject = (object: Object3D | null): string | null => {
  if (!object) {
    return null
  }
  if (object.userData?.id) {
    return object.userData.id
  }
  return findIdForObject(object.parent)
}

const UiEntity = ({id, outlined: _outlined = false, parentBlock}: UiEntityProps) => {
  const {isDraggingGizmo} = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const activeSpaceId = useActiveSpace()?.id
  const {layoutMetrics, layoutChildren, uiOrder, stackingContext} = useYogaParentContext()
  const object = derivedScene.getObject(id)
  const ui = object?.ui || {}
  const texture = useTexture(ui.image)
  const video = useVideo(ui.video)
  const layoutMetric = layoutMetrics.get(id)
  const font = useUiFont(ui.font)
  // const font = DEFAULT_FONT
  const children = layoutChildren?.get(id) ?? []
  const selection = useObjectSelection(id)
  const outlined = _outlined || selection.isSelected

  // TODO(chloe): Created ref to avoid creating new `VideoTexture` upon every render, but later
  // support texture instancing for video textures so multiple video/image UI elements can share
  // same texture and deleting one element doesn't affect others.
  const videoTextureRef = useRef<Texture>(null)

  const visibleTexture = texture || videoTextureRef.current

  const order = uiOrder.get(id) ?? 0
  const context = stackingContext.get(id) ?? 0
  const uiBlock = useUiBlock(
    (object && !object.hidden && !object.disabled) ? {...ui, ignoreRaycast: false} : null,
    layoutMetric,
    font.json,
    font.png,
    visibleTexture,
    true,
    id,
    order,
    context,
    outlined
  )

  // This is a hack to ensure that the UI Object 3D structure is updated when
  // things are popping in and out of existence. This should have been
  // unnecessary if the structure was properly updated by Drei. However, I have
  // encountered issues where the UI elements are not properly updated when they
  // are toggled on and off. This is a temporary fix until the underlying issue
  // is resolved.
  React.useEffect(() => {
    if (uiBlock && parentBlock) {
      parentBlock.add(uiBlock)

      return () => {
        uiBlock.removeFromParent()
      }
    }

    return undefined
  }, [uiBlock, parentBlock])

  React.useEffect(() => () => {
    if (uiBlock) {
      uiBlock.children.forEach(child => child.removeFromParent())
    }
  }, [uiBlock])

  useEffect(() => {
    if (texture) {
      texture.colorSpace = 'srgb'
      return () => {
        texture?.dispose()
      }
    } else if (video) {
      const videoTexture = new VideoTexture(video)
      videoTexture.colorSpace = 'srgb'
      videoTextureRef.current = videoTexture
      return () => {
        videoTexture.image.pause()
        videoTexture.image.removeAttribute('src')
        videoTexture.image.load()
        videoTexture.image.remove()
        videoTexture.image = null
        videoTexture.dispose()
        videoTextureRef.current = null
      }
    }
    return undefined
  }, [video, texture])

  return uiBlock && (
    <>
      <primitive
        object={uiBlock}
        onClick={(e: ThreeEvent<PointerEvent>) => {
          if (e.altKey || isDraggingGizmo) {
            return
          }
          const topMostIxn = e.intersections.length && e.intersections
            .filter(ixn => ixn.object.userData.rootUi === uiBlock.userData.rootUi)
            .reduce((topIxn, ixn) => {
              const diff = customRaycastSort(topIxn, ixn)
              return diff > 0 ? ixn : topIxn
            }, e.intersections[0])

          e.stopPropagation()

          const topId = findIdForObject(topMostIxn?.object)

          getObjectSelection(topId ?? id, stateCtx, derivedScene, activeSpaceId)
            .onClick(e.ctrlKey || e.metaKey || e.shiftKey)
        }}
      />
      {children.map(childId => (
        <UiEntity key={childId} id={childId} outlined={outlined} parentBlock={uiBlock} />
      ))}
    </>
  )
}

export {
  UiEntity,
}
