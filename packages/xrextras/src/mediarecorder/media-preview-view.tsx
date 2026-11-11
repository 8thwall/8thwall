import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useCallback, useEffect, useMemo, useRef, useState} from 'preact/hooks'

import {combine} from '../classnames'
import {configure, getConfig} from './capture-config'
import CLOSE_SRC from './close.svg'
import DOWNLOAD_SRC from './download.svg'
import SHARE_SRC from './share.svg'
import SOUND_OFF_SRC from './sound-off.svg'
import SOUND_ON_SRC from './sound-on.svg'
import styles from './media-preview.module.scss'
import type {MediaRecorderStrings} from './strings'
import VIEW_SRC from './view.svg'

const IOS_DOWNLOAD_LOCATION = 'shareddocuments:///private/var/mobile/Library/Mobile Documents/com~apple~CloudDocs/Downloads/'

interface MediaPreviewOptions {
  actionButtonShareText?: string
  actionButtonViewText?: string
  finalizeText?: string
}

interface MediaPreviewState {
  downloaded: boolean
  finalizeProgress: number
  finalizeTotal: number
  finalizeWaiting: boolean
  imageSrc?: string
  interactable: boolean
  muted: boolean
  previewType: 'image' | 'video' | null
  videoSrc?: string
  visible: boolean
}

interface MediaPreviewViewProps {
  options: MediaPreviewOptions
  strings: MediaRecorderStrings
}

type FinalizeAction = 'download' | 'share' | null

