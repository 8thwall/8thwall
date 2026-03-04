import type {DeepReadonly} from 'ts-essentials'
import {isEqual} from 'lodash-es'

import type {BaseGraphObject, GraphObject, PrefabInstanceDeletions} from '../shared/scene-graph'
import type {World} from './world'
import type {Eid} from '../shared/schema'
import {
  applyAudioComponent, applyCameraComponent, applyColliderComponent, applyComponentUpdates,
  applyDisabledComponent, applyFaceComponent, applyGeometryComponent, applyGltfModelComponent,
  applyHiddenComponent, applyImageTargetComponent, applyLightComponent,
  applyMaterialComponent,
  applyOrderComponent, applyPersistentComponent, applyPositionComponent, applyQuaternionComponent,
  applyScaleComponent, applyShadowComponent, applySplatComponent, applyUiComponent,
  applyVideoControlsComponent,
} from './apply-update'
import {getInstanceChild} from './prefab-entity'

const spawnIntoObjectWithDiff = (
  world: World,
  eid: Eid,
  e: DeepReadonly<BaseGraphObject>,
  graphIdToEid: Map<string, Eid>,
  prevObject: DeepReadonly<BaseGraphObject> | null | undefined
) => {
  const shouldUpdatePosition = !prevObject || !isEqual(e.position, prevObject.position)
  if (shouldUpdatePosition) {
    applyPositionComponent(world, eid, e.position)
  }

  const shouldUpdateScale = !prevObject || !isEqual(e.scale, prevObject.scale)
  if (shouldUpdateScale) {
    applyScaleComponent(world, eid, e.scale)
  }

  const shouldUpdateQuaternion = !prevObject || !isEqual(e.rotation, prevObject.rotation)
  if (shouldUpdateQuaternion) {
    applyQuaternionComponent(world, eid, e.rotation)
  }

  const shouldUpdateGeometry = !prevObject || !isEqual(e.geometry, prevObject.geometry)
  if (shouldUpdateGeometry) {
    applyGeometryComponent(world, eid, e.geometry, prevObject?.geometry)
  }

  const shouldUpdateMaterial = !prevObject || !isEqual(e.material, prevObject?.material)
  if (shouldUpdateMaterial) {
    applyMaterialComponent(world, eid, e.material, prevObject?.material)
  }

  applyOrderComponent(world, eid, e.order || 0)

  const shouldUpdateShadow = !prevObject || !isEqual(e.shadow, prevObject.shadow)
  if (shouldUpdateShadow) {
    applyShadowComponent(world, eid, e.shadow)
  }

  const shouldUpdateCamera = !prevObject || !isEqual(e.camera, prevObject.camera)
  if (shouldUpdateCamera) {
    applyCameraComponent(world, eid, e.camera)
  }

  const shouldUpdateLight = !prevObject || !isEqual(e.light, prevObject.light)
  if (shouldUpdateLight) {
    applyLightComponent(world, eid, e.light)
  }

  const shouldUpdateGltfModel = !prevObject || !isEqual(e.gltfModel, prevObject.gltfModel)
  if (shouldUpdateGltfModel) {
    applyGltfModelComponent(world, eid, e.gltfModel)
  }

  const shouldUpdateSplat = !prevObject || !isEqual(e.splat, prevObject.splat)
  if (shouldUpdateSplat) {
    applySplatComponent(world, eid, e.splat)
  }

  const shouldUpdateCollider = !prevObject || !isEqual(e.collider, prevObject.collider) ||
    !isEqual(e.geometry, prevObject.geometry)
  if (shouldUpdateCollider) {
    applyColliderComponent(world, eid, e.collider, e.geometry, e.gltfModel)
  }

  const shouldUpdateAudio = !prevObject || !isEqual(e.audio, prevObject.audio)
  if (shouldUpdateAudio) {
    applyAudioComponent(world, eid, e.audio)
  }

  const shouldUpdateVideoControls = !prevObject || !isEqual(
    e.videoControls, prevObject.videoControls
  )
  if (shouldUpdateVideoControls) {
    applyVideoControlsComponent(world, eid, e.videoControls)
  }

  const shouldUpdateUi = !prevObject || !isEqual(e.ui, prevObject.ui)
  if (shouldUpdateUi) {
    applyUiComponent(world, eid, e.ui)
  }

  const shouldUpdateFace = !prevObject || !isEqual(e.face, prevObject.face)
  if (shouldUpdateFace) {
    applyFaceComponent(world, eid, e.face)
  }

  const shouldUpdateImageTarget = !prevObject || !isEqual(e.imageTarget, prevObject.imageTarget)
  if (shouldUpdateImageTarget) {
    applyImageTargetComponent(world, eid, e.imageTarget)
  }

  const shouldUpdateHidden = !prevObject || !isEqual(e.hidden, prevObject.hidden)
  if (shouldUpdateHidden) {
    applyHiddenComponent(world, eid, e.hidden)
  }

  const shouldUpdateDisabled = !prevObject || !isEqual(e.disabled, prevObject.disabled)
  if (shouldUpdateDisabled) {
    applyDisabledComponent(world, eid, e.disabled)
  }

  const shouldUpdatePersistent = !prevObject || !isEqual(e.persistent, prevObject.persistent)
  if (shouldUpdatePersistent) {
    applyPersistentComponent(world, eid, e.persistent)
  }

  applyComponentUpdates(world, eid, e.components, graphIdToEid, prevObject?.components)
}

