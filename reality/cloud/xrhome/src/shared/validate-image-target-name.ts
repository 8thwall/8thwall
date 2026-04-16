import {DISALLOWED_NAME_CHARACTERS} from './image-target-constants'

// TODO(christoph): Remove this
const useOtherImageNames: any = () => []

type ValidateOptions = {
  otherImageNames?: string[]
}

const validateImageTargetName = (name: string, options?: ValidateOptions) => {
  if (!name) {
    return 'You need to specify name'
  } else if (options?.otherImageNames?.includes(name)) {
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
