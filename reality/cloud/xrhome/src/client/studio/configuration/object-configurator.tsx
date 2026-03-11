import React from 'react'
import {useTranslation} from 'react-i18next'
import type {DeepReadonly} from 'ts-essentials'
import type {BaseGraphObject, GraphObject, Vec4Tuple} from '@ecs/shared/scene-graph'
import {getActiveCameraIdFromSceneGraph} from '@ecs/shared/get-camera-from-scene-graph'
import {createUseStyles} from 'react-jss'
import {quat} from '@ecs/runtime/math/math'

import {useSceneContext} from '../scene-context'
import {MeshConfigurator} from './mesh-configurator'
import {ComponentConfigurators} from './component-configurator'
import {AudioConfigurator} from './audio-configurator'
import {ColliderConfigurator} from './collider-configurator'
import {LightConfigurator} from './light-configurator'
import {NameConfigurator} from './name-configurator'
import {TransformConfigurator} from './transform-configurator'
import {NewComponentButton} from './new-component-button'
import {CameraConfigurator} from './camera-configurator'
import {FaceConfigurator} from './face-configurator'
import {useSelectedObjects} from '../hooks/selected-objects'
import {GroupedUiConfigurator} from './grouped-ui-configurator'
import {useStudioStateContext} from '../studio-state-context'
import {makeFaceMeshObject} from '../make-object'
import {createAndSelectObject} from '../new-primitive-button'
import {useActiveSpace} from '../hooks/active-space'
import {DropTarget} from '../ui/drop-target'
import {useCurrentGit} from '../../git/hooks/use-current-git'
import {setSectionCollapsed} from '../hooks/collapsed-section'
import {useTreeHierarchyStyles} from '../ui/tree-hierarchy-styles'
import {useStudioComponentsContext} from '../studio-components-context'
import {ImageTargetConfigurator} from './image-target-configurator'
import {useRootAttributeId} from '../hooks/use-root-attribute-id'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {sceneInterfaceContext} from '../scene-interface-context'
import {StudioScenePreview} from '../studio-scene-preview'
import {createIdSeed} from '../id-generation'
import {enterPrefabEditor} from '../prefab-editor'
import {VideoControlsConfigurator} from './video-controls-configurator'
import {useDerivedScene} from '../derived-scene-context'
import {resetPrefabInstanceComponents, applyPrefabOverridesToRoot} from './prefab'
import {DISCARD_ROOT_PATH, ScenePathRootProvider} from '../scene-path-input-context'

const useStyles = createUseStyles(() => ({
  'prefabSection': {
    padding: '1rem',
    paddingTop: '0.5rem',
  },
}))

const shouldShowTransform = (object: DeepReadonly<GraphObject>, rootUiId?: string): boolean => {
  if (object.ui && rootUiId) {
    return rootUiId === object.id
  }

  return true
}

