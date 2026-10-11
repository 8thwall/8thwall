import {createStartFlowPipelineModule} from './pipeline-module'
import {createStartFlowRenderer} from './renderer'
import type {StartFlowController, StartFlowOptions, StartFlowStringOverrides} from './types'

const create = (): StartFlowController => {
  const renderer = createStartFlowRenderer()

  return {
    pipelineModule: createStartFlowPipelineModule(renderer),
    configure: (options: Partial<StartFlowOptions>) => renderer.configure(options),
    setStringOverrides: (overrides: StartFlowStringOverrides) => {
      renderer.setStringOverrides(overrides)
    },
    show: () => renderer.show({type: 'loading', requestingPermissions: false}),
    hide: renderer.hide,
    isVisible: renderer.isVisible,
  }
}

let instance: StartFlowController | null = null

const StartFlowFactory = (): StartFlowController => {
  if (!instance) {
    instance = create()
  }
  return instance
}

export {StartFlowFactory}
export type {
  StartFlowOptions,
  StartFlowController,
  StartFlowPipelineModule,
  StartFlowStringOverrides,
  StartFlowStrings,
} from './types'
