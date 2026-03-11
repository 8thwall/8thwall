type UnixPath = string & {__unixPath: true}

// NOTE(christoph): Only works for non-absolute paths
const toUnixPath = process.platform === 'win32'
  ? (p: string) => p.replace(/\\/g, '/').replace(/\/+/g, '/') as UnixPath
  : (p: string) => p as UnixPath

export {
  toUnixPath,
}

export type {
  UnixPath,
}
