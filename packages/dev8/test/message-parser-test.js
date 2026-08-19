const assert = require('node:assert/strict')
const fs = require('node:fs')
const {test} = require('node:test')
const ts = require('typescript')

require.extensions['.ts'] = (module, filename) => {
  const source = fs.readFileSync(filename, 'utf8')
  const output = ts.transpileModule(source, {
    compilerOptions: {
      module: ts.ModuleKind.CommonJS,
      target: ts.ScriptTarget.ES2020,
    },
  })
  module._compile(output.outputText, filename)
}

const {getMessageOffsets} = require('../src/xrsimulator/message-parser.ts')

const makeMessage = (segmentWordLengths) => {
  const tableByteLength = 4 + segmentWordLengths.length * 4
  const alignedTableByteLength = tableByteLength + tableByteLength % 8
  const payloadByteLength = segmentWordLengths.reduce((sum, words) => sum + words * 8, 0)
  const data = new ArrayBuffer(alignedTableByteLength + payloadByteLength)
  const view = new DataView(data)

  view.setUint32(0, segmentWordLengths.length - 1, true)
  segmentWordLengths.forEach((words, index) => {
    view.setUint32(4 + index * 4, words, true)
  })

  return new Uint8Array(data)
}

const concatenate = (...arrays) => {
  const result = new Uint8Array(arrays.reduce((sum, array) => sum + array.byteLength, 0))
  let offset = 0
  arrays.forEach((array) => {
    result.set(array, offset)
    offset += array.byteLength
  })
  return result.buffer
}

test('accepts an empty recording', () => {
  assert.deepEqual(getMessageOffsets(new ArrayBuffer(0)), [])
})

test('accepts a message with an empty segment', () => {
  const message = makeMessage([0])

  assert.deepEqual(getMessageOffsets(message.buffer), [{start: 0, end: message.byteLength}])
})

test('finds boundaries for concatenated messages', () => {
  const first = makeMessage([1])
  const second = makeMessage([0, 2])
  const data = concatenate(first, second)

  assert.deepEqual(getMessageOffsets(data), [
    {start: 0, end: first.byteLength},
    {start: first.byteLength, end: first.byteLength + second.byteLength},
  ])
})

test('rejects a truncated segment count', () => {
  assert.throws(
    () => getMessageOffsets(new ArrayBuffer(3)),
    /Invalid Cap'n Proto message at byte offset 0/
  )
})

test('rejects a truncated segment table', () => {
  const data = new ArrayBuffer(8)
  new DataView(data).setUint32(0, 1, true)

  assert.throws(
    () => getMessageOffsets(data),
    /Invalid Cap'n Proto message at byte offset 0/
  )
})

test('rejects a truncated segment payload', () => {
  const data = makeMessage([1]).slice(0, 8).buffer

  assert.throws(
    () => getMessageOffsets(data),
    /Invalid Cap'n Proto message at byte offset 0/
  )
})

test('reports trailing invalid data at its byte offset', () => {
  const message = makeMessage([0])
  const data = concatenate(message, new Uint8Array([1, 2, 3]))

  assert.throws(
    () => getMessageOffsets(data),
    new RegExp(`Invalid Cap'n Proto message at byte offset ${message.byteLength}`)
  )
})
