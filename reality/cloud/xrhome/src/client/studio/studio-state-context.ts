import React from 'react'
import type {DeepReadonly} from 'ts-essentials'

import type {EditorFileLocation} from '../editor/editor-file-location'
import type {Clipboard} from './clipboard'
import type {RefScroller} from '../hooks/use-ref-scroller'

enum TRANSFORM_TOOL_TYPES {
  TRANSLATE_TOOL = 'translate',
  ROTATE_TOOL = 'rotate',
  SCALE_TOOL = 'scale',
}

type PanelSection = 'settings' | 'inspector'

enum PanelSelection {
  SETTINGS = 'settings',
  INSPECTOR = 'inspector',
}

type ImageTargetShowOption = 'showGeometry' | 'showFullImage' | 'showTrackedRegion'

type FileBrowserSection = 'files' | 'prefabs' | 'imageTargets' | 'assetLab'

type StudioState = DeepReadonly<{
  playing: boolean
  isPreviewPaused: boolean
  pointing: boolean
  objectToBeErased: string
  errorMsg?: string | undefined
  showUILayer: boolean
  showGrid: boolean
  collapsedSections: Record<string, string[]>
  selectedIds: string[]
  selectedPrefab: string | undefined
  selectedAsset: EditorFileLocation
  selectedFiles: string[]
  lastSelectedFileIndex: number | null
  selectedImageTarget: string | undefined
  selectedImageTargetViews: Record<string, ImageTargetShowOption[]>
  imageTargetUploadProgress: number | undefined
  shadingMode: 'shaded' | 'unlit'
  cameraView: 'orthographic' | 'perspective'
  activeSpace: string | undefined
  transformMode: TRANSFORM_TOOL_TYPES
  simulatorCollapsed: boolean
  simulatorMode: 'full' | 'inline'
  clipboard: Clipboard
  currentPanelSection: PanelSection
  currentBrowserSection: FileBrowserSection
  restartKey: number
  fileBrowserScroller: RefScroller<string>
  sceneDiffOpen: boolean
}>

type StudioStateUpdater = Partial<StudioState> | ((current: StudioState) => StudioState)
type StudioStateUpdate = (updater: StudioStateUpdater) => void

type StudioStateContext = {
  state: StudioState
  update: StudioStateUpdate
  setSelection: (...ids: string[]) => void
  removeFromSelection: (...ids: string[]) => void
  addToSelection: (...ids: string[]) => void
  selectImageTarget: (id: string) => void
}

const studioStateContext = React.createContext<StudioStateContext | null>(null)

const useStudioStateContext = (): StudioStateContext => {
  const ctx = React.useContext(studioStateContext)
  if (!ctx) {
    throw new Error('useStudioStateContext must be used within a StudioStateContextProvider')
  }
  return ctx
}

const StudioStateContextProvider = studioStateContext.Provider

export {
  StudioStateContextProvider,
  TRANSFORM_TOOL_TYPES as TransformToolTypes,
  PanelSelection,
  useStudioStateContext,
}
export type {
  StudioStateUpdate,
  StudioStateUpdater,
  StudioStateContext,
  StudioState,
  PanelSection,
  ImageTargetShowOption,
  FileBrowserSection,
}
