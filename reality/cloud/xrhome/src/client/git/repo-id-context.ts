import React from 'react'

const RepoIdContext = React.createContext<string | null>(null)

const RepoIdProvider = RepoIdContext.Provider

const useCurrentRepoId = () => React.useContext(RepoIdContext)

export {
  RepoIdProvider,
  useCurrentRepoId,
}
