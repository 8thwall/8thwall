import type {DefinitionKey} from '../src/shared/buildif'

const DEFAULTS: Partial<Record<DefinitionKey, boolean>> = {
  EXPERIMENTAL: false,
  ALL_QA: false,
  LOCAL_DEV: false,
  LOCAL: false,
  DISABLED: false,
  FULL_ROLLOUT: true,
  UI_TEST: false,
  UNIT_TEST: true,
}

let flagOverrides = {} as Partial<Record<DefinitionKey, boolean>>

let softFail = false

const BuildIfProxy = new Proxy(DEFAULTS, {
  get: (target, prop) => {
    if (prop in flagOverrides) {
      return flagOverrides[prop]
    }
    if (Reflect.has(target, prop)) {
      return Reflect.get(target, prop)
    }
    const propName = String(prop)
    const msg = `Cannot access BuildIf.${propName} without calling \
enableFlag('${propName}')/disableFlag('${propName}') in unit test definition.`
    const error = new Error(msg)
    if (!softFail) {
      // eslint-disable-next-line no-console
      console.error(error)
      process.exit(1)
    }
    throw error
  },
  set: () => { throw new Error('Do not directly set BuildIf properties.') },
})

const setBuildIfMock = () => {
  Object.defineProperty(globalThis, 'BuildIf', {
    get() { return BuildIfProxy },
    set() { throw new Error('Do not reassign globalThis.BuildIf.') },
    configurable: true,
  })
}

const enableFlag = (flag: DefinitionKey) => { flagOverrides[flag] = true }

const disableFlag = (flag: DefinitionKey) => { flagOverrides[flag] = false }

const resetFlag = (flag: DefinitionKey) => { delete flagOverrides[flag] }

const failSoft = () => {
  softFail = true
}

const resetAllFlags = () => {
  flagOverrides = {}
  softFail = false
}

export {
  setBuildIfMock,
  enableFlag,
  disableFlag,
  resetFlag,
  resetAllFlags,
  failSoft,
}
