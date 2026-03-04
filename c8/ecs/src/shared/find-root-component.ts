import type {World} from '../runtime/world'
import type {Eid, Schema} from './schema'
import type {RootAttribute} from '../runtime/world-attribute'

/**
 * Finds the root entity with a Component for the given entity ID.
 *
 * @param world - The world to search in.
 * @param eid - The entity ID to find the root for.
 * @param component - The component to search for.
 * @returns The entity ID of the root entity with a Component, or 0n if no such entity is
 * found.
 */
const findRootComponent = <T extends Schema>(
  world: World, eid: Eid, component: RootAttribute<T>
): Eid => {
  let root = component.has(world, eid) ? eid : 0n
  let cursor = eid
  do {
    const parent = world.getParent(cursor)
    if (parent && component.has(world, parent)) {
      root = parent
    }
    cursor = parent
  } while (cursor)
  return root
}

export {findRootComponent}
