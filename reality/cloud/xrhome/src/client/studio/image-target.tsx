import React from 'react'
import {Vector2, DoubleSide, TextureLoader} from 'three'
import {Wireframe, Edges} from '@react-three/drei'
import type {Mesh} from 'three'

import {mango} from '../static/styles/settings'
import {enableOutline, removeOutline} from './selected-outline'
import {useImageTarget} from './hooks/use-image-target'
import {useStudioStateContext} from './studio-state-context'

interface IImageTargetMesh {
  name?: string
  outlined?: boolean
  objectId?: string
}

interface ISubTargetMesh {
  imageSrc: string
  metadata: string
  originalImagePath: string
  isRotated: boolean
  showGeometry: boolean
  showFullImage: boolean
  showTrackedRegion: boolean
}
interface ICylinder {
  radiusTop?: number
  radiusBottom?: number
  cylinderHeight?: number
  openEnded?: boolean
  thetaStart?: number
  thetaLength?: number
  scale?: number
  texturePath?: string
  textureLandscape?: boolean
  greyTexture?: boolean
  yOffset?: number
}

interface IPlane {
  imagePath: string
  width: number
  height: number
  textureLandscape: boolean
  greyTexture?: boolean
}

const WIREFRAME_OPACITY = 0.6

const useRotatedTexture = (texturePath: string, isRotated: boolean) => React.useMemo(() => {
  if (!texturePath) {
    return null
  }
  const tex = new TextureLoader().load(texturePath)
  tex.anisotropy = 16

  if (isRotated) {
    tex.center = new Vector2(0.5, 0.5)
    tex.rotation = Math.PI / 2
    tex.updateMatrix()
  }
  return tex
}, [texturePath, isRotated])

const Plane = React.forwardRef<Mesh, IPlane>(({
  imagePath, width, height, textureLandscape, greyTexture = false,
}, ref) => {
  const texture = useRotatedTexture(imagePath, textureLandscape)

  return (
    <mesh ref={ref}>
      <planeGeometry args={[width, height]} attach='geometry' />
      <meshBasicMaterial
        attach='material'
        map={texture}
        color={greyTexture ? 0x7a7a7a : 0xffffff}
        side={DoubleSide}
      />
    </mesh>
  )
})

const Cylinder = React.forwardRef<Mesh, ICylinder>(({
  radiusTop = undefined, radiusBottom = undefined, cylinderHeight = undefined,
  openEnded = false, thetaStart = undefined, thetaLength = undefined, scale = 1,
  texturePath = null, textureLandscape = false, greyTexture = false, yOffset = 0,
}, ref) => {
  const DEFAULT_RADIAL_SEGMENTS = 64
  const DEFAULT_HEIGHT_SEGMENTS = 1

  const texture = useRotatedTexture(texturePath, textureLandscape)

  let material = null
  if (texturePath) {
    material = (
      <meshBasicMaterial
        attach='material'
        color={greyTexture ? 0x7a7a7a : 0xffffff}
        map={texture}
        side={DoubleSide}
      />
    )
  } else {
    material = (
      <meshBasicMaterial
        attach='material'
        color={mango}
        transparent
        opacity={WIREFRAME_OPACITY}
      />
    )
  }

  return (
    <mesh ref={ref} scale={[scale, scale, scale]} position={[0, yOffset, 0]}>
      <cylinderGeometry
        args={[radiusTop, radiusBottom, cylinderHeight, DEFAULT_RADIAL_SEGMENTS,
          DEFAULT_HEIGHT_SEGMENTS, openEnded, thetaStart, thetaLength]}
        attach='geometry'
      />
      {material}
      {!texturePath && <Edges threshold={0.1} color={mango} />}
    </mesh>
  )
})

