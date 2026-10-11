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
  getOptions: () => StartFlowOptions
  hide: () => void
  isVisible: () => boolean
  setStringOverrides: (overrides: StartFlowStringOverrides) => void
  show: (screen: StartFlowScreen) => void
  showStart: (onStart?: () => void) => void
}

const FADE_OUT_DELAY_MS = 300
const FADE_OUT_DURATION_MS = 300

const createStartFlowRenderer = (): StartFlowRenderer => {
  let hideTimeout: number | undefined
  let root: HTMLDivElement | null = null
  let options: StartFlowOptions | undefined
  let optionOverrides: Partial<StartFlowOptions> = {}
  let screen: StartFlowScreen = 'start'
  let stringOverrides: StartFlowStringOverrides | undefined
  let pendingStringOverrides: StartFlowStringOverrides = {}
  let startHandler: () => void

  const getOptions = () => {
    options ??= {...resolveDefaultOptions(), ...optionOverrides}
    return options
  }

  const getStringOverrides = () => {
    stringOverrides ??= {...resolveDefaultStringOverrides(), ...pendingStringOverrides}
    return stringOverrides
  }

  const remove = () => {
    if (!root) {
      return
    }
    render(null, root)
    root.remove()
    root = null
  }

  const cancelHide = () => {
    if (hideTimeout !== undefined) {
      window.clearTimeout(hideTimeout)
      hideTimeout = undefined
    }
    root?.firstElementChild?.classList.remove(sharedStyles.fadeOut)
  }

  const hide = () => {
    if (!root || hideTimeout !== undefined) {
      return
    }
    if (typeof screen !== 'object' || screen.type !== 'loading' ||
      screen.requestingPermissions || !root.firstElementChild) {
      remove()
      return
    }
    root.firstElementChild.classList.add(sharedStyles.fadeOut)
    hideTimeout = window.setTimeout(() => {
      hideTimeout = undefined
      remove()
    }, FADE_OUT_DELAY_MS + FADE_OUT_DURATION_MS)
  }

  const viewOptions = (): StartFlowScreenOptions => ({
    previewSrc: getOptions().previewSrc,
    onStart: startHandler || hide,
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
        stringOverrides={getStringOverrides()}
      />,
      root
    )
  }

  const show = (nextScreen: StartFlowScreen) => {
    cancelHide()
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
    showStart: (onStart = hide) => {
      startHandler = onStart
      show('start')
    },
    hide,
    getOptions,
    configure: (nextOptions) => {
      if (options) {
        options = {...options, ...nextOptions}
      } else {
        optionOverrides = {...optionOverrides, ...nextOptions}
      }
      draw()
    },
    setStringOverrides: (nextOverrides) => {
      if (stringOverrides) {
        stringOverrides = {...stringOverrides, ...nextOverrides}
      } else {
        pendingStringOverrides = {...pendingStringOverrides, ...nextOverrides}
      }
      draw()
    },
    isVisible: () => Boolean(root),
  }
}

export {createStartFlowRenderer}
export type {StartFlowRenderer}
