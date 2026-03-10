// @rule(js_cli)
// @name(polyfill-cli)
/* eslint-disable no-console */
import path from 'path'
import {Buffer} from 'buffer'

console.log('path/to/folder + my-file.txt = ', path.join('path/to/folder', 'my-file.txt'))

const inputBuffer = Buffer.from('this is a buffer.')
const outputBuffer = Buffer.from(inputBuffer.toString('base64'), 'base64')
console.log(outputBuffer.toString('utf8'))
