import React from 'react'
import {v4 as uuid} from 'uuid'
import type {DeepReadonly} from 'ts-essentials'
import type {SceneGraph} from '@ecs/shared/scene-graph'
import {createUseStyles} from 'react-jss'

import {DropTarget} from '../editor/drop-target'
import {StandardTextAreaField} from '../ui/components/standard-text-area-field'

import {EXAMPLE_SCENES} from './example/scenes/scene-list'
import {useStudioStateContext} from './studio-state-context'
import {useActiveSpace} from './hooks/active-space'
import {deleteObjects} from './configuration/delete-object'
import {StandardModal} from '../ui/components/standard-modal'
import {useSceneContext} from './scene-context'
import {TertiaryButton} from '../ui/components/tertiary-button'
import {SpaceBetween} from '../ui/layout/space-between'
import {StandardRadioButton} from '../ui/components/standard-radio-group'
import {FloatingPanelButton} from '../ui/components/floating-panel-button'
import {deriveScene} from './derive-scene'

// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

const useStyles = createUseStyles({
  modalInner: {
    padding: '2rem',
  },
  textArea: {
    width: '100%',
    height: '500px',
    maxHeight: '50vh',
  },
})

const parseScene = (json: string): DeepReadonly<SceneGraph> | null => {
  try {
    return JSON.parse(json)
  } catch (err) {
    return null
  }
}

interface ISceneJsonEditor {
  currentScene?: DeepReadonly<SceneGraph> | null
  onSceneChange: (scene: DeepReadonly<SceneGraph>) => void
  collapse?: () => void
}

const SceneJsonEditor: React.FC<ISceneJsonEditor> = ({
  currentScene, onSceneChange, collapse,
}) => {
  const stateCtx = useStudioStateContext()
  const classes = useStyles()
  const activeSpace = useActiveSpace()

  const [expectedScene, setExpectedScene] = React.useState<DeepReadonly<SceneGraph> | null>(null)
  const [json, setJson] = React.useState('')
  const [shouldAddToSpace, setShouldAddToSpace] = React.useState(true)

  const visibleJson = expectedScene === currentScene
    ? json
    : JSON.stringify(currentScene, null, 2)

  const getObjectsForSpace = (exampleScene: DeepReadonly<SceneGraph>, spaceId: string) => {
    const oldToNewIdMap = new Map<string, string>()
    const newObjects = Object.values(exampleScene.objects).map((object) => {
      const newId = uuid()
      oldToNewIdMap.set(object.id, newId)
      return {
        ...object,
        id: newId,
      }
    })
    newObjects.forEach((object) => {
      object.parentId = oldToNewIdMap.get(object.parentId ?? '') ?? spaceId
    })
    return newObjects.reduce((acc, obj) => {
      acc[obj.id] = obj
      return acc
    }, {})
  }

  const displayError = (text: string) => {
    stateCtx.update(p => ({...p, errorMsg: text}))
    collapse?.()
  }

  const handleSceneChange = (sceneName: string) => {
    const matchedScene = EXAMPLE_SCENES.find(scene => scene.name === sceneName)
    if (!matchedScene) return
    if (!shouldAddToSpace || !activeSpace) {
      onSceneChange(matchedScene.scene)
    } else {
      const derivedScene = deriveScene(currentScene)
      const oldObjects = derivedScene.getChildren(activeSpace.id)
      const oldScene = deleteObjects(currentScene, oldObjects)
      const newObjects = getObjectsForSpace(matchedScene.scene, activeSpace.id)
      const newScene = {
        ...oldScene,
        objects: {
          ...oldScene.objects,
          ...newObjects,
        },
      }
      onSceneChange(newScene)
    }
    collapse?.()
  }

  const handleInput = (value: string) => {
    setJson(value)
    const parsed = parseScene(value)
    if (parsed) {
      setExpectedScene(parsed)
      onSceneChange(parsed)
    } else {
      setExpectedScene(currentScene)
    }
  }

  const textarea = (
    <StandardTextAreaField
      id='scene-json'
      // eslint-disable-next-line local-rules/ui-component-styling
      className={classes.textArea}
      boldLabel
      label='Edit'
      value={visibleJson}
      onChange={(e) => {
        handleInput(e.target.value)
      }}
      onBlur={() => setExpectedScene(null)}
    />
  )

  return (
    <DropTarget
      onDrop={async (e: React.DragEvent) => {
        const file = e.dataTransfer.files[0]
        if (!file) {
          displayError('Missing file.')
          return
        }

        try {
          const text = await file.text()
          const parsedScene = parseScene(text)
          if (parsedScene) {
            onSceneChange(parsedScene)
          } else {
            displayError('Invalid json.')
          }
        } catch (err) {
          displayError('Failed to read file.')
        }
      }}
    >
      <SpaceBetween direction='vertical'>
        <SpaceBetween direction='horizontal' narrow centered>
          Reset to...{' '}
          {EXAMPLE_SCENES.map(scene => (
            <TertiaryButton
              key={scene.name}
              height='small'
              onClick={() => handleSceneChange(scene.name)}
            >
              {scene.name}
            </TertiaryButton>
          ))}
        </SpaceBetween>

        <SpaceBetween direction='horizontal'>
          <StandardRadioButton
            id='add-to-space'
            label='Replace current space'
            checked={shouldAddToSpace}
            onChange={e => setShouldAddToSpace(e.target.checked)}
          />
          <StandardRadioButton
            id='reset-all'
            label='Reset everything'
            checked={!shouldAddToSpace}
            onChange={e => setShouldAddToSpace(!e.target.checked)}
          />
        </SpaceBetween>

        {textarea}

        <SpaceBetween direction='horizontal' narrow>
          <TertiaryButton
            height='small'
            onClick={() => navigator.clipboard.writeText(visibleJson)}
          >
            Copy
          </TertiaryButton>
          <TertiaryButton
            height='small'
            onClick={async () => {
              const text = await navigator.clipboard.readText()
              handleInput(text)
            }}
          >
            Paste
          </TertiaryButton>
        </SpaceBetween>
      </SpaceBetween>
    </DropTarget>
  )
}

const StudioSceneGraphView: React.FC = () => {
  const classes = useStyles()
  const ctx = useSceneContext()
  return (
    <StandardModal
      trigger={(
        <FloatingPanelButton onClick={null}>
          Scene JSON
        </FloatingPanelButton>
      )}
    >
      {collapse => (
        <div className={classes.modalInner}>
          <SceneJsonEditor
            currentScene={ctx.scene}
            onSceneChange={newScene => ctx.updateScene(() => newScene)}
            collapse={collapse}
          />
        </div>
      )}
    </StandardModal>
  )
}

export {
  SceneJsonEditor,
  StudioSceneGraphView,
}
