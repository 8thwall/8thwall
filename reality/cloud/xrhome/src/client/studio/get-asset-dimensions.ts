import {fileExt} from '../editor/editor-common'
import {ASSET_EXT_TO_KIND} from './common/studio-files'

const getAssetDimensions = (
  filePath: string, onLoad: (width: number, height: number) => void
) => {
  const ext = fileExt(filePath)
  const fileType = ASSET_EXT_TO_KIND[ext]

  if (fileType === 'image') {
    const img = new Image()
    img.src = filePath
    img.onload = () => onLoad(img.naturalWidth, img.naturalHeight)
  } else if (fileType === 'video') {
    const video = document.createElement('video')
    video.src = filePath
    video.onloadedmetadata = () => onLoad(video.videoWidth, video.videoHeight)
  }
}

export {
  getAssetDimensions,
}
