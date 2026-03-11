import React from 'react'
import {Vector2, Vector3, DoubleSide, TextureLoader} from 'three'
import {Canvas, useFrame, useThree} from '@react-three/fiber'
import {OrbitControls, OrbitControlsProps} from '@react-three/drei'
import {createUseStyles} from 'react-jss'
import type * as THREE from 'three'

import {combine} from '../../common/styles'
import {useCamera} from '../../hooks/use-camera'

const HighlightRing = ({
  radius = undefined, tube = undefined, thetaStart = undefined,
  scale = undefined, thetaLength = undefined, yOffset = 0,
}) => {
  const mesh = React.useRef<THREE.Mesh>()

  return (
    <mesh
      position={[0, yOffset, 0]}
      ref={mesh}
      scale={[scale, scale, scale]}
      rotation={[Math.PI / 2, 0, Math.PI / 2 + (thetaStart || 0)]}
    >
      <torusGeometry
        args={[radius, tube, 32, 64, thetaLength]}
        attach='geometry'
      />
      <meshBasicMaterial attach='material' color={0x56de6f} side={DoubleSide} />
    </mesh>
  )
}

const Cylinder = ({
  radiusTop = undefined, radiusBottom = undefined, cylinderHeight = undefined,
  radialSegments = 64, heightSegments = 1, openEnded = false, thetaStart = undefined,
  thetaLength = undefined, scale = 1, texturePath = null, textureLandscape = false, yOffset = 0,
  greyTexture = false, setCamera = false,
}) => {
  const mesh = React.useRef<THREE.Mesh>()
  const texture = React.useMemo(() => {
    const tex = new TextureLoader().load(texturePath)
    tex.anisotropy = 16
    // TODO(dat): tex.rotation doesn't seem to work the way I think it does
    // .          consider remapping the uv by hand
    if (textureLandscape) {
      tex.center = new Vector2(0.5, 0.5)
      tex.rotation = Math.PI / 2
      tex.updateMatrix()
    }
    return tex
  }, [texturePath, textureLandscape])

  // We create several Cylinder objects, but only one needs to set the position of the camera.
  const camera = useCamera()
  React.useEffect(() => {
    if (!setCamera || !camera) {
      return
    }

    // Solve for the position of the camera which would result cylinder taking up the full FOV.
    // See: https://stackoverflow.com/a/14614736/4979029
    const verticalZ = cylinderHeight / (2 * Math.tan(camera.fov / (2 * 180 / Math.PI)))
    const horizontalZ = 2 * (Math.max(radiusTop, radiusBottom) / camera.aspect) /
      (2 * Math.tan(camera.fov / (2 * 180 / Math.PI)))

    camera.position.set(0, cylinderHeight, -1.2 * Math.max(verticalZ, horizontalZ))
    camera.lookAt(new Vector3(0, 0, 0))
  }, [radiusTop, radiusBottom, cylinderHeight, setCamera, camera])

  let material = null
  if (texturePath && greyTexture) {
    material = (
      <meshBasicMaterial
        attach='material'
        color={0x7a7a7a}
        map={texture}
        side={DoubleSide}
      />
    )
  } else if (texturePath) {
    material = (
      <meshBasicMaterial
        attach='material'
        color={0xffffff}
        map={texture}
        side={DoubleSide}
      />
    )
  } else {
    material = <meshBasicMaterial attach='material' color={0xf2f1f3} transparent wireframe />
  }

  return (
    <mesh position={[0, yOffset, 0]} ref={mesh} scale={[scale, scale, scale]}>
      <cylinderGeometry
        args={[radiusTop, radiusBottom, cylinderHeight, radialSegments, heightSegments, openEnded,
          thetaStart, thetaLength]}
        attach='geometry'
      />
      {material}
    </mesh>
  )
}

const Controls = (props) => {
  const camera = useCamera()
  const {gl: {domElement}} = useThree()

  if (camera) {
    camera.fov = 35
    camera.near = 0.1
    camera.far = 2000
    camera.updateProjectionMatrix()
  }

  const controls: React.Ref<OrbitControlsProps> = React.useRef()
  useFrame(() => controls.current && controls.current.update())
  return <OrbitControls ref={controls} args={[camera, domElement]} {...props} />
}

const useStyles = createUseStyles({
  canvas: {
    backgroundColor: '#8083A2',
  },
})

const PanningMesh = ({isAutoPan, thetaStart, thetaLength, children}) => {
  const mesh = React.useRef<THREE.Mesh>()
  const dir = React.useRef(1.0)
  React.useEffect(() => {
    if (!isAutoPan && mesh.current) {
      mesh.current.rotation.x = 0
    }
  }, [isAutoPan])

  useFrame(() => {
    if (isAutoPan && mesh.current) {
      const rotY = mesh.current.rotation.y
      if (rotY < thetaStart) {
        dir.current = 1.0
        mesh.current.rotation.y = thetaStart
      } else if (rotY >= thetaStart + thetaLength) {
        dir.current = -1.0
        mesh.current.rotation.y = thetaStart + thetaLength
      }
      mesh.current.rotation.y += dir.current * 0.01
    }
  })
  return (
    <mesh ref={mesh}>
      {children}
    </mesh>
  )
}

