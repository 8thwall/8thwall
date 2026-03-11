import type {BrowserWindow} from 'electron'

import {STUDIO_HUB_PROTOCOL} from '../desktop-protocol'
import {ensureWindowOnTop} from './window-visibility'

const navigateToDeepLink = (win: BrowserWindow, url: string) => {
  if (!url || !win) {
    return
  }
  if (!url.startsWith(STUDIO_HUB_PROTOCOL)) {
    return
  }
  try {
    const parsedUrl = new URL(url)
    if (parsedUrl.protocol !== STUDIO_HUB_PROTOCOL) {
      return
    }
    const pathAndQuery = `${parsedUrl.pathname}${parsedUrl.search}`
    win.webContents.send('navigate-to-path', pathAndQuery)
    ensureWindowOnTop(win)
  } catch (err) {
    // eslint-disable-next-line no-console
    console.error('Failed to handle navigation:', err)
  }
}

export {
  navigateToDeepLink,
}
