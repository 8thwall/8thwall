import type {Vector3} from 'three'

import {resolveSpaceIdForObjectOrSpace} from '@ecs/shared/object-hierarchy'
import type {BasicMaterial, PlaneGeometry} from '@ecs/shared/scene-graph'

import type {SceneContext} from './scene-context'
import {basename, fileExt} from '../editor/editor-common'
import {ASSET_EXT_TO_KIND} from './common/studio-files'
import {makeEmptyObject, makeGeometry, makeGltfModel, makeMaterial, makeSplat} from './make-object'
import type {StudioStateContext} from './studio-state-context'
import {getActiveSpaceFromCtx} from './hooks/active-space'
import {getAssetDimensions} from './get-asset-dimensions'
import {createAndSelectObject} from './new-primitive-button'

// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

const insertAssetAsObject = (
  ctx: SceneContext, stateCtx: StudioStateContext, filePath: string, resourceUrl: string,
  parentId?: string, position?: Vector3
) => {
  const spaceId = resolveSpaceIdForObjectOrSpace(ctx.scene, parentId) ||
    getActiveSpaceFromCtx(ctx, stateCtx)?.id

  if (!filePath) {
    return
  }

  const newObject = makeEmptyObject(parentId || spaceId)
  const ext = fileExt(filePath)
  const fileType = ASSET_EXT_TO_KIND[ext]

  switch (fileType) {
    case 'model':
      newObject.gltfModel = makeGltfModel(filePath)
      break
    case 'audio':
      newObject.audio = {
        src: {
          type: 'asset',
          asset: filePath,
        },
      }
      break
    case 'splat':
      newObject.splat = makeSplat(filePath)
      break
    case 'image':
    case 'video':
      getAssetDimensions(resourceUrl, (width, height) => {
        newObject.geometry = makeGeometry('plane') as PlaneGeometry
        newObject.geometry.width = width > height ? 1 : width / height
        newObject.geometry.height = width > height ? height / width : 1
        newObject.material = makeMaterial() as BasicMaterial
        newObject.material.textureSrc = filePath
      })
      break
    default:
      stateCtx.update(p => ({...p, errorMsg: `Unsupported file type: ${fileType}`}))
      return
  }

  if (position) {
    newObject.position = position.toArray()
  }

  newObject.name = basename(filePath)
  createAndSelectObject(newObject, ctx, stateCtx)
}

export {
  insertAssetAsObject,
}
