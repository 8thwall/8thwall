import {CreateControllers, MAX_TRACER_LENGTH} from './create-controllers'
import {createDomTablet} from './tablet/dom-tablet'
import {createInteractionProvider} from './tablet/interaction-provider'
import {rotationForMatrixGen, rotationForUnitVectorGen} from './transforms'
import {CameraAttachmentManagerFactory} from './xr-camera-attachment-manager'
import {createDomTabletAlignment} from './tablet/dom-tablet-alignment'
import {createDomWristModel} from './tablet/dom-wrist-model'
import {isPartOf} from './scene-graph'

const XrInputManagerFactory = () => {
  const SHOW_DEBUGGER_CONTROLLERS = false
  let controllers_ = null
  let cameraAttachmentManager_ = null
  let scene_ = null
  let camera_ = null
  let canvas_ = null
  let attached_ = false
  let sceneScale_ = 0
  let isFirstUpdate_ = true
  let alignment_
  let tabletMinimized_ = false
  let pauseSession_ = null
  let tablet_
  let wristGroup_
  let wristModel_
  let tabletEnabled_ = false
  let touchEmulationEnabled_ = false

  // Stores pointer state for interactions in the scene
  // Pointer state contains:
  //   - identifier: the unique index of the pointer within interaction provider
  //   - tracerToIntersection: the quaternion to rotate the touch position ray to match the initial
  //       scene position of the raycast
  //   - isOnTablet: true if the pointer is interacting with the tablet screen
  //   - isOnHandle: true if the pointer is dragging the tablet handle
  //   - isSqueeze: true if the pointer was initiated using a squeeze as opposed to a select
  const pointerByControllerId_ = {}

  const interactionProvider_ = createInteractionProvider()

  // A widget for debugging controller location. Set SHOW_DEBUGGER_CONTROLLERS to true to enable
  // locally.
  const controllerDebugger = () => {
    // The first marker (mint) shows the 3d intersection location. This is added as a child of the
    // controller group so that it gets filtered from hit tests.
    const worldMarker_ = new window.THREE.Mesh(
      new window.THREE.SphereGeometry(0.03, 8, 8),
      new window.THREE.MeshBasicMaterial({color: 0x00EDAF})
    )
    controllers_.left.controller.parent.add(worldMarker_)

    // The second marker (cherry) shows the intersection projected into the camera and then
    // unprojected to a point in the world. From the camera's point of view, this is along the
    // same ray as the original 3d intersection location.
    const hudMarker_ = new window.THREE.Mesh(
      new window.THREE.SphereGeometry(0.003, 8, 8),
      new window.THREE.MeshBasicMaterial({color: 0xDD0065})
    )
    camera_.add(hudMarker_)

    const updateMarkers = (point, x, y) => {
      if (point) {
        worldMarker_.position.copy(point)
        worldMarker_.updateMatrix()
        worldMarker_.updateMatrixWorld(true)
      }

      const unProject =
        new window.THREE.Vector3(x, y, 0).applyMatrix4(camera_.projectionMatrixInverse)
      const d = -0.9
      hudMarker_.position.set(
        (unProject.x / unProject.z) * d, (unProject.y / unProject.z) * d, d
      )
      hudMarker_.updateMatrix()
      hudMarker_.updateMatrixWorld(true)
    }

    return {
      updateMarkers,
    }
  }

  const controllerDebugger_ = SHOW_DEBUGGER_CONTROLLERS ? controllerDebugger() : null

  const updateScaleInvert = obj => obj && obj.scale.setScalar(1 / sceneScale_)

  // Extracts a quaternion rotation from a TRS matrix.
  const rotationForMatrix = rotationForMatrixGen()

  // Extracts rotation from a unit vector.
  const rotationFromUnitVector = rotationForUnitVectorGen()

  const cameraPointAtInfinity = (worldQuat) => {
  // Project the world ray quaternion into the camera quaternion.
    const qc = rotationForMatrix(camera_.matrixWorldInverse).multiply(worldQuat)
    // Find a point along the camera space quaternion to the projected point, and then convert that
    // point to world coordinates.
    return new window.THREE.Vector3(0, 0, -1).applyQuaternion(qc)
  }

  const getControllerIntersection = (ctrl) => {
    // Get controller's ray
    const tracer = ctrl.getObjectByName('tracer')
    const ray = new window.THREE.Ray().applyMatrix4(tracer.matrixWorld)

    // Get a raycaster for this ray. Limit intersection radius with lines and points.
    const raycaster = new window.THREE.Raycaster(ray.origin, ray.direction)
    raycaster.params.Line.threshold = 0.01
    raycaster.params.Points.threshold = 0.01

    // Compute object hits that aren't children of the controller group.
    try {
      const rawIntersections = raycaster.intersectObject(scene_, true)
      const intersection = rawIntersections
        .find(({object, point}) => {
          const canInteract = !isPartOf(object, ctrl.parent) ||
                (tablet_ && tablet_.isTablet(object)) ||
                isPartOf(object, wristGroup_)
          if (!canInteract) {
            return false
          }
          // Don't intersect objects too close to the camera. This is -0.05 because of three.js
          // right handed coordinates where z-negative is in front of the camera.
          return point.clone().applyMatrix4(camera_.matrixWorldInverse).z <= -0.05
        })

      if (intersection) {
        return intersection
      }
    } catch (e) {
      // If What we are trying to intersecting can't be detected using raycasted then proceed with
      // infinity point
      if (!(e instanceof TypeError)) {
        throw e
      }
    }

    // Find the quaternion of the controller projected onto the infinite sphere.
    return {
      distance: Infinity,
      point: cameraPointAtInfinity(rotationFromUnitVector(ray.direction))
        .applyMatrix4(camera_.matrixWorld),
    }
  }

  const getCtrlClipSpacePoint = (point, tracking) => {
    // If we're debugging controls and not tracking, keep an unprojected copy of the point. If we
    // are tracking, the passed in point wasn't generated by a hit-test but is a fixed distance in
    // front of the camera. It's a better visualization to just keep the starting position of the
    // interaction.
    const ptCopy = (controllerDebugger_ && !tracking) ? point.clone() : null

    // Project point into clipspace
    const {x, y} = point.project(camera_)

    if (controllerDebugger_) {
      controllerDebugger_.updateMarkers(ptCopy, x, y)
    }

    // Get point in window space
    const wx = ((x + 1) * window.innerWidth) / 2
    const wy = ((1 - y) * window.innerHeight) / 2
    return {x: wx, y: wy}
  }

  const toggleTabletMinimize = () => {
    tabletMinimized_ = !tabletMinimized_

    tablet_.setMinimized(tabletMinimized_)
    wristModel_.setMinimized(tabletMinimized_)
    alignment_.setPaused(tabletMinimized_)
    if (tabletMinimized_) {
      const draggingId = Object.keys(pointerByControllerId_)
        .find(id => pointerByControllerId_[id].isOnHandle)
      if (draggingId) {
        alignment_.onDragEnd()
        delete pointerByControllerId_[draggingId]
      }
    } else {
      alignment_.resetToFront()
    }
  }

  const isDraggingHandle = () => Object.values(pointerByControllerId_).some(e => e.isOnHandle)

  const handleControllerSelectStart = ({target: ctrl}) => {
    const currentPointer = pointerByControllerId_[ctrl.uuid]
    if (currentPointer) {
      return
    }

    const intersection = getControllerIntersection(ctrl)

    if (tabletEnabled_) {
      if (tablet_.isExitButton(intersection.object)) {
        if (pauseSession_) {
          pauseSession_()
        }
        return
      }

      if (tablet_.isHandle(intersection.object)) {
        if (isDraggingHandle()) {
          return
        }

        alignment_.onDragStart(ctrl)
        pointerByControllerId_[ctrl.uuid] = {
          isOnHandle: true,
        }
        return
      }

      const isTabletToggleObject = isPartOf(intersection.object, wristGroup_) ||
                                 tablet_.isMinimizeButton(intersection.object)

      if (isTabletToggleObject) {
        toggleTabletMinimize()
        return
      }

      if (tablet_.isTablet(intersection.object)) {
        pointerByControllerId_[ctrl.uuid] = {
          identifier: tablet_.onControllerStart(intersection),
          isOnTablet: true,
        }
        return
      }
    }

    if (touchEmulationEnabled_) {
      // Compute the camera space vectors for the current tracer at infinity and the current
      // intersection point, and compute the angle between them so that we can update motion
      // from the starting point.
      const tracerInfinityInCamera = cameraPointAtInfinity(
        rotationForMatrix(ctrl.getObjectByName('tracer').matrixWorld)
      ).normalize()

      const intersectionQuaternionInCamera =
      intersection.point.clone().applyMatrix4(camera_.matrixWorldInverse).normalize()

      const tracerToIntersection = new window.THREE.Quaternion().setFromUnitVectors(
        tracerInfinityInCamera, intersectionQuaternionInCamera
      )

      pointerByControllerId_[ctrl.uuid] = {
        identifier: interactionProvider_.startPointer(
          getCtrlClipSpacePoint(intersection.point, false), canvas_
        ),
        tracerToIntersection,
      }
    }
  }

  const handleControllerSelectEnd = ({target: ctrl}) => {
    const currentPointer = pointerByControllerId_[ctrl.uuid]
    if (!currentPointer || currentPointer.isSqueeze) {
      return
    }
    delete pointerByControllerId_[ctrl.uuid]

    if (currentPointer.isOnTablet) {
      const intersection = getControllerIntersection(ctrl)
      tablet_.onControllerEnd(currentPointer.identifier, intersection)
      return
    }

    if (currentPointer.isOnHandle) {
      alignment_.onDragEnd()
      return
    }

    interactionProvider_.endPointer(currentPointer.identifier)
  }

  const updatePointerForController = (ctrl) => {
    const activePointer = pointerByControllerId_[ctrl.uuid]
    if (!activePointer) {
      return
    }

    if (activePointer.isOnHandle) {
      alignment_.onDragMove(ctrl)
      return
    }

    if (activePointer.isOnTablet) {
      const intersection = getControllerIntersection(ctrl)
      tablet_.onControllerMove(activePointer.identifier, intersection)
      return
    }

    // While we're tracking we need to have smooth motion. So to implement this we take the
    // projection of the controller ray to the infinite sphere and then correct for it by the
    // offset to the initial intersection vector.
    const p = cameraPointAtInfinity(
      rotationForMatrix(ctrl.getObjectByName('tracer').matrixWorld)
    )
    p.applyQuaternion(activePointer.tracerToIntersection).applyMatrix4(camera_.matrixWorld)
    interactionProvider_.updatePointerPosition(
      activePointer.identifier, getCtrlClipSpacePoint(p, true)
    )
  }

  const handleControllerSqueezeStart = ({target: ctrl}) => {
    const currentPointer = pointerByControllerId_[ctrl.uuid]
    if (currentPointer) {
      return
    }

    if (tabletEnabled_) {
      if (isDraggingHandle()) {
        return
      }

      const intersection = getControllerIntersection(ctrl)

      if (tablet_.isHandle(intersection.object)) {
        alignment_.onDragStart(ctrl)
        pointerByControllerId_[ctrl.uuid] = {
          isOnHandle: true,
          isSqueeze: true,
        }
      }
    }
  }

  const handleControllerSqueezeEnd = ({target: ctrl}) => {
    const currentPointer = pointerByControllerId_[ctrl.uuid]
    if (!currentPointer || !currentPointer.isSqueeze) {
      return
    }

    delete pointerByControllerId_[ctrl.uuid]
    if (currentPointer.isOnHandle) {
      alignment_.onDragEnd()
    }
  }

  const processEvents = () => {
    if (!attached_) {
      return
    }

    updatePointerForController(controllers_.left.controller)
    updatePointerForController(controllers_.right.controller)

    interactionProvider_.flushMoveEvents()
  }

  const setOriginTransform = (originTransform, sceneScale) => {
    if (!sceneScale) {
      return
    }

    sceneScale_ = sceneScale
    controllers_.setOriginTransform(originTransform)
    if (cameraAttachmentManager_) {
      updateScaleInvert(cameraAttachmentManager_.getCamAttachedChildren())
    }
  }

  const initializeTablet = () => {
    if (tablet_) {
      return
    }
    tablet_ = createDomTablet({element: document.body, interactionProvider: interactionProvider_})
    wristModel_ = createDomWristModel()

    wristGroup_ = new window.THREE.Group()

    wristModel_.onLoad().then((modelObject) => {
      wristGroup_.add(modelObject)
    })
  }

  const attach = ({renderer, canvas, scene, camera, pauseSession, sessionConfiguration}) => {
    controllers_ = CreateControllers(renderer, scene)
    if (!sessionConfiguration.disableCameraReparenting) {
      cameraAttachmentManager_ = CameraAttachmentManagerFactory({
        controllers: controllers_,
        camera,
      })
    }
    scene_ = scene
    camera_ = camera
    canvas_ = canvas
    pauseSession_ = pauseSession
    tabletEnabled_ = !sessionConfiguration.disableXrTablet
    touchEmulationEnabled_ = !sessionConfiguration.disableXrTouchEmulation
    attached_ = true

    if (tabletEnabled_ || touchEmulationEnabled_) {
      controllers_.left.controller.addEventListener('selectstart', handleControllerSelectStart)
      controllers_.left.controller.addEventListener('selectend', handleControllerSelectEnd)
      controllers_.left.controller.addEventListener('squeezestart', handleControllerSqueezeStart)
      controllers_.left.controller.addEventListener('squeezeend', handleControllerSqueezeEnd)

      controllers_.right.controller.addEventListener('selectstart', handleControllerSelectStart)
      controllers_.right.controller.addEventListener('selectend', handleControllerSelectEnd)
      controllers_.right.controller.addEventListener('squeezestart', handleControllerSqueezeStart)
      controllers_.right.controller.addEventListener('squeezeend', handleControllerSqueezeEnd)
    }

    if (tabletEnabled_) {
      tabletMinimized_ = !!sessionConfiguration.xrTabletStartsMinimized
      initializeTablet()

      // We initialize alignment separately because it depends on controllers.
      alignment_ = createDomTabletAlignment({
        camera: camera_,
        controllersGroup: controllers_.controllersGroup,
      })

      tablet_.attach()
      alignment_.attach()

      tablet_.setMinimized(tabletMinimized_)
      wristModel_.setMinimized(tabletMinimized_)
      alignment_.setPaused(tabletMinimized_)

      const tabletObject = tablet_.getObject()
      tabletObject.position.z = 0.2
      const tabletParent = alignment_.getObject()
      tablet_.setParent(tabletParent)

      controllers_.left.controller.add(wristGroup_)
      wristGroup_.position.z = 0.1
    }

    camera_.updateMatrixWorld(true)
    controllers_.controllersGroup.updateMatrixWorld(true)
  }

  const detach = () => {
    if (!attached_) {
      return
    }
    attached_ = false
    pauseSession_ = null
    controllers_.left.controller.removeEventListener('selectstart', handleControllerSelectStart)
    controllers_.left.controller.removeEventListener('selectend', handleControllerSelectEnd)
    controllers_.left.controller.removeEventListener('squeezestart', handleControllerSqueezeStart)
    controllers_.left.controller.removeEventListener('squeezeend', handleControllerSqueezeEnd)

    controllers_.right.controller.removeEventListener('selectstart', handleControllerSelectStart)
    controllers_.right.controller.removeEventListener('selectend', handleControllerSelectEnd)
    controllers_.right.controller.removeEventListener('squeezestart', handleControllerSqueezeStart)
    controllers_.right.controller.removeEventListener('squeezeend', handleControllerSqueezeEnd)

    if (cameraAttachmentManager_) {
      cameraAttachmentManager_.reset()
    }

    if (tabletEnabled_) {
      tablet_.setParent(null)
      if (wristGroup_.parent) {
        wristGroup_.parent.remove(wristGroup_)
      }
      tablet_.detach()
      alignment_.detach()
      alignment_ = null
      tabletEnabled_ = false
    }

    isFirstUpdate_ = true
    tabletMinimized_ = false
    touchEmulationEnabled_ = false
  }

  const updateTracers = () => {
    [controllers_.left, controllers_.right].forEach(({controller, tracer}) => {
      const {distance} = getControllerIntersection(controller)
      const trueDis = distance / sceneScale_
      tracer.scale.z = Math.min(trueDis, MAX_TRACER_LENGTH)
    })
  }

  const update = (() => {
    const tempVector = new window.THREE.Vector3()
    return (frame) => {
      controllers_.update(frame)
      updateTracers()
      processEvents()

      if (tabletEnabled_) {
        tablet_.update(camera_)

        if (isFirstUpdate_) {
          isFirstUpdate_ = false
          alignment_.resetToFront()
        } else {
          alignment_.update()
        }

        tempVector.setFromMatrixPosition(camera_.matrixWorld)
        wristGroup_.lookAt(tempVector)
      }
    }
  })()

  const moveCameraChildrenToController = () => {
    if (!cameraAttachmentManager_) {
      return
    }
    cameraAttachmentManager_.moveToController()
    if (sceneScale_) {
      updateScaleInvert(cameraAttachmentManager_.getCamAttachedChildren())
    }
  }

  return {
    initializeTablet,
    attach,
    detach,
    isAttached: () => attached_,
    moveCameraChildrenToController,
    setOriginTransform,
    update,
  }
}

export {
  XrInputManagerFactory,
}
