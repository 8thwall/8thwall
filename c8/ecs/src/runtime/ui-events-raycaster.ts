import {findEidForObject} from '../shared/eid-operations'
import THREE from './three'
import type {World} from './world'
import {getProportionalPosition, RaycastStage, ScreenPosition} from './pointer-events'
import type {Events} from './events-types'
import {UI_HOVER_START, UI_HOVER_END, UiHoverEvent} from './ui-events'
import type {Eid} from '../shared/schema'
import {Ui} from './components'
import {THREE_LAYERS} from '../shared/three-layers'
import {makeRaycastIntersections} from '../shared/custom-sorting'

const eidsAreEqual = (a: Eid[], b: Eid[]) => {
  if (a.length !== b.length) {
    return false
  }
  return a.every(eid => b.includes(eid))
}

const getCapturedEids = (world: World, eid: Eid) => {
  const capturedEids: Eid[] = []
  let cursor: Eid | undefined = eid
  capturedEids.push(cursor)
  while (cursor) {
    cursor = world.getParent(cursor)
    if (cursor && Ui.has(world, cursor)) {
      capturedEids.push(cursor)
    }
  }
  return capturedEids
}

const createUiEventsRaycaster = (
  world: World, stages: RaycastStage[], events: Events, element: HTMLElement
) => {
  const origin_ = new THREE.Vector2()
  const raycaster_ = new THREE.Raycaster()
  const raycastIntersections_ = makeRaycastIntersections([])
  let currentHoveredEids: Eid[] = []
  // NOTE(johnny): BVH extends raycaster, but it is not incuded when generating the definition file.
  // @ts-ignore
  raycaster_.firstHitOnly = true
  raycaster_.layers.set(THREE_LAYERS.uiRaycasted)

  const getRaycastTargetsForStage = (stage: RaycastStage) => {
    raycaster_.setFromCamera(origin_, stage.getCamera())
    raycastIntersections_.length = 0
    raycaster_.intersectObjects(stage.scene.children, true, raycastIntersections_)
    const targets: Eid[] = []
    let capturedEids: Eid[] = []
    raycastIntersections_.forEach((intersection) => {
      const eid = findEidForObject(intersection.object)
      if (eid && capturedEids.length === 0) {
        capturedEids = getCapturedEids(world, eid)
        targets.push(eid)
      } else if (eid) {
        if (capturedEids.includes(eid)) {
          targets.push(eid)
        }
      }
    })
    return targets
  }

  const getRaycastTargets = (position: ScreenPosition) => {
    // Screen positions are in the range [0, 1] - we need to convert them to
    // [-1, 1] for raycasting
    // https://threejs.org/docs/#api/en/core/Raycaster.setFromCamera
    origin_.set(position.x * 2 - 1, -position.y * 2 + 1)

    let targets: Eid[] = []

    // note(owenmech): .some + return to act as a break
    stages.some((stage) => {
      targets = getRaycastTargetsForStage(stage)
      return targets.length > 0
    })
    return targets
  }

  const raycastFromCursor = (event: PointerEvent) => {
    const position = getProportionalPosition(element, event)
    const newHoveredEids = getRaycastTargets(position)
    if (newHoveredEids) {
      if (eidsAreEqual(currentHoveredEids, newHoveredEids)) {
        return
      }

      const newEids: Eid[] = []
      const lostEids: Eid[] = []

      newHoveredEids.forEach((eid) => {
        if (!currentHoveredEids.includes(eid)) {
          newEids.push(eid)
        }
      })
      currentHoveredEids.forEach((eid) => {
        if (!newHoveredEids.includes(eid)) {
          lostEids.push(eid)
        }
      })

      if (newEids.length > 0) {
        events.dispatch(newEids[0],
          UI_HOVER_START,
          {...position, targets: newEids} as UiHoverEvent)
      }
      if (lostEids.length > 0) {
        events.dispatch(
          lostEids[0], UI_HOVER_END, {...position, targets: lostEids} as UiHoverEvent
        )
      }
      currentHoveredEids = newHoveredEids
    } else if (currentHoveredEids.length > 0) {
      events.dispatch(
        currentHoveredEids[0],
        UI_HOVER_END,
        {...position, targets: [...currentHoveredEids]} as UiHoverEvent
      )
      currentHoveredEids = []
    }
  }

  const attach = () => {
    element.addEventListener('pointermove', raycastFromCursor)
  }
  const detach = () => {
    element.removeEventListener('pointermove', raycastFromCursor)
  }

  return {
    attach,
    detach,
  }
}

export {
  createUiEventsRaycaster,
}
