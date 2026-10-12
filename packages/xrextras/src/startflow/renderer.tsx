import {h, render} from 'preact'

import type {
  StartFlowOptions,
  StartFlowScreen,
  StartFlowScreenOptions,
  StartFlowStringOverrides,
} from './types'
import {resolveDefaultOptions, resolveDefaultStringOverrides} from './options'
import sharedStyles from './shared.module.scss'
import {StartFlowView} from './view'

interface StartFlowRenderer {
  configure: (options: Partial<StartFlowOptions>) => void
  fadeOut: () => void
  getOptions: () => StartFlowOptions
  hide: () => void
  isVisible: () => boolean
  setStringOverrides: (overrides: StartFlowStringOverrides) => void
  show: (screen: StartFlowScreen) => void
  showStart: (listener: () => void) => void
}

const FADE_OUT_DELAY_MS = 300
const FADE_OUT_DURATION_MS = 300

const createStartFlowRenderer = (): StartFlowRenderer => {
  let hideTimeout: number | undefined
  let root: HTMLDivElement | null = null
  let options = resolveDefaultOptions()
  let screen: StartFlowScreen = 'start'
  let stringOverrides = resolveDefaultStringOverrides()
  let pendingStartListener: (() => void) | undefined

  const hide = () => {
    if (!root) {
      return
    }
    if (hideTimeout !== undefined) {
      window.clearTimeout(hideTimeout)
      hideTimeout = undefined
    }
    render(null, root)
    root.remove()
    root = null
  }

  const handleStart = () => {
    const listener = pendingStartListener
    pendingStartListener = undefined
    listener?.()
  }

  const cancelFadeOut = () => {
    if (hideTimeout !== undefined) {
      window.clearTimeout(hideTimeout)
      hideTimeout = undefined
    }
    root?.firstElementChild?.classList.remove(sharedStyles.fadeOut)
  }

  const fadeOut = () => {
    if (!root || hideTimeout !== undefined) {
      return
    }
    if (typeof screen !== 'object' || screen.type !== 'loading' ||
      screen.requestingPermissions || !root.firstElementChild) {
      return
    }
    root.firstElementChild.classList.add(sharedStyles.fadeOut)
    hideTimeout = window.setTimeout(() => {
      hideTimeout = undefined
      hide()
    }, FADE_OUT_DELAY_MS + FADE_OUT_DURATION_MS)
  }

  const viewOptions = (): StartFlowScreenOptions => ({
    previewSrc: options.previewSrc,
    onStart: handleStart,
    onRefresh: () => window.location.reload(),
  })

  const draw = () => {
    if (!root) {
      return
    }
    render(
      <StartFlowView
        options={viewOptions()}
        screen={screen}
        stringOverrides={stringOverrides}
      />,
      root
    )
  }

  const show = (nextScreen: StartFlowScreen) => {
    cancelFadeOut()
    screen = nextScreen
    if (!root) {
      root = document.createElement('div') as unknown as HTMLDivElement
      root.dataset.xrextrasStartFlowRoot = ''
      document.body.appendChild(root)
    }
    draw()
  }

  return {
    show,
    showStart: (listener) => {
      pendingStartListener = listener
      show('start')
    },
    fadeOut,
    hide,
    getOptions: () => options,
    configure: (nextOptions) => {
      options = {...options, ...nextOptions}
      draw()
    },
    setStringOverrides: (nextOverrides) => {
      stringOverrides = {...stringOverrides, ...nextOverrides}
      draw()
    },
    isVisible: () => Boolean(root),
  }
}

export {createStartFlowRenderer}
export type {StartFlowRenderer}
