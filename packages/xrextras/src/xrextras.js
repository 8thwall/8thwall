import {AFrameFactory} from './aframe/aframe'
import {DebugWebViewsFactory} from './debugwebviews/debug-web-views'
import {FullWindowCanvasFactory} from './fullwindowcanvasmodule/full-window-canvas-module'
import {LifecycleFactory} from './lifecyclemodule/lifecycle'
import {PauseOnBlurFactory} from './pauseonblurmodule/pauseonblur'
import {PauseOnHiddenFactory} from './pauseonhiddenmodule/pauseonhidden'
import {PlayCanvasFactory} from './playcanvas/playcanvas'
import {PwaInstallerFactory} from './pwainstallermodule/pwa-installer-module'
import {StatsFactory} from './statsmodule/stats'
import {ThreeExtrasFactory} from './three/three-extras'
import {MediaRecorder} from './mediarecorder/mediarecorder'
import {SessionReconfigureFactory} from './sessionreconfiguremodule/session-reconfigure-module'
import {StartFlowFactory} from './startflow'

import './common.css'

const XRExtras = {
  AFrame: AFrameFactory(),
  DebugWebViews: DebugWebViewsFactory(),
  FullWindowCanvas: FullWindowCanvasFactory(),
  Lifecycle: LifecycleFactory(),
  PauseOnBlur: PauseOnBlurFactory(),
  PauseOnHidden: PauseOnHiddenFactory(),
  PlayCanvas: PlayCanvasFactory(),
  PwaInstaller: PwaInstallerFactory(),
  Stats: StatsFactory(),
  ThreeExtras: ThreeExtrasFactory(),
  MediaRecorder,
  SessionReconfigure: SessionReconfigureFactory(),
  StartFlow: StartFlowFactory(),
}

const setDeprecatedProperty = (object, property, value, message) => {
  let warned = false

  Object.defineProperty(object, property, {
    get: () => {
      if (!warned) {
        warned = true
        /* eslint-disable no-console */
        console.warn('[XR] Deprecation Warning:', message)
        console.warn(Error().stack.replace(/^Error.*\n\s*/, '').replace(/\n\s+/g, '\n'))
        /* eslint-enable no-console */
      }
      return value
    },
  })
}

const setRenameDeprecation = (oldName, newName, value, version) => {
  const message = `XRExtras.${oldName} was deprecated in ${version}. Use ${newName} instead.`
  setDeprecatedProperty(XRExtras, oldName, value, message)
}

setRenameDeprecation('PauseOnBlur', 'PauseOnHidden', XRExtras.PauseOnBlur, 'R17.0')

export {
  XRExtras,
}
