import type {DeepReadonly} from 'ts-essentials'

import {
  EditorFileLocation, editorFileLocationEqual, extractScopedLocation, ScopedFileLocation,
} from './editor-file-location'
import {NEW_TAB_PATH} from './editor-utils'
import {matchLocation, replaceLocation} from './files/paths'

type Location = DeepReadonly<EditorFileLocation>

type CustomState = Record<string, any>

type EditorTab = ScopedFileLocation & {
  id: number
  ephemeral?: boolean
  stackIndex: number
  customState: CustomState
  paneId: number
}

type EditorTabs = DeepReadonly<{
  idCounter: number
  stackCounter: number
  tabs: EditorTab[]
}>

const handleFileRename = (
  state: EditorTabs, oldLocation: Location, newLocation: Location
): EditorTabs => ({
  ...state,
  tabs: state.tabs.map(t => replaceLocation(t, oldLocation, newLocation)),
})

const handleFileDelete = (state: EditorTabs, deletedLocation: Location): EditorTabs => ({
  ...state,
  tabs: state.tabs.filter(t => !matchLocation(t, deletedLocation)),
})

const handleFileSelect = (
  state: EditorTabs, location: Location, paneId: number, persistent?: boolean
): EditorTabs => {
  const isPlaceholderTab = editorFileLocationEqual(location, NEW_TAB_PATH)
  const newState = {
    ...state,
    tabs: isPlaceholderTab
      ? [...state.tabs]
      : state.tabs.filter(e => !editorFileLocationEqual(e, NEW_TAB_PATH)),
  }

  const existingTabIndex = newState.tabs.findIndex(t => editorFileLocationEqual(t, location) &&
  t.paneId === paneId)

  if (existingTabIndex === -1) {
    const ephemeralTabIndex = newState.tabs.findIndex(t => t.ephemeral)

    const newTab: EditorTab = {
      ...extractScopedLocation(location),
      id: newState.idCounter++,
      stackIndex: newState.stackCounter++,
      ephemeral: !persistent,
      customState: {},
      paneId,
    }

    if (ephemeralTabIndex === -1) {
      newState.tabs.push(newTab)
    } else if (isPlaceholderTab) {
      const updatedTab = {
        ...newState.tabs[ephemeralTabIndex],
      }
      delete updatedTab.ephemeral
      newState.tabs[ephemeralTabIndex] = updatedTab
      newState.tabs.push(newTab)
    } else {
      newState.tabs[ephemeralTabIndex] = newTab
    }
  } else {
    const currentStackIndex = newState.tabs[existingTabIndex].stackIndex
    const isTop = !newState.tabs.some(e => e.stackIndex > currentStackIndex)

    newState.tabs[existingTabIndex] = {
      ...newState.tabs[existingTabIndex],
      ephemeral: newState.tabs[existingTabIndex].ephemeral && (isPlaceholderTab || !isTop),
      stackIndex: newState.stackCounter++,
    }
  }

  return newState
}

const handleTabSelect = (state: EditorTabs, id: number): EditorTabs => {
  const newState = {...state, tabs: []}
  state.tabs.forEach((t) => {
    if (t.id === id) {
      newState.tabs.push({...t, stackIndex: newState.stackCounter++})
    } else if (!editorFileLocationEqual(t, NEW_TAB_PATH)) {
      newState.tabs.push(t)
    }
  })
  return newState
}

const handleReorder = (
  state: EditorTabs, oldIndex: number, newIndex: number, srcPaneId: number, targetPaneId: number
): EditorTabs => {
  const newState = {
    ...state,
    tabs: [...state.tabs.filter(t => t.paneId !== srcPaneId && t.paneId !== targetPaneId)],
  }

  const srcPaneNewTabs = state.tabs.filter(t => t.paneId === srcPaneId)
  const [tab] = srcPaneNewTabs.splice(oldIndex, 1)
  const movedTab = {
    ...tab,
    paneId: targetPaneId,
    stackIndex: newState.stackCounter++,
  }
  delete movedTab.ephemeral

  if (srcPaneId === targetPaneId) {
    srcPaneNewTabs.splice(newIndex, 0, movedTab)
    newState.tabs.push(...srcPaneNewTabs)
    return newState
  }

  const targetPaneNewTabs = state.tabs.filter(t => t.paneId === targetPaneId)
  const existingTabIndex = targetPaneNewTabs.findIndex(t => editorFileLocationEqual(t, tab))

  if (existingTabIndex === -1) {
    targetPaneNewTabs.push(movedTab)
  } else {
    targetPaneNewTabs[existingTabIndex] = movedTab
  }
  newState.tabs.push(...srcPaneNewTabs, ...targetPaneNewTabs)
  return newState
}

