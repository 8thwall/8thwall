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

interface PreviewActionButton {
  icon: string
  kind: 'share' | 'view'
  layout: 'alone' | 'after-download' | 'with-download'
  text: string
}

interface PreviewActions {
  actionButton?: PreviewActionButton
  disabledDownload: boolean
  showDownloadButton: boolean
  webShareAPILevel2: boolean
}

export type {
  MediaPreviewOptions,
  MediaPreviewState,
  PreviewActionButton,
  PreviewActions,
}
