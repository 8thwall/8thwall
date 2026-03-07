import {
  GAMEPAD_CONNECTED, GAMEPAD_DISCONNECTED, GamepadConnectedEvent,
  GamepadDisconnectedEvent,
} from './input-events'
import {
  SCREEN_TOUCH_END, SCREEN_TOUCH_MOVE, SCREEN_TOUCH_START, ScreenTouchEndEvent,
  ScreenTouchMoveEvent, ScreenTouchStartEvent, GESTURE_START, GESTURE_MOVE, GESTURE_END,
  GestureStartEvent, GestureMoveEvent, GestureEndEvent,
} from './pointer-events'
import {
  UI_CLICK, UI_HOVER_START, UI_HOVER_END, UI_PRESSED, UI_RELEASED, UiClickEvent,
  UiHoverEvent,
} from './ui-events'

const input = {
  SCREEN_TOUCH_START,
  SCREEN_TOUCH_MOVE,
  SCREEN_TOUCH_END,
  GESTURE_START,
  GESTURE_MOVE,
  GESTURE_END,
  GAMEPAD_CONNECTED,
  GAMEPAD_DISCONNECTED,
  UI_CLICK,
  UI_PRESSED,
  UI_RELEASED,
  UI_HOVER_START,
  UI_HOVER_END,
}

export {
  input,
}

export type {
  ScreenTouchStartEvent,
  ScreenTouchMoveEvent,
  ScreenTouchEndEvent,
  GestureStartEvent,
  GestureMoveEvent,
  GestureEndEvent,
  GamepadConnectedEvent,
  GamepadDisconnectedEvent,
  UiClickEvent,
  UiHoverEvent,
}
