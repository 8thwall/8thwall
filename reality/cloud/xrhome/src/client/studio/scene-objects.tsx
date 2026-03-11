import React from 'react'
import ThreeMeshUI from '@ecs/runtime/8mesh'
import {useFrame} from '@react-three/fiber'
import {useThree} from '@react-three/fiber'
import {getReversePainterSortWithUi} from '@ecs/shared/custom-sorting'

import {Entity} from './entity'
import {useActiveSpace} from './hooks/active-space'
import {useActiveRoot} from '../hooks/use-active-root'
import {prefabContainsLight} from './configuration/prefab'
import {DEFAULT_DIRECTIONAL_LIGHT_POSITION} from './make-object'
import {useDerivedScene} from './derived-scene-context'

interface IStandalonePrefab {
  rootEntityId: string
  showHelpers?: boolean
}

const StandalonePrefab: React.FC<IStandalonePrefab> = ({rootEntityId, showHelpers}) => {
  const derivedScene = useDerivedScene()
  const containsLight = prefabContainsLight(derivedScene, rootEntityId)
  return (
    <>
      <Entity id={rootEntityId} showHelpers={showHelpers} />
      {!containsLight &&
        <>
          <directionalLight
            position={DEFAULT_DIRECTIONAL_LIGHT_POSITION}
            intensity={Math.PI}
          />
          <ambientLight intensity={1} />
        </>
      }
    </>
  )
}

interface ISpaceObjects {
  showHelpers?: boolean
}

const SpaceObjects: React.FC<ISpaceObjects> = ({showHelpers}) => {
  const space = useActiveSpace()
  const derivedScene = useDerivedScene()
  const spacesToRender = derivedScene.getAllIncludedSpaces(space?.id)

  return (
    // eslint-disable-next-line react/jsx-no-useless-fragment
    <>
      {spacesToRender.length
        ? spacesToRender
          .map(id => derivedScene.getTopLevelObjectIds(id).map(e => (
            <Entity key={e} id={e} showHelpers={showHelpers} />
          )))
        : derivedScene.getTopLevelObjectIds(undefined).map(e => (
          <Entity key={e} id={e} showHelpers={showHelpers} />
        ))}
    </>
  )
}
interface ISceneObjects {
  showHelpers?: boolean
}
const SceneObjects: React.FC<ISceneObjects> = ({showHelpers}) => {
  const activeRoot = useActiveRoot()
  const {gl, scene} = useThree()

  // We are rendering UI components in the viewport using ThreeMeshUI, which
  // requires a manual update call on every frame. This update manager will only
  // call updates on objects that have changed.
  useFrame(() => {
    ThreeMeshUI.update()
  })

  React.useEffect(() => {
    gl.setTransparentSort(getReversePainterSortWithUi(gl, scene))
  }, [gl, scene])

  return (
    activeRoot.prefab
      ? <StandalonePrefab rootEntityId={activeRoot.id} showHelpers={showHelpers} />
      : <SpaceObjects showHelpers={showHelpers} />
  )
}

export {
  SceneObjects,
  StandalonePrefab,
}
