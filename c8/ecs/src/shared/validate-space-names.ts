import type {Spaces} from './scene-graph'

const validateSpaceNames = (spaces: Spaces): boolean => {
  if (!spaces) {
    return true
  }
  const spaceNames = new Set<string>()
  let isValid = true
  Object.values(spaces).forEach(({name}) => {
    if (spaceNames.has(name)) {
      isValid = false
    }
    spaceNames.add(name)
  })
  return isValid
}

export {
  validateSpaceNames,
}
