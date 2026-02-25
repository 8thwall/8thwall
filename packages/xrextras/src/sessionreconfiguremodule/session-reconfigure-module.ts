import type {
  SessionReconfigureParameters,
} from '../aframe/components/session-reconfigure-components'

declare const XR8: any
let runConfig

// data BeforeSessionInitializeInput: input for onBeforeSessionInitialize in camera pipeline
// a function that return false on session with attributes that match a certain filter
type SessionPreventer = (data: any) => boolean

const create = () => {
  let sessionPreventer: SessionPreventer

  const restartSession = () => {
    XR8.reconfigureSession({runConfig})
  }

  const restartSessionWith3dFallback = () => {
    XR8.reconfigureSession({
      ...runConfig,
      sessionInitBehavior: 'fallback',
    })
  }

  const enableAnyDirection = () => {
    XR8.reconfigureSession({
      ...runConfig,
      cameraConfig: {...runConfig.cameraConfig, direction: 'any'},
    })
  }

  const disableVoidSpace = () => {
    XR8.reconfigureSession({
      ...runConfig,
      sessionConfiguration: {
        ...runConfig.sessionConfiguration,
        defaultEnvironment: {
          ...runConfig.sessionConfiguration?.defaultEnvironment,
          disabled: true,
        },
      },
    })
  }

  const enableVoidSpace = () => {
    XR8.reconfigureSession({
      ...runConfig,
      sessionConfiguration: {
        ...runConfig.sessionConfiguration,
        defaultEnvironment: {
          ...runConfig.sessionConfiguration?.defaultEnvironment,
          disabled: false,
        },
      },
    })
  }

  const preventHeadset = (data) => {
    if (data.sessionAttributes.usesWebXr) {
      return true
    }
    return false
  }

  const preventCamera = (data) => {
    if (data.sessionAttributes.fillsCameraTexture) {
      return true
    }
    return false
  }

  const configure = ({skipCameraSession}: SessionReconfigureParameters) => {
    if (skipCameraSession) {
      sessionPreventer = preventCamera
    }
  }

  const pipelineModule = () => {
    let el
    let errorMessageEl_
    const showErrorMessage = (msg: string) => {
      if (!errorMessageEl_) {
        errorMessageEl_ = document.createElement('div')
        errorMessageEl_.style.position = 'fixed'
        errorMessageEl_.style.left = '50vw'
        errorMessageEl_.style.top = '50vh'
        errorMessageEl_.style.transform = 'translate(-50%, -50%)'
        errorMessageEl_.style.zIndex = '9000'
        errorMessageEl_.style.color = 'white'
        document.body.appendChild(errorMessageEl_)
      }
      errorMessageEl_.textContent += ` ${msg}`
    }
    const removeErrorMessage = () => {
      if (errorMessageEl_) {
        document.body.removeChild(errorMessageEl_)
      }
      errorMessageEl_ = null
    }

    let sessionPreventer_ = false
    return {
      name: 'session-reconfigurator',
      onRunConfigure: (data) => {
        runConfig = data.config
        sessionPreventer_ = !!sessionPreventer
      },
      onBeforeSessionInitialize: (data) => {
        if (sessionPreventer && sessionPreventer(data)) {
          throw new Error('Session preventer choose to skip session')
        }
      },
      onCameraStatusChange: ({status}) => {
        if (status === 'failed') {
          // User has allowed deviceorientation, devicemotion, but camera failed
          showErrorMessage('Requesting camera permission failed.')
        } else if (status === 'hasStream') {
          // User has succeeded in granting all permissions
          removeErrorMessage()
        } else if (status === 'hasDesktop3D') {
          // We are in fallback mode, but did we try opt to prevent camera?
          if (!sessionPreventer_) {
            // We did not prevent camera, so we should have gotten camera access.
            // We are still in desktop3d, so we must have gotten here via a fallback.
            // Might be nice if we can figure out the sessions that got tried.
            showErrorMessage('Using desktop 3D camera fallback.')
          }
        }
      },
      onAttach: () => {
        if (!XR8.reconfigureSession) {
          throw new Error('Invalid XR8 version, missing reconfigureSession')
        }
        el = document.createElement('div')

        Object.assign(el.style, {
          position: 'fixed',
          left: 0,
          top: 0,
          padding: '1rem',
          zIndex: 900000,
        })

        const options = [
          // {name: 'restart same', action: restartSession},
          // {name: 'any direction camera', action: enableAnyDirection},
          // {name: 'prevent xr', action: restartSession, sessionPreventer: preventHeadset},
          {name: 'enable xr', action: restartSessionWith3dFallback},
          // {name: 'disable void', action: disableVoidSpace},
          // {name: 'enable void', action: enableVoidSpace},
          {name: 'prevent camera', action: restartSession, sessionPreventer: preventCamera},
        ]

        options.forEach((option) => {
          const button = document.createElement('button')
          button.textContent = option.name
          el.appendChild(button)
          button.addEventListener('click', () => {
            sessionPreventer = option.sessionPreventer
            option.action()
          })
        })

        document.body.appendChild(el)
      },
      onDetach: () => {
        if (el) {
          document.body.removeChild(el)
          el = null
        }
        removeErrorMessage()
      },
    }
  }

  return {
    pipelineModule,
    configure,
    restartSession,
    enableAnyDirection,
    disableVoidSpace,
    enableVoidSpace,
    preventCamera,
    preventHeadset,
  }
}
let SessionReconfigureModule = null

const SessionReconfigureFactory = () => {
  if (SessionReconfigureModule == null) {
    SessionReconfigureModule = create()
  }

  return SessionReconfigureModule
}

export {
  SessionReconfigureFactory,
}
