const fileExt = (filename: string) => (
  (filename && filename.includes('.'))
    ? filename.split('.').slice(-1)[0].toLowerCase()
    : ''
)

const joinExt = (fileName: string, ext: string) => (
  ext ? `${fileName}.${ext}` : fileName
)
const basename = (filePath: string) =>
  filePath.replace(/[\\/]+$/, '').split(/[\\/]/).pop() || ''
const dirname = (filePath: string) =>
  filePath.replace(/[\\/]+$/, '').split(/[\\/]/).slice(0, -1).join('/')

export {
  fileExt,
  joinExt,
  basename,
  dirname,
}
