import {isPrefab}
  from '@ecs/shared/object-hierarchy'
import type {StaticImageTargetOrientation, Vec4Tuple} from '@ecs/shared/scene-graph'
import {quat} from '@ecs/runtime/math/quat'
import type React from 'react'
import {v4 as uuid} from 'uuid'

import {isAssetPath} from '../common/editor-files'
import {insertAssetAsObject} from './insert-object'
import {handleObjectReorder} from './reparent-object'
import {makeInstance} from './configuration/prefab'
import type {SceneContext} from './scene-context'
import type {StudioStateContext} from './studio-state-context'
import {makeImageTarget} from './make-object'
import {createAndSelectObject} from './new-primitive-button'
import type {DerivedScene} from './derive-scene'

const handleDrop = (
  e: React.DragEvent,
  ctx: SceneContext,
  stateCtx: StudioStateContext,
  derivedScene: DerivedScene,
  setHovering: (hovering: boolean) => void,
  parentId: string | undefined,
  siblingId?: string
) => {
  e.preventDefault()
  setHovering(false)

  const objectId = e.dataTransfer.getData('objectId')
  const filePath = e.dataTransfer.getData('filePath')
  const imageTargetName = e.dataTransfer.getData('imageTargetName')
  const resourceUrl = e.dataTransfer.getData('resourceUrl')
  const imageTargetMetadataStr = e.dataTransfer.getData('imageTargetMetadata')
  let staticOrientation: StaticImageTargetOrientation | undefined
  if (imageTargetMetadataStr) {
    const imageTargetMetadata = JSON.parse(imageTargetMetadataStr)
    staticOrientation = imageTargetMetadata.staticOrientation
  }

  if (objectId) {
    const object = ctx.scene.objects[objectId]
    const targetPrefab = derivedScene.getParentPrefabId(objectId)
    const newParentPrefab = derivedScene.getParentPrefabId(parentId)

    // TODO: allow instantiating prefabs within other prefabs when nesting is ready
    if (object && isPrefab(object) && !(targetPrefab && newParentPrefab)) {
      const newId = uuid()
      ctx.updateScene(scene => makeInstance(scene, objectId, parentId, newId))
      stateCtx.setSelection(newId)
      return
    }

    const placement = siblingId
      ? {position: 'before' as const, from: siblingId}
      : {position: 'within' as const, from: parentId}

    handleObjectReorder(ctx, stateCtx, derivedScene, objectId, placement)
  } else if (filePath && isAssetPath(filePath)) {
    insertAssetAsObject(ctx, stateCtx, filePath, resourceUrl, parentId)
  } else if (imageTargetName) {
    const imgTarget = makeImageTarget(parentId, imageTargetName, staticOrientation)
    imgTarget.name = imageTargetName
    if (staticOrientation) {
      imgTarget.rotation = quat.pitchYawRollDegrees({
        x: staticOrientation.rollAngle,
        y: 0,
        z: staticOrientation.pitchAngle,
      }).data() as Vec4Tuple
    }
    createAndSelectObject(imgTarget, ctx, stateCtx)
  }
}

export {
  handleDrop,
}
