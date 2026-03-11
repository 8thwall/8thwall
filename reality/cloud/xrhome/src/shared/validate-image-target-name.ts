import {useEnclosedApp} from '../client/apps/enclosed-app-context'
import {useSelector} from '../client/hooks'
import {DISALLOWED_NAME_CHARACTERS} from './image-target-constants'

const useOtherImageNames = (thisUuid: string) => {
  const app = useEnclosedApp()
  const targetsByUuid = useSelector(s => s.imageTargets.targetsByUuid)
  const targetInfo = useSelector(s => s.imageTargets.targetInfoByApp[app.uuid])
  const imageTargets = targetInfo?.targetUuids?.map(uuid => targetsByUuid[uuid])
  return imageTargets?.filter(it => it.uuid !== thisUuid).map(it => it.name)
}

type ValidateOptions = {
  otherImageNames?: string[]
}

const validateImageTargetName = (name: string, options?: ValidateOptions) => {
  if (!name) {
    return 'You need to specify name'
  } else if (options?.otherImageNames.includes(name)) {
    return 'Another image target has the same name'
  } else if (DISALLOWED_NAME_CHARACTERS.some(char => name.includes(char))) {
    return `Image target name may not contain: ${DISALLOWED_NAME_CHARACTERS.join(', ')}`
  } else {
    return null
  }
}

export {
  useOtherImageNames,
  validateImageTargetName,
}
