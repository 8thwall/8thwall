import React from 'react'

import type {
  BuildNotificationData,
  HtmlShell,
  NaeBuilderRequest,
} from '../../../shared/nae/nae-types'
import {useEvent} from '../../hooks/use-event'

type NaeBuildStatus = 'noBuild' | 'building' | 'success' | 'failed'

type NaeBuildData = {
  naeBuildStatus: NaeBuildStatus
  buildRequest?: NaeBuilderRequest
  buildNotification?: BuildNotificationData
}

const DEFAULT_NAE_BUILD_DATA: NaeBuildData = {
  naeBuildStatus: 'noBuild',
  buildRequest: null,
  buildNotification: null,
}

type NaeBuildDataMap = Record<HtmlShell, NaeBuildData>

type PublishModalOption = HtmlShell | 'all-builds' | 'iframe'

type PublishingStateContext = {
  activePublishModalOption: PublishModalOption | null
  setActivePublishModalOption: (value: PublishModalOption) => void

  htmlShellToNaeBuildData: NaeBuildDataMap
  setHtmlShellToNaeBuildData: (platform: HtmlShell, data: NaeBuildData) => void
}

const publishingStateContext = React.createContext<PublishingStateContext | null>(null)

const usePublishingStateContext = (): PublishingStateContext => {
  const ctx = React.useContext(publishingStateContext)
  if (!ctx) {
    // eslint-disable-next-line max-len
    throw new Error('usePublishingStateContext must be used within a PublishingStateContextProvider')
  }
  return ctx
}

const PublishingStateContextProvider: React.FC<{children: React.ReactNode}> = ({children}) => {
  const [activePublishModalOption, setActivePublishModalOption] =
  React.useState<PublishModalOption | null>(null)

  const [htmlShellToNaeBuildData, setHtmlShellToNaeBuildData] = React.useState<NaeBuildDataMap>({
    android: DEFAULT_NAE_BUILD_DATA,
    ios: DEFAULT_NAE_BUILD_DATA,
    quest: DEFAULT_NAE_BUILD_DATA,
    osx: DEFAULT_NAE_BUILD_DATA,
    html: DEFAULT_NAE_BUILD_DATA,
  })

  const setNaeBuildData = React.useCallback((platform: HtmlShell, data: NaeBuildData) => {
    setHtmlShellToNaeBuildData(prev => ({...prev, [platform]: data}))
  }, [])

  const publishingContext: PublishingStateContext = {
    activePublishModalOption,
    setActivePublishModalOption: useEvent(setActivePublishModalOption),
    htmlShellToNaeBuildData,
    setHtmlShellToNaeBuildData: useEvent(setNaeBuildData),
  }

  return (
    <publishingStateContext.Provider value={publishingContext}>
      {children}
    </publishingStateContext.Provider>
  )
}

export {
  PublishingStateContextProvider,
  usePublishingStateContext,
}

export type {
  NaeBuildDataMap,
  NaeBuildStatus,
  PublishingStateContext,
  PublishModalOption,
}
