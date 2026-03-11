import React, {useEffect, useState} from 'react'

import {v4 as uuid} from 'uuid'

import type {ThreeEvent} from '@react-three/fiber'
import type {DeepReadonly} from 'ts-essentials'
import {
  BackSide, DoubleSide, FrontSide, Mesh, Vector2,
  Texture, MeshStandardMaterial, Vector3, LineBasicMaterial, LineSegments,
  EdgesGeometry, BufferGeometry, BufferAttribute,
} from 'three'
import type {
  Geometry, Material, GltfModel as Gltf,
  GraphObject, BasicMaterial, ShadowMaterial, UnlitMaterial,
  SimplificationMode,
} from '@ecs/shared/scene-graph'
import {Sphere} from '@react-three/drei'
import {ConvexGeometry} from 'three/examples/jsm/geometries/ConvexGeometry'
import {mergeVertices} from 'three/examples/jsm/utils/BufferGeometryUtils'

import {
  DEFAULT_CAP_SEGMENTS, DEFAULT_RADIAL_SEGMENTS, getPolyhedronGeometry,
} from '@ecs/shared/geometry-constants'
import {MATERIAL_DEFAULTS, DEFAULT_EMISSIVE_COLOR} from '@ecs/shared/material-constants'

import {
  faceAttachmentPointPosition,
  faceAttachmentPoints,
  faceAttachmentNames,
  earAttachmentNames,
  isFaceAttachmentPoint,
} from '@ecs/shared/face-mesh-data'

import {
  DEFAULT_OFFSET, DEFAULT_OFFSET_QUATERNION_W, DEFAULT_OFFSET_QUATERNION_X,
  DEFAULT_OFFSET_QUATERNION_Y, DEFAULT_OFFSET_QUATERNION_Z,
} from '@ecs/shared/physic-constants'

import {useSceneContext} from './scene-context'
import {GltfModel} from './gltf-model'
import {Light} from './light'
import {Camera} from './camera'
import {useObjectSelection} from './hooks/selected-objects'
import {useStudioStateContext} from './studio-state-context'
import {useTextureWithSettings} from './hooks/use-texture-with-settings'
import {useActiveSpace} from './hooks/active-space'
import ViewportIcon from './viewport-icon'
import {createAndSelectObject} from './new-primitive-button'
import {makeEmptyObject, SUPPORTED_PRIMITIVE_COLLIDER_GEOMETRIES} from './make-object'
import {enableOutline, removeOutline} from './selected-outline'

import {
  makeEarLMeshGeometry,
  makeEarRMeshGeometry,
  FaceMeshGeometry,
} from './geometry/face-geometry'
import {Splat} from './splat'
import {ImageTargetMesh} from './image-target'
import {MATERIAL_BLENDING} from './configuration/material-constants'
import {MakeMeshHiderMaterial} from './shader/mesh-hider-material'
import {useYogaParentContext} from './yoga-parent-context'
import {UiRoot} from './ui-root'
import {useDerivedScene} from './derived-scene-context'

const makeGeometry = (
  geometry: DeepReadonly<Geometry> | number[],
  simplificationMode?: SimplificationMode
) => {
  if (!geometry) {
    return null
  }
  if (Array.isArray(geometry)) {
    const vertices: Vector3[] = []
    for (let i = 0; i < geometry.length; i += 3) {
      vertices.push(new Vector3(geometry[i], geometry[i + 1], geometry[i + 2]))
    }

    let visualGeometry: BufferGeometry

    if (simplificationMode === 'concave') {
      visualGeometry = new BufferGeometry()
      const positionArray = new Float32Array(geometry)
      visualGeometry.setAttribute('position', new BufferAttribute(positionArray, 3))

      const indices = vertices.map((_, i) => i)
      visualGeometry.setIndex(indices)
      visualGeometry.computeVertexNormals()

      visualGeometry = mergeVertices(visualGeometry)
    } else {
      const convexGeometry = new ConvexGeometry(vertices)
      visualGeometry = mergeVertices(convexGeometry)
    }

    const edges = new EdgesGeometry(visualGeometry)
    return (
      <primitive object={new LineSegments(edges, new LineBasicMaterial({color: 'red'}))} />
    )
  }
  switch (geometry.type) {
    case 'box':
      return <boxGeometry args={[geometry.width, geometry.height, geometry.depth]} />
    case 'sphere':
      return <sphereGeometry args={[geometry.radius]} />
    case 'plane':
      return <planeGeometry args={[geometry.width, geometry.height]} />
    case 'capsule':
      return (
        <capsuleGeometry
          args={[geometry.radius, geometry.height, DEFAULT_CAP_SEGMENTS, DEFAULT_RADIAL_SEGMENTS]}
        />
      )
    case 'cone':
      return <coneGeometry args={[geometry.radius, geometry.height, DEFAULT_RADIAL_SEGMENTS]} />
    case 'cylinder':
      return (
        <cylinderGeometry
          args={
            [geometry.radius, geometry.radius, geometry.height, DEFAULT_RADIAL_SEGMENTS]
          }
        />
      )
    case 'tetrahedron':
      return <tetrahedronGeometry args={[geometry.radius]} />
    case 'polyhedron': {
      const polyhedron = getPolyhedronGeometry(geometry.faces)
      return (
        <polyhedronGeometry args={[polyhedron.vertices, polyhedron.indices, geometry.radius]} />
      )
    }
    case 'circle':
      return <circleGeometry args={[geometry.radius, DEFAULT_RADIAL_SEGMENTS]} />
    case 'ring':
      return (
        <ringGeometry
          args={[geometry.innerRadius, geometry.outerRadius, DEFAULT_RADIAL_SEGMENTS]}
        />
      )
    case 'torus':
      return (
        <torusGeometry
          args={[
            geometry.radius, geometry.tubeRadius, DEFAULT_RADIAL_SEGMENTS, DEFAULT_RADIAL_SEGMENTS,
          ]}
        />
      )
    default:
      // eslint-disable-next-line no-console
      console.error('Unknown Geometry', geometry)
      return null
  }
}

