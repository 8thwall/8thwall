import {assert} from '@repo/bzl/js/chai-js'

// @dep(//c8/ecs/tools:generated-features-list)
import * as FEATURES from '@repo/c8/ecs/tools/generated-features-list'

import {EDITIONS} from '@repo/c8/ecs/src/shared/features/edition'

const EXPECTED_EDITIONS: string[][] = []

while (EXPECTED_EDITIONS.length < EDITIONS.length) {
  EXPECTED_EDITIONS.push([])
}

Object.values(FEATURES)
  .filter(e => typeof e === 'object' && !Array.isArray(e))
  .forEach((feature: any) => {
    // NOTE(christoph): All features on past editions must be present in the editions list
    if (feature.edition < EDITIONS.length) {
      EXPECTED_EDITIONS[feature.edition].push(feature.name)
      EXPECTED_EDITIONS[feature.edition].sort()
    }
  })

assert.deepEqual(
  EDITIONS, EXPECTED_EDITIONS,
  `Mismatch: ${JSON.stringify(EXPECTED_EDITIONS, null, 2)} vs ${JSON.stringify(EDITIONS, null, 2)}`
)
