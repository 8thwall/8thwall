/* eslint-disable local-rules/hardcoded-copy */

const defaultStrings = {
  shareCta: 'Share',
  viewCta: 'View',
  preparingLabel: 'Preparing...',
  fileNamePrefix: 'my-capture-',
  safariDownloadOnlyMessage: 'Downloading is only available in Safari',
  tapAndHoldToShareMessage: 'Tap + Hold to Share',
  closePreviewLabel: 'Close preview',
  capturedPreviewAlt: 'Captured preview',
  capturedVideoLabel: 'Captured video preview',
  mutePreviewLabel: 'Mute preview',
  unmutePreviewLabel: 'Unmute preview',
  downloadLabel: 'Download',
  finalizeProgressLabel: 'Media preparation progress',
  recordCta: 'Record',
}

type MediaRecorderStrings = typeof defaultStrings
type MediaRecorderStringOverrides = Partial<MediaRecorderStrings>

export {defaultStrings}
export type {MediaRecorderStrings, MediaRecorderStringOverrides}