interface IFacePoint {
  index: number
  object: DeepReadonly<GraphObject>
  position: Vector3
}

const FacePoint: React.FC<IFacePoint> = ({
  index, object, position,
}) => {
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const [hovered, setHovered] = useState(false)

  const onClick = () => {
    const newObject = makeEmptyObject(object.id)
    const attachmentPos = faceAttachmentPoints[index]
    newObject.name = faceAttachmentNames[index]
    newObject.position = [attachmentPos.x, attachmentPos.y, attachmentPos.z]

    // Adding in face attachment component.
    const id = uuid()
    const name = 'face-attachment'
    newObject.components = {
      [id]: {
        id,
        name,
        parameters: {point: faceAttachmentNames[index]},
      },
    }

    createAndSelectObject(newObject, ctx, stateCtx)
    ctx.updateObject(object.id, o => ({
      ...o,
      face: {...o.face, addAttachmentState: false},
    }))
  }

  return (
    <Sphere
      args={hovered ? [0.03, 10, 10] : [0.02, 10, 10]}
      onClick={onClick}
      onPointerEnter={() => setHovered(true)}
      onPointerLeave={() => setHovered(false)}
      position={position}
    >
      <meshBasicMaterial color={hovered ? '#D7D7D7' : '#FFFF00'} />
    </Sphere>
  )
}

interface IFaceHelper {
  childrenIds: string[]
  showAttachmentState: boolean
  object: DeepReadonly<GraphObject>
  isEarEnabled: boolean
}

const FaceHelper: React.FC<IFaceHelper> = ({
  childrenIds, showAttachmentState, object, isEarEnabled,
}) => {
  const derivedScene = useDerivedScene()
  // From the face attachment entities to generate the point list
  const ptNames: string[] = []

  if (showAttachmentState) {
    // Filters out the ears from display if ears are disabled.
    faceAttachmentNames.filter(name => (isEarEnabled || !earAttachmentNames.includes(name)))
      .forEach((name) => {
        ptNames.push(name)
      })
  } else {
    // Shows face attachment points that are currently attached to the face.
    childrenIds.forEach((id) => {
      const childObject = derivedScene.getObject(id)
      Object.values(childObject.components).forEach((component) => {
        if (isFaceAttachmentPoint(component)) {
          ptNames.push(component.parameters.point)
        }
      })
    })
  }

  const pointArray = new Float32Array(3 * ptNames.length)
  for (let i = 0; i < ptNames.length; i++) {
    const {x, y, z} = faceAttachmentPointPosition(ptNames[i])
    pointArray[3 * i] = x
    pointArray[3 * i + 1] = y
    pointArray[3 * i + 2] = z
  }

  return (
    <group>
      {showAttachmentState &&
        ptNames.map((name, i) => (
          <FacePoint
            key={name}
            index={i}
            position={faceAttachmentPoints[i]}
            object={object}
          />
        ))
      }
      {!showAttachmentState && (pointArray.length > 0) &&
        <points>
          <bufferGeometry>
            <bufferAttribute
              attach='attributes-position'
              array={pointArray}
              count={pointArray.length / 3}
              itemSize={3}
            />
          </bufferGeometry>
          <pointsMaterial
            size={0.06}
            color='#FFFF00'
          />
        </points>
      }
      <mesh>
        <FaceMeshGeometry hasFace hasEyes hasIris hasMouth includeUv={false} />
        <meshBasicMaterial color='#00BEDC' wireframe />
      </mesh>
      {isEarEnabled &&
        <mesh>
          {makeEarLMeshGeometry()}
          <meshBasicMaterial color='#00BEDC' wireframe />
        </mesh>
      }
      {isEarEnabled &&
        <mesh>
          {makeEarRMeshGeometry()}
          <meshBasicMaterial color='#00BEDC' wireframe />
        </mesh>
      }
    </group>
  )
}

