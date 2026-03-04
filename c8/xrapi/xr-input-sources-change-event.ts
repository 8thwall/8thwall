import type {XrInputSource, XrInputSourcesChangeEventInit, XrSession} from './xrapi-types'

class XrInputSourcesChangeEvent extends Event {
  readonly session: XrSession

  readonly added: XrInputSource[]

  readonly removed: XrInputSource[]

  constructor(type: string, eventInitDict: XrInputSourcesChangeEventInit) {
    super(type)
    this.session = eventInitDict.session
    this.added = eventInitDict.added
    this.removed = eventInitDict.removed
  }
}

export {XrInputSourcesChangeEvent}
