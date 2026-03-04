import type {Eid} from '../shared/schema'

type UiClickEvent = {
  x: number
  y: number
}

type UiHoverEvent = {
  x: number
  y: number
  targets: Eid[]
}

const UI_CLICK = 'click' as const
const UI_PRESSED = 'ui-pressed' as const
const UI_RELEASED = 'ui-released' as const
const UI_HOVER_START = 'ui-hover-start' as const
const UI_HOVER_END = 'ui-hover-end' as const

export {
  UI_CLICK,
  UI_PRESSED,
  UI_RELEASED,
  UI_HOVER_START,
  UI_HOVER_END,
}

export type {
  UiClickEvent,
  UiHoverEvent,
}
