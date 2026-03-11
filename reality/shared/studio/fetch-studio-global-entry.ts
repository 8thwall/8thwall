// @attr(visibility = ["//visibility:public"])

import {Ddb} from '@repo/reality/shared/dynamodb'

import {fromAttributes, type AttributesForRaw} from '@repo/reality/shared/typed-attributes'

const DDB_TABLE_NAME = 'studio-global'

type RefHead = 'staging' | 'production' | string

type StudioGlobalEntry = {
  commitId: string
  userSettingsHash: string
}

const fetchStudioGlobalEntry = async (
  workspace: string,
  appName: string,
  refHead: RefHead
): Promise<StudioGlobalEntry> => {
  const projectionExpressionKeys: Array<keyof StudioGlobalEntry> = ['commitId', 'userSettingsHash']
  const rawRequestItem = (await Ddb.use().getItem({
    Key: {
      pk: {S: `${workspace}.${appName}`},
      sk: {S: `ref:${refHead}`},
    },
    ProjectionExpression: projectionExpressionKeys.join(','),
    TableName: DDB_TABLE_NAME,
  })).Item as AttributesForRaw<StudioGlobalEntry>
  if (!rawRequestItem) {
    throw new Error('No deployment found for project')
  }

  return fromAttributes(rawRequestItem)
}

export {
  fetchStudioGlobalEntry,
  type StudioGlobalEntry,
  type RefHead,
}
