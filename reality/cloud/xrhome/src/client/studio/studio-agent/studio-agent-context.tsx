import React from 'react'
import type {DeepReadonly} from 'ts-essentials'

import type {Vec3Tuple} from '@ecs/shared/scene-graph'

import {useBooleanUrlState} from '../../hooks/url-state'
import type {MessageRequest} from './types'

type UndoStep = {
  // TODO(dat): Should we store function that operate on the scenegraph?
  //            Should we store something else?
  // Something like this is similar to scene-context. Perhaps store it there?
  // undo: () => void
  // redo: () => void
  requestId: string
  // Where to look at and see the objects getting changed?
  focalPoint: Readonly<Vec3Tuple>
  // Optional. Allows us to select these ids so the user can see the correct inspector value.
  affectedSceneIds?: string[]
}

// NOTE(dat): Consider an object type that allows a dag of undo steps instead
type RequestIdType = string
type UndoStack = {
  steps: Record<RequestIdType, UndoStep>
  stepOrder: RequestIdType[]
}

type StudioAgentState = DeepReadonly<{
  open: boolean
  undoStack: UndoStack
  playedMsgs: MessageRequest[]
}>

type StudioAgentStateUpdate = (updater: Partial<StudioAgentState>) => void

type StudioAgentStateContext = {
  state: StudioAgentState
  setState: StudioAgentStateUpdate
  pushUndoStep: (step: UndoStep) => void
  popUndoStep: () => UndoStep | undefined
  clearUndoStack: () => void
}

const studioAgentStateContext = React.createContext<StudioAgentStateContext>(
  {} as StudioAgentStateContext
)

const useStudioAgentStateContext = (): StudioAgentStateContext => React.useContext(
  studioAgentStateContext
)

const StudioAgentStateContextProvider: React.FC<{children: React.ReactNode}> = ({children}) => {
  const [open, setOpen] = useBooleanUrlState('studioAgentOpen', false)
  const [undoSteps, setUndoSteps] = React.useState<UndoStack['steps']>({})
  const [undoStepOrder, setUndoStepOrder] = React.useState<UndoStack['stepOrder']>([])
  const [playedMsgs, setPlayedMsgs] = React.useState<MessageRequest[]>([])
  const studioAgentState: StudioAgentState = {
    open,
    undoStack: {
      steps: undoSteps,
      stepOrder: undoStepOrder,
    },
    playedMsgs,
  }

  const ctx: StudioAgentStateContext = {
    state: studioAgentState,
    setState: (partial) => {
      Object.entries(partial).forEach(([key, value]) => {
        if (key === 'open') {
          setOpen(value as boolean)
        } else if (key === 'undoStack') {
          const valueUndoStack = value as UndoStack
          setUndoSteps(valueUndoStack.steps)
          setUndoStepOrder(valueUndoStack.stepOrder)
        } else if (key === 'playedMsgs') {
          setPlayedMsgs(value as MessageRequest[])
        }
        // default to doing nothing
      })
    },
    pushUndoStep: (step) => {
      setUndoSteps(old => ({...old, [step.requestId]: step}))
      setUndoStepOrder(old => [...old, step.requestId])
    },
    popUndoStep: () => {
      if (undoStepOrder.length === 0) {
        return undefined
      }
      const lastStepId = undoStepOrder[undoStepOrder.length - 1]
      const lastStep = undoSteps[lastStepId]
      setUndoSteps(old => (
        // old object without the lastStep key
        Object.fromEntries(
          Object.entries(old).filter(([key]) => key !== lastStepId)
        )
      ))
      setUndoStepOrder(old => old.slice(0, -1))
      return lastStep
    },
    clearUndoStack: () => {
      setUndoSteps({})
      setUndoStepOrder([])
    },
  }

  return (
    <studioAgentStateContext.Provider value={ctx}>
      {children}
    </studioAgentStateContext.Provider>
  )
}

export {
  StudioAgentStateContextProvider,
  useStudioAgentStateContext,
}

export type {
  StudioAgentState,
  StudioAgentStateUpdate,
  StudioAgentStateContext,
}
