const fileExt = (filename: string) => (
  (filename && filename.includes('.'))
    ? filename.split('.').slice(-1)[0].toLowerCase()
    : ''
)

const joinExt = (fileName: string, ext: string) => (
  ext ? `${fileName}.${ext}` : fileName
)
const basename = (filePath: string) => filePath.split('/').slice(filePath.endsWith('/')
  ? -2
  : -1)[0]
const dirname = (filePath: string): string => filePath.split('/').slice(0, -1).join('/')

export {
  fileExt,
  joinExt,
  basename,
  dirname,
}
