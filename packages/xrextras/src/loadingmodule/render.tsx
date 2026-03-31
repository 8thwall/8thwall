import * as Preact from 'preact'

import {ILoadingContainer, LoadingContainer} from './loading-container'

let root: HTMLDivElement

const show = (params: ILoadingContainer) => {
  Preact.render(Preact.h(LoadingContainer, params), root)
  root = document.querySelector('#loadingContainer') as any as HTMLDivElement
}

const hide = () => {
  if (!root) {
    return
  }
  root.parentNode.removeChild(root)
  root = null
}

const isVisible = () => !!root

export {
  show,
  hide,
  isVisible,
}
