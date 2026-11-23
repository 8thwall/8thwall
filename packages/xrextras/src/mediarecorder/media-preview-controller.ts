import {useCallback, useEffect, useMemo, useRef, useState} from 'preact/hooks'

import {configure, getConfig} from './capture-config'
import {createHtmlElement} from './dom'
import {resolvePreviewActions} from './media-preview-actions'
import type {MediaPreviewOptions, MediaPreviewState} from './media-preview-types'
import type {MediaRecorderStrings} from './strings'

const IOS_DOWNLOAD_LOCATION = 'shareddocuments:///private/var/mobile/Library/Mobile Documents/com~apple~CloudDocs/Downloads/'

type FinalizeAction = 'download' | 'share' | null

const clickAnchor = (properties: Partial<HTMLAnchorElement>) => {
  const anchor = createHtmlElement('a')
  Object.assign(anchor, properties)
  document.body.appendChild(anchor)
  anchor.click()
  anchor.remove()
}

const getTimestamp = () => {
  const now = new Date()
  return [now.getHours(), now.getMinutes(), now.getSeconds()]
    .map(n => n.toString().padStart(2, '0')).join('')
}

const defaultPreviewState = (): MediaPreviewState => ({
  downloaded: false,
  finalizeProgress: 0,
  finalizeTotal: 100,
  finalizeWaiting: false,
  imageSrc: undefined,
  interactable: false,
  muted: false,
  previewType: null,
  videoSrc: undefined,
  visible: false,
})

