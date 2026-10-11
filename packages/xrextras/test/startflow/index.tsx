import {h, render} from 'preact'

import {DebugApp} from './debug-app'

render(
  <DebugApp />,
  document.querySelector('[data-startflow-debug-root]')!
)
