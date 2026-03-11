import React from 'react'

import type {EditorRouteParams} from '../editor/editor-route'

type AppPathsContextValue = {
  getStudioRoute: (params: EditorRouteParams, state: Record<string, string>) => string
  getFileRoute: (params: EditorRouteParams) => string
}

const AppPathsContext = React.createContext<AppPathsContextValue | null>(null)

const useAppPathsContext = () => React.useContext(AppPathsContext)

export {
  AppPathsContext,
  useAppPathsContext,
}
