import React, {MutableRefObject} from 'react'
import type {Camera} from '@react-three/fiber'
import {PerspectiveCamera, OrthographicCamera, Vector3, Scene, Box3} from 'three'
import type CameraControlsImpl from 'camera-controls'
import {degreesToRadians} from '@ecs/shared/angle-conversion'
import {THREE_LAYERS} from '@ecs/shared/three-layers'

import {useStudioStateContext} from '../studio-state-context'
import {useEvent} from '../../hooks/use-event'
import {useDebounce} from '../../common/use-debounce'
import {RIGHT_PANEL_WIDTH} from '../floating-right-panel'
import {useSelectedObjects} from './selected-objects'
import {computeGizmoCenter} from '../object-transforms'
import {useDerivedScene} from '../derived-scene-context'
import {getRootAttributeIdFromScene} from './use-root-attribute-id'
import {useTargetResolution} from '../use-target-resolution'
import {calculateGlobalTransform} from '../global-transform'
import {useYogaParentContext} from '../yoga-parent-context'
import {calculateUiContainerOffset} from '../calculate-ui-container-offset'
import {VIEWPORT_UI_SCALE} from '../ui-root'

const DEFAULT_CAMERA_POSITION = new Vector3(0, 5, 10)
const DEFAULT_CAMERA_TARGET = new Vector3(0, 0, 0)
const FOV = 50
const FOV_RAD = degreesToRadians(FOV)
const FOV_HEIGHT_UNIT = 2 * Math.tan(FOV_RAD / 2)
const MAX_PERSPECTIVE_DISTANCE = 500
const MIN_PERSPECTIVE_DISTANCE = 0.1
const MIN_ORTHO_ZOOM = 2
const DEFAULT_ZOOM_FACTOR = 1.1

const getDirection = (
  position: Vector3, target: Vector3
) => position.clone().sub(target).normalize()

/**
 * How conversion works:
 *
 * Goal: Maintain the same screen size between cameras so that the "zoom" level is consistent
 *
 * "screen size" is the height of the camera view in world units
 * e.g. screenSize = 10 means the view fits a 10 unit tall object
 *
 * When converting to orthographic camera:
 * - fixed camera frustum and fixed camera distance (100 units away)
 * - calculate zoom given screen size
 *
 * When converting to perspective camera:
 * - fixed fov
 * - calculate position given screen size and target
 *
 */

type CameraPositionState ={
  target: Vector3
  direction: Vector3
  screenSize: number
}

type CanvasSize = { width: number, height: number }

type CanvasBounds = CanvasSize & { left: number, top: number, initial: boolean}

const DEFAULT_CAMERA_POSITION_STATE: CameraPositionState = {
  target: DEFAULT_CAMERA_TARGET,
  direction: getDirection(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET),
  screenSize: DEFAULT_CAMERA_POSITION.distanceTo(DEFAULT_CAMERA_TARGET) * FOV_HEIGHT_UNIT,
}

const getPerspectiveState = (
  target: Vector3, camera: PerspectiveCamera
): CameraPositionState => ({
  target: target.clone(),
  direction: getDirection(camera.position, target),
  screenSize: camera.position.distanceTo(target) * FOV_HEIGHT_UNIT,
})

const getOrthographicState = (
  target: Vector3, camera: OrthographicCamera
): CameraPositionState => ({
  target: target.clone(),
  direction: getDirection(camera.position, target),
  screenSize: (camera.top - camera.bottom) / camera.zoom,
})

const applyOrthographicState = (camera: OrthographicCamera, state: CameraPositionState) => {
  // NOTE (cindyhu): place orthographic camera at MAX_PERSPECTIVE_DISTANCE from target so the
  // view frustum does not clip off after switching from perspective
  camera.position.copy(state.direction).multiplyScalar(MAX_PERSPECTIVE_DISTANCE).add(state.target)
  camera.zoom = (camera.top - camera.bottom) / state.screenSize
  camera.lookAt(state.target)
  camera.updateProjectionMatrix()
  return camera
}

const applyPerspectiveState = (camera: PerspectiveCamera, state: CameraPositionState) => {
  const distance = state.screenSize / FOV_HEIGHT_UNIT
  camera.position.copy(state.direction).multiplyScalar(distance).add(state.target)
  camera.lookAt(state.target)
  camera.updateProjectionMatrix()
  return camera
}

const convertToPerspective = (canvasSize: CanvasSize, state: CameraPositionState) => {
  const aspectRatio = canvasSize.width / canvasSize.height
  const camera = new PerspectiveCamera(FOV, aspectRatio, 0.1, 2000)
  applyPerspectiveState(camera, state)
  return camera
}

