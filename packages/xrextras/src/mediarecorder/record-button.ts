import {h, render} from 'preact'

import {RecordButtonView} from './record-button-view'
import type {CaptureMode} from './record-button-view'
import {defaultStrings} from './strings'
import type {MediaRecorderStrings} from './strings'

let captureMode: CaptureMode = 'standard'
let recordButtonRoot: HTMLDivElement | null = null
let strings: MediaRecorderStrings = defaultStrings

const draw = () => {
  if (recordButtonRoot) {
    render(h(RecordButtonView, {captureMode, strings}), recordButtonRoot)
  }
}

const initRecordButton = (initialStrings: MediaRecorderStrings = defaultStrings) => {
  strings = initialStrings
  recordButtonRoot = document.createElement('div') as unknown as HTMLDivElement
  recordButtonRoot.dataset.xrextrasRecordButtonRoot = ''
  document.body.appendChild(recordButtonRoot)
  draw()
}

const removeRecordButton = () => {
  if (!recordButtonRoot) {
    return
  }
  render(null, recordButtonRoot)
  recordButtonRoot.remove()
  recordButtonRoot = null
}

const setCaptureMode = (mode: string) => {
  captureMode = mode === 'photo' || mode === 'fixed' ? mode : 'standard'
  draw()
}

const setRecordButtonStrings = (nextStrings: MediaRecorderStrings) => {
  strings = nextStrings
  draw()
}

export {
  initRecordButton,
  removeRecordButton,
  setCaptureMode,
  setRecordButtonStrings,
}
