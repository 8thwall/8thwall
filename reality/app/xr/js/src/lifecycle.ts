import type {ModuleName} from './types/pipeline'

// NOTE(christoph): A continuous lifecycle is a lifecycle with an explicit start and end point.
//   If a module is added between the start and end, it will receive the start event.
//   The lifecycle can also be paused, in which case any modules attached during that period will
//   be attached after we resume.
const createContinuousLifecycle = (
  runStartCallbacks: (shouldRun: (moduleName: ModuleName) => boolean) => void,
  runStopCallbacks: (shouldRun: (moduleName: ModuleName) => boolean) => void
) => {
  let isAttached = false
  let isPaused = false
  const attachedNames = new Set<string>()
  const addedNames = new Set<string>()

  const attach = () => {
    isAttached = true
    isPaused = false
    runStartCallbacks(m => addedNames.has(m))
    addedNames.forEach(m => attachedNames.add(m))
  }

  const pause = () => {
    isPaused = true
  }

  const resume = () => {
    isPaused = false
    if (isAttached) {
      runStartCallbacks(m => !attachedNames.has(m) && addedNames.has(m))
      addedNames.forEach(m => attachedNames.add(m))
    }
  }

  const add = (moduleName: ModuleName) => {
    addedNames.add(moduleName)
    if (isAttached && !isPaused) {
      runStartCallbacks(m => m === moduleName)
      attachedNames.add(moduleName)
    }
  }

  const remove = (moduleName: ModuleName) => {
    addedNames.delete(moduleName)
    if (attachedNames.has(moduleName)) {
      runStopCallbacks(m => m === moduleName)
      attachedNames.delete(moduleName)
    }
  }

  const detach = () => {
    runStopCallbacks(m => attachedNames.has(m))
    attachedNames.clear()
    isAttached = false
    isPaused = false
  }

  return {
    attach,
    pause,
    resume,
    detach,
    add,
    remove,
  }
}

export {
  createContinuousLifecycle,
}
