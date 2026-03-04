type Major = {major: number}
type Minor = {minor: number}
type Patch = {patch: number}

type P<T> = Partial<T>

type RuntimeVersionTarget =
  {type: 'version', level: 'major'} & Major & P<Minor> & P<Patch> |
  {type: 'version', level: 'minor'} & Major & Minor & P<Patch> |
  {type: 'version', level: 'patch'} & Major & Minor & Patch

type RuntimeVersionTargetLevel = RuntimeVersionTarget['level']

type RuntimeVersionInfo = {
  publishTime: number
  patchTarget: RuntimeVersionTarget
}

type RuntimeVersionList = RuntimeVersionInfo[]

type RuntimeMetadata = {
  version: string
  buildTime: string  // base 36 timestamp
  commitId: string
  features: {edition?: number | undefined} & Record<string, boolean>
  // TODO(christoph): Surface the correct typings
  componentSchema: unknown
  cloneableComponents: Array<unknown>
}

export type {
  RuntimeVersionTarget,
  RuntimeVersionTargetLevel,
  RuntimeVersionInfo,
  RuntimeVersionList,
  RuntimeMetadata,
}