interface ICITGeometryVisualizer {
  cylinderCircumferenceTop: number
  cylinderCircumferenceBottom: number
  cylinderSideLength: number
  targetCircumferenceTop: number
  originalImagePath: string
  imagePath: string
  imageLandscape: boolean
  x: number
  y: number
  width: number
  height: number
  originalWidth: number
  originalHeight: number
  isAutoPan: boolean
  highlightCircumferenceTop: boolean
  highlightCircumferenceBottom: boolean
  highlightTargetArcLength: boolean
  className: string
}

const CylindricalImageTargetGeometryVisualizer: React.FC<ICITGeometryVisualizer> = ({
  cylinderCircumferenceTop = 150.0, cylinderCircumferenceBottom = 150.0, cylinderSideLength = 100.0,
  targetCircumferenceTop = 100.0, originalImagePath, imagePath, imageLandscape = false, y = 0,
  x = 0, width = 480, height = 640, originalWidth = 480 * 2, originalHeight = 640 * 2,
  isAutoPan = false, highlightCircumferenceTop = false, highlightCircumferenceBottom = false,
  highlightTargetArcLength = false, className = null,
}) => {
  const classes = useStyles()

  const radiusTop = cylinderCircumferenceTop / (2.0 * Math.PI)
  const radiusBottom = cylinderCircumferenceBottom / (2.0 * Math.PI)
  const cylinderHeight = Math.sqrt((cylinderSideLength ** 2.0) - (radiusTop - radiusBottom) ** 2.0)

  if (imageLandscape) {
    // in the unrotated image:
    // 0,0 is the top left corner
    // +x (+left) goes from left to right
    // +y (+top) goes from top to bottom
    // in the rotated image the image is rotated +90 clockwise and then:
    // 0,0 is the top left corner
    // +x (+left) goes from left to right
    // +y (+top) goes from top to the bottom
    [originalWidth, originalHeight] = [originalHeight, originalWidth]
    ;[width, height] = [height, width]

    const tempY = y
    y = originalHeight - x - height
    x = tempY
  }

  // Compute full image target activation region
  const horizontalShift = (targetCircumferenceTop / (2.0 * cylinderCircumferenceTop))
  const fullLeft = 0.5 - horizontalShift
  const fullRight = 0.5 + horizontalShift

  // how much of the circumference does the target label take up
  const labelArcRatio = targetCircumferenceTop / cylinderCircumferenceTop
  // how much the cropped wraps around the cone, start and length in [0, 1]
  const croppedStart = 0.5 - (labelArcRatio / 2) + labelArcRatio * (x / originalWidth)
  const croppedLength = (width / originalWidth) * labelArcRatio

  const top = y / originalHeight
  const croppedRadiusTop = radiusTop * (1 - top) + radiusBottom * top
  const bottom = top + height / originalHeight
  const croppedRadiusBottom = radiusTop * (1 - bottom) + radiusBottom * bottom
  const croppedTargetHeight = (cylinderHeight * height) / originalHeight

  const yOffset = -(
    y * (cylinderHeight / originalHeight) +
    (croppedTargetHeight - cylinderHeight) / 2
  )
  return (
    <Canvas className={combine(classes.canvas, className)}>
      <ambientLight intensity={Math.PI} />
      <Controls />
      <PanningMesh
        thetaStart={croppedStart * (2 * Math.PI)}
        thetaLength={croppedLength * (2 * Math.PI)}
        isAutoPan={isAutoPan}
      >
        {/* Top circumference highlight */}
        {highlightCircumferenceTop &&
          <HighlightRing
            radius={radiusTop}
            tube={cylinderHeight * 0.009}
            scale={1.0}
            yOffset={cylinderHeight / 2}
          />
        }
        {/* Bottom circumference highlight */}
        {highlightCircumferenceBottom &&
          <HighlightRing
            radius={radiusBottom}
            tube={cylinderHeight * 0.009}
            scale={1.0}
            yOffset={-(cylinderHeight / 2)}
          />
        }
        {/* Label Arc highlight */}
        {highlightTargetArcLength &&
          <HighlightRing
            radius={radiusTop}
            tube={cylinderHeight * 0.009}
            thetaStart={fullLeft * (2 * Math.PI)}
            thetaLength={(fullRight - fullLeft) * (2 * Math.PI)}
            scale={1.0}
            yOffset={cylinderHeight / 2}
          />
        }

        {/* Cylinder mesh */}
        <Cylinder
          radiusTop={radiusTop}
          radiusBottom={radiusBottom}
          radialSegments={64}
          openEnded={false}
          cylinderHeight={cylinderHeight}
          scale={0.99}
          setCamera
        />
        {/* Full image */}
        <Cylinder
          radiusTop={radiusTop}
          radiusBottom={radiusBottom}
          radialSegments={64}
          openEnded
          cylinderHeight={cylinderHeight}
          thetaStart={fullLeft * (2 * Math.PI)}
          thetaLength={(fullRight - fullLeft) * (2 * Math.PI)}
          texturePath={originalImagePath}
          textureLandscape={imageLandscape}
          scale={0.995}
          greyTexture
        />
        {/* Cropped image */}
        <Cylinder
          radiusTop={croppedRadiusTop}
          radiusBottom={croppedRadiusBottom}
          radialSegments={64}
          openEnded
          cylinderHeight={croppedTargetHeight}
          thetaStart={croppedStart * (2 * Math.PI)}
          thetaLength={croppedLength * (2 * Math.PI)}
          texturePath={imagePath}
          textureLandscape={imageLandscape}
          yOffset={yOffset}
        />
      </PanningMesh>
    </Canvas>
  )
}
export default CylindricalImageTargetGeometryVisualizer
