import type {Eid, ReadData, Schema} from '../shared/schema'
import type {RootAttribute} from './world-attribute'
import type {Entity} from './entity-types'
import type {World} from './world'
import {ThreeObject} from './components'
import {Hidden} from './components/hidden-component'
import {Disabled} from './disabled'
import type {Vec3, Mat4, Vec3Source, QuatSource, Quat} from './math/math'

const extractEid = (entity: Entity | Eid): Eid => {
  if (typeof entity === 'bigint') {
    return entity
  }
  return entity.eid
}

class EntityImpl implements Entity {
  public eid: Eid

  private world: World

  constructor(world: World, eid: Eid) {
    this.world = world
    this.eid = eid
  }

  get<S extends Schema>(component: RootAttribute<S>) {
    return {...component.get(this.world, this.eid)}
  }

  has<S extends Schema>(component: RootAttribute<S>) {
    return component.has(this.world, this.eid)
  }

  set<S extends Schema>(component: RootAttribute<S>, data: Partial<ReadData<S>>) {
    component.set(this.world, this.eid, data)
  }

  reset<S extends Schema>(component: RootAttribute<S>) {
    component.reset(this.world, this.eid)
  }

  remove<S extends Schema>(component: RootAttribute<S>) {
    component.remove(this.world, this.eid)
  }

  hide() {
    this.reset(Hidden)
  }

  show() {
    this.remove(Hidden)
  }

  isHidden(): boolean {
    return this.has(Hidden)
  }

  disable() {
    this.reset(Disabled)
  }

  enable() {
    this.remove(Disabled)
  }

  isDisabled() {
    return this.has(Disabled)
  }

  delete() {
    this.world.deleteEntity(this.eid)
  }

  isDeleted() {
    return !ThreeObject.has(this.world, this.eid)
  }

  setParent(parent: Eid | Entity | undefined | null) {
    this.world.setParent(this.eid, (parent && extractEid(parent)) || 0n)
  }

  getChildren() {
    return Array.from(this.world.getChildren(this.eid), this.world.getEntity)
  }

  getParent() {
    const parentEid = this.world.getParent(this.eid)
    if (parentEid === 0n) {
      return null
    }
    return this.world.getEntity(parentEid)
  }

  addChild(child: Eid | Entity) {
    const childEid = extractEid(child)
    this.world.setParent(childEid, this.eid)
  }

  translateSelf(translation: Partial<Vec3Source>): void {
    this.world.transform.translateSelf(this.eid, translation)
  }

  translateLocal(translation: Partial<Vec3Source>): void {
    this.world.transform.translateLocal(this.eid, translation)
  }

  translateWorld(translation: Partial<Vec3Source>): void {
    this.world.transform.translateWorld(this.eid, translation)
  }

  rotateSelf(rotation: QuatSource): void {
    this.world.transform.rotateSelf(this.eid, rotation)
  }

  rotateLocal(rotation: QuatSource): void {
    this.world.transform.rotateLocal(this.eid, rotation)
  }

  lookAt(other: Eid | Entity): void {
    this.world.transform.lookAt(this.eid, extractEid(other))
  }

  lookAtLocal(position: Vec3Source): void {
    this.world.transform.lookAtLocal(this.eid, position)
  }

  lookAtWorld(position: Vec3Source): void {
    this.world.transform.lookAtWorld(this.eid, position)
  }

  getLocalPosition(out?: Vec3): Vec3 {
    return this.world.transform.getLocalPosition(this.eid, out)
  }

  getLocalTransform(out?: Mat4): Mat4 {
    return this.world.transform.getLocalTransform(this.eid, out)
  }

  getWorldPosition(out?: Vec3): Vec3 {
    return this.world.transform.getWorldPosition(this.eid, out)
  }

  getWorldTransform(out?: Mat4): Mat4 {
    return this.world.transform.getWorldTransform(this.eid, out)
  }

  setLocalPosition(position: Vec3Source) {
    this.world.transform.setLocalPosition(this.eid, position)
  }

  setLocalTransform(transform: Mat4) {
    this.world.transform.setLocalTransform(this.eid, transform)
  }

  setWorldPosition(position: Vec3Source) {
    this.world.transform.setWorldPosition(this.eid, position)
  }

  setWorldTransform(transform: Mat4) {
    this.world.transform.setWorldTransform(this.eid, transform)
  }

  getWorldQuaternion(out?: Quat): Quat {
    return this.world.transform.getWorldQuaternion(this.eid, out)
  }

  setWorldQuaternion(rotation: QuatSource) {
    this.world.transform.setWorldQuaternion(this.eid, rotation)
  }
}

const createEntityReference = (world: World, eid: Eid): EntityImpl => (
  new EntityImpl(world, eid)
)

export {
  createEntityReference,
}
