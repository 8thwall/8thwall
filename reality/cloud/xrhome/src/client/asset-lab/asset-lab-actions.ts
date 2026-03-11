import {dispatchify} from '../common'
import type {AsyncThunk, DispatchifiedActions} from '../common/types/actions'
import authenticatedFetch from '../common/authenticated-fetch'
import {
  AL_GET_ASSET_GENERATIONS, AL_GET_ASSET_REQUESTS, AL_UPDATE_ASSET_GENERATION_METADATA,
} from './action-types'
import type {AssetGeneration, AssetRequest, AssetRequestStatus} from '../common/types/db'
import type {GenerateRequest} from '../../shared/genai/types/generate'
import type {AssetGenerationMeshMetadata, AssetGenerationWithCost} from './types'
import type {AssetInput} from '../../shared/genai/types/base'
import {
  IMAGE_FORM_NAME, MESH_FORM_NAME, FILE_IMAGE_FORM_NAME, FILE_MESH_FORM_NAME,
} from '../../shared/genai/constants'

type AppendAssetInput = {
  formData: FormData
  fieldName: string
  fileFieldName: string
  assetInput: AssetInput
}
const appendAssetInputFormData = ({
  formData,
  fieldName,
  fileFieldName,
  assetInput,
}: AppendAssetInput) => {
  const {type, value} = assetInput
  if (type === 'file') {
    // Strip the value since files are uploaded separately.
    formData.append(fieldName, JSON.stringify({type, value: ''}))
    formData.append(fileFieldName, value)
  } else {
    formData.append(fieldName, JSON.stringify(assetInput))
  }
}

const makeGenerateFormData = (assetGenRequest: GenerateRequest) => {
  const formData = new FormData()
  if (assetGenRequest.type === 'IMAGE_TO_IMAGE' || assetGenRequest.type === 'IMAGE_TO_MESH') {
    assetGenRequest.images.forEach((assetInput) => {
      appendAssetInputFormData({
        formData,
        fieldName: IMAGE_FORM_NAME,
        fileFieldName: FILE_IMAGE_FORM_NAME,
        assetInput,
      })
    })
  }
  if (assetGenRequest.type === 'MESH_TO_ANIMATION') {
    appendAssetInputFormData({
      formData,
      fieldName: MESH_FORM_NAME,
      fileFieldName: FILE_MESH_FORM_NAME,
      assetInput: assetGenRequest.mesh,
    })
  }
  Object.entries(assetGenRequest).forEach(([key, value]) => {
    if (key !== 'images' && key !== 'mesh' && value !== undefined) {
      formData.append(key, value)
    }
  })
  return formData
}

type AssetGenReturnType = {assetGenerations: AssetGenerationWithCost[]}
const getAssetGenerations = (
  accountUuid: string
): AsyncThunk<AssetGenReturnType> => async (dispatch) => {
  const res: AssetGenReturnType = await dispatch(authenticatedFetch(
    `/v1/genAi/assetGenerations/${accountUuid}`
  ))

  dispatch({type: AL_GET_ASSET_GENERATIONS, assetGenerations: res.assetGenerations, accountUuid})
  return res
}

type AssetReqReturnType = {assetRequests: AssetRequest[]}
const getAssetRequests = (
  accountUuid: string,
  opts: {
    status?: AssetRequestStatus
    assetRequestUuids?: string[]
  } = {}
): AsyncThunk<AssetReqReturnType> => async (dispatch) => {
  const searchParams = new URLSearchParams()
  if (opts.status) {
    searchParams.append('status', opts.status)
  }
  if (opts.assetRequestUuids) {
    searchParams.append('assetRequestUuids', opts.assetRequestUuids.join(','))
  }
  const res: AssetReqReturnType = await dispatch(authenticatedFetch(
    `/v1/genAi/assetRequests/${accountUuid}?${searchParams.toString()}`
  ))

  dispatch({type: AL_GET_ASSET_REQUESTS, assetRequests: res.assetRequests, accountUuid})
  return res
}

type AssetReqWithGenerationsReturnType = {
  assetRequests: AssetRequest[]
  assetGenerations: AssetGeneration[]
}
const getAssetRequestsWithGenerations = (
  accountUuid: string,
  reqUuids: string[]
): AsyncThunk<AssetReqWithGenerationsReturnType> => async (dispatch) => {
  const res: AssetReqWithGenerationsReturnType = await dispatch(authenticatedFetch(
    `/v1/genAi/assetRequestsWithGenerations/${accountUuid}/${reqUuids.join(',')}`
  ))

  dispatch({type: AL_GET_ASSET_REQUESTS, assetRequests: res.assetRequests, accountUuid})
  dispatch({type: AL_GET_ASSET_GENERATIONS, assetGenerations: res.assetGenerations, accountUuid})
  return res
}

type GetSignedUrlStatus = 'already_exists' | 'success' | 'failed'
const getMeshUploadSignedUrl = (
  accountUuid: string, assetGenerationUuid: string, overwrite: boolean = false
): AsyncThunk<{status: GetSignedUrlStatus, signedUrl?: string}> => async (dispatch) => {
  const url = `/v1/genai/reuploadUrl/${accountUuid}/${assetGenerationUuid}`
  try {
    const response = await dispatch(authenticatedFetch(
      url + (overwrite ? '?overwrite=1' : '')
    )) as {signedUrl: string, status: GetSignedUrlStatus}
    return {
      status: response.status,
      signedUrl: response.signedUrl,
    }
  } catch (e) {
    throw new Error(`Failed to get mesh upload signed URL: ${e.message}`)
  }
}

type GenAssetsReturnType = {assetRequest: AssetRequest}
const generateAssets = (
  accountUuid: string, assetGenRequest: GenerateRequest
): AsyncThunk<GenAssetsReturnType> => async (dispatch) => {
  const formData = makeGenerateFormData(assetGenRequest)
  const res: GenAssetsReturnType = await dispatch(authenticatedFetch(
    `/v1/genAi/generateAssets/${accountUuid}`,
    {method: 'POST', body: formData, json: false}
  ))
  dispatch({type: AL_GET_ASSET_REQUESTS, assetRequests: [res.assetRequest], accountUuid})
  return res
}

// TODO(dat): More complete type for AssetGenerationMetadata. Right now this is only used for mesh.
type UpdateAssetGenerationMetadataStatus = {
  updatedMetadata: AssetGenerationMeshMetadata
}
const updateAssetGenerationMetadata = (
  assetGenerationUuid: string,
  metadata: Partial<AssetGenerationMeshMetadata>
): AsyncThunk<UpdateAssetGenerationMetadataStatus> => async (dispatch) => {
  const {updatedMetadata} = await dispatch(
    authenticatedFetch(`/v1/genai/assetGenerations/${assetGenerationUuid}/metadata`, {
      method: 'PATCH',
      body: JSON.stringify(metadata),
    })
  ) as UpdateAssetGenerationMetadataStatus
  dispatch({type: AL_UPDATE_ASSET_GENERATION_METADATA, assetGenerationUuid, updatedMetadata})
  return {updatedMetadata}
}
export const rawActions = {
  getAssetGenerations,
  getAssetRequests,
  getAssetRequestsWithGenerations,
  getMeshUploadSignedUrl,
  generateAssets,
  updateAssetGenerationMetadata,
}

export type AssetLabActions = DispatchifiedActions<typeof rawActions>

export default dispatchify(rawActions)
