// If no resourceCache is provided, we use a cache that lasts until the page is closed
const defaultCache = {}

// In order to use objects as keys we need to use a WeakMap otherwise all objects are serialized
// to the same '[object Object]` string.
const WEAK_MAP_KEY_SYMBOL = Symbol('cache-weak-map-key')

const cacheSet = (bucket, key, value, options) => {
  if (key === null || key === undefined) {
    throw new Error('Cannot set null key')
  }
  const cache = options.resourceCache || defaultCache
  if (!cache[bucket]) {
    cache[bucket] = {}
  }
  if (typeof key === 'object') {
    if (!cache[bucket][WEAK_MAP_KEY_SYMBOL]) {
      cache[bucket][WEAK_MAP_KEY_SYMBOL] = new WeakMap()
    }
    cache[bucket][WEAK_MAP_KEY_SYMBOL].set(key, value)
  } else {
    cache[bucket][key] = value
  }
}

const cacheGet = (bucket, key, options) => {
  if (key === null || key === undefined) {
    throw new Error('Cannot set null key')
  }
  const cache = options.resourceCache || defaultCache
  if (!cache[bucket]) {
    return undefined
  }
  if (typeof key === 'object') {
    if (!cache[bucket][WEAK_MAP_KEY_SYMBOL]) {
      return undefined
    }
    return cache[bucket][WEAK_MAP_KEY_SYMBOL].get(key)
  } else {
    return cache[bucket][key]
  }
}

export {
  cacheSet,
  cacheGet,
}
