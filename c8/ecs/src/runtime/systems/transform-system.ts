import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {Position, Quaternion, Scale, ThreeObject} from '../components'
import {changedQuery, defineQuery, enterQuery} from '../query'
import {notifyChanged} from '../matrix-refresh'

const makeTransformSystem = (world: World) => {
  const {scene} = world.three

  const enter = enterQuery(defineQuery([ThreeObject]))
  const changed = changedQuery(defineQuery([Position, Scale, Quaternion, ThreeObject]))

  const tempPosition = scene.position.clone()
  const tempScale = scene.position.clone()
  const tempRotation = scene.quaternion.clone()

  const position = Position.forWorld(world)
  const scale = Scale.forWorld(world)
  const quaternion = Quaternion.forWorld(world)

  const updateTransform = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)

    if (object) {
      const objectPosition = position.get(eid)
      tempPosition.set(objectPosition.x, objectPosition.y, objectPosition.z)

      const objectScale = scale.get(eid)
      tempScale.set(objectScale.x, objectScale.y, objectScale.z)

      const objectRotation = quaternion.get(eid)
      tempRotation.set(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w)
      object.matrix.compose(tempPosition, tempRotation, tempScale)
      notifyChanged(object)
    }
  }

  return () => {
    enter(world).forEach(updateTransform)
    changed(world).forEach(updateTransform)
  }
}
export {
  makeTransformSystem,
}
