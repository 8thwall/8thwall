import {h, FunctionComponent} from 'preact'

import {InAppBrowserScreen} from './in-app-browser-screen'
import {LoadingScreen} from './loading-screen'
import {PermissionsErrorScreen} from './permissions-error-screen'
import {StartScreen} from './start-screen'
import type {StartFlowViewProps} from './types'
import {UnexpectedErrorScreen} from './unexpected-error-screen'
import {UnsupportedBrowserScreen} from './unsupported-browser-screen'
import {UnsupportedDeviceScreen} from './unsupported-device-screen'

const StartFlowView: FunctionComponent<StartFlowViewProps> = ({
  options, screen, strings,
}) => {
  switch (screen) {
    case 'unsupported-device':
      return <UnsupportedDeviceScreen strings={strings} />
    case 'unsupported-browser':
      return <UnsupportedBrowserScreen strings={strings} />
    case 'in-app-browser':
      return <InAppBrowserScreen strings={strings} />
    case 'start':
      return <StartScreen options={options} strings={strings} />
    default:
      switch (screen.type) {
        case 'loading':
          return (
            <LoadingScreen
              requestingPermissions={screen.requestingPermissions}
              strings={strings}
            />
          )
        case 'permissions-error':
          return <PermissionsErrorScreen options={options} screen={screen} strings={strings} />
        case 'unexpected-error':
          return <UnexpectedErrorScreen options={options} screen={screen} strings={strings} />
        default:
          return null
      }
  }
}

export {StartFlowView}
