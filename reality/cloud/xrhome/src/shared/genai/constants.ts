import {MEGA} from '../app-constants'

// Image constraints are set by Bedrock.
const MAX_BEDROCK_IMG_SIZE_BYTES = 4 * MEGA
const MAX_BEDROCK_IMG_DIM = 8000
const IMAGE_FORM_NAME = 'images'
const MESH_FORM_NAME = 'mesh'
const FILE_IMAGE_FORM_NAME = 'fileImages'
const FILE_MESH_FORM_NAME = 'fileMesh'

const MIN_PROMPT_LEN = 3

const CDN_HOST = BuildIf.ALL_QA ? 'https://cdn-dev.8thwall.com' : 'https://cdn.8thwall.com'
const ASSET_GEN_PATH = 'asset-generations'

const getImageUrl = (
  accountUuid: string, assetGenUuid: string
) => `${CDN_HOST}/${ASSET_GEN_PATH}/${accountUuid}/${assetGenUuid}`

const getMeshUrl = (
  accountUuid: string, assetGenUuid: string
) => `${getImageUrl(accountUuid, assetGenUuid)}.glb`

const getOptimizedMeshUrl = (accountUuid: string, uuid: string, version?: number) => (
  `${getImageUrl(accountUuid, uuid)}/optimized.glb${version ? `?v=${version}` : ''}`
)

// e.g. https://cdn.8thwall.com/asset-generations/1234/5678/optimized.glb
// e.g. https://cdn.8thwall.com/asset-generations/1234/5678/optimized.glb?v=1234567890
const isOptimizedMeshUrl = (url: string) => {
  const urlWithoutParam = url?.split('?')[0] || ''
  return urlWithoutParam.endsWith('/optimized.glb')
}

export {
  MAX_BEDROCK_IMG_DIM,
  MAX_BEDROCK_IMG_SIZE_BYTES,
  IMAGE_FORM_NAME,
  MESH_FORM_NAME,
  FILE_IMAGE_FORM_NAME,
  FILE_MESH_FORM_NAME,
  CDN_HOST,
  ASSET_GEN_PATH,
  MIN_PROMPT_LEN,
  getImageUrl,
  getMeshUrl,
  getOptimizedMeshUrl,
  isOptimizedMeshUrl,
}
