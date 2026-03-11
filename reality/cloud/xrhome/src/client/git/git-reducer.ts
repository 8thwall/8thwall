import type {DeepReadonly} from 'ts-essentials'

import type {GitAction, RepoState} from './git-redux-types'
import {INITIAL_REPO_STATE} from './repo-state'

type GitReduxState = DeepReadonly<{
  byRepoId: Record<string, RepoState>
}>

const EMPTY_GIT_REDUX_STATE: GitReduxState = {
  byRepoId: {},
}

// Gets the current git state for the scope or returns an empty state.
const repoStateForRepoId = (
  state: GitReduxState,
  repoId: string
): RepoState => (
  (repoId && state.byRepoId[repoId]) ||
  INITIAL_REPO_STATE
)

let didLogMissing = false

const updateRepoState = (
  state: GitReduxState,
  repoId: string,
  update: Partial<RepoState>
): GitReduxState => {
  if (!repoId) {
    if (!didLogMissing) {
      didLogMissing = true
      // eslint-disable-next-line no-console
      console.error('Missing repo ID for state update:', update)
    }
    return state
  }
  const repoState = repoStateForRepoId(state, repoId)
  const repoStateWithUpdate = {...repoState, ...update}
  const byRepoIdWithUpdate = {...state.byRepoId, [repoId]: repoStateWithUpdate}

  return {...state, byRepoId: byRepoIdWithUpdate}
}

const Reducer = (
  state: GitReduxState = EMPTY_GIT_REDUX_STATE,
  action: GitAction
): GitReduxState => {
  const {type, repoId} = action

  switch (type) {
    // Set the git repo information. If the repo differs from the current repo, all other git info
    // will be set to the initial state.
    case 'GIT_REPO': {
      const scopeState = repoStateForRepoId(state, repoId)
      return updateRepoState(state, repoId, {...scopeState, repo: action.repo})
    }
    // Update subfields of action.git within the state. This allows for updating multiple fields
    // simultaneously. Only subfields that are part of initialState will be updated.
    case 'GIT_UPDATE': {
      const updatedState = {...action.git}
      Object.keys(updatedState).forEach(
        (k) => { if (!(k in INITIAL_REPO_STATE)) { delete updatedState[k] } }
      )
      return updateRepoState(state, repoId, updatedState)
    }
    default:
      return state
  }
}

export default Reducer

// NOTE(pawel) These are exported so that the top level reducer can be typed.
export type {
  GitReduxState,
}
