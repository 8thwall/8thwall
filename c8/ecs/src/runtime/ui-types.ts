// @visibility(//visibility:public)

import type {ReadData} from '../shared/schema'
import type {SchemaOf} from './world-attribute'
import type {Ui} from './components'

type LayoutMetrics = {
  top: number
  left: number
  width: number
  height: number
  parentWidth?: number
  parentHeight?: number
  overflow?: 'visible' | 'hidden' | 'scroll'
}

type UiRuntimeSettings = ReadData<SchemaOf<typeof Ui>>

export type {
  UiRuntimeSettings,
  LayoutMetrics,
}
