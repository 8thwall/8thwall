import type {World} from '../runtime/world'
import type {Eid, Schema} from './schema'
import type {RootAttribute} from '../runtime/world-attribute'

/**
 * Finds the closet ancestor entity with a given component for the given entity ID.
 *
 * @param eid - The entity ID to find the root for.
 * @returns The entity ID of the root entity with a Sprite component, or 0n if no such entity is
 * found.
 */
const findParentComponent = <T extends Schema>(
  world: World, eid: Eid, component: RootAttribute<T>
): Eid => {
  let current: Eid = eid
  do {
    current = world.getParent(current)
    if (current && component.has(world, current)) {
      return current
    }
  } while (current)
  return current
}

const findChildComponents = <T extends Schema>(
  world: World, eid: Eid, component: RootAttribute<T>
): Eid[] => {
  const children: Eid[] = []
  const queue: Eid[] = [eid]
  while (queue.length) {
    const current = queue.pop()!
    for (const child of world.getChildren(current)) {
      if (component.has(world, child)) {
        children.push(child)
      }
      queue.push(child)
    }
  }
  return children
}

export {
  findParentComponent,
  findChildComponents,
}
