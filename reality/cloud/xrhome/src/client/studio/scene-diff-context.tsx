import React from 'react'

import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph} from '@ecs/shared/scene-graph'

import type {ChangeLog} from './get-change-log'

interface DiffContext {
  changeLog: ChangeLog
  beforeScene: DeepReadonly<SceneGraph>
  afterScene: DeepReadonly<SceneGraph>
}

type SceneDiffContext = DeepReadonly<DiffContext>

const sceneDiffContext = React.createContext<SceneDiffContext | null>(null)

const useSceneDiffContext = (): SceneDiffContext => {
  const ctx = React.useContext(sceneDiffContext)
  return ctx
}

const SceneDiffContextProvider = sceneDiffContext.Provider

interface ISceneDiffContext {
  diffContext: SceneDiffContext
  children: React.ReactNode
}

const SceneDiffInfoContext: React.FC<ISceneDiffContext> = ({diffContext, children}) => (
  <SceneDiffContextProvider value={diffContext}>
    {children}
  </SceneDiffContextProvider>
)

export {
  SceneDiffInfoContext,
  useSceneDiffContext,
}

export type {
  SceneDiffContext,
}
