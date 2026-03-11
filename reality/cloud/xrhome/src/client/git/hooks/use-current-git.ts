import type {DeepReadonly} from 'ts-essentials'

import {useSelector} from '../../hooks'
import type {IGit} from '../g8-dto'
import type {RepoState} from '../git-redux-types'
import {useCurrentRepoId} from '../repo-id-context'
import {getRepoState} from '../repo-state'

type GitSelector<T> = (git: DeepReadonly<IGit>) => T

function useScopedGit(repoId: string): RepoState
// eslint-disable-next-line no-redeclare
function useScopedGit<T>(repoId: string, selector: GitSelector<T>): T
// eslint-disable-next-line no-redeclare
function useScopedGit<T>(repoId: string, selector?: GitSelector<T>) {
  return useSelector((s) => {
    if (!repoId) {
      return null
    }
    const git = getRepoState(s, repoId)
    if (selector) {
      return selector(git)
    } else {
      return git
    }
  })
}

function useCurrentGit(): RepoState
// eslint-disable-next-line no-redeclare
function useCurrentGit<T>(selector: GitSelector<T>): T
// eslint-disable-next-line no-redeclare
function useCurrentGit<T>(selector?: GitSelector<T>) {
  const repoId = useCurrentRepoId()
  return useSelector((s) => {
    const git = getRepoState(s, repoId)
    if (selector) {
      return selector(git)
    } else {
      return git
    }
  })
}
// TODO(christoph): Clean up
const useGitActiveClient = () => ({name: 'default'})

const useGitRepo = () => useCurrentGit(git => git.repo)

const useGitFile = (filePath: string) => useCurrentGit(git => git.filesByPath[filePath])
const useScopedGitFile = (repoId: string, path: string) => (
  useScopedGit(repoId, g => g.filesByPath[path])
)

const useGitFileContent = (filePath: string) => (
  useCurrentGit(git => git.filesByPath[filePath]?.content)
)

export {
  useScopedGit,
  useCurrentGit,
  useGitActiveClient,
  useGitRepo,
  useGitFile,
  useGitFileContent,
  useScopedGitFile,
}
