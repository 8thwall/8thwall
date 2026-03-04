import type {Eid} from '../shared/schema'
import THREE from './three'
import type {Events as EventApi} from './events'
import type {PointerApi, RaycastStage} from './pointer-events-types'
import {vec3, type Vec3} from './math/vec3'
import {findEidForObject} from '../shared/eid-operations'
import {makeRaycastIntersections} from '../shared/custom-sorting'
import type {DataForEvent} from './events-types'

const SCREEN_TOUCH_START = 'screen-touch-start' as const
const SCREEN_TOUCH_MOVE = 'screen-touch-move' as const
const SCREEN_TOUCH_END = 'screen-touch-end' as const

const GESTURE_START = 'gesture-start' as const
const GESTURE_MOVE = 'gesture-move' as const
const GESTURE_END = 'gesture-end' as const

type ScreenPosition = {
  x: number
  y: number
}

type PointerId = PointerEvent['pointerId']

interface ScreenTouchStartEvent {
  pointerId: PointerId
  position: ScreenPosition
  target: Eid | undefined
  worldPosition: Vec3 | undefined
}

interface ScreenTouchMoveEvent {
  pointerId: PointerId
  position: ScreenPosition
  start: ScreenPosition
  change: ScreenPosition
  target: Eid | undefined
}

interface ScreenTouchEndEvent {
  pointerId: PointerId
  position: ScreenPosition
  start: ScreenPosition
  target: Eid | undefined
  endTarget: Eid | undefined
  worldPosition: Vec3 | undefined
}

interface GestureState {
  touchCount: number
  startPosition: ScreenPosition
  position: ScreenPosition
  startSpread: number
  spread: number
}

interface GestureSnapshot {
  position: ScreenPosition
  touchCount: number
  spread: number
}

interface GestureStartEvent {
  startPosition: ScreenPosition
  position: ScreenPosition
  startSpread: number
  spread: number
  touchCount: number
}

interface GestureMoveEvent {
  startPosition: ScreenPosition
  position: ScreenPosition
  positionChange: ScreenPosition
  startSpread: number
  spread: number
  touchCount: number
  spreadChange: number
}

interface GestureEndEvent {
  startPosition: ScreenPosition
  position: ScreenPosition
  startSpread: number
  spread: number
  touchCount: number
  target: Eid | undefined

  // If the gesture is changing from 2 to 3 fingers, this will be 3
  nextTouchCount: number | undefined
}

const getProportionalPosition = (
  element: HTMLElement,
  event: PointerEvent
): ScreenPosition => {
  const rect = element.getBoundingClientRect()
  return ({
    x: (event.clientX - rect.left) / rect.width,
    y: (event.clientY - rect.top) / rect.height,
  })
}