const convertToOrthographic = (
  canvasSize: CanvasSize, state: CameraPositionState
) => {
  const halfWidth = canvasSize.width / 2
  const halfHeight = canvasSize.height / 2
  const camera = new OrthographicCamera(-halfWidth, halfWidth, halfHeight, -halfHeight, 0.1, 2000)
  applyOrthographicState(camera, state)
  return camera
}

const createCamera = (
  cameraView: 'orthographic' | 'perspective', canvasSize: CanvasSize,
  state: CameraPositionState
) => {
  const camera = cameraView === 'orthographic'
    ? convertToOrthographic(canvasSize, state)
    : convertToPerspective(canvasSize, state)
  camera.layers.enable(THREE_LAYERS.renderedNotRaycasted)
  return camera
}

const useSceneCamera = (
  controlsRef: MutableRefObject<CameraControlsImpl>,
  sceneRef: MutableRefObject<Scene>,
  leftPaneWidth: number
) => {
  const derivedScene = useDerivedScene()
  const {state: {cameraView, showUILayer}} = useStudioStateContext()
  const [canvasBounds, setCanvasBounds] = React.useState<CanvasBounds>({
    width: window.innerWidth,
    height: window.innerHeight,
    left: 0,
    top: 0,
    initial: true,
  })
  const [
    cameraState, setCameraState,
  ] = React.useState<CameraPositionState>(DEFAULT_CAMERA_POSITION_STATE)
  const selectedObjects = useSelectedObjects()
  const {width: overlayWidth, height: overlayHeight} = useTargetResolution()
  const {layoutMetrics} = useYogaParentContext()

  const defaultCamera = React.useMemo<Camera>(() => (
    createCamera(cameraView, {width: canvasBounds.width, height: canvasBounds.height}, cameraState)
  ), [cameraView])

  const updateCameraState = useEvent(() => {
    if (controlsRef.current) {
      const target = new Vector3()
      controlsRef.current.getTarget(target)
      if (defaultCamera instanceof PerspectiveCamera) {
        setCameraState(getPerspectiveState(target, defaultCamera))
      } else {
        setCameraState(getOrthographicState(target, defaultCamera))
      }
    }
  })

  const resetControlsSmoothTime = useEvent(() => {
    if (controlsRef.current) {
      controlsRef.current.smoothTime = 0
    }
  })

  const updateCameraStateRef = React.useRef(updateCameraState)
  const updateCanvasBoundsRef = React.useRef(setCanvasBounds)
  const resetControlsSmoothTimeRef = React.useRef(resetControlsSmoothTime)

  React.useEffect(() => () => {
    updateCameraStateRef.current = null
    updateCanvasBoundsRef.current = null
    resetControlsSmoothTimeRef.current = null
  }, [])

  const debouncedUpdateCameraState = useDebounce(updateCameraStateRef, 500)
  const debouncedUpdateCanvasBounds = useDebounce(updateCanvasBoundsRef, 500)
  const debouncedResetControlsSmoothTime = useDebounce(resetControlsSmoothTimeRef, 1000)

  const handleResetCamera = () => {
    if (controlsRef.current) {
      if (defaultCamera instanceof PerspectiveCamera) {
        applyPerspectiveState(defaultCamera, DEFAULT_CAMERA_POSITION_STATE)
      } else {
        applyOrthographicState(defaultCamera, DEFAULT_CAMERA_POSITION_STATE)
        controlsRef.current.zoomTo(defaultCamera.zoom)
      }
      controlsRef.current.setLookAt(
        defaultCamera.position.x,
        defaultCamera.position.y,
        defaultCamera.position.z,
        DEFAULT_CAMERA_TARGET.x,
        DEFAULT_CAMERA_TARGET.y,
        DEFAULT_CAMERA_TARGET.z
      )
    }
  }

  const handleCameraLookAt = (
    posX: number, posY: number, posZ: number, distance: number, transition: boolean,
    direction?: Vector3
  ) => {
    if (!controlsRef.current || !sceneRef.current) {
      return
    }

    if (defaultCamera instanceof PerspectiveCamera) {
      const cameraDirection = direction ?? cameraState.direction
      const scaledDistance = cameraDirection.multiplyScalar(distance)
      const newPosition = [
        posX + scaledDistance.x,
        posY + scaledDistance.y,
        posZ + scaledDistance.z,
      ]
      controlsRef.current.setLookAt(
        newPosition[0], newPosition[1], newPosition[2],
        posX, posY, posZ,
        transition
      )
    } else {
      const newScreenSize = distance * FOV_HEIGHT_UNIT
      const newCameraState = {
        target: new Vector3(posX, posY, posZ),
        direction: cameraState.direction,
        screenSize: newScreenSize,
      }
      applyOrthographicState(defaultCamera, newCameraState)
      if (direction) {
        const scaledDistance = direction.multiplyScalar(MAX_PERSPECTIVE_DISTANCE)
        const newPosition = [
          posX + scaledDistance.x,
          posY + scaledDistance.y,
          posZ + scaledDistance.z,
        ]
        controlsRef.current.setLookAt(
          newPosition[0], newPosition[1], newPosition[2],
          posX, posY, posZ,
          transition
        )
      } else {
        controlsRef.current.moveTo(posX, posY, posZ, transition)
      }
      controlsRef.current.zoomTo(defaultCamera.zoom, transition)
    }
  }

  const handleFocusObject = (
    id: string, transition: boolean = true, zoomScale: number = DEFAULT_ZOOM_FACTOR
  ) => {
    if (!controlsRef.current || !id || !sceneRef.current) {
      return
    }

    const rootUiIds = selectedObjects.map(
      o => getRootAttributeIdFromScene(derivedScene, o.id, 'ui')
    )
    const rootUi = derivedScene.getObject(rootUiIds[0])
    const singleOverlayUiGroup = rootUiIds.every(rootId => rootId && rootId === rootUi.id) &&
      rootUi.ui.type === 'overlay'

    const {width, height} = canvasBounds
    const panesWidth = showUILayer ? (RIGHT_PANEL_WIDTH + leftPaneWidth) : 0
    const viewAspectRatio = Math.max(width - panesWidth, 50) / height

    let objectCenter: Vector3
    let distance: number
    let direction: Vector3 | undefined

    if (singleOverlayUiGroup) {
      const elementCenter = computeGizmoCenter([rootUi], derivedScene)
      const layoutMetric = layoutMetrics.get(rootUi.id)
      const offset = calculateUiContainerOffset(layoutMetric).multiplyScalar(VIEWPORT_UI_SCALE)
      objectCenter = elementCenter.add(offset)

      const minOverlayHeight = overlayWidth / viewAspectRatio
      const targetHeight = Math.max(overlayHeight, minOverlayHeight) * VIEWPORT_UI_SCALE
      distance = (targetHeight / FOV_HEIGHT_UNIT) * zoomScale

      // note(owenmech): local z-axis is the third column of the world transform matrix
      const transform = calculateGlobalTransform(derivedScene, rootUi.id)
      const data = transform.data()
      direction = new Vector3(data[8], data[9], data[10]).normalize()
    } else {
      objectCenter = computeGizmoCenter(selectedObjects, derivedScene)
      let scale = 1
      if (objectCenter) {
        const boundingBox = new Box3()
        selectedObjects.forEach((obj) => {
          const objBoundingBox = new Box3()
          const objInScene = sceneRef.current.getObjectByName(obj.id)
          if (objInScene) {
            objBoundingBox.setFromObject(objInScene)
          } else {
            boundingBox.setFromCenterAndSize(
              new Vector3(obj.position[0], obj.position[1], obj.position[2]),
              new Vector3(obj.scale[0], obj.scale[1], obj.scale[2])
            )
          }
          boundingBox.union(objBoundingBox)
        })
        scale = boundingBox.getSize(new Vector3()).length() || 1
      }
      scale *= zoomScale
      distance = scale / (FOV_HEIGHT_UNIT * viewAspectRatio)
    }

    // temporary increase smooth time to enable smooth transition to object
    controlsRef.current.smoothTime = 0.25
    handleCameraLookAt(
      objectCenter.x, objectCenter.y, objectCenter.z, distance, transition, direction
    )
    debouncedResetControlsSmoothTime()
    updateCameraState()
  }

  return {
    defaultCamera,
    cameraState,
    canvasBounds,
    updateCameraState,
    handleResetCamera,
    setCanvasBounds,
    debouncedUpdateCanvasBounds,
    debouncedUpdateCameraState,
    handleFocusObject,
    handleCameraLookAt,
  }
}

export {
  useSceneCamera,
  FOV_HEIGHT_UNIT,
  MAX_PERSPECTIVE_DISTANCE,
  MIN_PERSPECTIVE_DISTANCE,
  MIN_ORTHO_ZOOM,
}

export type {
  CanvasBounds,
  CanvasSize,
}