const ObjectConfigurator: React.FC = () => {
  const {t} = useTranslation('cloud-studio-pages')
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const objects = useSelectedObjects()
  const activeSpace = useActiveSpace()
  const activeCameraId = getActiveCameraIdFromSceneGraph(ctx.scene, activeSpace?.id)
  const git = useCurrentGit()
  const componentsContext = useStudioComponentsContext()
  const rootUiId = useRootAttributeId(objects.length > 0 && objects[0].id, 'ui')
  const classes = useStyles()

  const [hovering, setHovering] = React.useState(false)
  const treeHierarchyStyles = useTreeHierarchyStyles()

  const activeCamera = derivedScene.getObject(activeCameraId)
  const camera = activeCamera ? activeCamera.camera : null
  const isFaceCamera = camera?.xr?.xrCameraType === 'face' ?? false
  const faceConfig = camera?.xr?.face ?? null
  let maxFaceDetections = 1

  const object = objects.length === 1 ? derivedScene.getObject(objects[0].id) : null
  const parentPrefab = derivedScene.getParentPrefabId(object?.id)
  const isSelectedPrefab = parentPrefab && parentPrefab === stateCtx.state.selectedPrefab
  const sceneInterfaceCtx = React.useContext(sceneInterfaceContext)

  const isRotationLocked = object?.imageTarget?.staticOrientation
    ? {x: true, y: false, z: true}
    : undefined

  if (isFaceCamera && faceConfig) {
    maxFaceDetections = faceConfig.maxDetections
  }

  const handleFileDrop = (e: React.DragEvent<Element>) => {
    setHovering(false)
    const filePath = e.dataTransfer.getData('filePath')
    const repoId = e.dataTransfer.getData('repoId')
    if (!filePath || repoId !== git.repo.repoId) {
      return
    }

    const components = componentsContext.getComponentsFromLocation({filePath, repoId})
    if (!components.length) {
      stateCtx.update(p => ({...p, errorMsg: t('object_configurator.error.no_components_found')}))
    } else {
      const seed = createIdSeed()
      objects.forEach((currObject) => {
        const componentsToAdd = components.filter(name => (
          !Object.values(currObject.components).some(c => c.name === name)))

        if (componentsToAdd.length === 0) return

        ctx.updateObject(currObject.id, (oldObject) => {
          const newComponents = {}
          componentsToAdd.forEach((name) => {
            const id = seed.fromId(`${oldObject.id}/${name}`)
            newComponents[id] = {
              id,
              name,
            }
          })

          Object.keys(newComponents).forEach((id) => {
            setSectionCollapsed(stateCtx, currObject.id, id, false)
          })

          return {
            ...oldObject,
            components: {
              ...oldObject.components,
              ...newComponents,
            },
          }
        })
      })
    }
  }

  const onResetToPrefab = (
    componentIdsOrName: (keyof BaseGraphObject)[] | string[], nonDirect: boolean
  ) => {
    ctx.updateScene(s => resetPrefabInstanceComponents(s, object.id, componentIdsOrName, nonDirect))
  }

  const onApplyOverridesToPrefab = (
    componentIdsOrName: (keyof BaseGraphObject)[] | string[], nonDirect: boolean
  ) => {
    ctx.updateScene(s => applyPrefabOverridesToRoot(s, object.id, componentIdsOrName, nonDirect))
  }

  if (objects.length > 1) {
    return (
      <div>
        {BuildIf.STUDIO_MULTI_ADD_COMPONENTS_20241202
          ? (
            <ScenePathRootProvider path={DISCARD_ROOT_PATH}>
              <DropTarget
                as='div'
                onHoverStart={() => setHovering(true)}
                onHoverStop={() => setHovering(false)}
                className={hovering ? treeHierarchyStyles.hovered : undefined}
                onDrop={e => handleFileDrop(e)}
              >
                <NameConfigurator />
                <TransformConfigurator isRotationLocked={isRotationLocked} />
                <NewComponentButton />
              </DropTarget>
            </ScenePathRootProvider>
          )
          : (
            <ScenePathRootProvider path={DISCARD_ROOT_PATH}>
              <NameConfigurator />
              <TransformConfigurator isRotationLocked={isRotationLocked} />
            </ScenePathRootProvider>
          )}
      </div>
    )
  }

  if (!object) {
    return null
  }

  return (
    <ScenePathRootProvider path={['objects', object.id]}>
      <DropTarget
        as='div'
        onHoverStart={() => setHovering(true)}
        onHoverStop={() => setHovering(false)}
        className={hovering ? treeHierarchyStyles.hovered : undefined}
        onDrop={e => handleFileDrop(e)}
      >
        <NameConfigurator
          showActiveCameraBtn={!!object.camera && !parentPrefab}
          isActive={activeCameraId === object.id}
          onSetActiveCamera={() => {
            ctx.updateScene((scene) => {
              if (activeSpace) {
                return {
                  ...scene,
                  spaces: {
                    ...scene.spaces,
                    [activeSpace.id]: {
                      ...scene.spaces[activeSpace.id],
                      activeCamera: object.id,
                    },
                  },
                }
              }
              return {
                ...scene,
                activeCamera: object.id,
              }
            })
          }}
        />

        {parentPrefab &&
          <div className={classes.prefabSection}>
            {!isSelectedPrefab &&
              <StudioScenePreview
                rootEntityId={parentPrefab}
              />
          }
            <FloatingPanelButton
              spacing='full'
              onClick={() => enterPrefabEditor(stateCtx, sceneInterfaceCtx, parentPrefab)}
              disabled={isSelectedPrefab}
            >
              {t('object_configurator.open_in_prefab_editor')}
            </FloatingPanelButton>
          </div>}

        {shouldShowTransform(object, rootUiId) && (
          <TransformConfigurator
            isRotationLocked={isRotationLocked}
            onApplyOverridesToPrefab={onApplyOverridesToPrefab}
          />
        )}

        {(object.geometry || object.material || object.gltfModel || object.splat) &&
          <MeshConfigurator
            object={object}
            onChange={(e) => {
              ctx.updateObject(object.id, oldObject => ({
                ...oldObject,
                ...e(oldObject),
              }))
            }}
            resetToPrefab={onResetToPrefab}
            applyOverridesToPrefab={onApplyOverridesToPrefab}
          />
      }

        {object.collider &&
          <ColliderConfigurator
            collider={object.collider}
            geometryType={object.geometry?.type}
            hasModel={!!object.gltfModel?.src}
            onChange={(e) => {
              ctx.updateObject(object.id, oldObject => ({
                ...oldObject,
                collider: e(oldObject.collider),
              }))
            }}
            resetToPrefab={onResetToPrefab}
          />
      }
        {object.ui &&
          <GroupedUiConfigurator
            objectId={object.id}
            settings={object.ui}
            onChange={(e) => {
              ctx.updateObject(object.id, oldObject => ({
                ...oldObject,
                ui: e(oldObject.ui),
              }))
            }}
            resetToPrefab={onResetToPrefab}
          />
      }

        {object.audio &&
          <AudioConfigurator
            audioSettings={object.audio}
            onChange={(e) => {
              ctx.updateObject(object.id, oldObject => ({
                ...oldObject,
                audio: e(oldObject.audio),
              }))
            }}
            resetToPrefab={onResetToPrefab}
          />
      }

        {object.videoControls &&
          <VideoControlsConfigurator
            videoControls={object.videoControls}
            onChange={(e) => {
              ctx.updateObject(object.id, oldObject => ({
                ...oldObject,
                videoControls: e(oldObject.videoControls),
              }))
            }}
          />
      }

        {object.light &&
          <LightConfigurator
            light={object.light}
            onChange={(e) => {
              ctx.updateObject(object.id, oldObject => ({
                ...oldObject,
                light: e(oldObject.light),
              }))
            }}
            resetToPrefab={onResetToPrefab}
          />
      }

        {object.camera &&
          <CameraConfigurator
            camera={object.camera}
            onChange={(e) => {
              ctx.updateObject(object.id, oldObject => ({
                ...oldObject,
                camera: e(oldObject.camera),
              }))
            }}
            resetToPrefab={onResetToPrefab}
          />
      }

        {object.face &&
          <FaceConfigurator
            face={object.face}
            maxDetections={maxFaceDetections}
            onChange={(e) => {
              ctx.updateObject(object.id, oldObject => ({
                ...oldObject,
                face: e(oldObject.face),
              }))
            }}
            onAddAttachment={() => {
            // Temporary state to display all attachment points on the face mesh.
              ctx.updateObject(object.id, o => ({
                ...o,
                face: {...o.face, addAttachmentState: !o.face.addAttachmentState},
              }))
            }}
            onAddMesh={() => {
              const newObject = makeFaceMeshObject(
                object.id, t('face_configurator.button.face_mesh')
              )
              createAndSelectObject(newObject, ctx, stateCtx)
            }}
          />
      }

        {object.imageTarget &&
          <ImageTargetConfigurator
            imageTarget={object.imageTarget}
            onChange={(e) => {
              ctx.updateObject(object.id, (oldObject) => {
                const newImageTarget = e(oldObject.imageTarget)
                let newRotation = oldObject.rotation
                if (newImageTarget?.staticOrientation) {
                  newRotation = quat.pitchYawRollDegrees({
                    x: newImageTarget.staticOrientation.rollAngle,
                    y: 0,
                    z: newImageTarget.staticOrientation.pitchAngle,
                  }).data() as Vec4Tuple
                }
                return {
                  ...oldObject,
                  imageTarget: newImageTarget,
                  rotation: newRotation,
                }
              })
            }
            }
            objectId={object.id}
            resetToPrefab={onResetToPrefab}
          />
      }

        <ComponentConfigurators
          components={object.components}
          onChange={(e) => {
            ctx.updateObject(object.id, oldObject => ({
              ...oldObject,
              components: e(oldObject.components),
            }))
          }}
          resetToPrefab={onResetToPrefab}
          applyOverridesToPrefab={onApplyOverridesToPrefab}
        />

        <NewComponentButton />
      </DropTarget>
    </ScenePathRootProvider>
  )
}

export {
  ObjectConfigurator,
  shouldShowTransform,
}
