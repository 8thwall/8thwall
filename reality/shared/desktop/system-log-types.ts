type SystemLogEvent = {
  type: 'log' | 'warn' | 'error'
  text: string
  appKey: string
}

type SystemLogHandler = (e: SystemLogEvent) => void

export type {
  SystemLogEvent,
  SystemLogHandler,
}
