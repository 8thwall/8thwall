// @sublibrary(:circular-lib)
import {circularDep2} from './circular-dep-2'  // eslint-disable-line import/no-cycle

const circularDep1 = (depth: number) => {
  // eslint-disable-next-line no-console
  console.log('hello from circularDep1')
  if (depth > 1) {
    circularDep2(depth - 1)
  }
}

export {
  circularDep1,
}
