import {protocol, CustomScheme} from 'electron'

import {handlePreferencesRequest} from './preferences-handler'

const PREFERENCES_SCHEME: CustomScheme = {
  scheme: 'preferences',
  privileges: {
    supportFetchAPI: true,
    stream: true,
  },
}

const registerPreferencesHandler = () => {
  protocol.handle(PREFERENCES_SCHEME.scheme, handlePreferencesRequest)
}

export {
  PREFERENCES_SCHEME,
  registerPreferencesHandler,
}