const sides = {
  'front': FrontSide,
  'back': BackSide,
  'double': DoubleSide,
}

const MakeMeshBasicMaterial: React.FC<{material: DeepReadonly<BasicMaterial>}> = ({material}) => {
  const texture = useTextureWithSettings(material, material?.textureSrc)
  const roughnessMap = useTextureWithSettings(material, material?.roughnessMap)
  const metalnessMap = useTextureWithSettings(material, material?.metalnessMap)
  const opacityMap = useTextureWithSettings(material, material?.opacityMap)
  const normalMap = useTextureWithSettings(material, material?.normalMap)
  const emissiveMap = useTextureWithSettings(material, material?.emissiveMap)

  const normalScale = material.normalScale ?? MATERIAL_DEFAULTS.normalScale
  const opacity = material.opacity ?? MATERIAL_DEFAULTS.opacity

  return (
    <meshPhysicalMaterial
      color={material.color}
      side={sides[material.side ?? 'front']}
      transparent={opacity < 1 || !!material.opacityMap || !!material.forceTransparent}
      metalness={material.metalness ?? MATERIAL_DEFAULTS.metalness}
      roughness={material.roughness ?? MATERIAL_DEFAULTS.roughness}
      opacity={opacity}
      map={texture}
      roughnessMap={roughnessMap}
      metalnessMap={metalnessMap}
      alphaMap={opacityMap}
      normalMap={normalMap}
      emissiveMap={emissiveMap}
      normalScale={new Vector2(normalScale, normalScale)}
      emissiveIntensity={material.emissiveIntensity ?? MATERIAL_DEFAULTS.emissiveIntensity}
      emissive={material.emissiveColor ?? DEFAULT_EMISSIVE_COLOR}
      blending={MATERIAL_BLENDING[material.blending] ?? MATERIAL_BLENDING.normal}
      depthTest={material.depthTest ?? MATERIAL_DEFAULTS.depthTest}
      depthWrite={material.depthWrite ?? MATERIAL_DEFAULTS.depthWrite}
      wireframe={material.wireframe ?? MATERIAL_DEFAULTS.wireframe}
    />
  )
}

const MakeMeshUnlitMaterial: React.FC<{material: DeepReadonly<UnlitMaterial>}> = ({material}) => {
  const texture = useTextureWithSettings(material, material?.textureSrc)
  const opacityMap = useTextureWithSettings(material, material?.opacityMap)
  const opacity = material.opacity ?? MATERIAL_DEFAULTS.opacity

  return (
    <meshBasicMaterial
      color={material.color}
      side={sides[material.side ?? 'front']}
      transparent={opacity < 1 || !!material.opacityMap || !!material.forceTransparent}
      opacity={opacity}
      map={texture}
      alphaMap={opacityMap}
      blending={MATERIAL_BLENDING[material.blending] ?? MATERIAL_BLENDING.normal}
      depthTest={material.depthTest ?? MATERIAL_DEFAULTS.depthTest}
      depthWrite={material.depthWrite ?? MATERIAL_DEFAULTS.depthWrite}
      wireframe={material.wireframe ?? MATERIAL_DEFAULTS.wireframe}
    />
  )
}

const MakeMeshShadowMaterial: React.FC<{material: DeepReadonly<ShadowMaterial>}> = ({material}) => (
  <shadowMaterial
    color={material.color}
    side={sides[material.side ?? 'front']}
    opacity={material.opacity ?? MATERIAL_DEFAULTS.opacity}
    transparent
    depthTest={material.depthTest ?? MATERIAL_DEFAULTS.depthTest}
    depthWrite={material.depthWrite ?? MATERIAL_DEFAULTS.depthWrite}
  />
)

