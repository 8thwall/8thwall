import React from 'react'
import Measure from 'react-measure'
import {Sphere} from '@react-three/drei'
import {Canvas} from '@react-three/fiber'
import {
  BackSide, MeshBasicMaterial, OrthographicCamera, Plane, Raycaster, Scene, Vector2, Vector3,
  PCFShadowMap,
} from 'three'
import CameraControlsImpl from 'camera-controls'
import {v4 as uuid} from 'uuid'

import {getSkyFromSceneGraph} from '@ecs/shared/get-sky-from-scene-graph'
import {getReflectionsFromSceneGraph} from '@ecs/shared/get-environment-from-scene-graph'
import type {CameraObject} from '@ecs/runtime/camera-manager-types'
import {getFogFromSceneGraph} from '@ecs/shared/get-fog-from-scene-graph'
import type {StaticImageTargetOrientation, Vec4Tuple} from '@ecs/shared/scene-graph'
import {quat} from '@ecs/runtime/math/quat'

import {useSceneContext} from './scene-context'
import {SceneObjects} from './scene-objects'
import {isAssetPath} from '../common/editor-files'
import {GroundIndicator} from './ground-indicator'
import {TransformGizmo} from './transform-gizmo'
import {FloatingLeftPanel} from './floating-left-panel'
import {FloatingRightPanel} from './floating-right-panel'
import {useStudioStateContext} from './studio-state-context'
import {insertAssetAsObject} from './insert-object'
import {CanvasMenuOptions} from './canvas-menu-options'
import {AiAssistantModal} from './ai-assistant'
import {PrefabDeleteModal} from './delete-prefab-modal'
import {SelectionBoxHelper} from './selection-box-helper'
import {useSceneHotKeys} from './use-studio-hotkeys'
import {SceneInterface, SceneInterfaceContextProvider} from './scene-interface-context'
import {createThemedStyles, useUiTheme} from '../ui/theme'
import {
  ViewportControlTray, getViewportControlBottomPosition, getViewportControlLeftPosition,
  getGizmoBottomMargin, getGizmoLeftMargin, GIZMO_SPHERE_SCALE,
} from './viewport-control-tray'
import {
  useSceneCamera,
  MAX_PERSPECTIVE_DISTANCE,
  MIN_ORTHO_ZOOM,
  MIN_PERSPECTIVE_DISTANCE,
} from './hooks/use-scene-camera'
import {FloatingCenterTopBar, FloatingLeftTopBar, FloatingRightTopBar} from './floating-top-toolbar'
import {SceneBackground} from './scene-background'
import {GizmoHelper} from '../../third_party/drei/GizmoHelper'
import {GizmoViewport} from '../../third_party/drei/GizmoViewport'
import {CameraControls} from '../../third_party/drei/CameraControls'
import {TreeElementMenuOptions} from './tree-element-menu-options'
import {Keys} from './common/keys'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import {useDebounce} from '../common/use-debounce'
import {useStudioAgentStateContext} from './studio-agent/studio-agent-context'
import {useSelectedObjects} from './hooks/selected-objects'
import {useResourceUrl} from './hooks/resource-url'
import {OutlinePostProcessor} from './outline-postprocessor'
import {makeInstance, unlinkAllPrefabInstances} from './configuration/prefab'
import {makeImageTarget} from './make-object'
import {createAndSelectObject} from './new-primitive-button'
import SceneEnvironment from './scene-environment'
import {useAssetLabStateContext} from '../asset-lab/asset-lab-context'
import {NaeNotificationButton} from './nae-build-notification-button'

import {useActiveRoot} from '../hooks/use-active-root'
import {deleteAllPrefabInstances, deleteObjects} from './configuration/delete-object'
import {usePublishingStateContext} from '../editor/publishing/publish-context'
import {useDerivedScene} from './derived-scene-context'
import {SceneFog} from './scene-fog'
import {CameraPreview} from './configuration/camera-preview'
import {isAgentBetaEnabled} from '../../shared/account-utils'
import useCurrentAccount from '../common/use-current-account'
import {ErrorBoundary} from '../common/error-boundary'
import {CaughtViewportError} from './caught-viewport-error'

