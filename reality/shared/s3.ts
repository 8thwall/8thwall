// @attr(visibility = ["//visibility:public"])

import type S3ClientApi from '@aws-sdk/client-s3'

import {entry} from '@nia/reality/cloud/xrhome/src/shared/registry'

type CommandFunction<Command> = Command extends S3ClientApi.$Command<
infer I, infer O, any, any, any
> ? (input: I) => Promise<O> : never

type S3Api = {
  putObject: CommandFunction<S3ClientApi.PutObjectCommand>
  getObject: CommandFunction<S3ClientApi.GetObjectCommand>
  deleteObject: CommandFunction<S3ClientApi.DeleteObjectCommand>
  deleteObjects: CommandFunction<S3ClientApi.DeleteObjectsCommand>
  copyObject: CommandFunction<S3ClientApi.CopyObjectCommand>
  listObjectsV2: CommandFunction<S3ClientApi.ListObjectsV2Command>
}

const S3 = entry<S3Api>('S3')

export {
  S3,
}

export type {
  S3Api,
}
