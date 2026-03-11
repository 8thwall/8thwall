import localforage from 'localforage'
import {useHistory, useLocation} from 'react-router-dom'
import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {LocationDescriptor} from 'history'

import {useAbandonableEffect} from '../../hooks/abandonable-effect'
import {resolveEditorFileLocation, EditorFileLocation} from '../editor-file-location'
import {EditorPanes, getCurrentTabbedPaneId, getTabbedPanes, handleClosePane} from '../pane-state'
import {removeRichSuffix} from '../browsable-multitab-editor-view-helpers'
import {NEW_TAB_PATH, parseHashFragment} from '../editor-utils'
import {useCurrentRepoId} from '../../git/repo-id-context'
import {
  EditorTab, EditorTabs, getTopTabInStack, handleFileSelect, getPaneTabCount,
} from '../tab-state'
import type {ScrollTo} from './use-ace-scroll'
import type {EditorRouteParams} from '../editor-route'

const DEFAULT_PANE_ID = 1

const DEFAULT_PANE_STATE: DeepReadonly<EditorPanes> = {
  idCounter: 2,
  rootId: 0,
  panes: {
    0: {
      id: 0,
      parentId: null,
      type: 'root',
      childrenIds: [DEFAULT_PANE_ID],
    },
    1: {
      id: DEFAULT_PANE_ID,
      parentId: 0,
      type: 'tabbed-editor',
    },
  },
}

const DEFAULT_TAB_STATE: DeepReadonly<EditorTabs> = {
  tabs: [],
  idCounter: 0,
  stackCounter: 0,
}

const makeSessionKeyForRepoId = (repoId: string) => `8wIRE.session.${repoId}.0`

interface ILocalStorageData {
  openFiles?: string[]
  currentPath?: string
  tabState?: EditorTabs
  paneState?: EditorPanes
  currentLocation?: EditorFileLocation
}

interface PersistentEditorSession {
  tabState: EditorTabs
  setTabState: React.Dispatch<React.SetStateAction<EditorTabs>>
  paneState: EditorPanes
  setPaneState: React.Dispatch<React.SetStateAction<EditorPanes>>
  scrollTo: ScrollTo
  setScrollTo: React.Dispatch<React.SetStateAction<ScrollTo>>
  currentTabbedPaneId: number
  currentFileLocation: EditorTab
  isTabsFromLocalForageLoaded: boolean
}