const AssetLabModal = React.lazy(() => import('../asset-lab/asset-lab-modal'))

// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

const useStyles = createThemedStyles(theme => ({
  sceneContainer: {
    'backgroundColor': theme.studioFullscreenBackground,
    'height': '100%',
    '& *::-webkit-scrollbar': {
      width: '6px',
    },
    '& *::-webkit-scrollbar-track': {
      backgroundColor: theme.scrollbarTrackBackground,
      borderRadius: '6px',
    },
    '& *::-webkit-scrollbar-thumb': {
      'backgroundColor': theme.scrollbarThumbColor,
      '&:hover': {
        backgroundColor: theme.scrollbarThumbHoverColor,
      },
    },
  },
  viewportCanvas: {
    '& canvas': {  // NOTE(christoph): There are some wrapper divs around the inner canvas
      position: 'absolute',
    },
  },
  uiLayer: {
    position: 'absolute',
    height: '100%',
    width: '100%',
    display: 'flex',
    flexDirection: 'column',
    zIndex: 10,
    pointerEvents: 'none',
  },
  toolbarRow: {
    'display': 'flex',
    'flexDirection': 'row',
    'justifyContent': 'space-between',
    'padding': '0.75rem',
    'zIndex': '1',
    '& > *': {
      pointerEvents: 'auto',
    },
  },
  toolbarRowLeft: {
    display: 'flex',
    flexDirection: 'row',
    justifyContent: 'flex-start',
    gap: '0.5rem',
  },
  toolbarRowRight: {
    display: 'flex',
    flexDirection: 'row',
    justifyContent: 'flex-end',
    gap: '0.5rem',
  },
  paneContainer: {
    height: '100%',
    position: 'relative',
    display: 'flex',
    justifyContent: 'space-between',
  },
  viewportControlContainer: {
    position: 'absolute',
  },
  centerUiContainer: {
    flexGrow: 1,
    position: 'relative',
  },
}))

interface ISceneEdit {
  navigationMenu?: React.ReactNode
  buildControlTray?: React.ReactNode
  fileBrowser?: React.ReactNode
  errorMessage?: React.ReactNode
  codeEditorToggle?: React.ReactNode
  simulatorPanel?: React.ReactNode
  fixedHelpButtonTray?: React.ReactNode
  playbackControls?: React.ReactNode
  renderScenePlayback?: (defaultCamera: CameraObject) => React.ReactNode
  hideViewport?: boolean
}

