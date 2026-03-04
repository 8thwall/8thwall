const createInstanced = <K extends object, V>(create: (k: K) => V) => {
  const instanceMap = new WeakMap<K, V>()

  return (key: K) => {
    if (instanceMap.has(key)) {
      return instanceMap.get(key)!
    }

    const instance = create(key)
    instanceMap.set(key, instance)
    return instance
  }
}

export {
  createInstanced,
}
