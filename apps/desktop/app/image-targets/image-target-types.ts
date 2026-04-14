import {z} from 'zod'

const ListTargetsParams = z.object({
  appKey: z.string().nonempty(),
})

const GetTextureParams = z.object({
  appKey: z.string().nonempty(),
  name: z.string().nonempty(),
  type: z.enum([
    'original',
    'thumbnail',
    'cropped',
    'geometry',
    'luminance',
  ]),
})

export {
  ListTargetsParams,
  GetTextureParams,
}