const getTopTabInPane = (
  state: EditorTabs, paneId: number
): EditorTab => state.tabs.reduce((a, b) => {
  if (b.paneId === paneId && (!a || a.stackIndex < b.stackIndex)) {
    return b
  } else {
    return a
  }
}, null)

const handleTabClose = (state: EditorTabs, id: number): EditorTabs => {
  const paneId = state.tabs.find(t => t.id === id)?.paneId
  const newTabState = {...state, tabs: state.tabs.filter(t => t.id !== id)}
  const nextTab = getTopTabInPane(newTabState, paneId)
  return nextTab ? handleTabSelect(newTabState, nextTab.id) : newTabState
}

const handleStateChange = (state: EditorTabs, id: number, update: CustomState): EditorTabs => ({
  ...state,
  tabs: state.tabs.map(t => (t.id === id ? {...t, customState: {...t.customState, ...update}} : t)),
})

type SavedTabData = {state?: EditorTabs}

const handleSessionLoad = (state: EditorTabs, data: SavedTabData): EditorTabs => {
  if (data?.state) {
    return data.state
  } else {
    return state
  }
}

const getTopTabInStack = (state: EditorTabs): EditorTab => state.tabs.reduce((a, b) => {
  if (!a || a.stackIndex < b.stackIndex) {
    return b
  } else {
    return a
  }
}, null)

const getAdjacentTab = (
  tabState: EditorTabs, id: number, paneId: number, direction: 'left' | 'right'
): EditorTab => {
  const tabsInPane = tabState.tabs.filter(t => t.paneId === paneId)
  const tabIdx = tabsInPane.findIndex(t => t.id === id)
  const dir = direction === 'left' ? -1 : 1

  if (!tabsInPane[tabIdx + dir]) {
    return direction === 'left' ? tabsInPane[tabsInPane.length - 1] : tabsInPane[0]
  }

  return tabsInPane[tabIdx + dir]
}

const getPaneTabCount = (
  tabState: EditorTabs, paneId: number
): number => tabState.tabs.filter(t => t.paneId === paneId).length

const canCloseTabsInDirection = (
  tabState: EditorTabs, id: number, paneId: number, direction: 'left' | 'right'
): boolean => {
  const tabsInPane = tabState.tabs.filter(t => t.paneId === paneId)
  const tabIdx = tabsInPane.findIndex(t => t.id === id)
  const dir = direction === 'left' ? -1 : 1

  return !!tabsInPane[tabIdx + dir]
}

const closeTabsInPane = (
  tabState: EditorTabs, paneId: number
): EditorTabs => ({
  ...tabState,
  tabs: tabState.tabs.filter(t => t.paneId !== paneId),
})

const closeOtherTabsInPane = (
  tabState: EditorTabs, id: number, paneId: number
): EditorTabs => ({
  ...tabState,
  tabs: tabState.tabs.filter(t => t.paneId !== paneId || t.id === id),
})

const closeTabsInDirection = (
  tabState: EditorTabs, id: number, paneId: number, direction: 'left' | 'right'
): EditorTabs => {
  const tabIdx = tabState.tabs.findIndex(t => t.id === id)
  return ({
    ...tabState,
    tabs: tabState.tabs.filter((t, i) => {
      if (t.paneId !== paneId) {
        return true
      }
      return direction === 'left' ? i >= tabIdx : i <= tabIdx
    }),
  })
}

export {
  handleFileRename,
  handleFileDelete,
  handleFileSelect,
  handleTabSelect,
  handleTabClose,
  handleReorder,
  handleStateChange,
  handleSessionLoad,
  getTopTabInStack,
  getTopTabInPane,
  getAdjacentTab,
  getPaneTabCount,
  canCloseTabsInDirection,
  closeTabsInPane,
  closeOtherTabsInPane,
  closeTabsInDirection,
}

export type {
  EditorTabs,
  EditorTab,
  CustomState,
}
