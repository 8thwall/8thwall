let baseUrl: string | null = null

const setBaseUrl = (url: string) => {
  if (!url) {
    throw new Error('[XR] Base URL cannot be empty.')
  }
  baseUrl = url
}

const getBaseUrl = (): string => {
  if (baseUrl === null) {
    throw new Error('[XR] Base URL has not been set yet.')
  }
  return baseUrl
}

const resolver = (resourcePath: string) => () => getBaseUrl() + resourcePath

const unsupported = (resource: string) => () => {
  throw new Error(`[XR] Resource "${resource}" is not supported in this environment.`)
}

const ResourceUrls = {
  /* eslint-disable max-len */
  resolveFaceModel: resolver('resources/face-model.tflite'),
  resolveFaceMeshModel: resolver('resources/face-mesh-model.tflite'),
  resolveFaceEarModel: resolver('resources/face-ear-model.tflite'),
  resolveSemanticsWorker: resolver('resources/semantics-worker.js'),
  resolveSemanticsModel: resolver('resources/semantics-model.tflite'),
  resolveMediaWorker: resolver('resources/media-worker.js'),
  resolvePoweredLogo: resolver('resources/powered-by.svg'),
  resolveDracoWorker: unsupported('https://cdn.8thwall.com/web/resources/draco-worker-l5sniji5.js'),
  resolveDracoWrapper: unsupported('https://cdn.8thwall.com/web/resources/draco_wasm_wrapper-l325u8do.js'),
  resolveDomTabletFrameGlb: resolver('resources/dom-tablet-frame.glb'),
  resolveDomTabletButtonGlb: resolver('resources/dom-tablet-button.glb'),
  setBaseUrl,
  getBaseUrl,
}

export {
  ResourceUrls,
}
