import THREE from './three'

import {Position, Quaternion, Scale, ThreeObject} from './components'
import type {World} from './world'
import {asm} from './asm'
import type {Eid} from '../shared/schema'
import type {Object3D} from './three-types'
import {addChild} from './matrix-refresh'

const createPrefab = (world: World): Eid => {
  const eid = asm.createPrefab(world._id)
  if (!eid) {
    throw new Error(`Failed to create prefab: ${eid}`)
  }

  ThreeObject.reset(world, eid)
  Position.reset(world, eid)
  Scale.reset(world, eid)
  Quaternion.reset(world, eid)

  return eid
}

const createPrefabChild = (world: World, prefabEid: Eid): Eid => {
  // @ts-ignore: PrefabChildResult is returned as a struct from wasm
  const prefabChild = asm.createPrefabChild(world._id, prefabEid)
  const baseChild = asm.getBaseEntity(world._id, prefabChild)

  ThreeObject.reset(world, baseChild)
  Position.reset(world, baseChild)
  Scale.reset(world, baseChild)
  Quaternion.reset(world, baseChild)

  return prefabChild
}

const spawnPrefab = (world: World, prefabEid: Eid): Eid => {
  const instanceEid = asm.spawnPrefab(world._id, prefabEid)
  const eids = new Set<Eid>()
  eids.add(instanceEid)
  const addChildren = (eid: Eid) => {
    for (const child of world.getChildren(eid)) {
      eids.add(child)
      addChildren(child)
    }
  }
  addChildren(instanceEid)

  eids.forEach((eid) => {
    world.allEntities.add(eid)

    const object = new THREE.Mesh(
      undefined,
      // NOTE(christoph): It seems like different versions of THREE behave differently,
      // where passing undefined gives you a default material.
      new THREE.MeshBasicMaterial({visible: false})
    ) as unknown as Object3D
    object.userData.entity = true
    object.userData.eid = eid
    object.matrixAutoUpdate = false
    addChild(world.three.scene, object)
    world.three.entityToObject.set(eid, object)
  })

  const setParents = (eid: Eid) => {
    for (const child of world.getChildren(eid)) {
      world.setParent(child, eid)
      setParents(child)
    }
  }

  setParents(instanceEid)

  return instanceEid
}

const getInstanceChild = (world: World, instanceEid: Eid, prefabChildEid: Eid): Eid => (
  asm.getInstanceChild(world._id, instanceEid, prefabChildEid)
)

export {
  createPrefab,
  createPrefabChild,
  spawnPrefab,
  getInstanceChild,
}
