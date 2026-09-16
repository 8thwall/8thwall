import {protocol, CustomScheme} from 'electron'

import {isProjectPath, isFilePath} from './paths'
import {handleProjectRequest} from './project-handler'
import {handleFileRequest} from './file-handler'

const FILE_SYNC_SCHEME: CustomScheme = {
  scheme: 'file-sync',
  privileges: {
    supportFetchAPI: true,
    stream: true,
  },
}

const registerFileSyncHandler = () => {
  protocol.handle('file-sync', (request) => {
    const requestUrl = new URL(request.url)
    const {pathname} = requestUrl

    if (isProjectPath(pathname)) {
      return handleProjectRequest(request)
    }

    if (isFilePath(pathname)) {
      return handleFileRequest(request)
    }

    return new Response('Not found', {status: 404})
  })
}

export {
  FILE_SYNC_SCHEME,
  registerFileSyncHandler,
}