const FlatImageTargetMesh = React.forwardRef<Mesh, ISubTargetMesh>(({
  imageSrc, metadata, originalImagePath, isRotated, showGeometry, showFullImage, showTrackedRegion,
}, ref) => {
  const geoInfo = React.useMemo(() => {
    const parsedMetadata = JSON.parse(metadata)

    let {
      left,
      top,
      width,
      height,
      originalWidth,
      originalHeight,
    } = parsedMetadata

    if (isRotated) {
      [originalWidth, originalHeight] = [originalHeight, originalWidth]
      ;[width, height] = [height, width]

      const tempY = top
      top = originalHeight - left - height
      left = tempY
    }

    return {
      originalWidth,
      originalHeight,
      width,
      height,
      left,
      top,
    }
  }, [metadata])

  const scaleFactor = Math.min(1 / geoInfo.originalWidth, 1 / geoInfo.originalHeight)

  return (
    <group scale={scaleFactor}>
      {showFullImage && (
        <Plane
          ref={ref}
          imagePath={originalImagePath}
          greyTexture
          width={geoInfo.originalWidth}
          height={geoInfo.originalHeight}
          textureLandscape={isRotated}
        />
      )}
      {(showGeometry && !showFullImage) && (
        <mesh ref={ref} scale={1}>
          <planeGeometry args={[geoInfo.originalWidth, geoInfo.originalHeight]} />
          <meshBasicMaterial color={mango} transparent opacity={WIREFRAME_OPACITY} />
          <Wireframe stroke={mango} />
        </mesh>
      )}
      {showTrackedRegion && (
        <group position={[
          (geoInfo.left + geoInfo.width / 2 - geoInfo.originalWidth / 2),
          -(geoInfo.top + geoInfo.height / 2 - geoInfo.originalHeight / 2),
          0.2,
        ]}
        >
          <Plane
            ref={showFullImage || showGeometry ? null : ref}
            imagePath={imageSrc}
            width={geoInfo.width}
            height={geoInfo.height}
            textureLandscape={isRotated}
          />
        </group>)}
    </group>
  )
})

const CurvedImageTargetMesh = React.forwardRef<Mesh, ISubTargetMesh>(({
  imageSrc, metadata, originalImagePath, isRotated, showGeometry, showFullImage, showTrackedRegion,
}, ref) => {
  const geoInfo = React.useMemo(() => {
    const parsedMetadata = JSON.parse(metadata)

    const {
      cylinderCircumferenceTop,
      cylinderCircumferenceBottom,
      cylinderSideLength,
      targetCircumferenceTop,
    } = parsedMetadata
    let {
      left,
      top,
      width,
      height,
      originalWidth,
      originalHeight,
    } = parsedMetadata

    if (isRotated) {
      [originalWidth, originalHeight] = [originalHeight, originalWidth]
      ;[width, height] = [height, width]

      const tempY = top
      top = originalHeight - left - height
      left = tempY
    }

    const radiusTop = (cylinderCircumferenceTop / (2 * Math.PI))
    const radiusBottom = (cylinderCircumferenceBottom / (2 * Math.PI))
    const cylinderHeight = Math.sqrt(
      (cylinderSideLength ** 2.0) - (radiusTop - radiusBottom) ** 2.0
    )

    // Compute full image target activation region
    const horizontalShift = (targetCircumferenceTop / (2.0 * cylinderCircumferenceTop))
    const fullLeft = 0.5 - horizontalShift
    const fullRight = 0.5 + horizontalShift

    // how much of the circumference does the target label take up
    const labelArcRatio = targetCircumferenceTop / cylinderCircumferenceTop
    // how much the cropped wraps around the cone, start and length in [0, 1]
    const croppedStart = 0.5 - (labelArcRatio / 2) + labelArcRatio * (left / originalWidth)
    const croppedLength = (width / originalWidth) * labelArcRatio

    const y = top / originalHeight
    const croppedRadiusTop = radiusTop * (1 - y) + radiusBottom * y
    const bottom = y + height / originalHeight
    const croppedRadiusBottom = radiusTop * (1 - bottom) + radiusBottom * bottom
    const croppedTargetHeight = (cylinderHeight * height) / originalHeight

    const yOffset = -(
      top * (cylinderHeight / originalHeight) +
      (croppedTargetHeight - cylinderHeight) / 2
    )

    const uniformScale = 1 / cylinderHeight

    return {
      radiusTop,
      radiusBottom,
      cylinderHeight,
      fullLeft,
      fullRight,
      croppedStart,
      croppedLength,
      croppedRadiusTop,
      croppedRadiusBottom,
      croppedTargetHeight,
      yOffset,
      uniformScale,
    }
  }, [metadata])

  return (
    <group quaternion={[0, 1, 0, 0]} scale={geoInfo.uniformScale}>
      {showGeometry && <Cylinder
        ref={ref}
        radiusTop={geoInfo.radiusTop}
        radiusBottom={geoInfo.radiusBottom}
        cylinderHeight={geoInfo.cylinderHeight}
        scale={0.99}
      />}
      {showFullImage && <Cylinder
        ref={showGeometry ? null : ref}
        radiusTop={geoInfo.radiusTop}
        radiusBottom={geoInfo.radiusBottom}
        openEnded
        cylinderHeight={geoInfo.cylinderHeight}
        thetaStart={geoInfo.fullLeft * (2 * Math.PI)}
        thetaLength={(geoInfo.fullRight - geoInfo.fullLeft) * (2 * Math.PI)}
        texturePath={originalImagePath}
        textureLandscape={isRotated}
        scale={0.995}
        greyTexture
      />}
      {showTrackedRegion && <Cylinder
        ref={(showFullImage || showGeometry) ? null : ref}
        radiusTop={geoInfo.croppedRadiusTop}
        radiusBottom={geoInfo.croppedRadiusBottom}
        openEnded
        cylinderHeight={geoInfo.croppedTargetHeight}
        thetaStart={geoInfo.croppedStart * (2 * Math.PI)}
        thetaLength={geoInfo.croppedLength * (2 * Math.PI)}
        texturePath={imageSrc}
        textureLandscape={isRotated}
        yOffset={geoInfo.yOffset}
      />}
    </group>
  )
})

