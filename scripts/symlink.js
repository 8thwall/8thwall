// Usage: node ./reality/scripts/symlink.js symlink/to/create place/to/point

const {relative, resolve, dirname} = require('path')
const fs = require('fs/promises')

const run = async () => {
  const [, , targetRaw, sourceRaw] = process.argv

  const cwd = process.cwd()

  const target = resolve(cwd, targetRaw)
  const source = resolve(cwd, sourceRaw)

  const targetDir = dirname(target)
  const relativeSource = relative(targetDir, source)

  try {
    const stats = await fs.lstat(target)
    if (stats.isSymbolicLink()) {
      await fs.unlink(target)
    } else {
      throw new Error(`Target ${target} already exists and is not a symlink`)
    }
  } catch (err) {
    if (err.code === 'ENOENT') {
      await fs.mkdir(targetDir, {recursive: true})
    } else {
      throw err
    }
  }
  await fs.symlink(relativeSource, target)
}

run()