const createPointerListener = (
  stages: RaycastStage[],
  events: EventApi,
  element: HTMLElement
): PointerApi => {
  const {globalId} = events

  type PointerState = {
    start: ScreenPosition
    last: ScreenPosition
    target: Eid | undefined
  }

  const pointers = new Map<PointerEvent['pointerId'], PointerState>()

  const raycaster_ = new THREE.Raycaster()
  // Note/TODO(Dale): generate-ecs-definitions fails here. We extend the Raycaster in
  // node_modules/three-mesh-bvh/src/index.d.ts but it wasn't being carried over. Potentially could
  // be fixed by adding a type definition in the project and updating the tsconfig.
  // @ts-ignore
  raycaster_.firstHitOnly = true
  const origin_ = new THREE.Vector2()
  const raycastIntersections_ = makeRaycastIntersections([])

  const getRaycastTargetForStage = (stage: RaycastStage) => {
    raycaster_.setFromCamera(origin_, stage.getCamera())
    raycastIntersections_.length = 0
    raycaster_.intersectObject(stage.scene, true, raycastIntersections_)
    let worldPosition: Vec3 | undefined
    let target: Eid | undefined
    if (raycastIntersections_.length > 0) {
      const intersection = raycastIntersections_[0]
      worldPosition = stage.includeWorldPosition ? vec3.from(intersection.point) : undefined
      target = findEidForObject(intersection.object)
    }

    return {target, worldPosition}
  }

  const getRaycastTarget = (position: ScreenPosition) => {
    // Screen positions are in the range [0, 1] - we need to convert them to
    // [-1, 1] for raycasting
    // https://threejs.org/docs/#api/en/core/Raycaster.setFromCamera
    origin_.set(position.x * 2 - 1, -position.y * 2 + 1)

    let hit: {target?: Eid, worldPosition?: Vec3} = {}
    // note(owenmech): .some + return to act as a break
    stages.some((stage) => {
      hit = getRaycastTargetForStage(stage)
      return !!hit.target
    })
    return hit
  }

  let gestureState_: GestureState | null = null

  const dispatchMaybeTargeted = <T extends (
    typeof SCREEN_TOUCH_START | typeof SCREEN_TOUCH_MOVE | typeof SCREEN_TOUCH_END
  )>(event: T, data: DataForEvent<T>) => {
    if (data.target) {
      events.dispatch(data.target, event, data)
    } else {
      events.dispatch(globalId, event, data)
    }
  }

  const getGestureSnapshot = (): GestureSnapshot | null => {
    const touchList = Array.from(pointers.values()).map(pointer => pointer.last)

    if (touchList.length === 0) {
      return null
    }

    let averageX = 0
    let averageY = 0

    for (const touch of touchList) {
      averageX += touch.x
      averageY += touch.y
    }

    averageX /= touchList.length
    averageY /= touchList.length

    let spread = 0
    // Calculate average spread of touches from the center point
    if (touchList.length >= 2) {
      for (const touch of touchList) {
        spread += Math.sqrt(
          (averageX - touch.x) ** 2 +
          (averageY - touch.y) ** 2
        )
      }
    }

    spread /= touchList.length

    return {
      touchCount: touchList.length,
      position: {x: averageX, y: averageY},
      spread,
    }
  }

  const maybeDispatchMultiTouch = () => {
    const previousState = gestureState_
    const currentState = getGestureSnapshot()

    const gestureContinues = (
      previousState &&
      currentState &&
      currentState.touchCount === previousState.touchCount
    )

    const gestureEnded = previousState && !gestureContinues
    const gestureStarted = currentState && !gestureContinues

    if (gestureEnded) {
      const event: GestureEndEvent = {
        startPosition: previousState.startPosition,
        position: previousState.position,
        startSpread: previousState.startSpread,
        spread: previousState.spread,
        touchCount: previousState.touchCount,
        target: undefined,
        nextTouchCount: currentState?.touchCount,
      }
      gestureState_ = null
      events.dispatch(globalId, GESTURE_END, event)
    }

    if (gestureStarted) {
      gestureState_ = {
        startPosition: currentState.position,
        position: currentState.position,
        touchCount: currentState.touchCount,
        startSpread: currentState.spread,
        spread: currentState.spread,
      }

      const startEvent: GestureStartEvent = {
        startPosition: currentState.position,
        position: currentState.position,
        startSpread: currentState.spread,
        spread: currentState.spread,
        touchCount: currentState.touchCount,
      }
      events.dispatch(globalId, GESTURE_START, startEvent)
    }

    if (gestureContinues) {
      const event: GestureMoveEvent = {
        startPosition: previousState.startPosition,
        position: currentState.position,
        positionChange: {
          x: currentState.position.x - previousState.position.x,
          y: currentState.position.y - previousState.position.y,
        },
        startSpread: previousState.startSpread,
        spread: currentState.spread,
        touchCount: currentState.touchCount,
        spreadChange: currentState.spread - previousState.spread,
      }

      if (gestureState_) {
        gestureState_.position = currentState.position
        gestureState_.spread = currentState.spread
      }

      events.dispatch(globalId, GESTURE_MOVE, event)
    }
  }

  const handlePointerDown = (event: PointerEvent) => {
    const position = getProportionalPosition(element, event)

    const pointer: PointerState = {
      start: position,
      last: {...position},
      target: undefined,
    }

    const {target, worldPosition} = getRaycastTarget(position)
    pointer.target = target

    pointers.set(event.pointerId, pointer)

    const startEvent: ScreenTouchStartEvent = {
      pointerId: event.pointerId,
      position,
      worldPosition,
      target: pointer.target,
    }

    dispatchMaybeTargeted(SCREEN_TOUCH_START, startEvent)

    maybeDispatchMultiTouch()
  }

  const handlePointerMove = (event: PointerEvent) => {
    const pointer = pointers.get(event.pointerId)
    if (!pointer) {
      return
    }

    const position = getProportionalPosition(element, event)

    const changeX = position.x - pointer.last.x
    const changeY = position.y - pointer.last.y

    pointer.last = position

    const moveEvent: ScreenTouchMoveEvent = {
      pointerId: event.pointerId,
      position,
      start: pointer.start,
      change: {
        x: changeX,
        y: changeY,
      },
      target: pointer.target,
    }

    dispatchMaybeTargeted(SCREEN_TOUCH_MOVE, moveEvent)

    maybeDispatchMultiTouch()
  }

  const handlePointerUp = (event: PointerEvent) => {
    const pointer = pointers.get(event.pointerId)
    if (!pointer) {
      return
    }
    const position = getProportionalPosition(element, event)

    const {target: endTarget, worldPosition} = getRaycastTarget(position)

    const endEvent: ScreenTouchEndEvent = {
      pointerId: event.pointerId,
      position,
      start: pointer.start,
      target: pointer.target,
      endTarget,
      worldPosition,
    }

    dispatchMaybeTargeted(SCREEN_TOUCH_END, endEvent)

    pointers.delete(event.pointerId)

    maybeDispatchMultiTouch()
  }

  const attach = () => {
    element.addEventListener('pointerdown', handlePointerDown)
    element.addEventListener('pointermove', handlePointerMove)
    element.addEventListener('pointerup', handlePointerUp)
    element.addEventListener('pointerleave', handlePointerUp)
  }

  const detach = () => {
    element.removeEventListener('pointerdown', handlePointerDown)
    element.removeEventListener('pointermove', handlePointerMove)
    element.removeEventListener('pointerup', handlePointerUp)
    element.addEventListener('pointerleave', handlePointerUp)
  }

  return {
    attach,
    detach,
  }
}

export {
  getProportionalPosition,
  createPointerListener,
  SCREEN_TOUCH_START,
  SCREEN_TOUCH_MOVE,
  SCREEN_TOUCH_END,
  GESTURE_START,
  GESTURE_MOVE,
  GESTURE_END,
}

export type {
  ScreenPosition,
  ScreenTouchStartEvent,
  ScreenTouchMoveEvent,
  ScreenTouchEndEvent,
  GestureStartEvent,
  GestureMoveEvent,
  GestureEndEvent,
  RaycastStage,
}
