import {configure} from './capture-config'
import {initMediaPreview, removeMediaPreview, setMediaPreviewStrings} from './media-preview'
import type {MediaPreviewOptions} from './media-preview-view'
import {
  initRecordButton, removeRecordButton, setCaptureMode, setRecordButtonStrings,
} from './record-button'
import {defaultStrings} from './strings'
import type {MediaRecorderStringOverrides, MediaRecorderStrings} from './strings'

interface MediaRecorderController {
  configure: typeof configure
  initMediaPreview: (options?: MediaPreviewOptions) => void
  initRecordButton: () => void
  removeMediaPreview: () => void
  removeRecordButton: () => void
  reset: () => void
  setCaptureMode: (mode: string) => void
  setStringOverrides: (overrides: MediaRecorderStringOverrides) => void
}

const create = (): MediaRecorderController => {
  let strings: MediaRecorderStrings = {...defaultStrings}

  const updateStrings = (nextStrings: MediaRecorderStrings) => {
    strings = nextStrings
    setMediaPreviewStrings(strings)
    setRecordButtonStrings(strings)
  }

  return {
    initMediaPreview: options => initMediaPreview(options, strings),
    removeMediaPreview,
    initRecordButton: () => initRecordButton(strings),
    removeRecordButton,
    configure,
    setCaptureMode,
    reset: () => updateStrings({...defaultStrings}),
    setStringOverrides: (overrides) => {
      updateStrings({...strings, ...overrides})
    },
  }
}

let mediaRecorder: MediaRecorderController | null = null

const MediaRecorderFactory = () => {
  if (mediaRecorder === null) {
    mediaRecorder = create()
  }

  return mediaRecorder
}

// TODO: export MediaRecorderFactory
const MediaRecorder = MediaRecorderFactory()
export {
  MediaRecorder,
}
export type {MediaRecorderController}
