import {useState, useEffect, useRef} from 'react'
import type {DeepReadonly} from 'ts-essentials'

const MAX_UNDO_DEPTH = 300

const useUndoStack = <T>() => {
  const [undoStack, setUndoStack] = useState<DeepReadonly<T[]>>([])
  const [redoStack, setRedoStack] = useState<DeepReadonly<T[]>>([])
  const timeoutIdRef = useRef<ReturnType<typeof setTimeout>>(null)

  const addToUndo = (scene: DeepReadonly<T>) => {
    if (timeoutIdRef.current) {
      return
    }
    setUndoStack((oldStack) => {
      if (oldStack[oldStack.length - 1] === scene) {
        return oldStack
      }
      setRedoStack([])
      if (oldStack.length > MAX_UNDO_DEPTH) {
        return [...oldStack.slice(1), scene]
      }
      return [...oldStack, scene]
    })

    timeoutIdRef.current = setTimeout(() => {
      timeoutIdRef.current = null
    }, 500)
  }

  const undo = (currentScene: DeepReadonly<T>) => {
    if (!undoStack.length) {
      return null
    }
    const newStack = [...undoStack]
    const prevScene = newStack.pop()
    setUndoStack(newStack)
    setRedoStack([...redoStack, currentScene])
    return prevScene
  }

  const redo = (currentScene: DeepReadonly<T>) => {
    if (!redoStack.length) {
      return null
    }
    const newStack = [...redoStack]
    const nextScene = newStack.pop()
    setRedoStack(newStack)
    setUndoStack([...undoStack, currentScene])
    return nextScene
  }

  const canUndo = undoStack.length > 0
  const canRedo = redoStack.length > 0

  useEffect(() => () => clearTimeout(timeoutIdRef.current), [])

  return {undoStack, addToUndo, undo, redo, canUndo, canRedo}
}

export {
  useUndoStack,
}
