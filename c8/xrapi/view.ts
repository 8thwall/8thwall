import type {XrFrame, XrView} from './xrapi-types'

type InternalXrView = XrView & {
  _frame: XrFrame

  // [Implementation specific]
  _recommendedColorWidth: number
  _recommendedColorHeight: number
  _recommendedDepthWidth: number
  _recommendedDepthHeight: number
  _recommendedMotionVectorWidth: number
  _recommendedMotionVectorHeight: number
}

export type {InternalXrView}
