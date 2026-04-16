import type {IImageTarget} from '../common/types/models'
import type {ImageTargetFilterOptions} from './types'

const targetMatchesFilter = (target: IImageTarget, filter: ImageTargetFilterOptions): boolean => {
  if (filter.nameLike) {
    const lowerFilter = filter.nameLike.toLowerCase()
    if (!target.name.toLowerCase().includes(lowerFilter)) {
      return false
    }
  }

  if (filter.type && filter.type.length) {
    if (!filter.type.includes(target.type)) {
      return false
    }
  }

  if (filter.hasMetadata) {
    if (!target.userMetadata) {
      return false
    }
  }

  return true
}

export {
  targetMatchesFilter,
}
