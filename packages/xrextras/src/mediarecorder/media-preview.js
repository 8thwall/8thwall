import {h, render} from 'preact'

import './media-preview.css'
import {MediaPreviewView} from './media-preview-view'

let mediaPreviewRoot

const initMediaPreview = (options = {}) => {
  mediaPreviewRoot = document.createElement('div')
  mediaPreviewRoot.dataset.xrextrasMediaPreviewRoot = ''
  document.body.appendChild(mediaPreviewRoot)
  render(h(MediaPreviewView, {options}), mediaPreviewRoot)
}

const removeMediaPreview = () => {
  render(null, mediaPreviewRoot)
  mediaPreviewRoot.remove()
  mediaPreviewRoot = null
}

export {
  initMediaPreview,
  removeMediaPreview,
}
