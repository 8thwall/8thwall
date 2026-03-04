import type {XrApi} from './xrapi-types'

type Window = any

const installXrApi = (target: Window, api: XrApi) => {
  const {xr, XrWebGlLayerClass, XrWebGlBindingClass, XrRigidTransformClass} = api
  // @ts-ignore
  if (!target.navigator) {
  // @ts-ignore
    target.navigator = {}
  }
  // @ts-ignore
  target.navigator.xr = xr
  // @ts-ignore
  target.XRWebGLLayer = XrWebGlLayerClass
  // @ts-ignore
  target.XRWebGLBinding = XrWebGlBindingClass
  // @ts-ignore
  target.XRRigidTransform = XrRigidTransformClass
}

export {
  installXrApi,
}
