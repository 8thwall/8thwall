// @attr[](data = ":wildcard-external")
import fs from 'fs'
import path from 'path'

const runfilesDir = process.env.RUNFILES_DIR

if (!runfilesDir) {
  console.error('RUNFILES_DIR not set')
  process.exit(1)
}

const outputPath = path.join(runfilesDir, '_main/bzl/examples/js/import/wildcard-external.js')

const fileSize = fs.readFileSync(outputPath, 'utf8').length

if (fileSize > 5000) {
  console.error('file is too big')
  process.exit(1)
} else if (fileSize < 4000) {
  console.error('file is too small')
  process.exit(1)
} else {
  process.exit(0)
}
