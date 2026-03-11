import type {RegistryEntry} from './registry'

/* eslint-disable arrow-parens */

const createMock = <ApiType extends {}>(entry: RegistryEntry<ApiType>) => {
  const handlers = {} as ApiType

  const setHandler = <ApiFunction extends keyof ApiType>(
    functionName: ApiFunction, handler: ApiType[ApiFunction]
  ) => {
    handlers[functionName] = handler
  }

  const register = () => {
    entry.register(handlers)
  }

  const clearHandlers = () => {
    Object.keys(handlers).forEach((key) => {
      delete handlers[key]
    })
  }

  const remove = () => {
    entry.unregister()
  }

  return {
    setHandler,
    clearHandlers,
    register,
    remove,
  }
}

const generateCreateMock = <T>(entry: RegistryEntry<T>) => () => createMock(entry)

export {
  generateCreateMock,
}
