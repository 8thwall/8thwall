import {app, BrowserWindow} from 'electron'

import {navigateToDeepLink} from './deep-link'

const registerOnOpenUrlHandler = (win: BrowserWindow) => {
  app.on('open-url', async (event, url) => {
    event.preventDefault()
    navigateToDeepLink(win, url)
  })
}

export {
  registerOnOpenUrlHandler,
}
