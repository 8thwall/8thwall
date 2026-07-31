import {setRenameDeprecation} from './deprecate'
import {ResourceUrls} from './resources'
import type {ChunkName, ChunkSpecifier, IXR8} from './types/api'
import type {RunConfig} from './types/pipeline'

const loadSpecificChunk = async (
  jsxr: IXR8,
  runConfig: RunConfig,
  xrccPromise: Promise<any>,
  chunk: ChunkName | ChunkSpecifier,
  logCallback: (isError: boolean) => (msg: string) => void,
  errorCallback: (error: Error) => void
) => {
  const chunkName = typeof chunk === 'string' ? chunk : chunk.name
  const urlOverride = typeof chunk === 'string' ? null : chunk.url

  const baseUrl = ResourceUrls.getBaseUrl()
  if (chunkName === 'face') {
    if (jsxr.FaceController) {
      return
    }

    const url = urlOverride || `${baseUrl}xr-face.js`

    // @ts-ignore
    // eslint-disable-next-line
    const faceChunk = await import(/* webpackIgnore: true */ url)
    const {FaceControllerFactory} = faceChunk
    faceChunk.ResourceUrls.setBaseUrl(baseUrl)
    jsxr.FaceController = await FaceControllerFactory(
      runConfig,
      xrccPromise,
      logCallback,
      errorCallback
    )
  } else if (chunkName === 'slam') {
    if (jsxr.XrController) {
      return
    }

    const url = urlOverride || `${baseUrl}xr-tracking.js`

    // @ts-ignore
    // eslint-disable-next-line
    const slamChunk = await import(/* webpackIgnore: true */ url)
    const {XrControllerFactory} = slamChunk
    slamChunk.ResourceUrls.setBaseUrl(baseUrl)
    jsxr.XrController = await XrControllerFactory(
      runConfig,
      xrccPromise,
      logCallback,
      errorCallback
    )

    // @ts-expect-error
    setRenameDeprecation(jsxr, 'xrController', 'XR8.XrController', jsxr.XrController.xrController)
  } else {
    // eslint-disable-next-line no-console
    console.warn('[XR]: Attempted to load unknown chunk:', chunkName)
  }
}

export {
  loadSpecificChunk,
}
