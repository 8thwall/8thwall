import type {SceneGraph} from '@ecs/shared/scene-graph'
import React from 'react'

// eslint-disable-next-line max-len
// From: https://stackoverflow.com/questions/59455679/typescript-type-definition-for-an-object-property-path
// NOTE (Carson): Doesn't work for general concatenation, but nice for type checking explicit paths.

type Path<T> = T extends object
  ? {
    [K in keyof T]-?: K extends string
      ? [K] | (Path<T[K]> extends infer P ? P extends readonly string[] ? [K, ...P] : never : never)
      : never
  }[keyof T]
  : never

type ScenePath = Path<SceneGraph> | null

const scenePathContext = React.createContext<ScenePath>(null)

const ScenePathContextProvider = scenePathContext.Provider

const useScenePathContext = (): ScenePath => {
  const path = React.useContext<ScenePath>(scenePathContext)
  if (!path) {
    throw new Error('useScenePathContext must be used within a ScenePathContextProvider')
  }
  return path
}

export {
  ScenePathContextProvider,
  useScenePathContext,
  ScenePath,
}
