type TimeoutId = number

type Callback = () => void

type TimeState = {
  elapsed: number
  delta: number
  absolute: number
  absoluteDelta: number
}

type PendingTimeout = {
  id: TimeoutId
  deadline: number
  cb: Callback
}

type InternalTimeState = TimeState & {
  _absoluteOffset: number
  _pendingTimeouts: PendingTimeout[]
}

type TimeApi = {
  setTimeout: (cb: Callback, timeout: number) => TimeoutId
  setInterval: (cb: Callback, timeout: number) => TimeoutId
  clearTimeout: (id: TimeoutId) => void
}

type Time = TimeState & TimeApi

type InternalTime = InternalTimeState & TimeApi

const generateTimeoutId = (time: InternalTimeState) => {
  let maxId = 0
  for (const pending of time._pendingTimeouts) {
    maxId = Math.max(maxId, pending.id)
  }
  return maxId + 1
}

const setTimeout = (time: InternalTimeState, cb: Callback, timeout: number) => {
  const id = generateTimeoutId(time)
  time._pendingTimeouts.push({
    id,
    deadline: time.elapsed + timeout,
    cb,
  })
  return id
}

const setInterval = (time: InternalTimeState, cb: () => void, timeout: number) => {
  const id = generateTimeoutId(time)
  const interval = () => {
    time._pendingTimeouts.push({
      id,
      deadline: time.elapsed + timeout,
      cb: interval,
    })
    cb()
  }
  time._pendingTimeouts.push({
    id,
    deadline: time.elapsed + timeout,
    cb: interval,
  })
  return id
}

const clearTimeout = (time: InternalTimeState, id: TimeoutId) => {
  const index = time._pendingTimeouts.findIndex(t => t.id === id)
  if (index !== -1) {
    time._pendingTimeouts.splice(index, 1)
  }
}

const createTime = (): InternalTime => {
  const absolute = Date.now()

  const time: InternalTimeState = {
    delta: 0,
    elapsed: 0,
    absolute,
    absoluteDelta: 0,
    _absoluteOffset: 0,
    _pendingTimeouts: [],
  }

  return Object.assign(time, {
    setTimeout: setTimeout.bind(null, time),
    setInterval: setInterval.bind(null, time),
    clearTimeout: clearTimeout.bind(null, time),
  })
}

// NOTE(christoph): timestamp is in "page time", counting milliseconds since the page loaded.
const updateTime = (time: InternalTime, timestamp: number) => {
  if (time._absoluteOffset === 0) {
    // NOTE(christoph): We calculate the absolute offset on the first tick to ensure we use a
    // high resolution timestamp from requestAnimationFrame.
    // We saw that performance.now() and the reported timestamp can differ by up to 20 ms.
    time.absolute = Date.now()
    time._absoluteOffset = time.absolute - timestamp
  } else {
    const nextAbsolute = timestamp + time._absoluteOffset
    time.absoluteDelta = nextAbsolute - time.absolute
    time.absolute = nextAbsolute
  }

  // In the case where a frame is the first one in over 100ms, don't advance the time
  // further than that, to avoid large jumps in animation for example
  time.delta = Math.min(time.absoluteDelta, 100)
  time.elapsed += time.delta

  time._pendingTimeouts.sort((a, b) => a.deadline - b.deadline)
  while (time._pendingTimeouts.length && time._pendingTimeouts[0].deadline <= time.elapsed) {
    const {cb} = time._pendingTimeouts.shift()!
    try {
      cb()
    } catch (err) {
      // eslint-disable-next-line no-console
      console.error(err)
    }
  }
}

const clearDelta = (time: Time) => {
  time.delta = 0
  time.absoluteDelta = 0
}

export {
  createTime,
  updateTime,
  clearDelta,
}

export type {
  Time,
}