const MeshMaterial: React.FC<{material: DeepReadonly<Material>}> = ({material}) => {
  if (!material) {
    // NOTE(christoph): It seems like different versions of THREE behave differently,
    // where passing undefined gives you a default material.
    return <meshBasicMaterial visible={false} />
  }

  switch (material.type) {
    case 'basic':
      return <MakeMeshBasicMaterial material={material} />
    case 'unlit':
      return <MakeMeshUnlitMaterial material={material} />
    case 'shadow':
      return <MakeMeshShadowMaterial material={material} />
    case 'hider':
      return <MakeMeshHiderMaterial />
    case 'video':
      return null  // NOTE(chloe): Videos not needed since can be added as textures for primitives
    default:
      // eslint-disable-next-line no-console
      console.error('Unknown Material', material)
      return null
  }
}

const forceUpdateTextures = (m: MeshStandardMaterial) => {
  const forceUpdateTexture = (tex: Texture) => {
    if (tex) {
      tex.needsUpdate = true
    }
  }

  forceUpdateTexture(m.map)
  forceUpdateTexture(m.roughnessMap)
  forceUpdateTexture(m.metalnessMap)
  forceUpdateTexture(m.alphaMap)
  forceUpdateTexture(m.normalMap)
  forceUpdateTexture(m.emissiveMap)
}

type EntityProps = {
  id: string
  lightsDisabled?: boolean
  outlined?: boolean
  showHelpers?: boolean
}