const ImageTargetMeshFull: React.FC<IImageTargetMesh> = ({
  name, outlined = false, objectId,
}) => {
  const [targetData] = useImageTarget(name)
  const meshRef = React.useRef<Mesh>(null)
  const stateCtx = useStudioStateContext()
  const {state: {selectedImageTargetViews}} = stateCtx
  const targetViews = selectedImageTargetViews[objectId]
  const showGeometry = !targetViews || targetViews.includes('showGeometry')
  const showFullImage = !targetViews || targetViews.includes('showFullImage')
  const showTrackedRegion = !targetViews || targetViews.includes('showTrackedRegion')

  const meshRefHandler = React.useCallback((mesh: Mesh) => {
    if (meshRef.current) {
      removeOutline(meshRef.current)
    }
    meshRef.current = mesh

    if (mesh && outlined) {
      enableOutline(mesh)
    }
  }, [outlined])

  const {
    imageSrc, type, metadata, geometryTextureImageSrc,
    originalImageSrc, isRotated,
  } = targetData || {}
  const originalImagePath = geometryTextureImageSrc || originalImageSrc

  if (!targetData || !name) {
    return (
      <mesh ref={meshRefHandler} scale={0.25}>
        <planeGeometry args={[3, 4]} />
        <meshBasicMaterial color={mango} transparent opacity={WIREFRAME_OPACITY} />
        <Wireframe stroke={mango} />
      </mesh>
    )
  }

  return (
    type === 'PLANAR'
      ? <FlatImageTargetMesh
          ref={meshRefHandler}
          imageSrc={imageSrc}
          metadata={metadata}
          originalImagePath={originalImagePath}
          isRotated={isRotated}
          showGeometry={showGeometry}
          showFullImage={showFullImage}
          showTrackedRegion={showTrackedRegion}
      />
      : <CurvedImageTargetMesh
          ref={meshRefHandler}
          imageSrc={imageSrc}
          metadata={metadata}
          originalImagePath={originalImagePath}
          isRotated={isRotated}
          showGeometry={showGeometry}
          showFullImage={showFullImage}
          showTrackedRegion={showTrackedRegion}
      />
  )
}

const ImageTargetMeshMinimal: React.FC<{outlined?: boolean}> = ({outlined = false}) => {
  const meshRef = React.useRef<Mesh>(null)

  const meshRefHandler = React.useCallback((mesh: Mesh) => {
    if (meshRef.current) {
      removeOutline(meshRef.current)
    }
    meshRef.current = mesh

    if (mesh && outlined) {
      enableOutline(mesh)
    }
  }, [outlined])

  return (
    <mesh ref={meshRefHandler} scale={0.25}>
      <planeGeometry args={[3, 4]} />
      <meshBasicMaterial
        color={0x7a7a7a}
        opacity={WIREFRAME_OPACITY}
        side={DoubleSide}
        transparent
      />
      <Wireframe stroke={0x7a7a7a} />
    </mesh>
  )
}

const ImageTargetMesh: React.FC<IImageTargetMesh> = BuildIf.STUDIO_IMAGE_TARGETS_20260210
  ? ImageTargetMeshFull
  : ImageTargetMeshMinimal

export {
  ImageTargetMesh,
}
