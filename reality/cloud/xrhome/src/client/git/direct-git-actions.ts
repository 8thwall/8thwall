import type {DeepReadonly as RO} from 'ts-essentials'

import type {
  GitRepoAction,
  GitUpdateAction,
} from './git-redux-types'

import type {
  IGit, IRepo,
} from './g8-dto'

const gitUpdateAction = (
  repoId: string, context: string, git: Partial<IGit>
): GitUpdateAction => ({
  type: 'GIT_UPDATE', repoId, context, git,
})

const gitRepoAction = (repoId: string, repo: RO<IRepo>): GitRepoAction => ({
  type: 'GIT_REPO' as const, repoId, repo,
})

export {
  gitUpdateAction,
  gitRepoAction,
}
