import {z} from 'zod'

const ProjectRequestParams = z.object({
  appKey: z.string(),
})

type IProjectRequestParams = z.infer<typeof ProjectRequestParams>

const InitializeProjectParams = z.object({
  appName: z.string().nonempty(),
  location: z.enum(['default', 'prompt']),
  templateZipUrl: z.string().optional(),
})

type IInitializeProjectParams = z.infer<typeof InitializeProjectParams>

const OpenDiskParams = z.object({
  location: z.string().optional(),
  acceptNonStudio: z.enum(['true', 'false']).optional(),
})

const MoveProjectParams = z.object({
  appKey: z.string(),
  newLocation: z.string().nonempty().optional(),
})

type IMoveProjectParams = z.infer<typeof MoveProjectParams>

const FixConfigParams = z.object({
  appKey: z.string(),
  fix: z.enum(['inject', 'copy-plugin', 'dev-socket']),
})

type IFixConfigParams = z.infer<typeof FixConfigParams>

const InstallRequest = z.object({
  packages: z.array(z.object({name: z.enum(['@8thwall/ecs']), version: z.string()})),
})

export {
  ProjectRequestParams,
  InitializeProjectParams,
  MoveProjectParams,
  FixConfigParams,
  InstallRequest,
  OpenDiskParams,
}

export type {
  IProjectRequestParams,
  IInitializeProjectParams,
  IMoveProjectParams,
  IFixConfigParams,
}
