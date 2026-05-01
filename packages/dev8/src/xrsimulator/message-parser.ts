// eslint-disable-next-line max-len
// based off of: https://github.com/jdiaz5513/capnp-ts/blob/108850ece76b33755d552f78eac736055ec8f1b4/packages/capnp-ts/src/serialization/message.ts#L229
const getMessageSize = (frameData: ArrayBuffer, offset = 0): number => {
  if (offset >= frameData.byteLength) {
    // Finished reading
    return -1
  }

  const dv = new DataView(frameData, offset)

  const segmentCount = dv.getUint32(0, true) + 1

  let byteOffset = offset + 4 + segmentCount * 4
  byteOffset += byteOffset % 8

  if (byteOffset + segmentCount * 4 > frameData.byteLength) {
    // The message is invalid
    return -1
  }

  for (let i = 0; i < segmentCount; i++) {
    const byteLength = dv.getUint32(4 + i * 4, true) * 8

    if (byteOffset + byteLength > frameData.byteLength) {
      // The message is invalid
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
    messageBoundaries.push({start: offset, end: messageEnd})
    offset = messageEnd
  }
  return messageBoundaries
}

export {
  getMessageOffsets,
}
