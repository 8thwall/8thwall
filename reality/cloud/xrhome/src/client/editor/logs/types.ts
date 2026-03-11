import type {DeepReadonly} from 'ts-essentials'

interface ISourceLocation extends DeepReadonly<Partial<{
  line: number
  column: number
  file: string
}>> {}

interface StackFrame extends DeepReadonly<ISourceLocation & {
  function: string
}> {}

interface ILog extends DeepReadonly<{
  key: number
  timestamp: number
  text: string
  ua?: string
  type?: string
  numRedundant: number
  sourceLocation?: ISourceLocation
  tags?: string[]
  stack?: StackFrame[]
}> {}

interface ILogStream extends DeepReadonly<{
  name: string  // Unique identifier for stream
  title: string  // User friendly display name for stream
  logs: ILog[]
  deviceId?: string
  isClearOnRunActive: boolean
  isDebugHudActive: boolean
}> {}

export type {
  ISourceLocation,
  StackFrame,
  ILog,
  ILogStream,
}
