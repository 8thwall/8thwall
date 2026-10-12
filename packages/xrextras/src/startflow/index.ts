import {createStartFlowPipelineModule} from './pipeline-module'
import {createStartFlowRenderer} from './renderer'
import type {StartFlowController} from './types'

const create = (): StartFlowController => {
  let renderer: ReturnType<typeof createStartFlowRenderer> | undefined

  // NOTE(christoph): We late-initialize so that we don't initialize default options based on
  // meta tags too early - the xrextras script may be above he meta tags in <head>.
  const getRenderer = () => {
    renderer ??= createStartFlowRenderer()
    return renderer
  }

  return {
    pipelineModule: () => createStartFlowPipelineModule(getRenderer()),
    configure: options => getRenderer().configure(options),
    setStringOverrides: overrides => getRenderer().setStringOverrides(overrides),
    show: () => getRenderer().show({type: 'loading', requestingPermissions: false}),
    hide: () => renderer?.hide(),
    isVisible: () => renderer?.isVisible() ?? false,
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
