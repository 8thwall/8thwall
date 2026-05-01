type XrHudCallback = () => void

type XrHudBaseSettings = {
  version: boolean
  console: boolean
  verbose: boolean
}

type XrHudLogger = {
  log: (args: any[]) => void
  info: (args: any[]) => void
  warn: (args: any[]) => void
  error: (args: any[]) => void
}

export type {
  XrHudCallback,
  XrHudBaseSettings,
  XrHudLogger,
}
