import {app, BrowserWindow} from 'electron'

import {navigateToDeepLink} from './deep-link'

const registerSecondInstanceHandler = (win: BrowserWindow) => {
  app.on('second-instance', (_, commandLine) => {
    navigateToDeepLink(win, commandLine.pop() || '')
  })
}

export {
  registerSecondInstanceHandler,
}
