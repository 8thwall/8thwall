import type {BrowserWindow} from 'electron'

const ensureWindowOnTop = (win: BrowserWindow) => {
  if (win.isMinimized()) win.restore()
  win.focus()

  if (process.platform === 'win32') {
    win.setAlwaysOnTop(true)
    setTimeout(() => {
      win.setAlwaysOnTop(false)
    }, 100)
  }
}

export {
  ensureWindowOnTop,
}
