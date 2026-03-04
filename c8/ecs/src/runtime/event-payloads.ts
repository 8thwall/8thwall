import type {CameraEvents} from './camera-events'
import type {CameraObject} from './camera-manager-types'

import type {events} from './event-ids'
import type * as UiEvents from './ui-events'
import type {ProgressInfo, FrameInfo} from './xr/xr-manager-types'
import type * as THREE_TYPES from './three-types'
import type * as PointerEvents from './pointer-events'
import type * as PhysicsEvents from './physics-events'
import type * as InputEvents from './input-events'
import type {LocationSpawnedEvent} from './map-types'
import type {Eid} from '../shared/schema'

declare global {
  interface EcsEventTypes {
    // Media recorder
    [events.RECORDER_VIDEO_ERROR]: Error
    [events.RECORDER_VIDEO_READY]: {videoBlob: Blob}
    [events.RECORDER_SCREENSHOT_READY]: Blob
    [events.RECORDER_FINALIZE_PROGRESS]: ProgressInfo
    [events.RECORDER_PREVIEW_READY]: {videoBlob: Blob}
    [events.RECORDER_PROCESS_FRAME]: FrameInfo

    // Input
    [UiEvents.UI_CLICK]: {x: number; y: number}
    [UiEvents.UI_PRESSED]: {x: number; y: number}
    [UiEvents.UI_RELEASED]: {x: number; y: number}
    [UiEvents.UI_HOVER_START]: UiEvents.UiHoverEvent
    [UiEvents.UI_HOVER_END]: UiEvents.UiHoverEvent
    [PointerEvents.SCREEN_TOUCH_START]: PointerEvents.ScreenTouchStartEvent
    [PointerEvents.SCREEN_TOUCH_MOVE]: PointerEvents.ScreenTouchMoveEvent
    [PointerEvents.SCREEN_TOUCH_END]: PointerEvents.ScreenTouchEndEvent
    [PointerEvents.GESTURE_START]: PointerEvents.GestureStartEvent
    [PointerEvents.GESTURE_MOVE]: PointerEvents.GestureMoveEvent
    [PointerEvents.GESTURE_END]: PointerEvents.GestureEndEvent
    [InputEvents.GAMEPAD_CONNECTED]: InputEvents.GamepadConnectedEvent
    [InputEvents.GAMEPAD_DISCONNECTED]: InputEvents.GamepadDisconnectedEvent

    // Camera
    [CameraEvents.ACTIVE_CAMERA_CHANGE]: {camera: CameraObject}
    [CameraEvents.ACTIVE_CAMERA_EID_CHANGE]: {eid: Eid}
    [CameraEvents.XR_CAMERA_STOP]: {}
    [CameraEvents.XR_CAMERA_EDIT]: {camera: CameraObject}

    // Assets
    [events.SPLAT_MODEL_LOADED]: {model: THREE_TYPES.Object3D}
    [events.GLTF_MODEL_LOADED]: {model: THREE_TYPES.Group}
    [events.GLTF_ANIMATION_FINISHED]: {name: string}
    [events.GLTF_ANIMATION_LOOP]: {name: string}
    [events.AUDIO_END]: undefined

    // Animation completion events
    [events.POSITION_ANIMATION_COMPLETE]: undefined
    [events.SCALE_ANIMATION_COMPLETE]: undefined
    [events.ROTATE_ANIMATION_COMPLETE]: undefined
    [events.CUSTOM_VEC3_ANIMATION_COMPLETE]: undefined
    [events.CUSTOM_PROPERTY_ANIMATION_COMPLETE]: undefined

    'audio-error': {error: Error}
    [events.VIDEO_CAN_PLAY_THROUGH]: {src: string}
    [events.VIDEO_END]: {src: string}
    'video-error': {error: Error}

    // Physics
    [PhysicsEvents.COLLISION_START_EVENT]: {other: Eid}
    [PhysicsEvents.COLLISION_END_EVENT]: {other: Eid}
    [PhysicsEvents.UPDATE_EVENT]: {}

    // Maps
    [events.LOCATION_SPAWNED]: LocationSpawnedEvent
  }
}
