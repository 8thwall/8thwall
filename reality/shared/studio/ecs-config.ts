type Script = {
  src: string
}

type EcsConfig = {
  runtimeUrl?: string
  dev8Url?: string
  app8Url?: string
  preloadChunks?: string[]
  deferXr8?: boolean
  scripts?: Script[]
  runtimeTypeCheck?: boolean
  backgroundColor?: string
}

const DEFAULT_ECS_PAGE_BACKGROUND_COLOR = '#1c1d2a'

export {
  DEFAULT_ECS_PAGE_BACKGROUND_COLOR,
}

export type {
  EcsConfig,
  Script,
}
