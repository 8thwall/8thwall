// @rule(js_binary)
// @package(npm-desktop)
// @attr(esnext = 1)
// @attr(target = "node")
// @attr(externals = "electron, sharp")

import path from 'path'
import {app, BrowserWindow, protocol, dialog, ipcMain} from 'electron'
import {autoUpdater} from 'electron-updater'
import log from 'electron-log'

import {initDb} from '../application-state'
import {FILE_SYNC_SCHEME, registerFileSyncHandler} from './file-sync/protocol'
import {
  DESKTOP_SCHEME, maybeUpdateOnBeforeRequest, registerDesktopAppHandler,
  registerWindowOpenHandler,
} from './desktop-app/protocol'
import {registerOnOpenUrlHandler} from './desktop-app/register-on-open-url-handler'
import {STUDIO_HUB_PROTOCOL} from './desktop-protocol'
import {setUpMainFileWatchPort} from './file-watch/ports'
import {PREFERENCES_SCHEME, registerPreferencesHandler} from './preferences/protocol'
import {registerSecondInstanceHandler} from './desktop-app/register-second-instance-handler'
import {registerPermissionHandler} from './desktop-app/permissions'
import {navigateToDeepLink} from './desktop-app/deep-link'
import {setupMenu} from './menu'
import {createDev8WebSocketServer} from './dev8-socket/dev8-socket-server'
import {IMAGE_TARGETS_SCHEME, registerImageTargetsHandler} from './image-targets/protocol'

const CONSOLE_WEBSOCKET_PORT = 62009

const UPDATE_CHECK_INTERVAL = 60 * 60 * 1000

const setupAutoUpdater = (win: BrowserWindow) => {
  autoUpdater.logger = log

  const appVersion = app.getVersion()
  if (appVersion.includes('-beta')) {
    autoUpdater.channel = 'beta'
  }

  autoUpdater.checkForUpdatesAndNotify()

  setInterval(() => {
    autoUpdater.checkForUpdatesAndNotify()
  }, UPDATE_CHECK_INTERVAL)

  autoUpdater.on('update-downloaded', () => {
    const result = dialog.showMessageBoxSync(win, {
      type: 'info',
      buttons: ['Restart now', 'Later'],
      title: 'Update ready',
      message: 'A new version has been downloaded. Restart now to apply updates?',
    })
    if (result === 0) {
      autoUpdater.quitAndInstall()
    }
  })
}

const gotTheLock = app.requestSingleInstanceLock()
if (!gotTheLock) {
  log.info('Another instance is already running, quitting...')
  app.quit()
  process.exit(0)
}

const distRoot = (() => {
  if (app.isPackaged) {
    // In packaged app, desktop-dist should be in Resources directory (outside asar)
    return path.resolve(process.resourcesPath, 'desktop-dist')
  } else {
    // In development, use the original path
    return path.resolve(__dirname, '../../../../reality/cloud/xrhome/desktop-dist')
  }
})()

try {
  // Write to ~/Library/Application Support/desktop (or 8th Wall on prod) on mac
  const userDataPath = app.getPath('userData')
  initDb(userDataPath)
} catch (error) {
  // eslint-disable-next-line no-console
  console.error('Failed to initialize database:', error)
  process.exit(1)
}

app.commandLine.appendSwitch('ignore-certificate-errors')

const createWindow = () => {
  const win = new BrowserWindow({
    width: 1600,
    height: 1200,
    minWidth: 900,
    minHeight: 700,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      devTools: !process.env.RELEASE,
    },
    frame: false,
    title: app.getName(),
    titleBarStyle: 'hidden',
    ...(process.platform !== 'darwin'
      ? {
        titleBarOverlay: {
          symbolColor: '#ffffff',
          color: '#000000',
        },
      }
      : {
        trafficLightPosition: {x: 10, y: 10},
      }),
    backgroundColor: '#fff',
  })
  win.loadURL('desktop://dist/index.html')
  registerWindowOpenHandler(win)
  registerOnOpenUrlHandler(win)
  registerSecondInstanceHandler(win)
  registerPermissionHandler(win)

  // IPC handlers for window controls
  ipcMain.on('minimize-window', () => {
    win.minimize()
  })
  ipcMain.on('maximize-window', () => {
    if (win.isMaximized()) {
      win.unmaximize()
    } else {
      win.maximize()
    }
  })

  ipcMain.on('close-window', () => {
    win.close()
  })
  return win
}

const ws = createDev8WebSocketServer(CONSOLE_WEBSOCKET_PORT)

app.on('before-quit', () => {
  ws.close()
})

app.on('window-all-closed', () => {
  app.quit()
})

protocol.registerSchemesAsPrivileged([
  DESKTOP_SCHEME,
  FILE_SYNC_SCHEME,
  PREFERENCES_SCHEME,
  IMAGE_TARGETS_SCHEME,
])

// NOTE(johnny): Remove trailing colon
const protocolName = STUDIO_HUB_PROTOCOL.slice(0, -1)
log.info('Registering as default protocol client for:', protocolName)
log.info('Current platform:', process.platform)

if (app.setAsDefaultProtocolClient(protocolName)) {
  log.info('Successfully registered as default protocol client')
} else {
  log.error('Failed to register as default protocol client')
}

app.whenReady()
  .then(async () => {
    registerDesktopAppHandler(distRoot)
    registerFileSyncHandler()
    registerPreferencesHandler()
    registerImageTargetsHandler()
    maybeUpdateOnBeforeRequest()

    const win = createWindow()

    setupMenu()

    win.webContents.on('did-finish-load', () => {
      setUpMainFileWatchPort(win)
      navigateToDeepLink(win, process.argv.pop() || '')
    })

    if (app.isPackaged) {
      setupAutoUpdater(win)
    }
  })
