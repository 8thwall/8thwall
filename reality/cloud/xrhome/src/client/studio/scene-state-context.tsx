import React from 'react'

import type {DeepReadonly} from 'ts-essentials'

import {useBooleanUrlState, useStringArrayUrlState, useStringUrlState} from '../hooks/url-state'
import {
  StudioStateContextProvider, StudioState, StudioStateContext, StudioStateUpdater,
  TransformToolTypes, PanelSection, PanelSelection,
  FileBrowserSection,
} from './studio-state-context'
import type {EditorFileLocation} from '../editor/editor-file-location'
import type {Clipboard} from './clipboard'
import {useEvent} from '../hooks/use-event'
import {useRefScroller} from '../hooks/use-ref-scroller'

interface ISceneStateContext {
  children: React.ReactNode
}

const SceneStateContext: React.FC<ISceneStateContext> = ({
  children,
}) => {
  const [playing, setPlaying] = useBooleanUrlState('playing', false)
  const [isPreviewPaused, setIsPreviewPaused] = useBooleanUrlState('paused', false)
  const [pointing, setPointing] = useBooleanUrlState('pointing', false)
  const [objectToBeErased, setObjectToBeErased] = React.useState<string | undefined>(undefined)
  const [errorMsg, setErrorMsg] = React.useState('')
  const [showUILayer, setShowUILayer] = useBooleanUrlState('showUILayer', true)
  const [showGrid, setShowGrid] = useBooleanUrlState('showGrid', true)
  const [collapsedSections, setCollapsedSections] = React.useState({})
  const [selectedIds, setSelectedIds] = useStringArrayUrlState('selected', [])
  const [selectedPrefab, setSelectedPrefab] = useStringUrlState('prefab', undefined)
  const [selectedAsset, setSelectedAsset] = React.useState<EditorFileLocation>(null)
  const [selectedFiles, setSelectedFiles] = React.useState<string[]>([])
  const [lastSelectedFileIndex, setLastSelectedIndex] = React.useState<number | null>(null)
  const [selectedImageTarget, setSelectedImageTarget] = React
    .useState<string | undefined>(undefined)
  const [selectedImageTargetViews, setSelectedImageTargetViews] = React.useState({})
  const [imageTargetUploadProgress, setImageTargetUploadProgress] = React.useState(undefined)
  const [shadingMode, setShadingMode] = React.useState<'shaded' | 'unlit'>('shaded')
  const [cameraView, setCameraView] = React.useState<'orthographic' | 'perspective'>('perspective')
  const [activeSpace, setActiveSpace] = useStringUrlState('space', undefined)
  const [transformMode, setTransformMode] = React
    .useState<TransformToolTypes>(TransformToolTypes.TRANSLATE_TOOL)
  const [simulatorCollapsed, setSimulatorCollapsed] = React.useState<boolean>(false)
  const [simulatorMode, setSimulatorMode] = React.useState<StudioState['simulatorMode']>('full')
  const [clipboard, setClipboard] = React.useState<Clipboard>({type: 'empty'})
  const [currentPanelSection, setCurrentPanelSection] = React
    .useState<PanelSection>(selectedIds.length ? PanelSelection.INSPECTOR : PanelSelection.SETTINGS)
  const [sceneDiffOpen, setSceneDiffOpen] = React.useState(false)

  const [currentBrowserSection, setCurrentBrowserSection] = React.useState<FileBrowserSection>(
    'files'
  )

  // Incremented when the playback restart button is clicked
  const [restartKey, setRestartKey] = React.useState(1)

  const fileBrowserScroller = useRefScroller<string>()

  const studioState: StudioState = {
    playing,
    isPreviewPaused,
    pointing,
    objectToBeErased,
    errorMsg,
    showUILayer,
    showGrid,
    collapsedSections,
    selectedIds,
    selectedPrefab,
    selectedAsset,
    selectedFiles,
    lastSelectedFileIndex,
    selectedImageTarget,
    selectedImageTargetViews,
    imageTargetUploadProgress,
    shadingMode,
    cameraView,
    activeSpace,
    transformMode,
    simulatorCollapsed,
    simulatorMode,
    clipboard,
    currentPanelSection,
    currentBrowserSection,
    restartKey,
    fileBrowserScroller,
    sceneDiffOpen,
  }

  // NOTE(christoph+chloe): This stores the most recent update made to the state. It handles the
  // case where the state is updated multiple times in the same render, which makes the studioState
  // stale. However, since state can update due to URL changes, if we do rerender, studioState will
  // be a new reference and overrideFor will no longer match, in effect "invalidating" that ref
  // without requiring a useEffect for example.

  type LastUpdatedTracking = {
    overrideFor: StudioState
    value: StudioState
  }

  const lastUpdateRef = React.useRef<LastUpdatedTracking | undefined>(undefined)

  const update = useEvent((updater: StudioStateUpdater) => {
    const oldState = lastUpdateRef.current?.overrideFor === studioState
      ? lastUpdateRef.current.value
      : studioState

    const newState = {
      ...oldState,
      ...typeof updater === 'function' ? updater(oldState) : updater,
    }

    lastUpdateRef.current = {overrideFor: studioState, value: newState}

    setPlaying(newState.playing)
    setIsPreviewPaused(newState.isPreviewPaused)
    setPointing(newState.pointing)
    setObjectToBeErased(newState.objectToBeErased)
    setErrorMsg(newState.errorMsg)
    setShowUILayer(newState.showUILayer)
    setShowGrid(newState.showGrid)
    setCollapsedSections(newState.collapsedSections)
    setSelectedPrefab(newState.selectedPrefab)
    setSelectedAsset(newState.selectedAsset)
    setSelectedFiles([...newState.selectedFiles])
    setLastSelectedIndex(newState.lastSelectedFileIndex)
    setSelectedImageTarget(newState.selectedImageTarget)
    setSelectedImageTargetViews(newState.selectedImageTargetViews)
    setImageTargetUploadProgress(newState.imageTargetUploadProgress)
    setSelectedIds(newState.selectedIds)
    setShadingMode(newState.shadingMode)
    setCameraView(newState.cameraView)
    setActiveSpace(newState.activeSpace)
    setTransformMode(newState.transformMode)
    setSimulatorCollapsed(newState.simulatorCollapsed)
    setSimulatorMode(newState.simulatorMode)
    setClipboard(newState.clipboard)
    setCurrentPanelSection(newState.currentPanelSection)
    setCurrentBrowserSection(newState.currentBrowserSection)
    setRestartKey(newState.restartKey)
    setSceneDiffOpen(newState.sceneDiffOpen)
  })

  const setSelectedIdsUnique = (setter: (prev: DeepReadonly<string[]>) => string[]) => {
    update((prev: StudioState): StudioState => {
      const selected = [...new Set(setter(selectedIds))]
      const panelSection = selected.length > 0 ? PanelSelection.INSPECTOR : PanelSelection.SETTINGS
      return {
        ...prev,
        selectedAsset: null,
        selectedImageTarget: undefined,
        selectedIds: selected,
        currentPanelSection: panelSection,
        selectedFiles: [],
        lastSelectedFileIndex: null,
      }
    })
  }

  const stateCtx: StudioStateContext = {
    state: studioState,
    update,
    setSelection: (...ids: string[]) => {
      setSelectedIdsUnique(() => ids)
    },
    removeFromSelection: (...ids: string[]) => {
      setSelectedIdsUnique(s => s.filter(id => !ids.includes(id)))
    },
    addToSelection: (...ids: string[]) => {
      setSelectedIdsUnique(s => ([...s, ...ids]))
    },
    selectImageTarget: (id: string) => {
      update((prev: StudioState): StudioState => ({
        ...prev,
        selectedAsset: null,
        selectedImageTarget: id,
        selectedIds: [],
        currentPanelSection: PanelSelection.INSPECTOR,
      }))
    },
  }

  return (
    <StudioStateContextProvider value={stateCtx}>
      {children}
    </StudioStateContextProvider>
  )
}

export {
  SceneStateContext,
}

export type {
  ISceneStateContext,
}
