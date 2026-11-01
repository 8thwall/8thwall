import {h, render} from 'preact'

import './record-button.css'
import {RecordButtonView} from './record-button-view'

let captureMode = 'standard'
let recordButtonRoot

const draw = () => {
  if (recordButtonRoot) {
    render(h(RecordButtonView, {captureMode}), recordButtonRoot)
  }
}

const initRecordButton = () => {
  recordButtonRoot = document.createElement('div')
  recordButtonRoot.dataset.xrextrasRecordButtonRoot = ''
  document.body.appendChild(recordButtonRoot)
  draw()
}

const removeRecordButton = () => {
  render(null, recordButtonRoot)
  recordButtonRoot.remove()
  recordButtonRoot = null
}

const setCaptureMode = (mode) => {
  captureMode = mode === 'photo' || mode === 'fixed' ? mode : 'standard'
  draw()
}

export {
  initRecordButton,
  removeRecordButton,
  setCaptureMode,
}
