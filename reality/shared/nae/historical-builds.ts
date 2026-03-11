import {Ddb} from '../dynamodb'

import {
  type AttributesForRaw,
  toAttributes,
  fromAttributes,
} from '../typed-attributes'

import type {HistoricalBuildData} from './nae-types'
import {defaultHistoricalBuildData, naeHistoricalBuildPk} from './nae-utils'

type HistoricalBuildKey = {
  pk: string
  sk: number
}

const putBuildDataToDb = async (
  tableName: string, appUuid: string, historicalBuildData: HistoricalBuildData
) => {
  const {buildRequestTimestampMs} = historicalBuildData

  await Ddb.use().putItem({
    TableName: tableName,
    Item: {
      pk: {S: naeHistoricalBuildPk(appUuid)},
      sk: {N: buildRequestTimestampMs.toString()},
      ...toAttributes(historicalBuildData),
    },
  })
}

const tryFetchHistoricalBuild = async (
  tableName: string,
  historicalBuildKey: HistoricalBuildKey
): Promise<HistoricalBuildData | null> => {
  const projectionExpressionKeys = Object.keys(defaultHistoricalBuildData())

  const reservedWords = new Set(['status'])
  const expressionAttributeNames: Record<string, string> = {}
  const projectionExpression = projectionExpressionKeys
    .map((key) => {
      if (reservedWords.has(key)) {
        expressionAttributeNames[`#${key}`] = key
        return `#${key}`
      }
      return key
    })
    .join(',')

  const rawRequestItem = (await Ddb.use().getItem({
    Key: {
      pk: {S: historicalBuildKey.pk},
      sk: {N: historicalBuildKey.sk.toString()},
    },
    ProjectionExpression: projectionExpression,
    ExpressionAttributeNames: expressionAttributeNames,
    TableName: tableName,
  })).Item as AttributesForRaw<HistoricalBuildData>
  if (!rawRequestItem) {
    return null
  }

  return fromAttributes(rawRequestItem)
}

export {
  defaultHistoricalBuildData,
  putBuildDataToDb,
  tryFetchHistoricalBuild,
}

export type {
  HistoricalBuildData,
  HistoricalBuildKey,
}
