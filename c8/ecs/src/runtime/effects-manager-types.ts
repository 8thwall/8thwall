import type {DeepReadonly} from 'ts-essentials'

import type {Fog, Sky} from '../shared/scene-graph'

type EffectsManager = {
  setFog: (fog: DeepReadonly<Fog> | undefined) => void
  getFog: () => DeepReadonly<Fog> | undefined
  setSky: (sky: DeepReadonly<EffectsManagerSky> | undefined) => void
  getSky: () => DeepReadonly<EffectsManagerSky> | undefined
  attach: () => void
  detach: () => void
}

type InternalEffectsManager = EffectsManager & {
  _hideSkyForXr: () => void
  _showSkyForXr: () => void
}

type EffectsManagerSky = Sky<string>

export type {
  EffectsManager,
  InternalEffectsManager,
  EffectsManagerSky,
}
