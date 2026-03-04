import type {XrFrame, XrInputSource, XrInputSourceEventInit} from './xrapi-types'

class XrInputSourceEvent extends Event {
  readonly frame: XrFrame

  readonly inputSource: XrInputSource

  constructor(type: string, eventInitDict: XrInputSourceEventInit) {
    super(type)
    this.frame = eventInitDict.frame
    this.inputSource = eventInitDict.inputSource
  }
}

export {XrInputSourceEvent}
