import {z} from 'zod'

const ProjectRequestParams = z.object({
  appKey: z.string(),
})

type IProjectRequestParams = z.infer<typeof ProjectRequestParams>

const InitializeProjectParams = z.object({
  appName: z.string().nonempty(),
  location: z.enum(['default', 'prompt']),
})

type IInitializeProjectParams = z.infer<typeof InitializeProjectParams>

const MoveProjectParams = z.object({
  appKey: z.string(),
  newLocation: z.string().nonempty().optional(),
})

type IMoveProjectParams = z.infer<typeof MoveProjectParams>

const FixConfigParams = z.object({
  appKey: z.string(),
  fix: z.enum(['inject']),
})

type IFixConfigParams = z.infer<typeof FixConfigParams>

export {
  ProjectRequestParams,
  InitializeProjectParams,
  MoveProjectParams,
  FixConfigParams,
}

export type {
  IProjectRequestParams,
  IInitializeProjectParams,
  IMoveProjectParams,
  IFixConfigParams,
}
