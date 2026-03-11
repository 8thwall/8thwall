import type {EcsConfig} from './ecs-config'

type EcsManifest = {
  version: 1
  config?: EcsConfig
}

export type {
  EcsManifest,
}
