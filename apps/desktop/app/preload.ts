// @rule(js_binary)
// @package(npm-desktop)
// @attr(esnext = 1)
// @attr(externals = "electron")
import {contextBridge, ipcRenderer} from 'electron'
import {ELECTRON_API_KEY, type ElectronApi} from '@repo/reality/shared/desktop/electron-api'

import {createFileWatchApi} from './file-watch/api'
import {STUDIO_HUB_PROTOCOL} from './desktop-protocol'

const electronApi: ElectronApi = {
  os: process.platform === 'darwin' ? 'mac' : 'other',
  onExternalNavigate: (callback) => {
    const listener = (_event: any, pathAndQuery: string) => {
      callback(pathAndQuery)
    }
    ipcRenderer.on('navigate-to-path', listener)

    return () => {
      ipcRenderer.removeListener('navigate-to-path', listener)
    }
  },
  fileWatch: createFileWatchApi(),
  studiohubProtocol: STUDIO_HUB_PROTOCOL,
  minimizeWindow: () => { ipcRenderer.send('minimize-window') },
  maximizeWindow: () => { ipcRenderer.send('maximize-window') },
  closeWindow: () => { ipcRenderer.send('close-window') },
}
contextBridge.exposeInMainWorld(ELECTRON_API_KEY, electronApi)
