import {z} from 'zod'

const ListTargetsParams = z.object({
  appKey: z.string().nonempty(),
})

const CropGeometry = z.object({
  top: z.number(),
  left: z.number(),
  width: z.number(),
  height: z.number(),
  isRotated: z.boolean().optional(),
  originalWidth: z.number(),
  originalHeight: z.number(),
})

const CylinderCropGeometry = z.object({
  targetCircumferenceTop: z.number(),
  cylinderSideLength: z.number(),
  cylinderCircumferenceTop: z.number(),
  cylinderCircumferenceBottom: z.number(),
  arcAngle: z.number(),
  coniness: z.number(),
  inputMode: z.enum(['ADVANCED']),
  unit: z.enum(['mm', 'in']),
}).and(CropGeometry)

const ConicalCropGeometry = z.object({
  topRadius: z.number(),
  bottomRadius: z.number(),
}).and(CylinderCropGeometry)

const CropResult = z.discriminatedUnion('type', [
  z.object({
    type: z.literal('PLANAR'),
    properties: CropGeometry,
  }),
  z.object({
    type: z.literal('CYLINDER'),
    properties: CylinderCropGeometry,
  }),
  z.object({
    type: z.literal('CONICAL'),
    properties: ConicalCropGeometry,
  }),
])

const UploadTargetParams = z.object({
  appKey: z.string().nonempty(),
  name: z.string().nonempty(),
  crop: z.string(),
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

const DeleteTargetParams = z.object({
  appKey: z.string().nonempty(),
  name: z.string().nonempty(),
})

export {
  ListTargetsParams,
  GetTextureParams,
  CropResult,
  UploadTargetParams,
  DeleteTargetParams,
}
