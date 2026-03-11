import {PanelSelection, StudioStateContext, useStudioStateContext} from '../studio-state-context'
import {useActiveSpace} from './active-space'
import {useDerivedScene} from '../derived-scene-context'
import type {DerivedScene} from '../derive-scene'

const useSelectedObjects = () => {
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const activeSpace = useActiveSpace()
  const spacesToShow = derivedScene.getAllIncludedSpaces(activeSpace?.id)
  return stateCtx.state.selectedIds.map(id => derivedScene.getObject(id))
    .filter((object) => {
      if (!object) {
        return false
      }
      const space = derivedScene.resolveSpaceForObject(object.id)
      if (activeSpace) {
        return spacesToShow?.includes(space?.id) || !space
      } else {
        return !space
      }
    })
}

const getObjectSelection = (
  id: string, stateCtx: StudioStateContext, derivedScene: DerivedScene, activeSpaceId?: string
) => {
  const isSelected = stateCtx.state.selectedIds.includes(id)
  const {selectedIds} = stateCtx.state

  return {
    isSelected,
    onClick: (multiSelect?: boolean, bulkSelect?: boolean) => {
      const spacesToShow = derivedScene.getAllIncludedSpaces(activeSpaceId)
      const objectIds: string[] = activeSpaceId ? [] : derivedScene.deepSortObjects(activeSpaceId)
      spacesToShow.forEach((spaceId) => {
        objectIds.push(...derivedScene.deepSortObjects(spaceId))
      })
      if (bulkSelect) {
        const currentIndex = objectIds.indexOf(id)
        const lastSelectedIndex = objectIds.indexOf(selectedIds.at(-1))
        if (lastSelectedIndex === -1) {
          stateCtx.setSelection(id)
        } else {
          const start = Math.min(currentIndex, lastSelectedIndex)
          const end = Math.max(currentIndex, lastSelectedIndex)
          const range = objectIds.slice(start, end + 1)

          if (isSelected) {
            const newSelection = selectedIds.filter(selectedId => !range.includes(selectedId))
            stateCtx.setSelection(...newSelection)
          } else {
            stateCtx.setSelection(...selectedIds, ...range)
          }
        }
      } else if (multiSelect) {
        if (isSelected) {
          stateCtx.removeFromSelection(id)
        } else {
          stateCtx.addToSelection(id)
        }
      } else {
        stateCtx.setSelection(id)
      }
      if (stateCtx.state.currentPanelSection !== PanelSelection.INSPECTOR) {
        stateCtx.update(p => ({...p, currentPanelSection: PanelSelection.INSPECTOR}))
      }
    },
  }
}

const useObjectSelection = (id: string) => {
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const activeSpaceId = useActiveSpace()?.id
  return getObjectSelection(id, stateCtx, derivedScene, activeSpaceId)
}

// Represents the target of actions like copy/paste
const useSelectedTarget = () => {
  const stateCtx = useStudioStateContext()
  return stateCtx.state.selectedIds.at(-1)
}

const selectAllObjects = (
  derivedScene: DerivedScene, stateCtx: StudioStateContext, spaces: string[], prefab?: string
) => {
  if (prefab) {
    const topLevelObjectIds = derivedScene.deepSortObjects(prefab)
    stateCtx.setSelection(...topLevelObjectIds, prefab)
  } else {
    const ids: string[] = []
    spaces.forEach((id) => {
      ids.push(...derivedScene.deepSortObjects(id))
    })
    stateCtx.setSelection(...ids)
  }
}

export {
  useSelectedObjects,
  getObjectSelection,
  useObjectSelection,
  useSelectedTarget,
  selectAllObjects,
}
