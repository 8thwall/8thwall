import * as React from 'preact'

import {RequestingCamera} from './requesting-camera'
import {LoadBackground} from './load-background'
import {AndroidCameraPermissionsError} from './android-camera-permissions-error'
import {CameraModeError} from './camera-mode-error'
import {DebugMessage} from './debug-message'

type ILoadingContainer = {
  loading: 'visible' | 'fade-out' | 'hidden'
  debugMessage?: string
} & (
  {
    view: null | 'requesting-camera' | 'android-camera-permissions-error'
  } | {
    view: 'camera-mode-error'
    message: string
  } | {
    view: 'camera-permissions-error-apple'
    appName: string
  } | {
    view: 'android-link-out-view'
  } | {
    view: 'apple-camera-permissions-error'
    appName: string
  }
)

const LoadingContainer = (props: ILoadingContainer) => {
  const renderView = () => {
    switch (props.view) {
      case 'requesting-camera':
        return <RequestingCamera />
      case 'android-camera-permissions-error':
        return <AndroidCameraPermissionsError />
      case 'camera-mode-error':
        return <CameraModeError message={props.message} />
      case 'camera-permissions-error-apple':
        return TODO
      case 'android-link-out-view':
        return TODO
      default:
        return null
    }
  }

    <div id='loadingContainer' className='absolute-fill'>
      {props.loading !== 'hidden' && <LoadBackground fading={props.loading === 'fade-out'} />}
      {renderView()}

      {props.debugMessage && <DebugMessage message={props.debugMessage} />}
    </div>
}

export {
  LoadingContainer,
}

export type {
  ILoadingContainer,
}
