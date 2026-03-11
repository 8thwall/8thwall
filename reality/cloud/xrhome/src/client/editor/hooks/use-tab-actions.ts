import type {EditorFileLocation} from '../editor-file-location'
import type {PersistentEditorSession} from './use-persistent-editor-session'
import * as EditorTabState from '../tab-state'
import {useEvent} from '../../hooks/use-event'

type SwitchTabOptions = {
  line?: number
  column?: number
  persistent?: boolean
}

type SwitchTab = (
  newLocation: EditorFileLocation, options?: SwitchTabOptions, paneId?: number
) => void

const useTabActions = ({
  setTabState, setScrollTo, currentTabbedPaneId,
}: PersistentEditorSession) => {
  const closeTabById = (tabId: number) => {
    setTabState(ts => EditorTabState.handleTabClose(ts, tabId))
  }

  const closeTabsInPane = (paneId: number) => {
    setTabState(ts => EditorTabState.closeTabsInPane(ts, paneId))
  }

  const closeOtherTabsInPane = (tabId: number) => {
    setTabState((ts) => {
      const paneId = ts.tabs.find(t => t.id === tabId)?.paneId
      const newTabState = EditorTabState.closeOtherTabsInPane(ts, tabId, paneId)
      return newTabState
    })
  }

  const closeTabsInDirection = (tabId: number, direction: 'left' | 'right') => {
    setTabState((ts) => {
      const paneId = ts.tabs.find(t => t.id === tabId)?.paneId
      const newTabState = EditorTabState.closeTabsInDirection(ts, tabId, paneId, direction)
      return newTabState
    })
  }

  const closeTab = (loc: EditorFileLocation) => {
    setTabState(ts => EditorTabState.handleFileDelete(ts, loc))
  }

  const switchTab: SwitchTab = useEvent((newLocation, options?, paneId?) => {
    const selectedPane = paneId || currentTabbedPaneId
    setTabState(ts => EditorTabState.handleFileSelect(
      ts, newLocation, selectedPane, options?.persistent
    ))
    setScrollTo({line: options?.line, column: options?.column})
  })

  // Act as closing and opening a new tab(s) but at the same tab location
  const updateTab = (oldLocation: EditorFileLocation, newLocation: EditorFileLocation) => {
    setTabState(ts => EditorTabState.handleFileRename(ts, oldLocation, newLocation))
  }

  const updateTabState = (tabId: number, customState: EditorTabState.CustomState) => {
    setTabState(ts => EditorTabState.handleStateChange(ts, tabId, customState))
  }

  const handleSwitchTabs = (e: KeyboardEvent, direction: 'left' | 'right') => {
    e.preventDefault()
    setTabState((ts) => {
      const tab = EditorTabState.getTopTabInStack(ts)
      const newTab = EditorTabState.getAdjacentTab(ts, tab.id, tab.paneId, direction)
      return EditorTabState.handleTabSelect(ts, newTab.id)
    })
  }

  return {
    closeTabById,
    closeTabsInPane,
    closeOtherTabsInPane,
    closeTabsInDirection,
    closeTab,
    switchTab,
    updateTab,
    updateTabState,
    handleSwitchTabs,
  }
}

export type {
  SwitchTab,
}

export {
  useTabActions,
}
