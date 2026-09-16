import {basename} from 'path'

import type {UnixPath} from '@repo/reality/shared/desktop/unix-path'

const getProjectSrcPath = (projectDir: string) => `${projectDir}/src`

// TODO: Fill in the rest of the ignored files, think about .git
const IGNORED_FILES = ['.DS_Store', '.git']

const isIgnoredFile = (filePath: string) => IGNORED_FILES.includes(basename(filePath))

const isAssetPath = (filePath: UnixPath) => (filePath.startsWith('assets/'))

export {
  getProjectSrcPath,
  isIgnoredFile,
  isAssetPath,
}
