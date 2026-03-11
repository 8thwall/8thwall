import type {Path, Paths, PathValue, PathValues} from '@studiomcp/schema/common'
import type {GraphObject} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

const getPathValue = (object: DeepReadonly<GraphObject>, path: Path) => {
  const pathParts = path.split('.')
  let current = object

  for (const part of pathParts) {
    if (current?.[part] === undefined) {
      return undefined
    }
    current = current[part]
  }
  return current
}

const getPathValues = (
  object: DeepReadonly<GraphObject>,
  paths: Paths
) => paths.reduce((acc, path) => ({
  ...acc,
  [path]: getPathValue(object, path),
}), {} as PathValues)

const setPathValue = (
  object: DeepReadonly<GraphObject>,
  path: Path,
  pathValue: PathValue
): DeepReadonly<GraphObject> => {
  const pathParts = path.split('.')
  const _setPathValue = (o: any, parts: string[]) => {
    if (parts.length === 0) {
      return pathValue
    }
    const [first, ...rest] = parts
    if (o?.[first] === undefined) {
      if (rest.length > 0) {
        return {
          ...o,
          [first]: _setPathValue({}, rest),
        }
      }
      return {
        ...o,
        [first]: pathValue,
      }
    }
    return {
      ...o,
      [first]: _setPathValue(o[first], rest),
    }
  }
  return _setPathValue(object, pathParts)
}

const setPathValues = (
  object: DeepReadonly<GraphObject>,
  pathValues: PathValues
): DeepReadonly<GraphObject> => {
  let updatedObject = object
  Object.entries(pathValues).forEach(([path, pathValue]) => {
    updatedObject = setPathValue(updatedObject, path, pathValue)
  })
  return updatedObject
}

export {
  getPathValues,
  setPathValues,
}
