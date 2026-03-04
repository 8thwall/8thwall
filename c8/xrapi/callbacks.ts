const createCallbacks = <T extends (...args: any[]) => void>() => {
  const callbacks_: Array<{id: number, cancelled: boolean, callback: T}> = []
  let id_ = 1

  const add = (callback: T) => {
    const id = id_++
    callbacks_.push({id, cancelled: false, callback})
    return id
  }

  const remove = (id: number) => {
    const callback = callbacks_.find(e => e.id === id)

    if (callback) {
      callback.cancelled = true
    }
  }

  const dispatch = (...args: Parameters<T>) => {
    const dispatchCount = callbacks_.length
    for (let i = 0; i < dispatchCount; i++) {
      const {cancelled, callback} = callbacks_[i]
      if (!cancelled) {
        callback(...args)
      }
    }
    callbacks_.splice(0, dispatchCount)
  }

  return {
    add,
    remove,
    dispatch,
  }
}

export {
  createCallbacks,
}
