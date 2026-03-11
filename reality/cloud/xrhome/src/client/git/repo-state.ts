import type {RootState} from '../reducer'
import type {IRepo} from './g8-dto'
import type {RepoState} from './git-redux-types'

const INITIAL_REPO_STATE: RepoState = {
  repo: null,
  files: [],
  filesByPath: {},
  filePaths: [],
  childrenByPath: {},
  topLevelPaths: [],
  msg: '',
}

type OuterState = Pick<RootState, 'git'>

const getRepoState = (provider: OuterState | (() => OuterState), repo: string | IRepo = null) => {
  const state = typeof provider === 'function' ? provider() : provider
  const gitState = state.git
  const repoId = typeof repo === 'string' ? repo : repo?.repoId

  return gitState?.byRepoId[repoId] || INITIAL_REPO_STATE
}

export {
  getRepoState,
  INITIAL_REPO_STATE,
}
