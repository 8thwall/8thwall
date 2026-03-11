import type {NaeInfo} from '../../common/types/db'
import type {
  HistoricalBuildData,
  HtmlShell,
  AppleSigningType,
  NaeBuilderRequest,
  GenerateCsrRequest,
  GenerateCsrResponse,
  UploadSigningFilesResponse,
  SigningFilesInfoResponse,
  UploadAuthKeyResponse,
  DownloadCsrResponse,
  DeleteCertificateResponse,
} from '../../../shared/nae/nae-types'
import {dispatchify} from '../../common'
import authenticatedFetch from '../../common/authenticated-fetch'
import type {DispatchifiedActions} from '../../common/types/actions'

const postNaeBuild = (
  buildConfig: NaeBuilderRequest
) => async (dispatch): Promise<any | null> => {
  try {
    return await dispatch(authenticatedFetch('/v1/nae/build', {
      method: 'POST',
      body: JSON.stringify(buildConfig),
    }))
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

type NaeInfoUpdate = {
  platform: HtmlShell
  appUuid: string
  displayName?: string
  bundleId?: string
  file?: File
  launchScreenFile?: File
  iconId?: string
  launchScreenIconId?: string
  lastBuiltVersionName?: string
  permissions?: string
}

const updateNaeInfo = (
  naeInfo: NaeInfoUpdate
) => async (dispatch): Promise<{naeInfo: NaeInfo} | null> => {
  try {
    const formData = new FormData()
    if (!naeInfo.platform) {
      throw new Error('must provide a platform when updating NaeInfo')
    }
    formData.append('platform', naeInfo.platform)
    if (naeInfo.file) {
      formData.append('image', naeInfo.file)
    }
    if (naeInfo.launchScreenFile) {
      formData.append('launchScreenFile', naeInfo.launchScreenFile)
    }
    if (naeInfo.iconId) {
      formData.append('iconId', naeInfo.iconId)
    }
    if (naeInfo.launchScreenIconId) {
      formData.append('launchScreenIconId', naeInfo.launchScreenIconId)
    }
    if (naeInfo.displayName) {
      formData.append('displayName', naeInfo.displayName)
    }
    if (naeInfo.bundleId) {
      formData.append('bundleId', naeInfo.bundleId)
    }
    if (naeInfo.lastBuiltVersionName) {
      formData.append('lastBuiltVersionName', naeInfo.lastBuiltVersionName)
    }
    if (naeInfo.permissions) {
      formData.append('permissions', naeInfo.permissions)
    }

    const res = await dispatch(authenticatedFetch(
      `/v1/nae/updateInfo/${naeInfo.appUuid}`, {
        method: 'PUT',
        body: formData,
        json: false,
      }
    ))
    dispatch({type: 'NAE_INFO_SET', naeInfo: res.naeInfo})
    return res
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

type SignedUrlForNaeBuildRequest = {
  appUuid: string
  buildId: string
  appExtension: string
}

const signedUrlForNaeBuild = (
  signedUrlForNaeBuildRequest: SignedUrlForNaeBuildRequest
) => async (dispatch): Promise<{signedUrl: string} | null> => {
  try {
    return await dispatch(authenticatedFetch('/v1/nae/signedUrlForBuild', {
      method: 'POST',
      body: JSON.stringify(signedUrlForNaeBuildRequest),
    }))
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

const getRecentNaeBuilds = (
  appUuid: string,
  platform?: HtmlShell
) => async (dispatch): Promise<{recentNaeBuilds: HistoricalBuildData[]} | null> => {
  try {
    const url = `/v1/nae/recentBuilds/${appUuid}${platform ? `?platform=${platform}` : ''}`
    return await dispatch(authenticatedFetch(url, {method: 'GET'}))
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

const generateCsr = (
  generateCsrBody: GenerateCsrRequest
) => async (dispatch): Promise<GenerateCsrResponse | null> => {
  try {
    return await dispatch(authenticatedFetch('/v1/nae/apple-signing/generate-csr', {
      method: 'POST',
      body: JSON.stringify(generateCsrBody),
    }))
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

const downloadCsr = (
  appUuid: string,
  signingType: AppleSigningType
) => async (dispatch): Promise<DownloadCsrResponse | null> => {
  try {
    const response = await dispatch(authenticatedFetch(
      `/v1/nae/apple-signing/download-csr/${appUuid}/${signingType}`, {
        method: 'GET',
        json: false,
      }
    ))
    const csrData: DownloadCsrResponse = JSON.parse(response)

    return csrData
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

type UploadSigningFilesInput = {
  appUuid: string
  accountUuid: string
  bundleIdentifier: string
  signingType: AppleSigningType
  certificate?: File
  certificateUuid?: string
  certificateP12Password?: string
  provisioningProfile: File
  certificateSigningRequestBase64?: string
  certificateSigningRequestPrivateKeyBase64?: string
}

const getUploadSigningFilesFormData = (input: UploadSigningFilesInput) => {
  const formData = new FormData()
  formData.append('appUuid', input.appUuid)
  formData.append('accountUuid', input.accountUuid)
  formData.append('bundleIdentifier', input.bundleIdentifier)
  formData.append('signingType', input.signingType)
  if (input.certificate) {
    formData.append('certificate', input.certificate)
  }
  if (input.certificateUuid) {
    formData.append('certificateUuid', input.certificateUuid)
  }
  if (input.certificateP12Password) {
    formData.append('certificateP12Password', input.certificateP12Password)
  }
  formData.append('provisioningProfile', input.provisioningProfile)
  if (input.certificateSigningRequestBase64) {
    formData.append('certificateSigningRequestBase64', input.certificateSigningRequestBase64)
  }
  if (input.certificateSigningRequestPrivateKeyBase64) {
    formData.append(
      'certificateSigningRequestPrivateKeyBase64', input.certificateSigningRequestPrivateKeyBase64
    )
  }
  return formData
}

const uploadSigningFiles = (
  input: UploadSigningFilesInput
) => async (dispatch): Promise<UploadSigningFilesResponse> => dispatch(
  authenticatedFetch('/v1/nae/apple-signing/upload-signing-files', {
    method: 'POST',
    body: getUploadSigningFilesFormData(input),
    json: false,
  })
)

const signingFilesInfo = (
  appUuid: string
) => async (dispatch): Promise<SigningFilesInfoResponse | null> => {
  try {
    return await dispatch(
      authenticatedFetch(`/v1/nae/apple-signing/signing-files-info/${appUuid}`, {
        method: 'GET',
      })
    )
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

type UploadAuthKeyInput = {
  appUuid: string
  keyId: string
  issuerId: string
  authKey: File
}

const getUploadAuthKeyFormData = (input: UploadAuthKeyInput) => {
  const formData = new FormData()
  formData.append('appUuid', input.appUuid)
  formData.append('keyId', input.keyId)
  formData.append('issuerId', input.issuerId)
  formData.append('authKey', input.authKey)
  return formData
}

const uploadAuthKey = (
  input: UploadAuthKeyInput
) => async (dispatch): Promise<UploadAuthKeyResponse | null> => {
  try {
    return await dispatch(authenticatedFetch('/v1/nae/apple-signing/upload-auth-key', {
      method: 'POST',
      body: getUploadAuthKeyFormData(input),
      json: false,
    }))
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

const deleteCertificate = (
  accountUuid: string,
  appUuid: string,
  certificateUuid: string
) => async (dispatch): Promise<DeleteCertificateResponse | null> => dispatch(
  authenticatedFetch(
    `/v1/nae/apple-signing/delete-certificate/${accountUuid}/${appUuid}/${certificateUuid}`, {
      method: 'DELETE',
    }
  )
)

const deleteSigningConfig = (
  appUuid: string,
  signingType: AppleSigningType
) => async (dispatch): Promise<{statusCode: boolean} | null> => {
  try {
    return await dispatch(authenticatedFetch(
      `/v1/nae/apple-signing/delete-signing-config/${appUuid}/${signingType}`, {
        method: 'DELETE',
      }
    ))
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

const uploadToAppStoreConnect = (
  appUuid: string,
  buildId: string
) => async (dispatch): Promise<{statusCode: boolean}> => {
  try {
    return await dispatch(authenticatedFetch(
      `/v1/nae/apple-signing/upload-to-app-store-connect/${appUuid}`, {
        method: 'POST',
        body: JSON.stringify({buildId}),
      }
    ))
  } catch (err) {
    dispatch({type: 'ERROR', msg: err.message})
    return null
  }
}

const rawActions = {
  postNaeBuild,
  updateNaeInfo,
  signedUrlForNaeBuild,
  getRecentNaeBuilds,
  generateCsr,
  downloadCsr,
  uploadSigningFiles,
  signingFilesInfo,
  uploadAuthKey,
  uploadToAppStoreConnect,
  deleteCertificate,
  deleteSigningConfig,
}

export {
  getUploadSigningFilesFormData,
  getUploadAuthKeyFormData,
  rawActions,
}

export type NaeActions = DispatchifiedActions<typeof rawActions>

export default dispatchify(rawActions)
