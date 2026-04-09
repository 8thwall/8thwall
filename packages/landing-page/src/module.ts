import {applyInferredParameters} from './inferred-parameters'
import {defaultParameters, LandingParameters} from './parameters'
import {hide as hideLandingPage, isVisible, show as showLandingPage} from './render'
import type {RunConfig} from './xr'
import {showAlmostThere, hideAlmostThere, showAlmostThereCollisionError} from './almost-there'

declare const XR8: any

const options: LandingParameters = {...defaultParameters}

const render = () => {
  showLandingPage(applyInferredParameters(options))
}

const configure = (params: Partial<LandingParameters>) => {
  Object.keys(options).forEach((key) => {
    if (params[key] !== undefined) {
      options[key] = params[key]
    }
  })
  if (isVisible()) {
    render()
  }
}

let runConfig_: RunConfig

const pipelineModule = () => ({
  name: 'landing8',
  onBeforeRun: ({config}: {config: RunConfig}) => {
    runConfig_ = config
  },
  onException: () => {
    if (XR8.XrDevice.isDeviceBrowserCompatible(runConfig_)) {
      // Error for some reason other than compatibility
      return
    }

    const didShow = showAlmostThere(runConfig_, applyInferredParameters(options).url)
    if (didShow) {
      hideLandingPage()
    } else {
      hideAlmostThere()
      render()
    }

    // Using a timeout in case xrextras-almost-here is going to receive the callback right after
    setTimeout(() => {
      const containerCount = document.querySelectorAll('#almostthereContainer').length

      // We're pulling some HTML from xrextras, if we use it, only ours should be present.
      const expectedContainerCount = didShow ? 1 : 0

      if (containerCount > expectedContainerCount) {
        showAlmostThereCollisionError()
      }
    }, 0)
  },
  onRemove: () => {
    runConfig_ = null
    hideLandingPage()
    hideAlmostThere()
  },
})

export {
  configure,
  pipelineModule,
}
