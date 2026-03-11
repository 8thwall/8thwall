import React from 'react'
import type {DeepReadonly} from 'ts-essentials'

import {useBooleanUrlState, useStringUrlState} from '../hooks/url-state'
import useActions from '../common/use-actions'
import assetLabActions from './asset-lab-actions'
import useCurrentAccount from '../common/use-current-account'
import {extractUuidInput} from './generate-request'
import {useRefScroller, type RefScroller} from '../hooks/use-ref-scroller'
import {useCurrentAccountAssetGenerations} from './hooks'
import {useSelector} from '../hooks'

const assetLabModes = [
  'library',
  'image',
  'model',
  'animation',
  'detailView',
  'optimizer',
] as const

type AssetLabMode = typeof assetLabModes[number]
type AssetLabWorkflow = 'gen-image' | 'gen-animated-char' | 'gen-3d-model' | undefined

const isValidMode = (mode: AssetLabMode): mode is AssetLabState['mode'] => (
  typeof mode === 'string' && assetLabModes.includes(mode)
)

const isValidWorkflow = (workflow: AssetLabWorkflow): workflow is AssetLabState['workflow'] => (
  typeof workflow === 'string' &&
  ['gen-image', 'gen-animated-char', 'gen-3d-model'].includes(workflow)
)

type AssetLabState = DeepReadonly<{
  // --- URL-readable states
  open: boolean
  mode: AssetLabMode
  // When mode is not 'library' or 'detailView', is one of: 'gen-image', 'gen-animated-char',
  // 'gen-3d-model'
  workflow?: AssetLabWorkflow
  // - These are for viewings (right hand side of different workflow modes)
  assetRequestUuid?: string  // UUID of the asset request being viewed, e.g. for remixing
  assetGenerationUuid?: string  // UUID of the asset generation being viewed, e.g. for remixing
  // - These are for inputting (left hand side of different workflow modes)
  // Comma separated AssetGeneration UUIDs for image prompt front, back, left, right
  inputFblr?: string
  inputMode?: string  // 'singleView' | 'multiView'
  // Input uuid of mesh for animation rigging
  animInputUuid?: string
  // Input uuid of image for image step
  imageInputUuid?: string

  // In `optimizer` mode, we use this to get back to where the user was.
  prevMode?: AssetLabMode

  // Error message relating to user's input when generating
  userInputError?: string

  requestLineage: {
    root: string
    // You can generate multiple multi-views from a single front-view.
    multiViews: string[]
    model: string
    animation: string
  }

  // Missing from the state
  // search: string
  // sortBy: string
  // filterBy: string
  // view: 'list' | 'grid'
}>

type AssetLabStateUpdate = (updater: Partial<AssetLabState>) => void

type AssetLabPartialLineageUpdate = (updater: Partial<AssetLabState['requestLineage']>) => void

type AssetLabLineageFromUpdate = (assetRequestUuid: string) => AssetLabState['requestLineage']

type FocusedAsset = DeepReadonly<{
  id: string | null
  img: React.RefObject<HTMLImageElement> | null
  url: string | null
}>

type FocusedAssetUpdate = (updater: Partial<FocusedAsset>) => void

type IAssetLabStateContext = {
  state: AssetLabState
  setState: AssetLabStateUpdate
  setPartialRequestLineage: AssetLabPartialLineageUpdate
  setRequestLineageFromInput: AssetLabLineageFromUpdate
  focusedAsset: FocusedAsset
  setFocusedAsset: FocusedAssetUpdate
  librarySquareRefScroller: RefScroller<string>
  librarySearchResults: string[]
  setLibrarySearchResults: (results: string[] | undefined) => void
}

const AssetLabStateContext = React.createContext<IAssetLabStateContext>({} as IAssetLabStateContext)

const useAssetLabStateContext = (): IAssetLabStateContext => React.useContext(AssetLabStateContext)

const DEFAULT_STATE: Partial<AssetLabState> = {
  open: false,
  mode: 'library',
}

