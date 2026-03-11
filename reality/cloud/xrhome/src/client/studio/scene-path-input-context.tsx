import React from 'react'

import {ScenePath, useScenePathContext, ScenePathContextProvider} from './scene-path-context'

interface IScenePathRootContext {
  children: React.ReactNode
  path: ScenePath
}

interface IScenePathScopeContext {
  children: React.ReactNode
  // TODO(Carson): Think about better ways to type this
  path: string[]
}

// This root ensures all subsequent paths map to nothing.
const DISCARD_ROOT_PATH = ['NULL'] as unknown as ScenePath

const ScenePathRootProvider: React.FC<IScenePathRootContext> = ({children, path}) => (
  <ScenePathContextProvider value={path}>
    {children}
  </ScenePathContextProvider>
)

const ScenePathScopeProvider: React.FC<IScenePathScopeContext> = ({children, path}) => {
  const currentPath = useScenePathContext()
  const newPath: string[] = [...currentPath, ...path]

  return (
    <ScenePathContextProvider value={newPath as ScenePath}>
      {children}
    </ScenePathContextProvider>
  )
}

export {
  ScenePathRootProvider,
  ScenePathScopeProvider,
  DISCARD_ROOT_PATH,
}
