type ModelManagerMessage = 'loadBytes' | 'updateView' | 'workerLoaded'

interface LoadBytesRequestData {
  filename: string
  bytes: Uint8Array
}

interface MergeBytesRequestData {
  filename: string
  position: number[]
  rotation: number[]
  bytes: Uint8Array
}

interface LoadFilesRequest {
  msg: 'loadFiles'
  data: LoadBytesRequestData[]
}

interface LoadBytesRequest {
  msg: 'loadBytes'
  data: LoadBytesRequestData
}

interface MergeBytesRequest {
  msg: 'mergeBytes'
  data: MergeBytesRequestData
}

interface DisposeRequest {
  msg: 'dispose'
}

interface UpdateViewRequestData {
  cameraPos: number[]
  cameraRot: number[]
  modelPos: number[]
  modelRot: number[]
}

interface UpdateViewRequest {
  msg: 'updateView'
  data: UpdateViewRequestData
}

interface WorkerLoadedRequest {
  msg: 'workerLoaded'
}

interface LoadBytesResponse {
  msg: 'loadBytes'
  data: Uint8Array
}

interface LoadFilesResponse {
  msg: 'loadFiles'
  data: Uint8Array
}

interface MergeBytesResponse {
  msg: 'mergeBytes'
  data: Uint8Array
}

interface UpdateViewResponse {
  msg: 'updateView'
  updated: boolean
  data?: Uint8Array
}

interface DisposeResponse {
  msg: 'dispose'
}

interface WorkerLoadedResponse {
  msg: 'workerLoaded'
}

interface ModelManagerConfiguration {
  coordinateSpace: number          // E.g. RUF (1) or RUB (2) (see model-manager.h)
  splatResortRadians: number       // Angle delta before resorting splats.
  splatResortMeters: number        // Distance delta before resorting splats.
  bakeSkyboxMeters: number         // Distance from origin to bake skybox (0 to disable).
  sortTexture: boolean             // Resort the splat texture on every update, if preferTexture is true.
  multiTexture: boolean            // Use multiple textures to store splat data.
  preferTexture: boolean           // Represent splats as interleaved data in a texture vs attributes.
  pointPruneDistance: number       // Distance from origin to prune points (0 to disable).
  pointMinDistance: number         // Distance from camera to prune points (0 to disable).
  pointFrustumLimit: number        // Ray units from camera center to prune points (0 to disable).
  pointSizeLimit: number           // Ray unit size of points to render (0 to disable).
  useOctree: boolean               // Represent splats in an Octree for visibility culling
  outputLinearColorSpace: boolean  // Gamma-correct fragment shader for linear color space output
}

interface ConfigureRequest {
  msg: 'configure'
  config: Partial<ModelManagerConfiguration>
}

interface ConfigureResponse {
  msg: 'configure'
}

interface ModelSrc {
  filename?: string
  url: string
}

type ModelManagerRequest = LoadBytesRequest | UpdateViewRequest | WorkerLoadedRequest |
  ConfigureRequest | LoadFilesRequest | MergeBytesRequest | DisposeRequest
type ModelManagerResponse = LoadBytesResponse | UpdateViewResponse | WorkerLoadedResponse |
  ConfigureResponse | LoadFilesResponse | MergeBytesResponse | DisposeResponse

export {
  LoadBytesRequest,
  LoadBytesRequestData,
  LoadFilesRequest,
  LoadFilesResponse,
  LoadBytesResponse,
  ConfigureResponse,
  MergeBytesRequest,
  MergeBytesRequestData,
  MergeBytesResponse,
  ModelManagerConfiguration,
  ModelManagerMessage,
  ModelManagerRequest,
  ModelManagerResponse,
  ModelSrc,
  UpdateViewRequest,
  UpdateViewRequestData,
  UpdateViewResponse,
  WorkerLoadedRequest,
  WorkerLoadedResponse,
}
