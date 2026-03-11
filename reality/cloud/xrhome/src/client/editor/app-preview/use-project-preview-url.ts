import {DeviceRemoteType, useLocalBuildUrl} from '../../studio/local-sync-context'
import type {IApp} from '../../common/types/models'

const useProjectPreviewUrl = (_: IApp,
  deviceType: DeviceRemoteType = 'same-device') => useLocalBuildUrl(deviceType)

export {useProjectPreviewUrl}
