import type {Eid} from '../shared/schema'
import type {Mat4, Quat, QuatSource, Vec3, Vec3Source} from './math/math'

type TransformManager = {
  getLocalPosition(eid: Eid, out?: Vec3): Vec3
  getLocalTransform(eid: Eid, out?: Mat4): Mat4
  getWorldPosition(eid: Eid, out?: Vec3): Vec3
  getWorldQuaternion(eid: Eid, out?: Quat): Quat
  getWorldTransform(eid: Eid, out?: Mat4): Mat4

  setLocalPosition(eid: Eid, position: Vec3Source): void
  setLocalTransform(eid: Eid, mat4: Mat4): void
  setWorldPosition(eid: Eid, position: Vec3Source): void
  setWorldQuaternion(eid: Eid, rotation: QuatSource): void
  setWorldTransform(eid: Eid, mat4: Mat4): void

  translateSelf(eid: Eid, translation: Partial<Vec3Source>): void
  translateLocal(eid: Eid, translation: Partial<Vec3Source>): void
  translateWorld(eid: Eid, translation: Partial<Vec3Source>): void

  rotateSelf(eid: Eid, rotation: QuatSource): void
  rotateLocal(eid: Eid, rotation: QuatSource): void

  lookAt(eid: Eid, other: Eid): void
  lookAtLocal(eid: Eid, position: Vec3Source): void
  lookAtWorld(eid: Eid, position: Vec3Source): void
}

export type {
  TransformManager,
}
