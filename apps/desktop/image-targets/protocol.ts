import {protocol, CustomScheme} from 'electron'

import {handleImageTargetRequest} from './image-target-handler'

const IMAGE_TARGETS_SCHEME: CustomScheme = {
  scheme: 'image-targets',
  privileges: {
    supportFetchAPI: true,
    stream: true,
  },
}

const registerImageTargetsHandler = () => {
  protocol.handle(IMAGE_TARGETS_SCHEME.scheme, handleImageTargetRequest)
}

export {
  IMAGE_TARGETS_SCHEME,
  registerImageTargetsHandler,
}
