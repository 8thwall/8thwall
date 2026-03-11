import {streamExec} from 'c8/cli/proc'

/* eslint-disable no-console */

interface UploadCommand {
  repo: string
  localPath: string
  repoType?: string
  exclude?: string[]
}

async function uploadToHuggingFace(
  {repo, localPath, repoType = 'dataset', exclude = ['*.DS_Store']}: UploadCommand
): Promise<void> {
  console.log(`Uploading ${localPath} to HuggingFace repo ${repo}`)
  let command = `hf upload ${repo} ${localPath} --repo-type=${repoType}`
  for (const pattern of exclude) {
    command += ` --exclude "${pattern}"`
  }
  await streamExec(command)
  console.log('Successfully uploaded to HuggingFace.')
}

export {
  uploadToHuggingFace,
}
