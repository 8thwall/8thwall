import {dispatchify} from '../../common'
import authenticatedFetch from '../../common/authenticated-fetch'
import type {DispatchifiedActions} from '../../common/types/actions'

const convertAssetRequest = (
  filename: string,
  conversionType: string,
  conversionParams: Object = {}
) => dispatch => dispatch(
  authenticatedFetch('/v1/asset-converter/upload', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    json: false,
    body: JSON.stringify({
      filename,
      conversionType,
      conversionParams,
    }),
  })
)

const uploadFileToS3 = (file: File, uploadUrl: string) => dispatch => dispatch(
  authenticatedFetch(uploadUrl, {
    method: 'PUT',
    body: file,
    json: false,
  })
)

const rawActions = {
  convertAssetRequest,
  uploadFileToS3,
}

type AssetConverterActions = DispatchifiedActions<typeof rawActions>

export default dispatchify(rawActions)

export {
  AssetConverterActions,
  rawActions,
}