const SceneEdit: React.FC<ISceneEdit> = ({
  navigationMenu, buildControlTray,
  fileBrowser, errorMessage, codeEditorToggle,
  simulatorPanel,
  fixedHelpButtonTray, playbackControls, renderScenePlayback,
  hideViewport,
}) => {
  const classes = useStyles()
  const theme = useUiTheme()

  const pointerDownTime = React.useRef<number | null>(null)
  const stateCtx = useStudioStateContext()
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const activeRoot = useActiveRoot()
  const isRootPrefab = activeRoot.prefab
  const activeSpace = isRootPrefab ? undefined : activeRoot
  const sky = getSkyFromSceneGraph(ctx.scene, activeSpace?.id)
  const fog = getFogFromSceneGraph(ctx.scene, activeSpace?.id)
  const reflections = getReflectionsFromSceneGraph(ctx.scene, activeSpace?.id)
  const reflectionsUrl = useResourceUrl(reflections || '')
  const sceneRef = React.useRef<Scene>()
  const {
    playing, pointing, showUILayer, showGrid, shadingMode, selectedIds,
  } = stateCtx.state
  const {htmlShellToNaeBuildData} = usePublishingStateContext()

  const {isDraggingGizmo} = ctx
  const assetLabCtx = useAssetLabStateContext()
  const [isAltPressed, setIsAltPressed] = React.useState(false)
  const [isPointerOnViewportGizmo, setIsPointerOnViewportGizmo] = React.useState(false)
  const [isTrackpad, setIsTrackpad] = React.useState(false)

  const controlsRef = React.useRef<CameraControlsImpl>(null)
  const pointerMoveRef = React.useRef<boolean>(false)
  // triggers rerender for canvas context menu
  const [counterUseState, setCounterUseState] = React.useState(false)

  const [leftPaneWidth, setLeftPaneWidth] = React.useState(0)
  const updateLeftPaneWidthRef = React.useRef(setLeftPaneWidth)
  const debouncedLeftPaneWidth = useDebounce(updateLeftPaneWidthRef, 500)
  const account = useCurrentAccount()

  const isIsolated = !!activeRoot.prefab

  const {
    defaultCamera,
    cameraState,
    canvasBounds,
    setCanvasBounds,
    debouncedUpdateCanvasBounds,
    handleResetCamera,
    updateCameraState,
    debouncedUpdateCameraState,
    handleFocusObject,
    handleCameraLookAt,
  } = useSceneCamera(controlsRef, sceneRef, leftPaneWidth)

  const getMouseButtonsConfig = () => {
    let wheelAction
    if (isTrackpad && isAltPressed) {
      wheelAction = CameraControlsImpl.ACTION.TRUCK
    } else if (defaultCamera instanceof OrthographicCamera) {
      wheelAction = CameraControlsImpl.ACTION.ZOOM
    } else {
      wheelAction = CameraControlsImpl.ACTION.DOLLY
    }
    return {
      left: (isAltPressed || isPointerOnViewportGizmo) ? CameraControlsImpl.ACTION.ROTATE : null,
      right: CameraControlsImpl.ACTION.TRUCK,
      middle: CameraControlsImpl.ACTION.TRUCK,
      wheel: wheelAction,
    }
  }

  React.useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key === Keys.ALT) {
        setIsAltPressed(true)
      }
    }

    const handleKeyUp = (event: KeyboardEvent) => {
      if (event.key === Keys.ALT) {
        setIsAltPressed(false)
      }
    }

    const handleWheel = (event: WheelEvent & {wheelDeltaY: number}) => {
      let isOnTrackpad = false
      if (event.wheelDeltaY) {
        if (event.wheelDeltaY === (event.deltaY * -3)) {
          isOnTrackpad = true
        }
      } else if (event.deltaMode === 0) {
        isOnTrackpad = true
      }
      setIsTrackpad(isOnTrackpad)
    }
    window.addEventListener('keydown', handleKeyDown)
    window.addEventListener('keyup', handleKeyUp)
    window.addEventListener('wheel', handleWheel)

    return () => {
      window.removeEventListener('keydown', handleKeyDown)
      window.removeEventListener('keyup', handleKeyUp)
      window.removeEventListener('wheel', handleWheel)
      updateLeftPaneWidthRef.current = null
    }
  }, [])

  const handleDrop = (e: React.DragEvent) => {
    e.preventDefault()
    e.stopPropagation()
    const filePath = e.dataTransfer.getData('filePath')
    const objectId = e.dataTransfer.getData('objectId')
    const resourceUrl = e.dataTransfer.getData('resourceUrl')
    const imageTargetName = e.dataTransfer.getData('imageTargetName')
    const imageTargetMetadataStr = e.dataTransfer.getData('imageTargetMetadata')
    let staticOrientation: StaticImageTargetOrientation | undefined
    if (imageTargetMetadataStr) {
      const imageTargetMetadata = JSON.parse(imageTargetMetadataStr)
      staticOrientation = imageTargetMetadata.staticOrientation
    }
    const parentPrefabId = derivedScene.getParentPrefabId(objectId)
    if ((!filePath || !isAssetPath(filePath)) && !parentPrefabId && !imageTargetName) {
      stateCtx.update(p => ({...p, errorMsg: 'Invalid file dropped'}))
      return
    }

    const mouse = new Vector2(
      ((e.clientX - canvasBounds.left) / canvasBounds.width) * 2 - 1,
      -((e.clientY - canvasBounds.top) / canvasBounds.height) * 2 + 1
    )
    const raycaster = new Raycaster()
    const intersectingPoint = new Vector3(0, 0, 0)
    const dropTargetPlane = new Plane(new Vector3(0, 1, 0))

    raycaster.setFromCamera(mouse, defaultCamera)
    raycaster.ray.intersectPlane(dropTargetPlane, intersectingPoint)

    if (parentPrefabId) {
      const newId = uuid()
      ctx.updateScene(scene => makeInstance(scene, objectId, stateCtx.state.activeSpace, newId,
        [intersectingPoint.x, intersectingPoint.y, intersectingPoint.z]))
      stateCtx.setSelection(newId)
      return
    } else if (imageTargetName) {
      const imgTarget = makeImageTarget(activeRoot.id, imageTargetName, staticOrientation)
      imgTarget.name = imageTargetName
      if (staticOrientation) {
        imgTarget.rotation = quat.pitchYawRollDegrees({
          x: staticOrientation.rollAngle,
          y: 0,
          z: staticOrientation.pitchAngle,
        }).data() as Vec4Tuple
      }
      imgTarget.position = [intersectingPoint.x, intersectingPoint.y, intersectingPoint.z]
      createAndSelectObject(imgTarget, ctx, stateCtx)
      return
    }

    insertAssetAsObject(ctx, stateCtx, filePath, resourceUrl, activeRoot.id, intersectingPoint)
  }

  const onContextMenuOpenChange = () => setCounterUseState(!counterUseState)
  const menuState = useContextMenuState(onContextMenuOpenChange)
  const collapseContextMenu = () => menuState.setContextMenuOpen(false)

  const handlePointerUp = (e: React.MouseEvent) => {
    setIsPointerOnViewportGizmo(false)
    if (!pointerMoveRef.current && e.button === 2) {
      menuState.handleContextMenu(e)
    } else {
      collapseContextMenu()
    }
    pointerMoveRef.current = false
  }

  useSceneHotKeys(handleFocusObject)

  const {state: studioAgentStateCtx} = useStudioAgentStateContext()

  const selectedObjects = useSelectedObjects()
  const gizmoDisabled = selectedObjects.some(obj => obj.mapPoint)
  const showGizmo = stateCtx.state.selectedIds.length > 0 &&
    selectedObjects.every((obj) => {
      const prefab = derivedScene.getParentPrefabId(obj.id)
      return !prefab || activeRoot.prefab?.id === prefab
    })

  const selectedCamera = selectedObjects.find(obj => obj.camera)

  const sceneInterfaceCtx: SceneInterface = {
    focusObject: handleFocusObject,
    cameraLookAt: (position) => {
      if (!controlsRef.current) {
        return
      }
      // TODO(dat): Better distance?
      handleCameraLookAt(position[0], position[1], position[2], 100, true)
    },
    getCameraTransform: () => (
      (controlsRef.current?.camera || defaultCamera).matrixWorld
    ).clone(),
    selectIds: (ids) => {
      if (!ids) {
        return
      }
      stateCtx.setSelection(...ids)
    },
  }
  return (
    <Measure
      bounds
      onResize={(contentRect) => { debouncedLeftPaneWidth(contentRect.bounds.width) }}
    >
      {leftPaneMeasure => (
        <SceneInterfaceContextProvider value={sceneInterfaceCtx}>
          <Measure
            bounds
            onResize={(contentRect) => {
              // NOTE(carson): There is an issue where canvasBounds can be stale if the canvas isn't
              // full screen, since the default value is set to the window size. We still want to
              // debounce in general, but we want to fix the initial value immediately to avoid
              // the value being stale for the full 500 ms at the beginning since this is much
              // more noticeable than some staleness on user resize.
              if (canvasBounds.initial) {
                setCanvasBounds({
                  width: contentRect.bounds.width,
                  height: contentRect.bounds.height,
                  left: contentRect.bounds.left,
                  top: contentRect.bounds.top,
                  initial: false,
                })
              } else {
                debouncedUpdateCanvasBounds({
                  width: contentRect.bounds.width,
                  height: contentRect.bounds.height,
                  left: contentRect.bounds.left,
                  top: contentRect.bounds.top,
                  initial: false,
                })
              }
            }}
          >
            {canvasMeasure => (
              <div className={classes.sceneContainer} ref={canvasMeasure.measureRef}>
                <ContextMenu
                  menuState={menuState}
                  options={collapse => (
                    <>
                      {selectedIds.length > 0 && (
                        <TreeElementMenuOptions id={selectedIds.at(-1)!} collapse={collapse} />
                      )}
                      <CanvasMenuOptions collapse={collapse} />
                    </>
                  )}
                />
                {showUILayer &&
                  <div className={classes.uiLayer}>
                    <div className={classes.paneContainer}>
                      <FloatingLeftPanel
                        ref={leftPaneMeasure.measureRef}
                        fileBrowser={fileBrowser}
                        errorMessage={errorMessage}
                        fixedHelpButtonTray={fixedHelpButtonTray}
                        topBar={(
                          <FloatingLeftTopBar
                            navigationMenu={navigationMenu}
                          />
                        )}
                      />
                      {(BuildIf.CLOUD_STUDIO_AI_20240413 || isAgentBetaEnabled(account)) &&
                      studioAgentStateCtx.open &&
                        <AiAssistantModal />
                      }
                      {!!stateCtx.state.objectToBeErased &&
                        <PrefabDeleteModal
                          onClose={() => {
                            stateCtx.update(p => ({...p, objectToBeErased: undefined}))
                          }}
                          onDelete={() => {
                            const deletedId = stateCtx.state.objectToBeErased
                            stateCtx.update(p => ({
                              ...p,
                              objectToBeErased: undefined,
                              selectedPrefab: stateCtx.state.selectedPrefab === deletedId
                                ? undefined
                                : p.selectedPrefab,
                            }))
                            ctx.updateScene(scene => deleteAllPrefabInstances(scene, deletedId))
                            ctx.updateScene(scene => deleteObjects(scene, [deletedId]))
                          }}
                          onUnlink={() => {
                            const prefabId = stateCtx.state.objectToBeErased
                            stateCtx.update(p => ({
                              ...p,
                              objectToBeErased: undefined,
                              selectedPrefab: stateCtx.state.selectedPrefab === prefabId
                                ? undefined
                                : p.selectedPrefab,
                            }))
                            ctx.updateScene(scene => unlinkAllPrefabInstances(scene, prefabId))
                            ctx.updateScene(scene => deleteObjects(scene, [prefabId]))
                          }}
                        />}
                      <div className={classes.centerUiContainer}>
                        <FloatingCenterTopBar
                          playbackControls={playbackControls}
                        />
                        {simulatorPanel}
                        <React.Suspense fallback={null}>
                          {assetLabCtx.state?.open && <AssetLabModal />}
                        </React.Suspense>
                        {selectedCamera &&
                          <CameraPreview cameraObject={selectedCamera} />
                        }
                      </div>
                      {Object.values(htmlShellToNaeBuildData)
                        .some(data => data.naeBuildStatus !== 'noBuild') &&
                          <NaeNotificationButton />
                      }
                      <FloatingRightPanel
                        topBar={(
                          <FloatingRightTopBar
                            buildControlTray={buildControlTray}
                            codeEditorToggle={codeEditorToggle}
                          />
                        )}
                      />
                    </div>
                    <div
                      className={classes.viewportControlContainer}
                      style={{
                        left: getViewportControlLeftPosition(leftPaneMeasure.contentRect),
                        bottom: getViewportControlBottomPosition(leftPaneMeasure.contentRect),
                      }}
                    >
                      <ViewportControlTray
                        onResetCamera={handleResetCamera}
                        onCameraViewChange={updateCameraState}
                      />
                    </div>
                  </div>
                }
                {!hideViewport &&
                  <ErrorBoundary fallback={CaughtViewportError}>
                    <Canvas
                      frameloop='demand'
                      ref={menuState.refs.setReference}
                      {...menuState.getReferenceProps()}
                      flat
                      onPointerMissed={(e) => {
                        if (
                          e.button === 0 && pointerDownTime.current &&
                        performance.now() - pointerDownTime.current < 200
                        ) {
                          stateCtx.setSelection()
                        }
                      }}
                      shadows={{type: PCFShadowMap}}
                      onDrop={handleDrop}
                      onDragOver={(e) => {
                        e.preventDefault()
                        e.stopPropagation()
                      }}
                      className={classes.viewportCanvas}
                      onPointerDown={() => {
                        pointerDownTime.current = performance.now()
                      }}
                      onPointerUp={handlePointerUp}
                      onContextMenu={(e) => {
                        e.preventDefault()
                        e.stopPropagation()
                      }}
                      onPointerMove={(e) => {
                        if (e.buttons === 2) {
                          pointerMoveRef.current = true
                        }
                      }}
                      camera={defaultCamera}
                      onCreated={({scene}) => {
                        sceneRef.current = scene
                      }}
                    >
                      {!playing && <OutlinePostProcessor />}
                      <SceneBackground
                        sky={sky}
                        isIsolated={isIsolated}
                      />
                      {!isIsolated &&
                        <>
                          <SceneEnvironment reflectionsUrl={reflectionsUrl} />
                          <SceneFog fog={fog} />
                        </>
                      }
                      {shadingMode === 'unlit' && !playing &&
                        <>
                          <ambientLight intensity={0.5 * Math.PI} />
                          <pointLight position={[10, 10, 10]} />
                        </>
                      }
                      {(!playing || !pointing) &&
                        <>
                          {!isDraggingGizmo && !isAltPressed && !isPointerOnViewportGizmo &&
                            <SelectionBoxHelper canvasBounds={canvasBounds} />}
                          <CameraControls
                            ref={controlsRef}
                            camera={defaultCamera}
                            target={cameraState.target}
                            enabled={!isDraggingGizmo}
                            mouseButtons={getMouseButtonsConfig()}
                            smoothTime={0}
                            draggingSmoothTime={0.05}
                            maxDistance={MAX_PERSPECTIVE_DISTANCE}
                            minDistance={MIN_PERSPECTIVE_DISTANCE}
                            minZoom={MIN_ORTHO_ZOOM}
                            makeDefault
                            dollyToCursor
                            onChange={debouncedUpdateCameraState}
                          />
                          <GizmoHelper
                            alignment='bottom-left'
                            margin={[
                              getGizmoLeftMargin(leftPaneMeasure.contentRect),
                              getGizmoBottomMargin(leftPaneMeasure.contentRect),
                            ]}
                          >
                            {showUILayer &&
                              <>
                                <Sphere
                                  material={new MeshBasicMaterial({
                                    color: theme.studioBgOpaque, side: BackSide,
                                  })}
                                  scale={new Vector3(...GIZMO_SPHERE_SCALE)}
                                  onPointerDown={() => setIsPointerOnViewportGizmo(true)}
                                  onPointerLeave={() => setIsPointerOnViewportGizmo(false)}
                                />
                                <GizmoViewport
                                  labelColor='white'
                                  scale={[35, 35, 35]}
                                  onClick={() => {
                                    setIsPointerOnViewportGizmo(true)
                                    return null
                                  }}
                                />
                              </>
                            }
                          </GizmoHelper>
                        </>
                      }
                      {(playing && renderScenePlayback)
                        ? (
                          renderScenePlayback(defaultCamera)
                        )
                        : (
                          <>
                            <SceneObjects />
                            {showGizmo &&
                              <TransformGizmo disabled={gizmoDisabled} />
                            }
                          </>
                        )}
                      {!playing && showGrid && <GroundIndicator isIsolated={isIsolated} />}
                    </Canvas>
                  </ErrorBoundary>
                }
              </div>
            )}
          </Measure>
        </SceneInterfaceContextProvider>
      )}
    </Measure>
  )
}

export {
  SceneEdit,
}

export type {
  ISceneEdit,
}
