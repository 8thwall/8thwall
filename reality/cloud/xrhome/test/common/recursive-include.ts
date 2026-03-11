import {assert} from 'chai'

const assertRecursiveInclude = (superset: any, subset: any, message?: string) => {
  let pairs: [any, any][]

  if (Array.isArray(subset)) {
    assert.isArray(superset, message)
    assert.strictEqual(superset.length, subset.length, message)
    pairs = subset.map((subValue, i) => [superset[i], subValue])
  } else {
    assert.isObject(superset, message)
    assert.isObject(subset, message)
    pairs = Object.keys(subset).map(key => [superset[key], subset[key]])
  }

  pairs.forEach(([superValue, subValue]) => {
    if (subValue && typeof subValue === 'object') {
      assertRecursiveInclude(superValue, subValue)
    } else {
      assert.strictEqual(superValue, subValue, message)
    }
  })
}

export {
  assertRecursiveInclude,
}
