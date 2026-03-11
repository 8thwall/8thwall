import React from 'react'

import {downloadBlob} from '../../common/download-utils'

const EXTENSIONS = {
  'image/png': 'png',
  'model/gltf-binary': 'glb',
}

const useDownloadAssets = () => {
  const [isDownloading, setIsDownloading] = React.useState(false)

  return {
    downloadFromUrl: async (url: string, uuid: string) => {
      setIsDownloading(true)
      const response = await fetch(url)
      if (!response.ok) {
        throw new Error(`Failed to download asset: ${response.statusText}`)
      }
      const blob = await response.blob()
      const extension = EXTENSIONS[blob.type]
      const filename = `${uuid}.${extension}`
      downloadBlob(blob, filename)
      setIsDownloading(false)
    },
    isDownloading,
  }
}

export {
  useDownloadAssets,
}
