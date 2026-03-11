import type {DeepReadonly as RO} from 'ts-essentials'

import type {IGit, IRepo} from './g8-dto'

type RepoState = RO<IGit & {
  msg: string
}>

type ActionBase = {appUuid?: string, repoId?: string}  // TODO(christoph): Make repoId required

type GitUpdateAction = ActionBase & {type: 'GIT_UPDATE', git: Partial<IGit>, context: string}
type GitRepoAction = ActionBase & {type: 'GIT_REPO', repo: IRepo}

type GitAction = GitUpdateAction | GitRepoAction

export type {
  RepoState,
  GitAction,
  GitUpdateAction,
  GitRepoAction,
}
