import {BUILD_TAG, REPO_TAG} from './log-constants'
import type {ILog, ILogStream} from './types'

const getLastLog = (logStream: ILogStream): ILog | undefined => (
  logStream.logs.length ? logStream.logs[logStream.logs.length - 1] : undefined
)

const logMatchesSearch = (log: ILog, searchString: string) => {
  if (!searchString) {
    return true
  }

  const searchLower = searchString.toLowerCase()
  if (log.text.toLowerCase().includes(searchLower)) {
    return true
  }

  if (log.sourceLocation?.file?.toLowerCase().includes(searchLower)) {
    return true
  }

  return false
}

const makeLogFilter =
  (filterError: boolean, filterWarn: boolean, filterInfo: boolean, searchString: string) => {
    // If all excluded, it's the same as all included
    const hasFilter = filterError !== filterWarn || filterWarn !== filterInfo
    const hasSearch = !!searchString

    if (hasFilter) {
      const includeByType = {
        'error': filterError,
        'warn': filterWarn,
        'log': filterInfo,
      }
      if (hasSearch) {
        return (log: ILog) => includeByType[log.type] && logMatchesSearch(log, searchString)
      } else {
        return (log: ILog) => includeByType[log.type]
      }
    } else if (hasSearch) {
      return (log: ILog) => logMatchesSearch(log, searchString)
    } else {
      // If no filter or search, return null to indicate no filtering
      return null
    }
  }

const makeSystemFilter = (filterBuild: boolean, filterRepo: boolean) => {
  // If all included or all excluded, return null to indicate no filtering
  if (filterBuild === filterRepo) {
    return null
  }

  const includeByTag = {
    [BUILD_TAG]: filterBuild,
    [REPO_TAG]: filterRepo,
  }

  return (log: ILog) => !!log.tags?.some(tag => includeByTag[tag])
}

export {
  getLastLog,
  logMatchesSearch,
  makeLogFilter,
  makeSystemFilter,
}
