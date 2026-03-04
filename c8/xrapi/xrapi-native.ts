import type {XrApi} from './xrapi-types'

const getNativeXrApi = (): XrApi => ({
  xr: (window.navigator as any).xr!,
  XrWebGlLayerClass: (window as any).XRWebGLLayer,
  XrWebGlBindingClass: (window as any).XRWebGLBinding,
  XrRigidTransformClass: (window as any).XRRigidTransform,
})

export {
  getNativeXrApi,
}
