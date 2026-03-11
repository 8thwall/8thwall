import {streamExec} from '@repo/c8/cli/proc'

/* eslint-disable no-console */

interface SyncCommand{
  dryrun: boolean
  deleteFromDestination: boolean
  remote: string
  local: string
}

async function syncLocalDirectoryToS3(
  {dryrun, deleteFromDestination, remote, local}: SyncCommand
): Promise<void> {
  console.log(`Cloning ${local} to ${remote}`)
  let command = `aws s3 sync ${local} ${remote}`
  if (dryrun) {
    command += ' --dryrun'
  }
  if (deleteFromDestination) {
    command += ' --delete'
  }
  await streamExec(command)
  console.log('Successfully uploaded the dataset.')
}

async function syncS3DirectoryToLocal(
  {dryrun, deleteFromDestination, remote, local}: SyncCommand
): Promise<void> {
  console.log(`Cloning ${remote} to ${local}`)
  let command = `aws s3 sync ${remote} ${local}`
  if (dryrun) {
    command += ' --dryrun'
  }
  if (deleteFromDestination) {
    command += ' --delete'
  }

  await streamExec(command)
  console.log('Successfully downloaded the dataset.')
}

export {
  syncLocalDirectoryToS3,
  syncS3DirectoryToLocal,
}
