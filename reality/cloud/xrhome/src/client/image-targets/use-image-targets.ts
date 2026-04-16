import React from 'react'
import {useQuery, useQueryClient, useSuspenseQuery} from '@tanstack/react-query'

import {
  CropResult, GetTargetTextureParams, ImageTargetData, TargetTextureType,
  TEXTURE_PATH,
} from '@repo/reality/shared/desktop/image-target-api'

import type {DeepReadonly} from 'ts-essentials'

import {useEnclosedAppKey} from '../apps/enclosed-app-context'
import {
  listImageTargets, uploadImageTarget, deleteImageTarget,
} from './image-target-api'
import {selectTargetsGalleryFilterOptions} from './state-selectors'
import {useSelector} from '../hooks'
import {DEFAULT_FILTER_OPTIONS} from './reducer'
import type {IImageTarget} from '../common/types/models'
import {targetMatchesFilter} from './gallery-filters'

const makeImageTargetTextureUrl = (
  appKey: string, target: ImageTargetData, type: TargetTextureType
) => {
  const params: GetTargetTextureParams = {
    appKey,
    name: target.name,
    v: String(target.updated),
    type,
  }
  return `image-targets://${TEXTURE_PATH}?${new URLSearchParams(params)}`
}

const expandTargetData = (appKey: string, target: ImageTargetData): IImageTarget => ({
  ...target,
  metadata: JSON.stringify(target.properties),
  AppUuid: appKey,
  appKey,
  originalImageSrc: makeImageTargetTextureUrl(appKey, target, 'original'),
  imageSrc: makeImageTargetTextureUrl(appKey, target, 'cropped'),
  geometryTextureImageSrc: makeImageTargetTextureUrl(appKey, target, 'geometry'),
  thumbnailImageSrc: makeImageTargetTextureUrl(appKey, target, 'thumbnail'),
  uuid: target.name,
  userMetadata: typeof target.metadata === 'string'
    ? target.metadata
    : JSON.stringify(target.metadata),
  userMetadataIsJson: typeof target.metadata !== 'string',
  status: 'DISABLED',
  createdAt: new Date(target.created).toString(),
  updatedAt: new Date(target.updated).toString(),
  isRotated: !!target.properties.isRotated,
})

const fetchImageTargets = async (appKey: string): Promise<DeepReadonly<IImageTarget[]>> => {
  const {targets} = await listImageTargets(appKey)
  return targets
    .map(target => expandTargetData(appKey, target))
    .sort((a, b) => b.created - a.created)
}

const useImageTargets = () => {
  const appKey = useEnclosedAppKey()

  return useSuspenseQuery({
    queryKey: ['image-targets', appKey],
    queryFn: () => fetchImageTargets(appKey),
  }).data
}

const useImageTargetsOrLoading = () => {
  const appKey = useEnclosedAppKey()
  return useQuery({
    queryKey: ['image-targets', appKey],
    queryFn: () => fetchImageTargets(appKey),
  })
}

const useGalleryTargets = (galleryUuid: string | undefined) => {
  const appKey = useEnclosedAppKey()
  const targets = useImageTargets()
  const filter = useSelector(s => (
    galleryUuid
      ? selectTargetsGalleryFilterOptions(appKey, galleryUuid, s.imageTargets)
      : DEFAULT_FILTER_OPTIONS
  ))
  return React.useMemo(
    () => targets.filter(t => targetMatchesFilter(t, filter)),
    [targets, filter]
  )
}

const useImageTargetActions = () => {
  const appKey = useEnclosedAppKey()
  const client = useQueryClient()
  return React.useMemo(() => {
    const refresh = () => {
      client.invalidateQueries({queryKey: ['image-targets', appKey]})
    }

    const deleteTarget = async (name: string) => {
      await deleteImageTarget(appKey, name)
      refresh()
    }

    const upload = async (
      image: Blob,
      name: string,
      crop: CropResult
    ) => {
      const target = await uploadImageTarget(appKey, image, name, crop)
      refresh()
      return expandTargetData(appKey, target)
    }

    return {
      uploadImageTarget: upload,
      deleteImageTarget: deleteTarget,
      refresh,
    }
  }, [appKey, client])
}

export {
  useImageTargets,
  useGalleryTargets,
  useImageTargetsOrLoading,
  useImageTargetActions,
}
