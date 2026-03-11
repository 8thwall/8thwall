import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {BaseGraphObject} from '@ecs/shared/scene-graph'
import {Canvas} from '@react-three/fiber'
import {getSkyFromSceneGraph} from '@ecs/shared/get-sky-from-scene-graph'
import {getReflectionsFromSceneGraph} from '@ecs/shared/get-environment-from-scene-graph'
import {useThree} from '@react-three/fiber'
import {OrthographicCamera, PerspectiveCamera, Quaternion, Vector3} from 'three'
import {CAMERA_DEFAULTS} from '@ecs/shared/camera-constants'
import {getFogFromSceneGraph} from '@ecs/shared/get-fog-from-scene-graph'

import {SceneObjects} from '../scene-objects'
import {SceneBackground} from '../scene-background'
import SceneEnvironment from '../scene-environment'
import {useSceneContext} from '../scene-context'
import {useActiveRoot} from '../../hooks/use-active-root'
import {useResourceUrl} from '../hooks/resource-url'
import {calculateGlobalTransform} from '../global-transform'
import {useDerivedScene} from '../derived-scene-context'
import {MARGIN_SIZE} from '../interface-constants'
import {SceneFog} from '../scene-fog'
import {useTargetResolution} from '../use-target-resolution'
import {createThemedStyles} from '../../ui/theme'

const CAMERA_PREVIEW_MAX_SIZE = 300

const useStyles = createThemedStyles(theme => ({
  cameraPreview: {
    position: 'absolute',
    bottom: MARGIN_SIZE,
    right: MARGIN_SIZE,
    borderRadius: 8,
    overflow: 'hidden',
    border: `2px solid ${theme.subtleBorder}`,
  },
  canvas: {
    width: '100%',
    height: '100%',
    backgroundColor: 'white',
  },
}))
interface ICameraUpdater {
  cameraObject: DeepReadonly<BaseGraphObject>
}

const CameraUpdater: React.FC<ICameraUpdater> = ({cameraObject}) => {
  const {camera, set, size} = useThree()
  const derivedScene = useDerivedScene()

  React.useLayoutEffect(() => {
    if (!(camera instanceof PerspectiveCamera) &&
      cameraObject.camera.type === 'perspective'
    ) {
      set({camera: new PerspectiveCamera()})
      return
    } else if (!(camera instanceof OrthographicCamera) &&
      cameraObject.camera.type === 'orthographic'
    ) {
      set({camera: new OrthographicCamera()})
      return
    }

    const fov = cameraObject.camera.fov || CAMERA_DEFAULTS.fov
    const nearClip = cameraObject.camera.nearClip || CAMERA_DEFAULTS.nearClip
    const farClip = cameraObject.camera.farClip || CAMERA_DEFAULTS.farClip
    const zoom = cameraObject.camera.zoom || CAMERA_DEFAULTS.zoom
    const top = cameraObject.camera.top || CAMERA_DEFAULTS.top
    const bottom = cameraObject.camera.bottom || CAMERA_DEFAULTS.bottom
    const left = cameraObject.camera.left || CAMERA_DEFAULTS.left
    const right = cameraObject.camera.right || CAMERA_DEFAULTS.right
    const aspect = size.width / size.height

    const transform = calculateGlobalTransform(derivedScene, cameraObject.id)
    const trs = transform.decomposeTrs()

    const position = [trs.t.x, trs.t.y, trs.t.z] as [number, number, number]
    const rotation = [trs.r.x, trs.r.y, trs.r.z, trs.r.w] as [number, number, number, number]

    if (camera instanceof PerspectiveCamera) {
      camera.fov = fov
      camera.aspect = aspect
    } else if (camera instanceof OrthographicCamera) {
      camera.bottom = bottom
      camera.top = top
      camera.left = left
      camera.right = right
    }
    camera.near = nearClip
    camera.far = farClip
    camera.zoom = zoom

    camera.position.set(...position)
    camera.quaternion.set(...rotation)
    camera.quaternion.multiply((new Quaternion()).setFromAxisAngle(new Vector3(0, 1, 0), Math.PI))

    camera.updateProjectionMatrix()
  }, [camera, derivedScene, cameraObject, set])

  return null
}

interface ICameraPreview {
  cameraObject: DeepReadonly<BaseGraphObject>
}
const CameraPreview: React.FC<ICameraPreview> = (
  {cameraObject}
) => {
  const {width, height} = useTargetResolution()
  const scale = CAMERA_PREVIEW_MAX_SIZE / Math.max(width, height)
  const classes = useStyles()

  const ctx = useSceneContext()
  const activeRoot = useActiveRoot()
  const isIsolated = !!activeRoot.prefab
  const activeSpace = isIsolated ? undefined : activeRoot
  const sky = getSkyFromSceneGraph(ctx.scene, activeSpace?.id)
  const reflections = getReflectionsFromSceneGraph(ctx.scene, activeSpace?.id)
  const reflectionsUrl = useResourceUrl(reflections || '')
  const fog = getFogFromSceneGraph(ctx.scene, activeSpace?.id)

  return (
    <div
      className={classes.cameraPreview}
      style={{width: `${width * scale}px`, height: `${height * scale}px`}}
    >
      <Canvas
        className={classes.canvas}
      >
        <CameraUpdater
          cameraObject={cameraObject}
        />
        <SceneBackground
          sky={sky}
          isIsolated={isIsolated}
        />
        <SceneEnvironment reflectionsUrl={reflectionsUrl} />
        <SceneObjects showHelpers={false} />
        <SceneFog fog={fog} />
      </Canvas>
    </div>
  )
}

export {
  CameraPreview,
}
