import React, {createContext, useState, useContext} from 'react'

import type {ModelInfo} from '../../editor/asset-preview-types'

interface IStudioModelPreviewContext {
  fileSize: number
  setFileSize: (value: number) => void
  modelInfo: ModelInfo | null
  setModelInfo: (value: ModelInfo | null) => void
  showSimplifyMeshWarning: boolean
  setShowSimplifyMeshWarning: (value: boolean) => void
  showGlbComputingLoader: boolean
  setShowGlbComputingLoader: (value: boolean) => void
  loadingFileSize: boolean
  setLoadingFileSize: (value: boolean) => void
  loadingTriCount: boolean
  setLoadingTriCount: (value: boolean) => void
  showTextureSizeWarning: boolean
  setShowTextureSizeWarning: (value: boolean) => void
}

const StudioModelPreviewContext = createContext<IStudioModelPreviewContext | undefined>(undefined)

const StudioModelPreviewContextProvider: React.FC<React.PropsWithChildren> = ({children}) => {
  const [fileSize, setFileSize] = useState(0)
  const [modelInfo, setModelInfo] = useState<ModelInfo | null>(null)
  const [showSimplifyMeshWarning, setShowSimplifyMeshWarning] = useState(false)
  const [showGlbComputingLoader, setShowGlbComputingLoader] = useState(false)
  const [loadingFileSize, setLoadingFileSize] = useState(false)
  const [loadingTriCount, setLoadingTriCount] = useState(false)
  const [showTextureSizeWarning, setShowTextureSizeWarning] = useState(false)

  return (
    <StudioModelPreviewContext.Provider
      value={{
        fileSize,
        setFileSize,
        modelInfo,
        setModelInfo,
        showSimplifyMeshWarning,
        setShowSimplifyMeshWarning,
        showGlbComputingLoader,
        setShowGlbComputingLoader,
        loadingFileSize,
        setLoadingFileSize,
        loadingTriCount,
        setLoadingTriCount,
        showTextureSizeWarning,
        setShowTextureSizeWarning,
      }}
    >
      {children}
    </StudioModelPreviewContext.Provider>
  )
}

const useStudioModelPreviewContext = () => {
  const context = useContext(StudioModelPreviewContext)
  if (!context) {
    throw new Error('useStudioModelPreviewContext must be used within an StudioModelPreviewContext')
  }
  return context
}

export {StudioModelPreviewContextProvider, useStudioModelPreviewContext}
