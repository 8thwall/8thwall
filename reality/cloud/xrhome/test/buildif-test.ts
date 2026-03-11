import {describe, it} from 'mocha'
import {assert} from 'chai'

import {getBuildIfReplacements} from '../src/shared/buildif'

const EXCLUDED_KEYS = [
  'LOCAL_DEV',
  'ALL_QA',
  'EXPERIMENTAL',
  'MATURE',
  'LOCAL',
  'DISABLED',
  'FULL_ROLLOUT',
  'UI_TEST',
  'UNIT_TEST',
]

const flags = getBuildIfReplacements({
  isLocalDev: false,
  isRemoteDev: false,
  isTest: true,
  flagLevel: 'experimental',
}).map(f => f.flag).filter(f => !EXCLUDED_KEYS.includes(f))

describe('BuildIf', () => {
  it('Keys are sorted alphabetically', async () => {
    for (let i = 0; i < flags.length - 1; i++) {
      const [current, next] = flags.slice(i, i + 2)
      assert.isTrue(
        current.localeCompare(next) < 0,
        `${current} comes before ${next} in the list, but alphabetically should be after.`
      )
    }
  })

  describe('Keys end with _YYYYMMDD', () => {
    for (const flag of flags) {
      it(flag, () => {
        const match = flag.match(/_(\d{8})$/)
        assert.isNotNull(match)
      })
    }
  })
})
