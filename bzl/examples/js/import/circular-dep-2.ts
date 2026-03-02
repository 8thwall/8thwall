// @sublibrary(:circular-lib)
import {circularDep1} from './circular-dep-1'  // eslint-disable-line import/no-cycle

const circularDep2 = (depth: number) => {
  // eslint-disable-next-line no-console
  console.log('hello from circularDep2')
  if (depth > 1) {
    circularDep1(depth - 1)
  }
}

export {
  circularDep2,
}
