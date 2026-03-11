import {useEffect} from 'react'
import {useTranslation} from 'react-i18next'
import type {GraphObject} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import {quat, vec3} from '@ecs/runtime/math/math'

import {useSelector} from '../../hooks'
import useActions from '../../common/use-actions'
import imageTargetsActions from '../../image-targets/actions'
import {useEnclosedApp} from '../../apps/enclosed-app-context'
import {calculateGlobalTransform} from '../global-transform'
import type {DerivedScene} from '../derive-scene'

type ImageTarget = {
  name: string
  translatedType: string
  type: string
  imageUrl: string
  originalImageUrl: string
  metadata: string
}

const useAppImageTargets = (galleryUuid: string): ImageTarget[] => {
  const app = useEnclosedApp()
  const targetInfo = useSelector(s => s.imageTargets.targetInfoByApp[app.uuid])
  const targetsByUuid = useSelector(s => s.imageTargets.targetsByUuid)
  const gallery = targetInfo?.galleries?.[galleryUuid]
  const imageTargets = gallery?.uuids?.map(uuid => targetsByUuid[uuid])
  const {
    fetchImageTargetsForApp, resetGalleryFilterOptionsForApp,
  } = useActions(imageTargetsActions)
  const {t} = useTranslation(['app-pages'])

  const getTranslatedType = (type: string) => {
    switch (type) {
      case 'PLANAR': return t('image_target_page.label.flat')
      case 'CYLINDER': return t('image_target_page.label.cylindrical')
      case 'CONICAL': return t('image_target_page.label.conical')
      default: return type
    }
  }

  useEffect(() => {
    if (!app) return
    resetGalleryFilterOptionsForApp(app.uuid, galleryUuid)
    fetchImageTargetsForApp(app.uuid, galleryUuid)
  }, [app.uuid])

  return imageTargets?.map(target => ({
    name: target.name,
    translatedType: getTranslatedType(target.type),
    type: target.type,
    imageUrl: target.imageSrc,
    originalImageUrl: target.geometryTextureImageSrc || target.originalImageSrc,
    metadata: target.metadata,
  })) ?? []
}

const getImageTargetRotation = (
  derivedScene: DerivedScene, name: string | undefined
): DeepReadonly<GraphObject['rotation']> | undefined => {
  if (!name) return undefined
  const obj = derivedScene.getAllSceneObjects().find(
    o => o.imageTarget?.name === name
  )

  if (!obj) return undefined

  const transform = calculateGlobalTransform(derivedScene, obj.id)
  const rotation = quat.zero()
  transform.decomposeTrs({t: vec3.zero(), r: rotation, s: vec3.zero()})
  return [rotation.x, rotation.y, rotation.z, rotation.w]
}

export {
  useAppImageTargets,
  getImageTargetRotation,
}

export type {
  ImageTarget,
}
