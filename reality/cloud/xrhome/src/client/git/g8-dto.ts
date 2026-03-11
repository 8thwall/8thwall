interface IGitFile {
  repositoryName: string
  repoId: string
  filePath: string
  isDirectory: boolean
  mode: number
  timestamp: Date
  content: string
}

// repositoryName is mandatory for all g8 operations.
// handle is mandatory only for remote g8 operations.
// Apps use projectSpecifier for repositoryName.
// Modules use repoId for repositoryName.
interface IRepo {
  repositoryName: string
  repoId?: string
  handle?: string
  branchName?: string
  commitId?: string
  Prefix?: string
  workspace?: string
}

interface IGit {
  repo: IRepo
  files: IGitFile[]
  filePaths: string[]
  filesByPath: Record<string, IGitFile>
  childrenByPath: Record<string, string[]>
  topLevelPaths: string[]
}

export {
  IGit,
  IGitFile,
  IRepo,
}