const Entity: React.FC<EntityProps> = ({id, lightsDisabled, outlined, showHelpers = true}) => {
  const {rootIds} = useYogaParentContext()
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const activeSpace = useActiveSpace()
  const cameraObj = derivedScene.getActiveCamera(activeSpace?.id)
  const camera = cameraObj ? cameraObj.camera : null
  const isFaceCamera = camera?.xr?.xrCameraType === 'face'
  const faceConfig = camera?.xr?.face ?? null
  const enableEars = (isFaceCamera && faceConfig) ? faceConfig.enableEars : false
  let hasFace = false
  let hasEyes = false
  let hasIris = false
  let hasMouth = false
  if (isFaceCamera && faceConfig) {
    hasFace = faceConfig.meshGeometryFace ?? false
    hasEyes = faceConfig.meshGeometryEyes ?? false
    hasIris = faceConfig.meshGeometryIris ?? false
    hasMouth = faceConfig.meshGeometryMouth ?? false
  }

  const object = derivedScene.getObject(id)
  const meshRef = React.useRef<Mesh>(null)
  const selection = useObjectSelection(id)
  const isRootUi = rootIds.has(id)
  const {state: {shadingMode}} = stateCtx

  const [gltfVertices, setGltfVertices] = React.useState<number[]>([])

  useEffect(() => {
    if (!object.gltfModel?.src) {
      setGltfVertices([])
    }
  }, [object.gltfModel?.src])

  const {isSelected} = selection

  // NOTE(chloe): When determining if entity `needsOutline`, we internally check if object is
  // selected because `SceneObjects` iterates over top-level entities, so children entities cannot
  // receive an 'outlined' prop there directly. 'outlined' is then passed down to children entities
  // via the selected `Entity` components.
  const needsOutline = showHelpers && (outlined || isSelected)

  // TODO(chloe): Set up BoxHelper only for maptiles and match color to new outlines
  // // @ts-expect-error
  // useHelper(isSelected && !object.splat && meshRef, BoxHelper, mango)

  const meshRefHandler = React.useCallback((mesh: Mesh) => {
    if (meshRef.current) {
      removeOutline(meshRef.current)
    }
    meshRef.current = mesh

    if (mesh && needsOutline) {
      enableOutline(mesh)
    }
  }, [needsOutline])

  // Notify target material whenever its properties change
  useEffect(() => {
    const material = meshRef.current?.material
    if (material instanceof Array) {
      material.forEach((m) => {
        m.needsUpdate = true
      })
    } else if (material) {
      material.needsUpdate = true
    }
  }, [object.material])

  const matType = object.material?.type
  const basicMaterial = matType === 'basic' || matType === 'unlit' ? object.material : undefined

  // Notify material textures whenever offset or repeat changes
  // (wrapS and wrapT don't update automatically)
  useEffect(() => {
    const material = meshRef.current?.material
    if (material instanceof Array) {
      material.forEach((m) => {
        forceUpdateTextures(m as MeshStandardMaterial)
      })
    } else if (material) {
      forceUpdateTextures(material as MeshStandardMaterial)
    }
  }, [
    basicMaterial?.offsetX, basicMaterial?.offsetY,
    basicMaterial?.repeatX, basicMaterial?.repeatY, basicMaterial?.wrap,
    basicMaterial?.textureFiltering,
    basicMaterial?.mipmaps,
  ])

  if (!object || object.hidden || object.disabled) {
    return null
  }

  const children = derivedScene.getChildren(id)

  const gltfModel = object.gltfModel as Gltf
  const getAutoGeometry = () => {
    if (object.gltfModel?.src) {
      return gltfVertices
    }
    if (!SUPPORTED_PRIMITIVE_COLLIDER_GEOMETRIES.includes(object.geometry?.type)) {
      return null
    }
    return object.geometry
  }

  const colliderGeometry = object.collider?.geometry?.type === 'auto'
    ? getAutoGeometry()
    : object.collider?.geometry

  // Disable face attachment when not selected.
  if (object.face && object.face.addAttachmentState && !isSelected) {
    ctx.updateObject(object.id, o => ({
      ...o,
      face: {...o.face, addAttachmentState: false},
    }))
  }

  return (
    <mesh
      ref={meshRefHandler}
      castShadow={object.shadow?.castShadow}
      receiveShadow={object.shadow?.receiveShadow}
      name={id}
      position={object.position}
      quaternion={object.rotation as any}
      scale={object.scale}
      // renderOrder is 0 by default. For now, we always want to render this material first
      // TODO: Make renderOrder configurable with layers
      renderOrder={object.material?.type === 'hider' ? -1 : 0}
      onClick={(e: ThreeEvent<PointerEvent>) => {
        if (e.altKey || ctx.isDraggingGizmo) {
          return
        }
        e.stopPropagation()

        selection.onClick(e.ctrlKey || e.metaKey || e.shiftKey)
      }}
    >
      {showHelpers &&
        (object.camera || object.light || object.audio || object.splat || object.location) &&
          <ViewportIcon object={object} scene={ctx.scene} />}
      {object.geometry?.type !== 'face' &&
        makeGeometry(object.geometry)
      }
      {showHelpers && object.geometry?.type === 'face' &&
        <FaceMeshGeometry
          hasFace={hasFace}
          hasEyes={hasEyes}
          hasIris={hasIris}
          hasMouth={hasMouth}
          includeUv
        />
      }
      {showHelpers && object.imageTarget &&
        <ImageTargetMesh
          name={object.imageTarget.name}
          outlined={needsOutline}
          objectId={object.id}
        />
      }
      <MeshMaterial material={object.material} />
      {object.light && shadingMode === 'shaded' &&
        <Light
          baseLightConfig={object.light}
          showHelpers={isSelected && showHelpers}
          disableLight={lightsDisabled}
        />}
      {gltfModel?.src &&
        <mesh>
          <GltfModel
            model={gltfModel}
            shadow={object.shadow}
            setVertices={Array.isArray(colliderGeometry) ? setGltfVertices : undefined}
            materialOverride={matType === 'hider' || matType === 'video'
              ? object.material
              : undefined}
            outlined={needsOutline}
          />
        </mesh>
      }
      {object.splat &&
        <Splat src={object.splat.src} skybox={!!object.splat.skybox} />
      }
      {object.camera && showHelpers &&
        <Camera baseCamera={object.camera} isSelected={isSelected} />
      }
      {showHelpers && object.face &&
        <FaceHelper
          childrenIds={children}
          showAttachmentState={object.face.addAttachmentState}
          object={object}
          isEarEnabled={enableEars}
        />
      }
      {showHelpers && colliderGeometry && isSelected &&
        <mesh
          position={[
            object.collider?.offsetX ?? DEFAULT_OFFSET,
            object.collider?.offsetY ?? DEFAULT_OFFSET,
            object.collider?.offsetZ ?? DEFAULT_OFFSET,
          ]}
          quaternion={[
            object.collider?.offsetQuaternionX ?? DEFAULT_OFFSET_QUATERNION_X,
            object.collider?.offsetQuaternionY ?? DEFAULT_OFFSET_QUATERNION_Y,
            object.collider?.offsetQuaternionZ ?? DEFAULT_OFFSET_QUATERNION_Z,
            object.collider?.offsetQuaternionW ?? DEFAULT_OFFSET_QUATERNION_W,
          ]}
        >
          {makeGeometry(colliderGeometry, object.collider.simplificationMode)}
          {!Array.isArray(colliderGeometry) && <meshBasicMaterial color='red' wireframe />}
        </mesh>
      }

      {isRootUi && (showHelpers || object.ui?.type === '3d') &&
        <UiRoot id={id} outlined={needsOutline} type={object.ui?.type} />
      }

      {children.map(e => (
        <Entity
          key={e}
          id={e}
          lightsDisabled={lightsDisabled}
          outlined={needsOutline}
          showHelpers={showHelpers}
        />
      ))}
    </mesh>
  )
}

export {
  Entity,
}
