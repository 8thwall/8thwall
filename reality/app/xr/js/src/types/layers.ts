import type {Coordinates} from './common'

const SUPPORTED_LAYERS: readonly string[] = ['sky']
type LayerName = typeof SUPPORTED_LAYERS[number]

const DEFAULT_INVERT_LAYER_MASK: boolean = false
const DEFAULT_EDGE_SMOOTHNESS: number = 0.0

interface LayerOptions {
  // If false then virtual content added to the layer scene will be visible
  // only in regions where the layer is found (i.e. the cube is shown in the sky). If false the mask
  // will be flipped (i.e. the cube is shown everywhere but the sky). Default is false.
  invertLayerMask: boolean
  // How much to smooth the edges of the layer. Valid values: [0.0, 1.0]. Default
  // is 1.0.
  edgeSmoothness: number
}

interface LayersControllerOptions {
  nearClip?: number
  farClip?: number
  coordinates?: Coordinates | null
  layers: Record<LayerName, LayerOptions>
}

interface LayersDebugOptions {
  // If true, the debug output will include input mask, e.g. semantics image for sky segmentation
  inputMask: boolean
}

export {
  SUPPORTED_LAYERS,
  DEFAULT_INVERT_LAYER_MASK,
  DEFAULT_EDGE_SMOOTHNESS,
  LayerName,
  LayerOptions,
  LayersControllerOptions,
  LayersDebugOptions,
}
