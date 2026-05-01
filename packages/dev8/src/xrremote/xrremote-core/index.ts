type Position = { x: number; y: number; z: number}
type Quaternion = { x: number; y: number; z: number; w: number}

type ControllerOrigin = {
  position: Position
  quaternion: Quaternion
}

type XrRemoteState = {
  inputSource: {
    controllerType: string
    connected: boolean
    origin: {
      _needsUpdate: boolean
      left: ControllerOrigin
      right: ControllerOrigin
    }
  }
}

class XRRemote {
  controllerPosition: EventTarget

  controller: EventTarget

  state: XrRemoteState

  constructor() {
    this.controllerPosition = new EventTarget()
    this.controller = new EventTarget()
    this.state = {
      inputSource: {
        origin: {
          _needsUpdate: true,
          right: {
            position: {x: 0, y: 0, z: 0},
            quaternion: {x: 0, y: 0, z: 0, w: 1},
          },
          left: {
            position: {x: 0, y: 0, z: 0},
            quaternion: {x: 0, y: 0, z: 0, w: 1},
          },
        },
        // The type of controller (quest 2, quest pro)
        controllerType: '',

        // Is something connected currently
        connected: false,
      },
    }
  }

  setConnected(isConnected: boolean) {
    this.state.inputSource = {
      ...this.state.inputSource,
      connected: isConnected,
    }
  }

  updateControllerType(controllerType: string) {
    this.state.inputSource = {
      ...this.state.inputSource,
      controllerType,
    }
  }

  updateControllerOrigin(left: ControllerOrigin, right: ControllerOrigin) {
    this.state.inputSource = {
      ...this.state.inputSource,
      origin: {
        _needsUpdate: false,
        left,
        right,
      },
    }
  }

  originNeedsSync() {
    return this.state.inputSource.origin._needsUpdate
  }

  setNeedsOriginSync() {
    this.state.inputSource = {
      ...this.state.inputSource,
      origin: {
        ...this.state.inputSource.origin,
        _needsUpdate: true,
      },
    }
  }

  cleanup() {
    this.updateControllerType('')
    this.setConnected(false)
  }
}

export default XRRemote
