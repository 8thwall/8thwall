import {createReadStream, ReadStream} from 'fs'
import mime from 'mime-types'

import {makeCodedError} from './errors'

const createReadStreamPromise = (filePath: string) => new Promise<ReadStream>((resolve, reject) => {
  const contentStream = createReadStream(filePath)

  contentStream.on('open', () => {
    resolve(contentStream)
  })

  contentStream.on('error', (error) => {
    if ('code' in error && error.code === 'ENOENT') {
      reject(makeCodedError('File not found', 404))
    } else {
      // eslint-disable-next-line no-console
      console.error('Error reading file: ', error)
      reject(makeCodedError('Error reading file', 500))
    }
  })
})

const makeStreamFileResponse = async (filePath: string) => {
  const contentStream = await createReadStreamPromise(filePath)

  return new Response(contentStream, {
    headers: {
      'Content-Type': mime.lookup(filePath) || 'application/octet-stream',
    },
  })
}

export {
  createReadStreamPromise,
  makeStreamFileResponse,
}
