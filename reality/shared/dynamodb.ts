// @attr(visibility = ["//visibility:public"])

import type {
  GetItemCommandInput, GetItemCommandOutput, PutItemCommandInput, PutItemCommandOutput,
  DeleteItemCommandInput, DeleteItemCommandOutput, QueryCommandInput, QueryCommandOutput,
  TransactWriteItemsInput, TransactWriteItemsOutput, BatchGetItemInput, BatchGetItemOutput,
  BatchWriteItemInput, BatchWriteItemOutput, UpdateItemInput, UpdateItemOutput,
} from '@aws-sdk/client-dynamodb'

type DynamoDbApi = {
  putItem: (input: PutItemCommandInput) => Promise<PutItemCommandOutput>
  getItem: (input: GetItemCommandInput) => Promise<GetItemCommandOutput>
  deleteItem: (input: DeleteItemCommandInput) => Promise<DeleteItemCommandOutput>
  query: (input: QueryCommandInput) => Promise<QueryCommandOutput>
  transactWriteItems: (input: TransactWriteItemsInput) => Promise<TransactWriteItemsOutput>
  batchGetItem: (input: BatchGetItemInput) => Promise<BatchGetItemOutput>
  batchWriteItem: (input: BatchWriteItemInput) => Promise<BatchWriteItemOutput>
  updateItem: (input: UpdateItemInput) => Promise<UpdateItemOutput>
}

let ddb: DynamoDbApi | null = null

const register = (impl: DynamoDbApi) => {
  ddb = impl
}

const use = (): DynamoDbApi => ddb!

const Ddb = {
  register,
  use,
}

export {
  Ddb,
  register,
  use,
}

export type {
  DynamoDbApi,
}