const spawnIntoObject = (
  world: World,
  eid: Eid,
  object: DeepReadonly<BaseGraphObject>,
  graphIdToEid: Map<string, Eid>
) => {
  spawnIntoObjectWithDiff(world, eid, object, graphIdToEid, null)
}

const updateInstanceProperties = (
  world: World,
  eid: Eid,
  e: DeepReadonly<Partial<GraphObject>>,
  graphIdToEid: Map<string, Eid>,
  prevObject: DeepReadonly<Partial<GraphObject>> | null | undefined,
  deletions: DeepReadonly<PrefabInstanceDeletions> | undefined,
  prevDeletions: DeepReadonly<PrefabInstanceDeletions> | undefined
) => {
  const shouldUpdatePosition = e.position !== undefined &&
    (!prevObject || !isEqual(e.position, prevObject.position))
  if (shouldUpdatePosition) {
    applyPositionComponent(world, eid, e.position)
  }

  const shouldUpdateScale = e.scale !== undefined &&
    (!prevObject || !isEqual(e.scale, prevObject.scale))
  if (shouldUpdateScale) {
    applyScaleComponent(world, eid, e.scale)
  }

  const shouldUpdateQuaternion = e.rotation !== undefined &&
    (!prevObject || !isEqual(e.rotation, prevObject.rotation))
  if (shouldUpdateQuaternion) {
    applyQuaternionComponent(world, eid, e.rotation)
  }

  const shouldUpdateGeometry = (e.geometry !== undefined || prevObject?.geometry !== undefined) &&
    (!prevObject || !isEqual(e.geometry, prevObject.geometry) ||
    (!deletions?.geometry && prevDeletions?.geometry))
  const shouldRemoveGeometry = deletions?.geometry
  if (shouldUpdateGeometry || shouldRemoveGeometry) {
    applyGeometryComponent(
      world, eid, shouldRemoveGeometry ? undefined : e.geometry, prevObject?.geometry
    )
  }

  const shouldUpdateMaterial = (e.material !== undefined || prevObject?.material !== undefined) &&
    (!prevObject || !isEqual(e.material, prevObject.material) ||
    (!deletions?.material && prevDeletions?.material))
  const shouldRemoveMaterial = deletions?.material
  if (shouldUpdateMaterial || shouldRemoveMaterial) {
    applyMaterialComponent(
      world, eid, shouldRemoveMaterial ? undefined : e.material, prevObject?.material
    )
  }

  const shouldUpdateShadow = (e.shadow !== undefined || prevObject?.shadow !== undefined) &&
    (!prevObject || !isEqual(e.shadow, prevObject.shadow) ||
    (!deletions?.shadow && prevDeletions?.shadow))
  const shouldRemoveShadow = deletions?.shadow
  if (shouldUpdateShadow || shouldRemoveShadow) {
    applyShadowComponent(world, eid, shouldRemoveShadow ? undefined : e.shadow)
  }

  const shouldUpdateCamera = (e.camera !== undefined || prevObject?.camera !== undefined) &&
    (!prevObject || !isEqual(e.camera, prevObject.camera) ||
    (!deletions?.camera && prevDeletions?.camera))
  const shouldRemoveCamera = deletions?.camera
  if (shouldUpdateCamera || shouldRemoveCamera) {
    applyCameraComponent(world, eid, shouldRemoveCamera ? undefined : e.camera)
  }

  const shouldUpdateLight = (e.light !== undefined || prevObject?.light !== undefined) &&
    (!prevObject || !isEqual(e.light, prevObject.light) ||
    (!deletions?.light && prevDeletions?.light))
  const shouldRemoveLight = deletions?.light
  if (shouldUpdateLight || shouldRemoveLight) {
    applyLightComponent(world, eid, shouldRemoveLight ? undefined : e.light)
  }

  const shouldUpdateGltfModel =
    (e.gltfModel !== undefined || prevObject?.gltfModel !== undefined) &&
    (!prevObject || !isEqual(e.gltfModel, prevObject.gltfModel) ||
    (!deletions?.gltfModel && prevDeletions?.gltfModel))
  const shouldRemoveGltfModel = deletions?.gltfModel
  if (shouldUpdateGltfModel || shouldRemoveGltfModel) {
    applyGltfModelComponent(world, eid, shouldRemoveGltfModel ? undefined : e.gltfModel)
  }

  const shouldUpdateSplat = (e.splat !== undefined || prevObject?.splat !== undefined) &&
    (!prevObject || !isEqual(e.splat, prevObject.splat) ||
    (!deletions?.splat && prevDeletions?.splat))
  const shouldRemoveSplat = deletions?.splat
  if (shouldUpdateSplat || shouldRemoveSplat) {
    applySplatComponent(world, eid, shouldRemoveSplat ? undefined : e.splat)
  }

  const shouldUpdateCollider = (e.collider !== undefined || prevObject?.collider !== undefined) &&
    (!prevObject || !isEqual(e.collider, prevObject.collider) ||
    (!deletions?.collider && prevDeletions?.collider))
  const shouldRemoveCollider = deletions?.collider
  if (shouldUpdateCollider || shouldRemoveCollider) {
    applyColliderComponent(
      world, eid, shouldRemoveCollider ? undefined : e.collider, e.geometry, e.gltfModel
    )
  }

  const shouldUpdateAudio = (e.audio !== undefined || prevObject?.audio !== undefined) &&
    (!prevObject || !isEqual(e.audio, prevObject.audio) ||
     (!deletions?.audio && prevDeletions?.audio))
  const shouldRemoveAudio = deletions?.audio
  if (shouldUpdateAudio || shouldRemoveAudio) {
    applyAudioComponent(world, eid, shouldRemoveAudio ? undefined : e.audio)
  }

  const shouldUpdateVideoControls =
    (e.videoControls !== undefined || prevObject?.videoControls !== undefined) &&
  (!prevObject || !isEqual(e.videoControls, prevObject.videoControls) ||
  (!deletions?.videoControls && prevDeletions?.videoControls))
  const shouldRemoveVideoControls = deletions?.videoControls
  if (shouldUpdateVideoControls || shouldRemoveVideoControls) {
    applyVideoControlsComponent(world, eid, shouldRemoveVideoControls ? undefined : e.videoControls)
  }

  const shouldUpdateUi = (e.ui !== undefined || prevObject?.ui !== undefined) &&
    (!prevObject || !isEqual(e.ui, prevObject.ui) || (!deletions?.ui && prevDeletions?.ui))
  const shouldRemoveUi = deletions?.ui
  if (shouldUpdateUi || shouldRemoveUi) {
    applyUiComponent(world, eid, shouldRemoveUi ? undefined : e.ui)
  }

  const shouldUpdateFace = (e.face !== undefined || prevObject?.face !== undefined) &&
    (!prevObject || !isEqual(e.face, prevObject.face) || (!deletions?.face && prevDeletions?.face))
  const shouldRemoveFace = deletions?.face
  if (shouldUpdateFace || shouldRemoveFace) {
    applyFaceComponent(world, eid, shouldRemoveFace ? undefined : e.face)
  }

  const shouldUpdateImageTarget =
    (e.imageTarget !== undefined || prevObject?.imageTarget !== undefined) &&
    (!prevObject || !isEqual(e.imageTarget, prevObject.imageTarget) ||
    (!deletions?.imageTarget && prevDeletions?.imageTarget))
  const shouldRemoveImageTarget = deletions?.imageTarget
  if (shouldUpdateImageTarget || shouldRemoveImageTarget) {
    applyImageTargetComponent(world, eid, shouldRemoveImageTarget ? undefined : e.imageTarget)
  }

  const shouldUpdateHidden = (e.hidden !== undefined || prevObject?.hidden !== undefined) &&
    (!prevObject || !isEqual(e.hidden, prevObject.hidden) ||
    (!deletions?.hidden && prevDeletions?.hidden))
  const shouldRemoveHidden = deletions?.hidden
  if (shouldUpdateHidden || shouldRemoveHidden) {
    applyHiddenComponent(world, eid, shouldRemoveHidden ? undefined : e.hidden)
  }

  const shouldUpdateDisabled = (e.disabled !== undefined || prevObject?.disabled !== undefined) &&
    (!prevObject || !isEqual(e.disabled, prevObject.disabled) ||
    (!deletions?.disabled && prevDeletions?.disabled))
  const shouldRemoveDisabled = deletions?.disabled
  if (shouldUpdateDisabled || shouldRemoveDisabled) {
    applyDisabledComponent(world, eid, shouldRemoveDisabled ? undefined : e.disabled)
  }

  const shouldUpdatePersistent =
    (e.persistent !== undefined || prevObject?.persistent !== undefined) &&
    (!prevObject || !isEqual(e.persistent, prevObject.persistent) ||
    (!deletions?.persistent && prevDeletions?.persistent))
  const shouldRemovePersistent = deletions?.persistent
  if (shouldUpdatePersistent || shouldRemovePersistent) {
    applyPersistentComponent(world, eid, shouldRemovePersistent ? undefined : e.persistent)
  }

  if (typeof e.order === 'number') {
    applyOrderComponent(world, eid, e.order)
  }

  applyComponentUpdates(world, eid, e.components || {}, graphIdToEid, prevObject?.components)
}

