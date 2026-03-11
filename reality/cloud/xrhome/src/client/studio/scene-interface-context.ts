import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {Vec3Tuple} from '@ecs/shared/scene-graph'
import type {Matrix4} from 'three'

type SceneInterface = DeepReadonly<{
  focusObject: (id: string, transition?: boolean, zoomScale?: number) => void
  cameraLookAt: (focalPoint: Readonly<Vec3Tuple>) => void
  selectIds: (ids: Readonly<string[]>) => void
  getCameraTransform: () => Matrix4
}>

const NOOP = () => {}
const sceneInterfaceContext = React.createContext<SceneInterface>({
  focusObject: NOOP,
  cameraLookAt: NOOP,
  selectIds: NOOP,
  getCameraTransform: () => {
    throw new Error('getCameraTransform called on default scene context')
  },
})

const SceneInterfaceContextProvider = sceneInterfaceContext.Provider

export {
  sceneInterfaceContext,
  SceneInterfaceContextProvider,
}

export type {
  SceneInterface,
}
