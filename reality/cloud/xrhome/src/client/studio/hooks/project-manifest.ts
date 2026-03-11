import useActions from '../../common/use-actions'
import coreGitActions from '../../git/core-git-actions'
import {useGitFile, useGitRepo} from '../../git/hooks/use-current-git'
import type {EcsManifest} from '../../../shared/studio/ecs-manifest'

const MANIFEST_PATH = 'manifest.json'

type ManifestUpdater = (manifest: EcsManifest) => EcsManifest

const readManifest = (content: string | null | undefined): EcsManifest => {
  if (!content) {
    return {
      version: 1,
    }
  }
  try {
    return JSON.parse(content)
  } catch (error) {
    return {
      version: 1,
    }
  }
}

const writeUpdate = (oldContent: string | null | undefined, updater: ManifestUpdater): string => {
  const oldManifest = readManifest(oldContent)
  const newManifest = updater(oldManifest)
  return JSON.stringify(newManifest, null, 2)
}

const useProjectManifest = () => {
  const repo = useGitRepo()
  const {mutateFile} = useActions(coreGitActions)

  const update = async (updater: ManifestUpdater) => {
    await mutateFile(repo, {
      filePath: MANIFEST_PATH,
      transform: oldFile => writeUpdate(oldFile.content, updater),
      generate: () => writeUpdate(null, updater),
    })
  }

  const file = useGitFile(MANIFEST_PATH)
  const {config} = readManifest(file?.content)

  return {
    config: config || {},
    update,
  }
}

export {
  useProjectManifest,
}