const AssetLabStateContextProvider: React.FC<{children: React.ReactNode}> = ({children}) => {
  const [open, setOpen] = useBooleanUrlState('assetLab', false)
  const [mode, setMode] = useStringUrlState('alMode', DEFAULT_STATE.mode)
  const [assetRequestUuid, setAssetRequestUuid] = useStringUrlState('alAssetRequest', undefined)
  const [assetGenerationUuid, setAssetGenerationUuid] = useStringUrlState('alAssetGeneration',
    undefined)
  const [inputFblr, setInputFblr] = useStringUrlState('alInputFblr', undefined)
  const [inputMode, setInputMode] = useStringUrlState('alInputMode', undefined)
  const [animInputUuid, setAnimInputUuid] = useStringUrlState('alAnimInput', undefined)
  const [imageInputUuid, setImageInputUuid] = useStringUrlState('alImageInput', undefined)
  const [workflow, setWorkflow] = useStringUrlState('alWorkflow', undefined)
  const [focusedAsset, setFocusedAsset] = React.useState<FocusedAsset>({
    id: null, img: null, url: null,
  })
  const {assetGenerationIds} = useCurrentAccountAssetGenerations()
  const [librarySearchResults, setLibrarySearchResults] =
    React.useState<string[] | undefined>(undefined)
  const [userInputError, setUserInputError] = React.useState<string | undefined>(undefined)
  const [prevMode, setPrevMode] = useStringUrlState('alPrevMode', undefined)
  const assetRequests = useSelector(s => s.assetLab.assetRequests)
  const [lineageRoot, setLineageRoot] = React.useState<string | undefined>()
  const [lineageMultiView, setLineageMultiView] = React.useState<string[] | undefined>()
  const [lineageModel, setLineageModel] = React.useState<string | undefined>()
  const [lineageAnimation, setLineageAnimation] = React.useState<string | undefined>()

  const {getAssetRequests} = useActions(assetLabActions)
  const currentAccount = useCurrentAccount()

  React.useEffect(() => {
    if (!currentAccount) {
      return
    }
    getAssetRequests(currentAccount.uuid, {status: 'SUCCESS'})
  }, [currentAccount?.uuid])

  const assetLabState: AssetLabState = {
    open,
    mode: mode as AssetLabMode,
    workflow: workflow as AssetLabWorkflow,
    assetRequestUuid,
    assetGenerationUuid,
    inputFblr,
    inputMode,
    animInputUuid,
    imageInputUuid,
    userInputError,
    prevMode: prevMode as AssetLabMode,
    requestLineage: {
      root: lineageRoot,
      multiViews: lineageMultiView,
      model: lineageModel,
      animation: lineageAnimation,
    },
  }

  // Newest requests should appear first.
  // TODO(kyle): Replace specific orientations in multi-views with the appropriate re-roll.
  const getMultiViewsFromReq = (parentReqUuid: string) => Object.values(assetRequests || {})
    .filter(e => (
      parentReqUuid &&
      e.ParentRequestUuid === parentReqUuid &&
      e.type === 'IMAGE_TO_IMAGE' &&
      !e.input?.orientation
    ))
    .sort((a, b) => a.createdAt.localeCompare(b.createdAt))
    .reverse()
    .map(e => e.uuid)

  const librarySquareRefScroller = useRefScroller<string>()
  const ctx: IAssetLabStateContext = {
    state: assetLabState,
    setState: (partial) => {
      Object.entries(partial).forEach(([key, value]) => {
        switch (key) {
          case 'open':
            setOpen(!!value)
            break
          case 'mode':
            if (isValidMode(value as AssetLabMode)) {
              setMode(value as AssetLabMode)
            } else {
              setMode('library')
            }
            break
          case 'workflow':
            if (isValidWorkflow(value as AssetLabWorkflow)) {
              setWorkflow(value as AssetLabWorkflow)
            } else {
              setWorkflow(undefined)
            }
            break
          case 'assetRequestUuid':
            setAssetRequestUuid(value as string)
            break
          case 'assetGenerationUuid':
            setAssetGenerationUuid(value as string)
            break
          case 'inputFblr':
            setInputFblr(value as string)
            break
          case 'inputMode':
            setInputMode(value as string)
            break
          case 'animInputUuid':
            setAnimInputUuid(value as string)
            break
          case 'imageInputUuid':
            setImageInputUuid(value as string)
            break
          case 'userInputError':
            setUserInputError(value as string | undefined)
            break
          case 'prevMode':
            setPrevMode(value as AssetLabMode | undefined)
            break
          default:
            break
        }
      })
    },
    setPartialRequestLineage: (partial: Partial<AssetLabState['requestLineage']>) => {
      if (partial.root) {
        setLineageRoot(partial.root)
      }
      if (partial.multiViews) {
        setLineageMultiView(partial.multiViews as string[])
      }
      if (partial.model) {
        setLineageModel(partial.model)
      }
      if (partial.animation) {
        setLineageAnimation(partial.animation)
      }
    },
    setRequestLineageFromInput: (assetReqUuid: string) => {
      const assetReq = assetRequests[assetReqUuid]
      if (!assetReq) {
        return {
          root: undefined,
          multiViews: undefined,
          model: undefined,
          animation: undefined,
        }
      }

      let root: string
      let multiViews: string[] = []
      let model: string
      let animation: string
      switch (assetReq?.type) {
        case 'TEXT_TO_IMAGE':
          root = assetReqUuid
          setLineageRoot(root)
          break
        case 'IMAGE_TO_IMAGE': {
          root = assetReq.ParentRequestUuid || assetReqUuid
          multiViews = getMultiViewsFromReq(root)
          setLineageRoot(root)
          setLineageMultiView(multiViews)
          setImageInputUuid(extractUuidInput(assetRequests[root])?.[0])
          break
        }
        case 'IMAGE_TO_MESH': {
          root = assetReq.ParentRequestUuid
          multiViews = getMultiViewsFromReq(assetReq.ParentRequestUuid)
          model = assetReqUuid
          setLineageRoot(root)
          setLineageMultiView(multiViews)
          setLineageModel(model)
          setAnimInputUuid(assetReq.AssetGenerations?.[0]?.uuid)
          const fblr = extractUuidInput(assetReq)
          setInputFblr(fblr.join(','))
          setInputMode(fblr.length === 1 ? 'singleView' : 'multiView')
          setImageInputUuid(extractUuidInput(assetRequests[root])?.[0])
          break
        }
        case 'MESH_TO_ANIMATION':  // NOTE(kyle): Animation input is not yet supported.
        default:
          break
      }
      return {root, multiViews, model, animation}
    },
    focusedAsset,
    setFocusedAsset: (updater) => {
      setFocusedAsset(prev => ({...prev, ...updater}))
    },
    librarySquareRefScroller,
    librarySearchResults: librarySearchResults ?? assetGenerationIds,
    setLibrarySearchResults,
  }

  return (
    <AssetLabStateContext.Provider value={ctx}>
      {children}
    </AssetLabStateContext.Provider>
  )
}

const setModeClearState = (
  assetLabCtx: IAssetLabStateContext, mode: AssetLabMode, partialState?: Partial<AssetLabState>
) => {
  assetLabCtx.setState({
    mode,
    assetRequestUuid: undefined,
    assetGenerationUuid: undefined,
    inputFblr: undefined,
    inputMode: undefined,
    animInputUuid: undefined,
    ...partialState,
  })
}

const backToLibraryClear = (
  assetLabCtx: IAssetLabStateContext,
  opts: {
    scrollToUuid?: string
    partialState?: Partial<AssetLabState>
  } = {}
) => {
  setModeClearState(assetLabCtx, 'library', opts.partialState)
  assetLabCtx.librarySquareRefScroller.scrollTo(opts.scrollToUuid)
  assetLabCtx.setLibrarySearchResults(undefined)
}

export {
  AssetLabStateContextProvider,
  useAssetLabStateContext,
  backToLibraryClear,
  setModeClearState,
}

export type {
  AssetLabState,
  AssetLabStateUpdate,
  IAssetLabStateContext,
  AssetLabMode,
  AssetLabWorkflow,
}
