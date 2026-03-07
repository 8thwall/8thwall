import type {Schema} from '../../shared/schema'
import type {RootAttribute} from '../world-attribute'
import {ThreeObject} from '../components'
import {defineQuery, lifecycleQueries} from '../query'

const makeSystemHelper = <T extends Schema>(component: RootAttribute<T>) => (
  lifecycleQueries(defineQuery([component, ThreeObject]))
)
export {
  makeSystemHelper,
}
