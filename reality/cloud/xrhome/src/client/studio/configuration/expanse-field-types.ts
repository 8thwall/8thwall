import type {DefaultableConfigDiffInfo} from './diff-chip-types'

type ExpanseField<DIFFTYPE extends DefaultableConfigDiffInfo<unknown, string[][]>> =
  | {
    leafPaths: string | string[][]
    overridePaths?: never
    diffOptions?: DIFFTYPE
  }
  | {
    overridePaths: string[][]
    leafPaths?: never
    diffOptions?: DIFFTYPE
  }
  // Equivalent to leafPaths: [[yourString]]
  | string

export type {
  ExpanseField,
}
