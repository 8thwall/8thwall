// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

// @inliner-off
import {IS_SIMD, VERSION} from 'reality/app/xr/js/build-constants'

import {loadJsxr} from './src/jsxr'
import {setDeprecatedProperty} from './src/deprecate'
import type {ChunkName} from './src/types/api'
import {ResourceUrls} from './src/resources'

const script = document.currentScript as HTMLScriptElement

const baseUrl = new URL('./', script.src).toString()

ResourceUrls.setBaseUrl(baseUrl)

loadJsxr().then(async (jsxr) => {
  // eslint-disable-next-line no-console
  console.log(`8th Wall XR Version: ${VERSION}${IS_SIMD ? 's' : ''}`)

  setDeprecatedProperty(window, 'XR', jsxr, 'XR has been renamed to XR8.')

  const chunksToPreload = script.getAttribute('data-preload-chunks')
    ?.split(',').map(e => e.trim()).filter(Boolean) as ChunkName[]

  await Promise.all((chunksToPreload || []).map(c => jsxr.loadChunk(c)))

  window.XR8 = jsxr

  setTimeout(() => {
    window.dispatchEvent(new CustomEvent('XRloaded'))
    window.dispatchEvent(new CustomEvent('xrloaded'))
  }, 1)
})
