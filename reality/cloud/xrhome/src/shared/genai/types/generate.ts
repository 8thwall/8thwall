import type {ImageToImage} from './image-to-image'
import type {ImageToMesh} from './image-to-mesh'
import type {TextToImage} from './text-to-image'
import type {MeshToAnimation} from './mesh-to-animation'

type GenerateRequest =
  | TextToImage
  | ImageToImage
  | ImageToMesh
  | MeshToAnimation

type AllKeys<T> = T extends any ? keyof T : never
type GenerateRequestKeys = AllKeys<GenerateRequest>
// "images" and "mesh" get rewritten as "imageUrls" and "meshUrl" in the DB.
interface GenerateRequestInput extends Omit<Record<GenerateRequestKeys, any>, 'images' | 'mesh'> {
  imageUrls: string[]
  meshUrl: string
}

export {
  GenerateRequest,
  GenerateRequestInput,
}
