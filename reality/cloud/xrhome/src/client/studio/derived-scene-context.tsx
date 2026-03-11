import React from 'react'

import {useSceneContext} from './scene-context'
import {DerivedScene, deriveScene} from './derive-scene'

const DerivedSceneContext = React.createContext<DerivedScene | null>(null)

interface IDerivedSceneContext {
  children: React.ReactNode
}

const DerivedSceneProvider: React.FC<IDerivedSceneContext> = ({children}) => {
  const {scene} = useSceneContext()
  const derivedScene = React.useMemo(() => deriveScene(scene), [scene])
  return (
    <DerivedSceneContext.Provider value={derivedScene}>
      {children}
    </DerivedSceneContext.Provider>
  )
}

const useDerivedScene = (): DerivedScene => {
  const ctx = React.useContext(DerivedSceneContext)
  if (!ctx) throw new Error('useDerivedScene must be used within a <DerivedSceneProvider>')
  return ctx
}

export {DerivedSceneProvider, useDerivedScene}
