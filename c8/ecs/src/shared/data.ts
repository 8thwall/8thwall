import {toByteArray, fromByteArray} from 'base64-js'

const bytesToString: (bytes: Uint8Array) => string = fromByteArray

const stringToBytes: (base64: string) => Uint8Array = toByteArray

export {
  bytesToString,
  stringToBytes,
}
