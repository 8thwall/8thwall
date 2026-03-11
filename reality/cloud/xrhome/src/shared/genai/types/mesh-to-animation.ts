import type {MeshToAnimationBase} from './base'

type MeshToAnimationMeshyAi = MeshToAnimationBase & {
  modelId: 'meshy-ai'
}

type MeshToAnimation =
  | MeshToAnimationMeshyAi

export type {
  MeshToAnimation,
  MeshToAnimationMeshyAi,
}
