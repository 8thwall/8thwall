// eslint-disable-next-line max-len
// based off of: https://github.com/jdiaz5513/capnp-ts/blob/108850ece76b33755d552f78eac736055ec8f1b4/packages/capnp-ts/src/serialization/message.ts#L229
const getMessageSize = (frameData: ArrayBuffer, offset = 0): number => {
  if (!Number.isInteger(offset) || offset < 0 || offset + 4 > frameData.byteLength) {
    // Finished reading or there isn't enough data for a segment count.
    return -1
  }

  const dv = new DataView(frameData)

  const segmentCount = dv.getUint32(offset, true) + 1
  const segmentTableByteLength = 4 + segmentCount * 4
  const alignedSegmentTableByteLength = segmentTableByteLength + segmentTableByteLength % 8

  let byteOffset = offset + alignedSegmentTableByteLength

  if (byteOffset > frameData.byteLength) {
    // The segment table is incomplete.
    return -1
  }

  for (let i = 0; i < segmentCount; i++) {
    const byteLength = dv.getUint32(offset + 4 + i * 4, true) * 8

    if (byteLength > frameData.byteLength - byteOffset) {
      // A segment extends beyond the available data.
      return -1
    }

    byteOffset += byteLength
  }

  return byteOffset
}

type MessageBoundary = {
  start: number
  end: number
}

const getMessageOffsets = (data: ArrayBuffer): MessageBoundary[] => {
  const messageBoundaries: Array<MessageBoundary> = []
  let offset = 0
  while (offset < data.byteLength) {
    const messageEnd = getMessageSize(data, offset)
    if (messageEnd <= offset) {
      throw new Error(`Invalid Cap'n Proto message at byte offset ${offset}`)
    }
    messageBoundaries.push({start: offset, end: messageEnd})
    offset = messageEnd
  }
  return messageBoundaries
}

export {
  getMessageOffsets,
}
