import THREE from '../three'

import type {World} from '../world'
import {makeSystemHelper} from './system-helper'
import type {Eid, ReadData, Schema} from '../../shared/schema'
import type {RootAttribute} from '../world-attribute'
import {maybeDisposeGeometry} from '../dispose'

const makeGeometrySystem = <T extends Schema>(
  world: World,
  component: RootAttribute<T>,
  geometryType: string,
  geometryProvider: (c: ReadData<T>) => unknown
) => {
  const worldComponent = component.forWorld(world)

  const {enter, changed, exit} = makeSystemHelper(component)

  const emptyGeometry = new THREE.BufferGeometry()

  const applyGeometry = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (object instanceof THREE.Mesh) {
      const data = worldComponent.get(eid)
      maybeDisposeGeometry(object.geometry)
      object.geometry = geometryProvider(data!)
      object.geometry.userData.disposable = true
      object.geometry.computeBoundsTree()
    }
  }

  const removeGeometry = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (object instanceof THREE.Mesh && object.geometry?.type === geometryType) {
      maybeDisposeGeometry(object.geometry)
      object.geometry = emptyGeometry
    }
  }

  return () => {
    exit(world).forEach(removeGeometry)
    enter(world).forEach(applyGeometry)
    changed(world).forEach(applyGeometry)
  }
}

export {
  makeGeometrySystem,
}
