import * as api from './api'

type Ecs = typeof api

// Allows you to do either:
//  import ecs from 'ecs' or
//  import {registerComponent} from 'ecs'
export default api
export * from './api'

export type {
  Ecs,
}