const spawnInstanceIntoObject = (
  world: World,
  eid: Eid,
  object: DeepReadonly<GraphObject>,
  graphIdToEid: Map<string, Eid>,
  graphIdToPrefab: Map<string, Eid>,
  prevObject: DeepReadonly<GraphObject> | null | undefined
) => {
  if (!object.instanceData) {
    return
  }

  const {deletions, children} = object.instanceData
  const prevDeletions = prevObject?.instanceData?.deletions

  updateInstanceProperties(world, eid, object, graphIdToEid, prevObject, deletions, prevDeletions)

  if (children) {
    Object.entries(children).forEach(([childId, child]) => {
      // eslint-disable-next-line @typescript-eslint/no-unused-vars
      const {deletions: childDeletions, deleted, ...childOverrides} = child
      const prefabChildEid = graphIdToPrefab.get(childId)
      if (!prefabChildEid) {
        throw new Error(`Prefab child ${childId} not found in graphIdToEid: ${prefabChildEid}`)
      }
      const instanceChildEid = getInstanceChild(world, eid, prefabChildEid)
      if (instanceChildEid) {
        const prevChild = prevObject?.instanceData?.children?.[childId]
        const prevChildDeletions = prevChild?.deletions
        updateInstanceProperties(
          world,
          instanceChildEid,
          childOverrides,
          graphIdToEid,
          prevChild,
          childDeletions,
          prevChildDeletions
        )
      }
    })
  }

  const prevChildren = prevObject?.instanceData?.children
  if (prevChildren) {
    Object.entries(prevChildren).forEach(([childId, prevChild]) => {
      if (children && children[childId]) {
        return
      }
      const prefabChildEid = graphIdToPrefab.get(childId)
      if (!prefabChildEid) {
        throw new Error(`Prefab child ${childId} not found in graphIdToEid: ${prefabChildEid}`)
      }
      const instanceChildEid = getInstanceChild(world, eid, prefabChildEid)
      if (instanceChildEid) {
        const prevChildDeletions = prevChild.deletions
        updateInstanceProperties(
          world,
          instanceChildEid,
          {},
          graphIdToEid,
          prevChild,
          undefined,
          prevChildDeletions
        )
      }
    })
  }
}

export {
  spawnIntoObject,
  spawnIntoObjectWithDiff,
  spawnInstanceIntoObject,
}