const usePersistentEditorSession = (
  defaultOpenFile: string,
  getLocationFromFile: (file: EditorFileLocation) => LocationDescriptor<unknown>,
  parsedParams: EditorRouteParams,
  openFileFromLocalForage: boolean = true
): PersistentEditorSession => {
  const history = useHistory()
  const location = useLocation()
  const repoId = useCurrentRepoId()
  const pathLocation = resolveEditorFileLocation(parsedParams)

  const [paneState, setPaneState] = React.useState(DEFAULT_PANE_STATE)
  const [tabState, setTabState] = React.useState(DEFAULT_TAB_STATE)
  const [isTabsFromLocalForageLoaded, setIsTabsFromLocalForageLoaded] = React.useState(false)
  const [scrollTo, setScrollTo] = React.useState<ScrollTo>(null)

  const currentTabbedPaneId = getCurrentTabbedPaneId(tabState, paneState)
  const currentFileLocation = getTopTabInStack(tabState)

  if (!currentFileLocation && isTabsFromLocalForageLoaded) {
    setTabState(ts => handleFileSelect(ts, NEW_TAB_PATH, currentTabbedPaneId))
  }

  const autoSaveTabSessions = React.useRef<ReturnType<typeof setInterval>>(null)
  const autoSaveCurrentFile = React.useRef<ReturnType<typeof setInterval>>(null)

  const saveTabSessionsNow = () => {
    if (!isTabsFromLocalForageLoaded) {
      return
    }
    if (autoSaveCurrentFile.current) {
      clearInterval(autoSaveCurrentFile.current)
    }
    const currentFilePath = currentFileLocation?.filePath
    localforage.setItem<ILocalStorageData>(makeSessionKeyForRepoId(repoId), {
      tabState,
      paneState,
      currentLocation: currentFilePath,
      // Preserve backwards compatible format for interoperability during rollout
      openFiles: tabState.tabs.map(e => e.filePath),
      currentPath: currentFilePath,
    })
  }

  React.useEffect(() => {
    if (autoSaveTabSessions.current) {
      clearInterval(autoSaveTabSessions.current)
    }
    autoSaveTabSessions.current = setTimeout(saveTabSessionsNow, 1000)
  }, [tabState, paneState, currentFileLocation])

  React.useEffect(() => () => {
    saveTabSessionsNow()
    clearTimeout(autoSaveTabSessions.current)
    clearTimeout(autoSaveCurrentFile.current)
  }, [])

  React.useEffect(() => {
    const tabbedPanes = getTabbedPanes(paneState)
    if (tabbedPanes.length > 1 && isTabsFromLocalForageLoaded) {
      tabbedPanes.forEach((pane) => {
        setPaneState((ps) => {
          if (ps.panes[pane.id] && getPaneTabCount(tabState, pane.id) === 0) {
            return handleClosePane(ps, pane.id)
          }
          return ps
        })
      })
    }
  }, [tabState.tabs])

  useAbandonableEffect((abandon) => {
    abandon(localforage.getItem<ILocalStorageData>(makeSessionKeyForRepoId(repoId)))
      .then((data: ILocalStorageData) => {
        const desiredFileFromRoute = pathLocation

        const savedLocation = data?.currentLocation || data?.currentPath
        const desiredFileFromLocalForage = openFileFromLocalForage ? savedLocation : null
        const desiredCurrentFile = desiredFileFromRoute || desiredFileFromLocalForage ||
        defaultOpenFile
        const {line, column} = parseHashFragment(location.hash)

        const currentPaneIdFromStorage = data?.tabState && data?.paneState
          ? getCurrentTabbedPaneId(data.tabState, data.paneState)
          : currentTabbedPaneId

        let newTabState: EditorTabs
        if (data?.tabState) {
          const tabbedEditorPanes = data?.paneState && Object.values(data.paneState.panes)
            .filter(e => e.type === 'tabbed-editor').map(editor => editor.id)

          newTabState = {
            ...data.tabState,
            tabs: data.tabState.tabs.map(
              (tab) => {
                const paneId = (tabbedEditorPanes?.includes(tab.paneId) && tab.paneId) ||
              currentPaneIdFromStorage
                return {...tab, paneId}
              }
            ),
          }
        } else if (data?.openFiles) {
        // NOTE(christoph): removeRichSuffix is needed here to strip out localForage state that
        // was retained from when we encoded the markdown preview state in the filename.
          const desiredOpenFiles = data.openFiles
            .map(removeRichSuffix)

          newTabState = {
            idCounter: desiredOpenFiles.length,
            stackCounter: desiredOpenFiles.length,
            tabs: desiredOpenFiles.map((e, i) => ({
              id: i,
              paneId: currentPaneIdFromStorage,
              stackIndex: i,
              filePath: e,
              customState: {},
            })),
          }
        } else {
          newTabState = tabState
        }

        setTabState(handleFileSelect(newTabState, desiredCurrentFile, currentPaneIdFromStorage))
        setScrollTo({line, column})
        setIsTabsFromLocalForageLoaded(true)

        if (data?.paneState) {
          setPaneState(data.paneState)
        }

        if (!pathLocation) {
          history.replace(getLocationFromFile(desiredCurrentFile))
        }
      })
  }, [])

  return {
    tabState,
    setTabState,
    paneState,
    setPaneState,
    scrollTo,
    setScrollTo,
    currentTabbedPaneId,
    currentFileLocation,
    isTabsFromLocalForageLoaded,
  }
}

export type {
  PersistentEditorSession,
}

export {
  usePersistentEditorSession,
}
