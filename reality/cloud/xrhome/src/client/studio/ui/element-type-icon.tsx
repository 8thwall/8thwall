import React from 'react'

import {isPrefabInstance, isPrefab} from '@ecs/shared/object-hierarchy'

import {useSceneContext} from '../scene-context'
import {Icon} from '../../ui/components/icon'
import {useDerivedScene} from '../derived-scene-context'

const ElementTypeIcon: React.FC<{id: string}> = ({id}) => {
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const object = derivedScene.getObject(id)
  const isInstance = ctx.scene.objects[id] && isPrefabInstance(ctx.scene.objects[id])
  const prefab = isPrefab(object)

  if (isInstance || prefab) {
    return <Icon stroke='prefabInstance' />
  }

  if (object.camera) {
    return <Icon stroke='camera12' />
  }

  if (object.light) {
    return <Icon stroke='light12' />
  }

  if (object.ui) {
    return <Icon stroke='ui12' />
  }

  if (object.location) {
    return <Icon stroke='vpsLocation12' />
  }

  if (object.imageTarget) {
    return <Icon stroke='imageTarget12' />
  }

  if (object.map) {
    return <Icon stroke='mapOutline12' />
  }

  if (object.geometry || object.gltfModel) {
    return <Icon stroke='object12' />
  }

  if (object.face) {
    return <Icon stroke='face12' />
  }

  return (
    <Icon stroke='empty12' />
  )
}

export {
  ElementTypeIcon,
}
