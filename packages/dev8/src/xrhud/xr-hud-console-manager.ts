import './xr-hud-core.css'
import {removeDom} from './xr-hud-core'
import type {XrHudBaseSettings, XrHudLogger} from './xr-hud-types'
import {getPrintableArgs} from '../printable'

type Action = {
  action: () => void
  text?: string
}

const ConsoleManager = () => {
  const actions_: Record<string, Action> = {
    'Recenter': {
      action: () => {
        window.XR8.XrController.recenter()
        if (window.XR8.LayersController) {
          // NOTE(paris): We guard here in case users are on old versions of the engine.
          window.XR8.LayersController.recenter()
        }
      },
    },
  }

  // LOG TAB
  const logTab_ = document.createElement('div')
  logTab_.id = 'logs'
  logTab_.classList.add('log-tab')

  // ACTIONS TAB
  const actionsTab_ = document.createElement('div')
  actionsTab_.id = 'actionsdiv'
  actionsTab_.classList.add('actions-tab')

  // CAMERA TAB CONTENT
  const cameraTab_ = document.createElement('div')
  cameraTab_.id = 'cameradiv'
  cameraTab_.classList.add('camera-tab')

  const tabs_ = [logTab_, actionsTab_, cameraTab_]

  // START CONSOLE TAB BUTTON SECTION
  const tabButtonContainer = document.createElement('div')
  tabButtonContainer.id = 'tabButtonContainerdiv'
  tabButtonContainer.classList.add('tab-button-container')

  const consoleTabButton_ = document.createElement('button')
  consoleTabButton_.id = 'console-tab-button'
  consoleTabButton_.value = logTab_.id
  consoleTabButton_.textContent = 'console'
  consoleTabButton_.classList.add('tab-button', 'selected-tab-button')
  tabButtonContainer.appendChild(consoleTabButton_)

  const actionsTabButton_ = document.createElement('button')
  actionsTabButton_.id = 'actions-tab-button'
  actionsTabButton_.value = actionsTab_.id
  actionsTabButton_.textContent = 'actions'
  actionsTabButton_.classList.add('tab-button', 'unselected-tab-button')
  tabButtonContainer.appendChild(actionsTabButton_)

  const cameraTabButton_ = document.createElement('button')
  cameraTabButton_.id = 'camera-tab-button'
  cameraTabButton_.value = cameraTab_.id
  cameraTabButton_.textContent = 'camera'
  cameraTabButton_.classList.add('tab-button', 'unselected-tab-button')
  tabButtonContainer.appendChild(cameraTabButton_)

  const consoleSection_ = document.createElement('div')
  consoleSection_.id = 'console-section'
  consoleSection_.classList.add('xr-tablet-ignore')

  let logstream_ = ''
  let logStreamLast_ = ''
  let logdiv_: HTMLDivElement | null = null

  // Size Toggle Buttons
  const maxBtn_ = document.createElement('button')
  maxBtn_.classList.add('xr-tablet-ignore')
  maxBtn_.id = 'maximize-button'
  maxBtn_.classList.add('actions-button', 'size-toggle-button', 'hidden')
  maxBtn_.style.cssText =
    'position: fixed;z-index: 999;bottom: 2px;right: 2px;'
  maxBtn_.textContent = '+'
  maxBtn_.addEventListener('click', () => {
    maxBtn_.classList.toggle('hidden')
    consoleSection_.classList.toggle('hidden')
  })

  const minBtn_ = document.createElement('button')
  minBtn_.id = 'minimize-button'
  minBtn_.classList.add('actions-button', 'size-toggle-button', 'min-button')
  minBtn_.addEventListener('click', () => {
    maxBtn_.classList.toggle('hidden')
    consoleSection_.classList.toggle('hidden')
  })

  const minBtnInner_ = document.createElement('span')
  minBtnInner_.textContent = '-'
  minBtn_.appendChild(minBtnInner_)
  tabButtonContainer.appendChild(minBtn_)

  // END CONSOLE TAB BUTTON SECTION

  const updateActions = () => {
    actionsTab_.textContent = ''
    Object.entries(actions_).forEach(([k, {text, action}]) => {
      const btn = document.createElement('button')
      btn.classList.add('actions-button')
      btn.textContent = text || k
      btn.addEventListener('click', action)
      actionsTab_.appendChild(btn)
    })
  }

  const setActions = (actions: Record<string, Action>) => {
    Object.assign(actions_, actions)
    updateActions()
  }

  const clearActions = (tags: string[]) => tags.forEach((tag) => {
    if (tag === 'Recenter') {
      return
    }
    delete actions_[tag]
    updateActions()
  })

  const handleTabButtonClick = (button: HTMLButtonElement) => {
    const tabId = button.value
    const tabElement = tabs_.find(e => e.id === tabId)
    const tabButtons = document.getElementsByClassName('tab-button')
    // eslint-disable-next-line no-restricted-syntax, prefer-const
    for (let tabButton of tabButtons) {
      if (tabButton.id === button.id) {
        tabButton.classList.remove('unselected-tab-button')
        tabButton.classList.add('selected-tab-button')
      } else {
        tabButton.classList.remove('selected-tab-button')
        tabButton.classList.add('unselected-tab-button')
      }
    }

    if (!logdiv_) {
      return
    }

    if (logdiv_.children.length && logdiv_.children[0].id !== tabId) {
      logdiv_.removeChild(logdiv_.children[0])
      if (tabElement) {
        logdiv_.appendChild(tabElement)
      }
    }
    logdiv_.scrollTop = logdiv_.scrollHeight
  }

  const createConsoleLogDiv = () => {
    if (!window.XR8) {
      requestAnimationFrame(createConsoleLogDiv)
      return
    }

    if (logdiv_) {
      return
    }

    logdiv_ = document.createElement('div')
    consoleSection_.appendChild(logdiv_)
    logdiv_.id = 'logdiv'
    logdiv_.classList.add('log-div')

    // Display Log at start up
    logdiv_.appendChild(logTab_)

    // add field for camera position
    const statsCameraPositionGroup = document.createElement('div')
    statsCameraPositionGroup.classList.add('camera-stats')
    cameraTab_.appendChild(statsCameraPositionGroup)

    const statsCameraPositionTitle = document.createElement('span')
    statsCameraPositionTitle.innerHTML = 'Position'
    statsCameraPositionTitle.style.textDecoration = 'underline'
    statsCameraPositionGroup.appendChild(statsCameraPositionTitle)

    const statsCameraPositionValue = document.createElement('div')
    statsCameraPositionGroup.appendChild(statsCameraPositionValue)

    // add field for camera rotation
    const statsCameraRotationGroup = document.createElement('div')
    statsCameraRotationGroup.classList.add('camera-stats')
    cameraTab_.appendChild(statsCameraRotationGroup)

    const statsCameraRotationTitle = document.createElement('span')
    statsCameraRotationTitle.innerHTML = 'Rotation'
    statsCameraRotationTitle.style.textDecoration = 'underline'
    statsCameraRotationGroup.appendChild(statsCameraRotationTitle)

    const statsCameraRotationValue = document.createElement('div')
    statsCameraRotationGroup.appendChild(statsCameraRotationValue)

    // Add field for sky percentage. Lazy initialize content once we see we need it.
    let statsSkyPercentageGroup: HTMLDivElement | null = null
    let statsSkyPercentageValue: HTMLDivElement | null = null

    const consoleModule = {
      name: 'xrhudconsolemodule',
      onUpdate: ({processCpuResult}) => {
        const {reality, facecontroller, handcontroller, layerscontroller} = processCpuResult
        if (!reality && !facecontroller && !handcontroller && !layerscontroller) {
          return
        }
        const {rotation, position} =
          (reality || facecontroller || handcontroller || layerscontroller)
        if (!rotation || !position) {
          statsCameraRotationValue.textContent = 'WebAR Not Enabled'
          statsCameraPositionValue.textContent = 'WebAR Not Enabled'
          return
        }

        // Convert quaternion to y-x-z euler angles.
        const wx = rotation.w * rotation.x
        const wy = rotation.w * rotation.y
        const wz = rotation.w * rotation.z
        const xx = rotation.x * rotation.x
        const xy = rotation.x * rotation.y
        const xz = rotation.x * rotation.z
        const yy = rotation.y * rotation.y
        const yz = rotation.y * rotation.z
        const zz = rotation.z * rotation.z

        const m00 = 1 - 2 * (yy + zz)
        const m02 = 2 * (xz + wy)
        const m10 = 2 * (xy + wz)
        const m11 = 1 - 2 * (xx + zz)
        const m12 = 2 * (yz - wx)
        const m20 = 2 * (xz - wy)
        const m22 = 1 - 2 * (xx + yy)

        const R2D = 180.0 / Math.PI

        const rx = Math.asin(Math.max(-1, Math.min(-m12, 1))) * R2D
        const hasZ = Math.abs(m12) < (1 - 1e-7)
        const ry = hasZ ? Math.atan2(m02, m22) * R2D : Math.atan2(-m20, m00) * R2D
        const rz = hasZ ? Math.atan2(m10, m11) * R2D : 0

        statsCameraRotationValue.textContent = `${rx.toFixed(0)} ${ry.toFixed(0)} ${rz.toFixed(0)}`
        statsCameraPositionValue.textContent =
          `${position.x.toFixed(1)} ${position.y.toFixed(1)} ${position.z.toFixed(1)}`

        if (layerscontroller) {
          if (!statsSkyPercentageGroup) {
            statsSkyPercentageGroup = document.createElement('div')
            statsSkyPercentageGroup.classList.add('camera-stats')
            cameraTab_.appendChild(statsSkyPercentageGroup)

            const statsSkyPercentageTitle = document.createElement('span')
            statsSkyPercentageTitle.innerHTML = 'Sky'
            statsSkyPercentageTitle.style.textDecoration = 'underline'
            statsSkyPercentageGroup.appendChild(statsSkyPercentageTitle)

            statsSkyPercentageValue = document.createElement('div')
            statsSkyPercentageGroup.appendChild(statsSkyPercentageValue)
          }
          if (statsSkyPercentageValue) {
            statsSkyPercentageValue.textContent =
            `${((layerscontroller.layers?.sky?.percentage || 0) * 100).toFixed(1)}%`
          }
        }
      },
      onDetach: () => {
        if (statsSkyPercentageGroup) {
          statsSkyPercentageGroup.parentNode?.removeChild(statsSkyPercentageGroup)
          statsSkyPercentageGroup = null
          statsSkyPercentageValue = null
        }
      },
    }

    window.XR8.addCameraPipelineModules([consoleModule])

    consoleSection_.appendChild(tabButtonContainer)
    // CONSOLE TAB SWITCH
    consoleTabButton_.onclick = () => handleTabButtonClick(consoleTabButton_)
    actionsTabButton_.onclick = () => handleTabButtonClick(actionsTabButton_)
    cameraTabButton_.onclick = () => handleTabButtonClick(cameraTabButton_)
  }

  let prevLogStr_ = ''
  let currLogStr_ = ''
  let count = 0
  const appendLog = (prefix: string, suffix: string) => (args: any[]) => {
    if (logdiv_) {
      currLogStr_ = getPrintableArgs(args)
      if (prevLogStr_ === currLogStr_) {
        count += 1
        if (logTab_.lastChild) {
          logTab_.removeChild(logTab_.lastChild)
        }
        // eslint-disable-next-line max-len
        logstream_ = `${logStreamLast_} ${prefix}<b class='console-dup'>${count}</b> ${currLogStr_}${suffix}`
      } else {
        count = 0
        logStreamLast_ = logstream_
        logstream_ = `${logstream_} ${prefix}${currLogStr_}${suffix}`
      }
      logTab_.innerHTML = logstream_
      logdiv_.scrollTop = logTab_.scrollHeight
      prevLogStr_ = currLogStr_
    }
  }

  const enableLogToScreen = (): XrHudLogger => {
    createConsoleLogDiv()
    updateActions()
    return {
      log: appendLog('<span>| ', '</span>'),
      info: appendLog('<span>| ', '</span>'),
      warn: appendLog('<span class="console-warn">| ', '</span>'),
      error: appendLog('<span class="console-error">| ', '</span>'),
    }
  }

  const disable = () => {
    removeDom(consoleSection_)
    removeDom(maxBtn_)
  }

  const enable = ({console: cs}: Pick<XrHudBaseSettings, 'console'>): XrHudLogger | undefined => {
    if (!cs) {
      disable()
      return undefined
    }
    document.body.appendChild(consoleSection_)
    document.body.appendChild(maxBtn_)
    return enableLogToScreen()
  }

  return {
    disable,
    enable,
    setActions,
    clearActions,
  }
}

export {
  ConsoleManager,
}
