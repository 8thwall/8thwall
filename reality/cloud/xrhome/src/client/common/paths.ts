const getHashForFile = (line: number, column?: number) => {
  const lineHashFragment = line ? `#L${line}` : ''
  const columnHashFragment = column ? `C${column}` : ''
  return lineHashFragment ? lineHashFragment + columnHashFragment : ''
}

const resolveServerRoute = (path: string) => (
  Build8.PLATFORM_TARGET === 'desktop'
    ? new URL(path, 'desktop://server')
    : path
)

export {
  getHashForFile,
  resolveServerRoute,
}
