import {h, render} from 'preact'

import {DebugApp} from './debug-app'

const DEBUG_ROOT = document.querySelector(
  '[data-startflow-debug-root]'
) as unknown as HTMLElement | null

if (DEBUG_ROOT) {
  render(<DebugApp />, DEBUG_ROOT)
}
