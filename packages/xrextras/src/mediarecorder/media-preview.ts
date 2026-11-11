import {h, render} from 'preact'

import {MediaPreviewView} from './media-preview-view'
import type {MediaPreviewOptions} from './media-preview-view'
import {defaultStrings} from './strings'
import type {MediaRecorderStrings} from './strings'

let mediaPreviewRoot: HTMLDivElement | null = null
let options: MediaPreviewOptions = {}
let strings: MediaRecorderStrings = defaultStrings

const draw = () => {
  if (mediaPreviewRoot) {
    render(h(MediaPreviewView, {options, strings}), mediaPreviewRoot)
  }
}

const initMediaPreview = (
  initialOptions: MediaPreviewOptions = {},
  initialStrings: MediaRecorderStrings = defaultStrings
) => {
  options = initialOptions
  strings = initialStrings
  mediaPreviewRoot = document.createElement('div') as unknown as HTMLDivElement
  mediaPreviewRoot.dataset.xrextrasMediaPreviewRoot = ''
  document.body.appendChild(mediaPreviewRoot)
  draw()
}

const removeMediaPreview = () => {
  if (!mediaPreviewRoot) {
    return
  }
  render(null, mediaPreviewRoot)
  mediaPreviewRoot.remove()
  mediaPreviewRoot = null
}

const setMediaPreviewStrings = (nextStrings: MediaRecorderStrings) => {
  strings = nextStrings
  draw()
}

export {
  initMediaPreview,
  removeMediaPreview,
  setMediaPreviewStrings,
}
