import {defaultScene} from './default-scene'
import {planetaryScene} from './planetary-scene'
import {physicsScene} from './physics-scene'

const EXAMPLE_SCENES = [
  {name: 'Default Cube', scene: defaultScene},
  {name: 'Planetary', scene: planetaryScene},
  {name: 'Physics', scene: physicsScene},
] as const

export {
  EXAMPLE_SCENES,
}
