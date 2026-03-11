import {z} from 'zod'

const FilePullParams = z.object({
  appKey: z.string().nonempty(),
  path: z.string().nonempty(),
})

type IFilePullParams = z.infer<typeof FilePullParams>

const FilePushParams = z.object({
  appKey: z.string().nonempty(),
  path: z.string().nonempty(),
})

type IFilePushParams= z.infer<typeof FilePushParams>

const FileDeleteParams = z.object({
  appKey: z.string().nonempty(),
  path: z.string().nonempty(),
})

type IFileDeleteParams= z.infer<typeof FileDeleteParams>

const FileStateParams = z.object({
  appKey: z.string().nonempty(),
})

type IFileStateParams = z.infer<typeof FileStateParams>

const FileMetadataParams = z.object({
  appKey: z.string().nonempty(),
  path: z.string().nonempty(),
})

type IFileMetadataParams = z.infer<typeof FileMetadataParams>

const FileRenameParams = z.object({
  appKey: z.string().nonempty(),
  oldPath: z.string().nonempty(),
  newPath: z.string().nonempty(),
})

type IFileRenameParams = z.infer<typeof FileRenameParams>

export {
  FilePullParams,
  FilePushParams,
  FileDeleteParams,
  FileStateParams,
  FileMetadataParams,
  FileRenameParams,
}

export type {
  IFilePullParams,
  IFilePushParams,
  IFileDeleteParams,
  IFileStateParams,
  IFileMetadataParams,
  IFileRenameParams,
}
