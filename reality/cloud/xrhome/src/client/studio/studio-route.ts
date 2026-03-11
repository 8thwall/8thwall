import type {Location} from 'history'

import type {EditorRouteParams} from '../editor/editor-route'

const parseStudioLocation = (location: Location<unknown>): EditorRouteParams => {
  if (!location) {
    return ''
  }
  const params = new URLSearchParams(location.search)
  const fileParam = params.get('file')
  if (fileParam) {
    // is a file in project
    return fileParam
  }
  return ''
}

const extractRemainingUrlParams = (
  queryParams: string, toExclude: string[]
): Record<string, string> => {
  const params = new URLSearchParams(queryParams)
  const result: Record<string, string> = {}
  params.forEach((value, key) => {
    if (!toExclude.includes(key)) {
      result[key] = value
    }
  })
  return result
}

export {
  parseStudioLocation,
  extractRemainingUrlParams,
}
