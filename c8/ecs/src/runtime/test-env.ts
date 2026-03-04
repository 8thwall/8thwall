// @attr(testonly = 1)

const fakeAudioValue = {
  value: 0,
  setValueAtTime: () => {},
}

const fakeEventTarget = {
  addEventListener: () => {},
  removeEventListener: () => {},
}

const fakeRenderLists = {
  get: () => ({objectIdToRenderItem: {get: () => {}}}),
}

const fakeRenderer = {
  domElement: fakeEventTarget,
  renderLists: fakeRenderLists,
  setTransparentSort: () => {},
}

class FakeAudioContext {
  // eslint-disable-next-line class-methods-use-this
  createGain() {
    return {
      gain: fakeAudioValue,
      connect: () => {},
    }
  }

  // eslint-disable-next-line class-methods-use-this
  createDynamicsCompressor() {
    return {
      threshold: fakeAudioValue,
      knee: fakeAudioValue,
      ratio: fakeAudioValue,
      attack: fakeAudioValue,
      release: fakeAudioValue,
      connect: () => {},
    }
  }
}

const fakeWindow = {
  ...fakeEventTarget,
  THREE: null as any as typeof import('three'),
  AudioContext: FakeAudioContext,
  document: {
    ...fakeEventTarget,
    body: {
      appendChild: () => {},
    },
    createElement: () => ({
      ...fakeEventTarget,
      style: {},
      getContext: () => ({
        canvas: {},
        fillRect: () => {},
      }),
    }),
  },
}

// @ts-expect-error
globalThis.window = fakeWindow
// @ts-expect-error
globalThis.document = fakeWindow.document

globalThis.requestAnimationFrame = () => 0
globalThis.cancelAnimationFrame = () => {}

// @ts-expect-error
globalThis.ProgressEvent = function ProgressEvent() {}

// @ts-expect-error
globalThis.HTMLVideoElement = function HTMLVideoElement() {}

const initThree = () => {
  const {THREE} = fakeWindow
  const scene = new THREE.Scene()
  const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000)
  // @ts-ignore
  const renderer: import('three').WebGLRenderer = fakeRenderer
  return [scene, renderer, camera] as const
}

export {
  initThree,
}
