import type {BrowserWindow} from 'electron'

const registerPermissionHandler = (win: BrowserWindow) => {
  win.webContents.session.setPermissionRequestHandler((_, permission, callback) => {
    const allowedPermissions = ['media', 'geolocation']

    // NOTE(lreyna): We can add more logic here to allow/block based on different factors.
    // In the future we may want to check the requesting URL origin.
    // For now, we can just auto grant permissions. OSX will still prompt users on the app level.
    if (allowedPermissions.includes(permission)) {
      callback(true)
    } else {
      callback(false)
    }
  })
}

export {
  registerPermissionHandler,
}
