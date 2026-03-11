import type {ILog} from './types'

const countReducer = (type: string) => (count: number, l: ILog): number => (
  l.type === type ? count + l.numRedundant : count
)

const countType = (logs: readonly ILog[], type: string) => (
  logs?.reduce(countReducer(type), 0) || 0
)

export {countReducer, countType}
