// @attr(visibility = ["//visibility:public"])

import {S3} from '@repo/reality/shared/s3'

import type {
  RefHead,
  StudioGlobalEntry,
} from './fetch-studio-global-entry'
import type {StoredAssetManifest} from '@repo/c8/ecs/src/shared/asset-manifest'

const S3_BUCKET_NAME = '8w-us-west-2-hosting'

const getS3KeyForProject = (
  workspace: string,
  appName: string,
  refHead: RefHead,
  studioGlobalEntry: StudioGlobalEntry
) => (
  `code/dist/${workspace}.${appName}/${refHead}/asset-manifest-${studioGlobalEntry.commitId}` +
    `-${studioGlobalEntry.userSettingsHash}.json`
)

const fetchAssetManifest = async (
  workspace: string,
  appName: string,
  refHead: RefHead,
  studioGlobalEntry: StudioGlobalEntry
): Promise<StoredAssetManifest> => {
  const s3ManifestKey = getS3KeyForProject(workspace, appName, refHead, studioGlobalEntry)
  const file = await S3.use().getObject({
    Bucket: S3_BUCKET_NAME,
    Key: s3ManifestKey,
  })

  const fileContents = await file.Body?.transformToString() ?? '{}'
  const manifest: StoredAssetManifest = JSON.parse(fileContents)
  return manifest
}

export {
  fetchAssetManifest,
}
