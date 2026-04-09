import * as Preact from 'preact'

import {View} from './components/view'
import type {LandingParameters} from './parameters'

import './styles.scss'
import '../../xrextras/src/fonts/fonts.css'
import '../../xrextras/src/almosttheremodule/almost-there-module.css'

let root: HTMLDivElement

const show = (params: LandingParameters) => {
  if (!root) {
    root = document.createElement('div')
    document.body.appendChild(root)
  }
  Preact.render(Preact.h(View, params), root)
}

const hide = () => {
  if (!root) {
    return
  }
  Preact.render(null, root)
  root.parentNode.removeChild(root)
  root = null
}

const isVisible = () => !!root

export {
  show,
  hide,
  isVisible,
}
