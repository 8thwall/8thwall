/* eslint react-hooks/exhaustive-deps: error */
import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {Expanse, SceneGraph} from '@ecs/shared/scene-graph'

import {useCurrentGit, useGitFileContent} from '../../git/hooks/use-current-git'
import useActions from '../../common/use-actions'
import coreGitActions from '../../git/core-git-actions'
import type {MutateCallback} from '../scene-context'
import {EXPANSE_FILE_PATH} from '../common/studio-files'
import {defaultScene} from '../example/scenes/default-scene'
import {useEvent} from '../../hooks/use-event'

const DEBOUNCE_TIME = Build8.PLATFORM_TARGET === 'desktop' ? 500 : 1500

type ExpanseHandle = {
  ready: boolean
  scene: DeepReadonly<SceneGraph>
  update: (fn: (expanse: DeepReadonly<SceneGraph>) => DeepReadonly<SceneGraph>) => void
}

type InternalExpanseState = {
  loadedFor: string
  dirty: boolean
}

const parseScene = (expanseFile: string): DeepReadonly<SceneGraph> => {
  if (expanseFile) {
    try {
      const expanseData: Expanse = JSON.parse(expanseFile)
      delete expanseData.history
      delete expanseData.historyVersion
      return expanseData
    } catch {
      // eslint-disable-next-line no-console
      console.error('Invalid expanse file found, showing default scene')
      // Invalid expanse json file, fallback to default
      return defaultScene
    }
  } else {
    // eslint-disable-next-line no-console
    console.error('No expanse file found, showing default scene')
    return defaultScene
  }
}

const useExpanse = (): ExpanseHandle => {
  const expanseFile = useGitFileContent(EXPANSE_FILE_PATH)
  const {saveFiles} = useActions(coreGitActions)
  const [scene, setScene] = React.useState<DeepReadonly<SceneGraph>>(() => parseScene(expanseFile))
  const stateRef = React.useRef<InternalExpanseState | null>(null)
  const repo = useCurrentGit(git => git.repo)
  const saveTimeoutRef = React.useRef<ReturnType<typeof setTimeout>>()

  const ready = !!scene

  const maybeSaveState = useEvent(async () => {
    clearTimeout(saveTimeoutRef.current)
    const internalState = stateRef.current
    if (!internalState?.dirty) {
      return
    }

    const newExpanse: Expanse = {...scene as SceneGraph}

    const expanseData = JSON.stringify(newExpanse, null, 2)
    internalState.dirty = false
    internalState.loadedFor = expanseData
    await saveFiles(repo, [
      {filePath: EXPANSE_FILE_PATH, content: expanseData},
    ])
  })

  React.useEffect(() => () => {
    stateRef.current = null
    clearTimeout(saveTimeoutRef.current)
  }, [])

  React.useEffect(() => {
    const isExpectedChange = stateRef.current && (
      stateRef.current.loadedFor === expanseFile
    )

    if (isExpectedChange) {
      return
    }
    const sceneFromFile = parseScene(expanseFile)

    stateRef.current = {loadedFor: expanseFile, dirty: false}
    setScene(sceneFromFile)
  }, [expanseFile])

  const update = (fn: MutateCallback<SceneGraph>) => {
    if (!stateRef.current) {
      throw new Error('Not ready to edit expanse')
    }
    setScene(fn)
    stateRef.current.dirty = true
    clearTimeout(saveTimeoutRef.current)
    saveTimeoutRef.current = setTimeout(maybeSaveState, DEBOUNCE_TIME)
  }

  return {
    ready,
    scene,
    update,
  }
}

export {
  useExpanse,
}