const useMediaPreview = (options: MediaPreviewOptions, strings: MediaRecorderStrings) => {
  const actions = useMemo(() => resolvePreviewActions(options, strings), [options, strings])
  const [state, setState] = useState<MediaPreviewState>(defaultPreviewState)
  const afterFinalizeAction = useRef<FinalizeAction>(null)
  const closedWhileFinalizing = useRef(false)
  const currentBlob = useRef<Blob | null>(null)
  const currentDownloadUrl = useRef<string | null>(null)
  const currentFilename = useRef<string | null>(null)
  const currentUrl = useRef<string | null>(null)
  const previewInteractableTimeout = useRef<number>()
  const stringsRef = useRef(strings)
  const videoPending = useRef(false)
  const videoReadyHandler = useRef<(video: HTMLVideoElement) => void>(() => {})
  const videoRef = useRef<HTMLVideoElement>(null)
  const visibleObjectUrl = useRef<string | null>(null)
  stringsRef.current = strings

  const clearResources = useCallback(() => {
    if (currentUrl.current) {
      URL.revokeObjectURL(currentUrl.current)
    }
    if (visibleObjectUrl.current) {
      URL.revokeObjectURL(visibleObjectUrl.current)
    }
    window.clearTimeout(previewInteractableTimeout.current)
    currentBlob.current = null
    currentUrl.current = null
    currentDownloadUrl.current = null
    currentFilename.current = null
    visibleObjectUrl.current = null
    videoPending.current = false
    afterFinalizeAction.current = null
    closedWhileFinalizing.current = false
    videoReadyHandler.current = () => {}
  }, [])

  const clearPreview = useCallback(() => {
    clearResources()
    setState(defaultPreviewState())
  }, [clearResources])

  const fileName = useCallback((extension: string) => (
    `${getConfig().fileNamePrefix || stringsRef.current.fileNamePrefix}` +
    `${getTimestamp()}.${extension}`
  ), [])

  const showPreview = useCallback(() => {
    setState(current => ({...current, visible: true}))
    window.dispatchEvent(new CustomEvent('mediarecorder-previewopened'))
    // Prevent the capture gesture from also activating a newly revealed preview control.
    window.clearTimeout(previewInteractableTimeout.current)
    previewInteractableTimeout.current = window.setTimeout(() => {
      setState(current => ({...current, interactable: true}))
    }, 100)
  }, [])

  const showImagePreview = useCallback((blob: Blob) => {
    clearResources()
    currentBlob.current = blob
    currentUrl.current = URL.createObjectURL(blob)
    currentFilename.current = fileName('jpg')
    setState({
      ...defaultPreviewState(),
      imageSrc: currentUrl.current,
      previewType: 'image',
    })
    showPreview()
  }, [clearResources, fileName, showPreview])

  const showVideoPreview = useCallback((videoBlob: Blob) => {
    clearResources()
    currentBlob.current = videoBlob
    currentUrl.current = URL.createObjectURL(videoBlob)
    currentFilename.current = fileName('mp4')

    let waitingOnDataUrl = true
    let readyVideo: HTMLVideoElement | null = null
    const maybeFinishLoading = () => {
      if (!readyVideo || waitingOnDataUrl) {
        return
      }
      showPreview()
      const playVideo = async () => {
        try {
          await readyVideo?.play()
          // Restarting playback fixes low audio volume on iOS.
          if (window.XR8.XrDevice.deviceEstimate().os === 'iOS') {
            readyVideo?.pause()
            readyVideo?.play()
          }
        } catch {
          setState(current => ({...current, muted: true}))
          if (readyVideo) {
            readyVideo.muted = true
            readyVideo.play()
          }
        }
      }
      playVideo()
    }

    videoReadyHandler.current = (video) => {
      readyVideo = video
      maybeFinishLoading()
    }

    // A data URL avoids iOS opening a broken player from the fallback View action.
    if (window.XR8.XrDevice.deviceEstimate().os === 'iOS' && !actions.webShareAPILevel2) {
      const reader = new FileReader()
      reader.readAsDataURL(videoBlob)
      reader.onloadend = () => {
        waitingOnDataUrl = false
        currentDownloadUrl.current = reader.result as string
        maybeFinishLoading()
      }
    } else {
      waitingOnDataUrl = false
    }

    setState({
      ...defaultPreviewState(),
      previewType: 'video',
      videoSrc: currentUrl.current || undefined,
    })
  }, [actions.webShareAPILevel2, clearResources, fileName, showPreview])

  const closePreview = useCallback(() => {
    const wasPending = videoPending.current
    clearPreview()
    window.dispatchEvent(new CustomEvent('mediarecorder-previewclosed'))
    closedWhileFinalizing.current = wasPending
  }, [clearPreview])

  const downloadFile = useCallback(() => {
    if (videoPending.current) {
      afterFinalizeAction.current = 'download'
      setState(current => ({...current, finalizeWaiting: true}))
      return
    }
    clickAnchor({
      href: currentDownloadUrl.current || currentUrl.current || '',
      download: currentFilename.current || '',
    })
    setState(current => ({...current, downloaded: true}))
  }, [])

  const openIosDownload = useCallback(() => {
    clickAnchor({href: IOS_DOWNLOAD_LOCATION + currentFilename.current, target: '_blank'})
  }, [])

  const share = useCallback(() => {
    if (videoPending.current) {
      afterFinalizeAction.current = 'share'
      setState(current => ({...current, finalizeWaiting: true}))
      return
    }
    const blob = currentBlob.current
    if (!blob) {
      return
    }
    const mediaType = currentFilename.current?.endsWith('.jpg')
      ? 'image/jpeg'
      : 'video/mp4'
    const file = new File([blob], currentFilename.current || '', {
      type: mediaType,
      lastModified: Date.now(),
    })
    navigator.share({title: '', text: '', files: [file]})
  }, [])

  const performAction = useCallback(() => {
    if (actions.actionButton?.kind === 'share') {
      share()
    } else if (actions.actionButton?.kind === 'view') {
      openIosDownload()
    }
  }, [actions.actionButton?.kind, openIosDownload, share])

  const toggleMuted = useCallback(() => {
    setState(current => ({...current, muted: !current.muted}))
  }, [])

  useEffect(() => {
    const showVideoHandler = (event: Event) => {
      const {videoBlob} = (event as CustomEvent<{videoBlob: Blob}>).detail
      if (closedWhileFinalizing.current) {
        closedWhileFinalizing.current = false
        return
      }
      if (!videoPending.current) {
        showVideoPreview(videoBlob)
        return
      }
      videoPending.current = false
      visibleObjectUrl.current = currentUrl.current
      currentBlob.current = videoBlob
      currentUrl.current = URL.createObjectURL(videoBlob)
      currentFilename.current = fileName('mp4')
      setState(current => ({...current, finalizeWaiting: false}))
      if (afterFinalizeAction.current === 'share') {
        share()
      } else if (afterFinalizeAction.current === 'download') {
        downloadFile()
      }
      afterFinalizeAction.current = null
    }

    const previewVideoHandler = (event: Event) => {
      const {videoBlob} = (event as CustomEvent<{videoBlob: Blob}>).detail
      showVideoPreview(videoBlob)
      videoPending.current = true
    }

    const showImageHandler = (event: Event) => {
      showImagePreview((event as CustomEvent<{blob: Blob}>).detail.blob)
    }

    const progressHandler = (event: Event) => {
      const {progress, total} = (event as CustomEvent<{progress: number, total: number}>).detail
      setState(current => ({...current, finalizeProgress: progress, finalizeTotal: total}))
    }

    configure()
    window.addEventListener('mediarecorder-recordcomplete', showVideoHandler)
    window.addEventListener('mediarecorder-photocomplete', showImageHandler)
    window.addEventListener('mediarecorder-previewready', previewVideoHandler)
    window.addEventListener('mediarecorder-finalizeprogress', progressHandler)
    return () => {
      window.removeEventListener('mediarecorder-recordcomplete', showVideoHandler)
      window.removeEventListener('mediarecorder-photocomplete', showImageHandler)
      window.removeEventListener('mediarecorder-previewready', previewVideoHandler)
      window.removeEventListener('mediarecorder-finalizeprogress', progressHandler)
      clearResources()
    }
  }, [clearResources, downloadFile, fileName, share, showImagePreview, showVideoPreview])

  useEffect(() => {
    if (!state.videoSrc) {
      videoRef.current?.pause()
      return
    }
    const video = videoRef.current
    if (video) {
      video.load()
      // Waiting for canplaythrough can stall indefinitely on iOS 17.4.
      videoReadyHandler.current(video)
    }
  }, [state.videoSrc])

  return {
    actions,
    closePreview,
    downloadFile,
    performAction,
    state,
    toggleMuted,
    videoRef,
  }
}

export {useMediaPreview}
