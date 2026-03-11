import React from 'react'
import {useTranslation} from 'react-i18next'
import JSZip from 'jszip'

import useActions from '../../common/use-actions'
import assetConverterActions from '../actions/asset-converter-actions'
import {makeRunQueue} from '../../../shared/run-queue'
import {sanitizeFilePath} from '../../common/editor-files'
import {MILLISECONDS_PER_SECOND} from '../../../shared/time-utils'

// TODO(cindyhu): Asset converter lambda timeout is 60 sec but might be changed later to accommodate
// larger models. Will need to update POLLING_TIMEOUT accordingly.
const MAX_CONVERSION_REQUESTS = 10
const POLLING_INTERVAL = 3 * MILLISECONDS_PER_SECOND
const POLLING_TIMEOUT = 300 * MILLISECONDS_PER_SECOND

type ConversionType = 'fbxToGlb' | 'fontToMtsdf'
type ConversionStatus = 'pending' | 'completed' | 'failed'
type ConversionState = {
  [fileName: string]:
  | {type: ConversionType, status: 'pending'}
  | {type: ConversionType, status: 'completed'; files: File[]}
  | {type: ConversionType, status: 'failed'; error: string}
}

const useAssetConverter = () => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const {convertAssetRequest, uploadFileToS3} = useActions(assetConverterActions)
  const [conversionState, setConversionState] = React.useState<ConversionState>({})
  const intervalIds = React.useRef<any[]>([])
  const timeoutIds = React.useRef<any[]>([])
  const pendingFetches = new Set<string>()

  React.useEffect(() => () => {
    intervalIds.current.forEach(clearInterval)
    timeoutIds.current.forEach(clearTimeout)
  }, [])

  const patchConversionState = (fn: (prev: any) => any) => {
    setConversionState(prev => ({
      ...prev,
      ...fn(prev),
    }))
  }

  const removeFileFromConversionState = (fileName: string) => {
    setConversionState((prev) => {
      const newState = {...prev}
      delete newState[fileName]
      return newState
    })
  }

  const pollDownloadUrl = (fileName: string, downloadUrl: string) => {
    let timeoutId

    const intervalId = setInterval(async () => {
      if (pendingFetches.has(downloadUrl)) {
        return
      }
      try {
        pendingFetches.add(downloadUrl)
        const response = await fetch(downloadUrl)
        pendingFetches.delete(downloadUrl)

        if (response.ok && response.headers.get('Content-Type') === 'application/zip') {
          clearInterval(intervalId)
          clearTimeout(timeoutId)

          const zip = new JSZip()
          const arrayBuffer = await response.arrayBuffer()
          const zipFile = await zip.loadAsync(arrayBuffer)
          const filesArray: File[] = []

          await Promise.all(Object.entries(zipFile.files).map(async ([relativePath, file]) => {
            if (!file.dir) {
              const content = await file.async('arraybuffer')
              const blob = new Blob([content])
              const fileObject = new File([blob], relativePath)
              filesArray.push(fileObject)
            }
          }))

          patchConversionState(() => ({
            [fileName]: {
              status: 'completed',
              files: filesArray,
            },
          }))
        }
      } catch (err) {
        // waiting for file conversion to complete on asset-converter-file-converter lambda
      }
    }, POLLING_INTERVAL)

    timeoutId = setTimeout(() => {
      clearInterval(intervalId)
      patchConversionState(() => ({
        [fileName]: {
          status: 'failed',
          error: t('fbx_to_glb_modal.error.timeout_error', {ns: 'cloud-studio-pages'}),
        },
      }))
    }, POLLING_TIMEOUT)

    intervalIds.current.push(intervalId)
    timeoutIds.current.push(timeoutId)
  }

  const convertAssets = async (filesAsArray: File[], convertTo: ConversionType) => {
    const unconvertedFiles = filesAsArray.filter(file => !conversionState[file.name])
    patchConversionState(() => unconvertedFiles.reduce((acc, file) => ({
      [file.name]: {
        type: convertTo,
        status: 'pending',
      },
      ...acc,
    }), {}))
    const runQueue = makeRunQueue(MAX_CONVERSION_REQUESTS)
    await Promise.all(unconvertedFiles.map(file => runQueue.next(async () => {
      try {
        const sanitizedFilePath = sanitizeFilePath(file.name)
        const res = await convertAssetRequest(sanitizedFilePath, convertTo)
        const {uploadUrl, downloadUrl} = res
        await uploadFileToS3(file, uploadUrl)
        patchConversionState(() => ({
          [file.name]: {
            type: convertTo,
            status: 'pending',
          },
        }))
        pollDownloadUrl(file.name, downloadUrl)
      } catch (err) {
        patchConversionState(() => ({
          [file.name]: {
            type: convertTo,
            status: 'failed',
            error: `${t(
              'fbx_to_glb_modal.error.conversion_error', {ns: 'cloud-studio-pages'}
            )}: ${err.message}`,
          },
        }))
      }
    })))
  }

  return {convertAssets, conversionState, removeFileFromConversionState}
}

export type {
  ConversionStatus,
}

export {
  useAssetConverter,
}
