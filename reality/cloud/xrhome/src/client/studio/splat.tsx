import React, {useState, useEffect} from 'react'

import type {Resource} from '@ecs/shared/scene-graph'
import {useFrame, useThree} from '@react-three/fiber'

import {Quaternion, Vector3} from 'three'

import {useResourceUrl} from './hooks/resource-url'
import {useSplatModelLoaded} from './use-splat-model'

const SKYBOX_DISTANCE = 200
const PRUNE_MARGIN = 0.9
const MODEL_CONFIG = {
  bakeSkyboxMeters: 0,
  pruneDistance: 190,
}

interface ISplat {
  src: Resource
  onLoad?: () => void
  filename?: string
  skybox?: boolean
}

interface ISplatInner {
  srcUrl: string
  onLoad?: () => void
  filename?: string
  skybox?: boolean
}

// TODO(Dale): Make splats with cleanup so we don't have to remount on every camera/src change
const SplatInner: React.FC<ISplatInner> = ({srcUrl, onLoad, filename, skybox = true}) => {
  const model = useSplatModelLoaded()

  const {camera: threeCamera, size, gl} = useThree()

  const [manager, setManager] = useState(null)
  const [prevWidth, setPrevWidth] = useState(0)
  const hasSkybox = React.useRef(false)
  const maxDimension = React.useRef(0)
  const [boundingSphere, setBoundingSphere] = React.useState(false)

  // TODO(cindyhu): instead of remounting SplatInner for splats to re-appear, calling
  // manager.setRenderWidthPixels after onLoaded could also work. See splat-system.ts for reference.
  useEffect(() => {
    let canceled = false

    if (!model || !threeCamera || !srcUrl) {
      setManager(null)
    } else {
      const splatManager = model.ThreejsModelManager.create(
        {camera: threeCamera, renderer: gl, config: MODEL_CONFIG}
      )
      splatManager.setModelSrcs([{url: srcUrl, filename}])
      splatManager.setOnLoaded(() => {
        if (!canceled) {
          splatManager.model().children[0].raycast = () => {}
          splatManager.model().raycast = () => {}
          onLoad?.()
          setManager(splatManager)
          const {width, height, depth} = splatManager.boundingBox()
          hasSkybox.current = Math.max(width, height, depth) / 2 >= SKYBOX_DISTANCE * PRUNE_MARGIN
          // This condition only covers for when the skybox is smaller than usual.
          /// Larger skybox is not supported because current iterations show
          // unintended point clouds outside the skybox.
          maxDimension.current = Math.min(SKYBOX_DISTANCE, Math.max(width, height, depth))
          splatManager.configure({pointPruneDistance: SKYBOX_DISTANCE + 10})
        }
      })

      return () => {
        canceled = true
        splatManager.dispose()
        setManager(null)
      }
    }
    return undefined
  }, [model, srcUrl, threeCamera])

  useFrame(() => {
    if (!manager) {
      return
    }

    if (size.width !== prevWidth) {
      manager.setRenderWidthPixels(size.width)
      setPrevWidth(size.width)
    }

    manager.tick()

    // If model comes with a skybox, we need to adjust the point prune distance
    if (hasSkybox.current) {
      const position = new Vector3()
      const scale = new Vector3()
      const quaternion = new Quaternion()
      manager.model().matrixWorld.decompose(position, quaternion, scale)
      const distance = threeCamera.position.distanceTo(position)
      const skyboxEstimatedRadius =
          (maxDimension.current / 2) *
          Math.max(scale.x, scale.y, scale.z)

      // If the camera is inside the skybox, we need to disable/enable the bounding sphere
      if (distance <= skyboxEstimatedRadius && boundingSphere) {
        manager.configure({pointPruneDistance: skybox ? 0 : SKYBOX_DISTANCE * PRUNE_MARGIN})
        setBoundingSphere(false)
      } else if (distance > skyboxEstimatedRadius && !boundingSphere) {
        manager.configure({pointPruneDistance: SKYBOX_DISTANCE * PRUNE_MARGIN})
        setBoundingSphere(true && skybox)
      } else if (boundingSphere && !skybox) {
        // turn off bounding sphere if skybox is disabled
        setBoundingSphere(false)
      }
    }
  })

  if (!manager) {
    return null
  }

  return (
    <>
      {boundingSphere &&
        <mesh raycast={() => {}}>
          <sphereGeometry args={[SKYBOX_DISTANCE]} />
          <meshStandardMaterial
            wireframe
            color='#ebcf34'
            emissive='#ebcf34'
            emissiveIntensity={1}
          />
        </mesh>
      }
      <primitive object={manager.model()} />
    </>
  )
}

const Splat: React.FC<ISplat> = ({src, onLoad, filename, skybox = true}) => {
  const {camera} = useThree()
  const splatSrcUrl = useResourceUrl(src)

  return (
    <SplatInner
      srcUrl={splatSrcUrl}
      onLoad={onLoad}
      key={`${splatSrcUrl}-${camera.type}`}
      filename={filename}
      skybox={skybox}
    />
  )
}

export {
  SplatInner,
  Splat,
}
