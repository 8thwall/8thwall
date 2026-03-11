import fs from 'fs/promises'
import path from 'path'
import JsZip from 'jszip'

import {makeRunQueue} from '@repo/reality/shared/run-queue'

import {NEW_PROJECT_ZIP_PATH} from '../resources'

type FileFilter = (filePath: string) => boolean

const unzipIntoFolder = async (
  savePath: string,
  zipData: Buffer | ArrayBuffer,
  filter?: FileFilter
) => {
  const zip = await JsZip.loadAsync(zipData)
  const filesToWrite = Object.entries(zip.files)
    .filter(([filePath]) => (filter ? filter(filePath) : true))
    .map(([filePath, fileContents]) => [path.resolve(savePath, filePath), fileContents] as const)
    .filter(([, fileContents]) => !fileContents.dir)

  const foldersNeeded = new Set<string>(filesToWrite.map(([filePath]) => path.dirname(filePath)))
  for (const folderPath of foldersNeeded) {
    // eslint-disable-next-line no-await-in-loop
    await fs.mkdir(folderPath, {recursive: true})
  }

  const runQueue = makeRunQueue(10)
  await Promise.all(filesToWrite.map(([filePath, fileContents]) => runQueue.next(async () => {
    const content = await fileContents.async('nodebuffer')
    await fs.writeFile(filePath, content)
  })))
}

const projectSetup = async (savePath: string, filter?: FileFilter) => {
  const zip = await fs.readFile(NEW_PROJECT_ZIP_PATH)
  await unzipIntoFolder(savePath, zip, filter)
}

export {
  projectSetup,
  unzipIntoFolder,
}
