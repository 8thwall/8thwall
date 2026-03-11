const initializedSet = new Set<number>()
const registeredSet = new Set<number>()

let keyCounter = 0

const keyToName: Record<number, string> = {}

const registry: Record<number, any> = {}

interface LazyRegistryEntry<T>{
  use: () => Promise<T>
  register: (value: () => Promise<T>) => void
  unregister: () => void
}

interface RegistryEntry<T>{
  use: () => T
  safeUse: () => [T, boolean]
  register: (value: T) => void
  unregister: () => void
}

const register = <T>(key: number, value: T): void => {
  registeredSet.add(key)
  registry[key] = value
}

const unregister = (key: number): void => {
  registeredSet.delete(key)
  delete registry[key]
}

const safeGet = <T>(key: number): [T, boolean] => {
  if (!registeredSet.has(key)) {
    return [null, false]
  }
  return [registry[key], true]
}

const get = <T>(key: number): T => {
  if (!registeredSet.has(key)) {
    throw new Error(`Accessed ${keyToName[key] || '(uninitialized key)'} before register`)
  }
  return registry[key]
}

const lazyGet = <T>(key: number): Promise<T> => {
  const valueOrFunction = get(key) as Promise<T> | (() => Promise<T>)
  if (typeof valueOrFunction === 'function') {
    const value = valueOrFunction()
    register(key, value)
    return value
  }

  return valueOrFunction
}

const key = (name: string): number => {
  const newKey = keyCounter++
  initializedSet.add(newKey)
  keyToName[newKey] = name
  return newKey
}

const validate = () => {
  const unregisteredKeys = Array.from(initializedSet.values()).filter(k => !registeredSet.has(k))
  if (unregisteredKeys.length > 0) {
    throw new Error(`Unregistered key(s): ${unregisteredKeys.map(k => keyToName[k]).join(', ')}`)
  }
}

const expectNoRegistrations = () => {
  const registeredNames = Array.from(registeredSet.values()).map(k => keyToName[k])
  if (registeredNames.length > 0) {
    throw new Error(`Expected no registrations found: ${registeredNames}`)
  }
}

const entry = <T>(name: string): RegistryEntry<T> => {
  const entryKey = key(name)
  return {
    use: () => get(entryKey),
    safeUse: () => safeGet(entryKey),
    register: value => register(entryKey, value),
    unregister: () => unregister(entryKey),
  }
}

const lazyEntry = <T>(name: string): LazyRegistryEntry<T> => {
  const entryKey = key(name)
  return {
    use: () => lazyGet(entryKey),
    register: value => register(entryKey, value),
    unregister: () => unregister(entryKey),
  }
}

export {
  entry,
  lazyEntry,
  register,
  unregister,
  get,
  key,
  validate,
  expectNoRegistrations,
}

export type {
  RegistryEntry,
  LazyRegistryEntry,
}