const clickAnchor = (properties: Partial<HTMLAnchorElement>) => {
  const anchor = document.createElement('a')
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

const defaultPreviewState = () => ({
  downloaded: false,
  finalizeProgress: 0,
  finalizeTotal: 100,
  finalizeWaiting: false,
  imageSrc: undefined,
  interactable: false,
  muted: false,
  previewType: null as 'image' | 'video' | null,
  videoSrc: undefined,
  visible: false,
})

const resolveActions = (options: MediaPreviewOptions, strings: MediaRecorderStrings) => {
  const estimate = window.XR8.XrDevice.deviceEstimate()
  const browser = estimate.browser.inAppBrowser || estimate.browser.name
  const isWkWebViewIos = ['Microsoft Edge', 'Google Chrome', 'Mozilla Firefox Focus', 'Firefox',
    'Opera Touch', 'Pinterest', 'Snapchat', 'Instagram', 'Facebook', 'Facebook Messenger', 'Line',
    'LinkedIn', 'Naver', 'Baidu', 'Brave'].includes(browser)
  const tmpFile = new File([new Blob()], 'tmp.mp4', {
    type: 'video/mp4',
    lastModified: Date.now(),
  })

  if (navigator.canShare?.({files: [tmpFile]})) {
    return {
      actionButton: {
        className: estimate.os === 'iOS' ? styles.showAlone : styles.showWithDownloadButton,
        icon: SHARE_SRC,
        kind: 'share' as const,
        text: options.actionButtonShareText || strings.shareCta,
      },
      disabledDownload: false,
      showDownloadButton: estimate.os !== 'iOS',
      webShareAPILevel2: true,
    }
  }
  if (estimate.os === 'iOS' && isWkWebViewIos) {
    return {
      actionButton: undefined,
      disabledDownload: true,
      showDownloadButton: false,
      webShareAPILevel2: false,
    }
  }
  if (estimate.os === 'iOS') {
    return {
      actionButton: {
        className: combine(styles.showWithDownloadButton, styles.showAfterDownload),
        icon: VIEW_SRC,
        kind: 'view' as const,
        text: options.actionButtonViewText || strings.viewCta,
      },
      disabledDownload: false,
      showDownloadButton: true,
      webShareAPILevel2: false,
    }
  }
  return {
    actionButton: undefined,
    disabledDownload: false,
    showDownloadButton: true,
    webShareAPILevel2: false,
  }
}

const MediaPreviewView: FC<MediaPreviewViewProps> = ({options, strings}) => {
  const actions = useMemo(() => resolveActions(options, strings), [options, strings])
  const [state, setState] = useState<MediaPreviewState>(defaultPreviewState)
  const afterFinalizeAction = useRef<FinalizeAction>(null)
  const closedWhileFinalizing = useRef(false)
  const currentBlob = useRef<Blob | null>(null)
  const currentDownloadUrl = useRef<string | null>(null)
  const currentFilename = useRef<string | null>(null)
  const currentUrl = useRef<string | null>(null)
  const previewInteractableTimeout = useRef<number>()
  const previewIsImage = useRef(false)
  const stringsRef = useRef(strings)
  const videoPending = useRef(false)
  const videoReadyHandler = useRef<(video: HTMLVideoElement) => void>(() => {})
  const videoRef = useRef<HTMLVideoElement>()
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
    visibleObjectUrl.current = null
    previewIsImage.current = false
    videoPending.current = false
    afterFinalizeAction.current = null
    closedWhileFinalizing.current = false
    videoReadyHandler.current = () => {}
  }, [])

  const clearPreview = useCallback(() => {
    clearResources()
    setState(current => ({...current, ...defaultPreviewState()}))
  }, [clearResources])

  const fileName = useCallback((extension: string) => (
    `${getConfig().fileNamePrefix || stringsRef.current.fileNamePrefix}` +
    `${getTimestamp()}.${extension}`
  ), [])

  const showPreview = useCallback(() => {
    setState(current => ({...current, visible: true}))
    window.dispatchEvent(new CustomEvent('mediarecorder-previewopened'))
    window.clearTimeout(previewInteractableTimeout.current)
    previewInteractableTimeout.current = window.setTimeout(() => {
      setState(current => ({...current, interactable: true}))
    }, 100)
  }, [])

  const showImagePreview = useCallback((blob: Blob) => {
    clearResources()
    setState(current => ({...current, ...defaultPreviewState()}))
    currentBlob.current = blob
    currentUrl.current = URL.createObjectURL(blob)
    currentFilename.current = fileName('jpg')
    previewIsImage.current = true
    const reader = new FileReader()
    reader.readAsDataURL(blob)
    reader.onloadend = () => {
      setState(current => ({
        ...current,
        imageSrc: reader.result as string,
        previewType: 'image',
      }))
      showPreview()
    }
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

    setState(current => ({
      ...current,
      ...defaultPreviewState(),
      previewType: 'video',
      videoSrc: currentUrl.current,
    }))
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
    const file = new File([currentBlob.current as Blob], currentFilename.current || '', {
      type: previewIsImage.current ? 'image/jpeg' : 'video/mp4',
      lastModified: Date.now(),
    })
    navigator.share({title: '', text: '', files: [file]})
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
      videoReadyHandler.current(video)
    }
  }, [state.videoSrc])

  const className = combine(
    styles.previewContainer,
    state.visible && styles.fadeIn,
    state.previewType === 'image' && styles.imagePreviewVisible,
    state.previewType === 'video' && styles.videoPreviewVisible,
    state.downloaded && styles.downloaded,
    state.finalizeWaiting && styles.finalizeWaiting,
    actions.disabledDownload && styles.disabledDownload
  )

  return (
    <div
      className={className}
      style={state.interactable ? {pointerEvents: 'auto'} : undefined}
    >
      <div className={styles.topBar}>
        <div className={styles.previewBox}>
          <button
            className={combine(styles.styleReset, styles.iconButton, styles.closePreviewButton)}
            type='button'
            aria-label={strings.closePreviewLabel}
            onClick={closePreview}
          >
            <img alt='' src={CLOSE_SRC} />
          </button>
          <img
            className={styles.imagePreview}
            alt={strings.capturedPreviewAlt}
            src={state.imageSrc}
          />
          {/* eslint-disable-next-line jsx-a11y/media-has-caption */}
          <video
            ref={videoRef}
            className={styles.videoPreview}
            src={state.videoSrc}
            muted={state.muted}
            aria-label={strings.capturedVideoLabel}
            playsInline
            loop
          />
          <button
            className={combine(styles.styleReset, styles.iconButton, styles.toggleMuteButton)}
            type='button'
            aria-label={state.muted
              ? strings.unmutePreviewLabel
              : strings.mutePreviewLabel}
            onClick={() => setState(current => ({...current, muted: !current.muted}))}
          >
            <img
              alt=''
              src={state.muted ? SOUND_OFF_SRC : SOUND_ON_SRC}
            />
          </button>
        </div>
      </div>

      <div className={styles.bottomBar}>
        <div className={styles.finalizeContainer}>
          <div className={styles.finalizeText}>
            {options.finalizeText || strings.preparingLabel}
          </div>
          <progress
            className={styles.finalizeProgressBar}
            value={state.finalizeProgress}
            max={state.finalizeTotal}
            aria-label={strings.finalizeProgressLabel}
          />
        </div>
        {actions.showDownloadButton &&
          <button
            className={combine(styles.styleReset, styles.iconButton)}
            type='button'
            aria-label={strings.downloadLabel}
            onClick={downloadFile}
          >
            <img alt='' src={DOWNLOAD_SRC} />
          </button>
        }
        <div className={combine(styles.styleReset, styles.openSafariText)}>
          {strings.safariDownloadOnlyMessage}
        </div>
        <div
          className={combine(styles.styleReset, styles.tapAndHoldText)}
        >
          {strings.tapAndHoldToShareMessage}
        </div>
        {actions.actionButton &&
          <button
            className={combine(styles.styleReset, styles.actionButton,
              actions.actionButton.className)}
            type='button'
            onClick={actions.actionButton.kind === 'share' ? share : openIosDownload}
          >
            <span>{actions.actionButton.text}</span>
            <img
              className={styles.actionButtonImg}
              alt=''
              src={actions.actionButton.icon}
            />
          </button>
        }
      </div>
    </div>
  )
}

export {MediaPreviewView}
export type {MediaPreviewOptions}
