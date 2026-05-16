// @attr(visibility = ["//visibility:public"])

type Handler<T> = (d: T) => void

type ListenerPool<T> = {
  addListener: (h: Handler<T>) => void
  removeListener: (h: Handler<T>) => void
  dispatch: (d: T) => void
}

const createListenerPool = <T>(): ListenerPool<T> => {
  const listeners = new Set<Handler<T>>()

  return {
    addListener: (h) => {
      listeners.add(h)
    },
    removeListener: (h) => {
      listeners.delete(h)
    },
    dispatch: (d) => {
      listeners.forEach(e => e(d))
    },
  }
}

export {
  createListenerPool,
}

export type {
  ListenerPool,
}
