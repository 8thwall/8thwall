import type {ChildProcess} from 'child_process'

import type {SystemLogEvent, SystemLogHandler} from '@repo/reality/shared/desktop/system-log-types'

const systemLogListeners = new Set<SystemLogHandler>()

const addSystemLogListener = (h: SystemLogHandler) => {
  systemLogListeners.add(h)
}

const removeSystemLogListener = (h: SystemLogHandler) => {
  systemLogListeners.delete(h)
}

const dispatchSystemLog = (event: SystemLogEvent) => {
  systemLogListeners.forEach(e => e(event))
}

const forwardProcessOutput = (appKey: string, process: ChildProcess) => {
  process.stdout?.on('data', (t) => {
    const rawText = t.toString()
    let type: SystemLogEvent['type'] = 'log'

    // NOTE(christoph): See https://github.com/webpack/webpack/issues/10022
    // Webpack errors are not sent to stderr.
    if (rawText.includes('ERROR')) {
      type = 'error'
    } else if (rawText.includes('WARNING')) {
      type = 'warn'
    }

    dispatchSystemLog({appKey, type, text: rawText})
  })
  process.stderr?.on('data', (t) => {
    const rawText = t.toString()
    // NOTE(christoph): See https://github.com/webpack/webpack/issues/10022
    // Webpack errors are not sent to stderr, we only see status updates from webpack dev server
    // which are not errors (messages starting with <i>).
    const type = rawText.startsWith('<i> ') ? 'log' : 'error'
    dispatchSystemLog({appKey, type, text: rawText})
  })
}

export {
  addSystemLogListener,
  removeSystemLogListener,
  dispatchSystemLog,
  forwardProcessOutput,
}
