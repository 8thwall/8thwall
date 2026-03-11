import type React from 'react'

import {useCanvasPool} from '../../common/resource-pool'
import {useFileUploadState} from '../../editor/hooks/use-file-upload-state'
import {getImageBlobFromUrl} from '../../apps/image-targets/image-helpers'
import {useEnclosedApp} from '../../apps/enclosed-app-context'

const convertElementToFile = async (
  img: React.MutableRefObject<HTMLImageElement>,
  fileName: string,
  canvasPool: ReturnType<typeof useCanvasPool>
) => {
  const currentImg = img.current
  const imageType = 'image/png'
  const imageBlob = await getImageBlobFromUrl(currentImg, currentImg.naturalWidth,
    currentImg.naturalHeight, imageType, canvasPool)
  const file = new File([imageBlob], fileName, {type: imageType})
  return file
}

const noOp = async () => {
  // no-op
}

// Safe to use even when there is no project. Will return no-op functions.
const useImportIntoProject = () => {
  const canvasPool = useCanvasPool()

  const app = useEnclosedApp()
  // We might not be in an app context
  const {uploadFiles} = useFileUploadState(app?.repoId, app?.assetLimitOverrides)
  if (!app) {
    return {
      importImg: noOp, importFromUrl: noOp, hasApp: false,
    }
  }

  return {
    hasApp: true,
    importImg: async (img: React.MutableRefObject<HTMLImageElement>, filename: string) => {
      const file = await convertElementToFile(img, filename, canvasPool)
      await uploadFiles([file])
    },
    importFromUrl: async (url: string, filename: string, fileType: string) => {
      // TODO(coco) here to switch to download/upload free importFromUrl in the future.
      const response = await fetch(url)
      const blob = await response.blob()
      const file = new File([blob], filename, {type: fileType})
      await uploadFiles([file])
    },
  }
}

export {
  convertElementToFile,
  useImportIntoProject,
}
