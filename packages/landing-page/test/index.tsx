import {applyInferredParameters} from '../src/inferred-parameters'
import {defaultParameters, normalizeParameters} from '../src/parameters'
import {show} from '../src/render'

import {getTestParameters} from '../src/test-parameters'

const refresh = () => {
  const params = applyInferredParameters(normalizeParameters({
    ...defaultParameters,
    ...getTestParameters(),
  }))
  show(params)
}

refresh()

/* @ts-ignore */
window.refreshLanding = refresh
