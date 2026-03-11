type CurvedGeometryUpdate = Partial<{
  arcAngle: number
  coniness: number
  cylinderCircumferenceTop: number
  cylinderCircumferenceBottom: number
  cylinderSideLength: number
  targetCircumferenceTop: number
}>

type CurvedGeometryUnit = 'in' | 'mm'

const getTargetCircumferenceBottom = (
  targetCircumferenceTop: number,
  cylinderCircumferenceTop: number,
  cylinderCircumferenceBottom: number
) => targetCircumferenceTop * (cylinderCircumferenceBottom / cylinderCircumferenceTop)

// When filling a calculated field, we don't want to have a large number of decimals, so round to
// hundredths before saving the value
const toHundredths = (n: number) => Math.round(n * 100) / 100

const MIN_CIRCUMFERENCE = 0.01
const MIN_CYLINDER_SIDE_LENGTH = 0.01
const MIN_ARC_ANGLE = 80.0
const MAX_ARC_ANGLE = 360.0
const MIN_CONINESS = -2.0
const MAX_CONINESS = 2.0

const DEFAULT_CYLINDER_CIRCUMFERENCE_TOP = 150.0
const DEFAULT_CYLINDER_CIRCUMFERENCE_BOTTOM = 150.0
const DEFAULT_CYLINDER_SIDE_LENGTH = 100.0
const DEFAULT_CONINESS = 0.0
const DEFAULT_ARC_ANGLE = 180.0
const DEFAULT_TARGET_CIRCUMFERENCE = 100.0
const DEFAULT_UNIT: CurvedGeometryUnit = 'mm'

// Returns the ratio between circumferenceTop and circumferenceBottom, using top/bottomRadius
// if available, otherwise using the preexisting circumference values.
// circumferenceTop/Bottom can degrade because of rounding, but if the radii haven't been set
// on the target, we fall back to circumference.
const getCircumferenceRatio = (
  circumferenceTop: number, circumferenceBottom: number, topRadius?: number, bottomRadius?: number
) => {
  if (topRadius && bottomRadius) {
    if (topRadius < 0) {
      return -bottomRadius / topRadius
    } else {
      return topRadius / bottomRadius
    }
  }
  return circumferenceTop / circumferenceBottom
}

const getConinessForRadii = (topRadius: number, bottomRadius: number) => {
  if (typeof topRadius !== 'number' || typeof bottomRadius !== 'number') {
    return DEFAULT_CONINESS
  }

  if (Number.isNaN(topRadius) || Number.isNaN(bottomRadius)) {
    return DEFAULT_CONINESS
  }

  if (topRadius === 0) {
    return DEFAULT_CONINESS
  }

  if (bottomRadius === 0) {
    return topRadius > 0 ? MAX_CONINESS : MIN_CONINESS
  }

  const baseConiness = Math.log2(Math.abs(topRadius / bottomRadius))

  const signedConiness = topRadius < 0 ? -baseConiness : baseConiness

  return Math.min(MAX_CONINESS, Math.max(MIN_CONINESS, signedConiness))
}

export {
  CurvedGeometryUpdate,
  CurvedGeometryUnit,
  getTargetCircumferenceBottom,
  toHundredths,
  getCircumferenceRatio,
  getConinessForRadii,
  MIN_CIRCUMFERENCE,
  MIN_CYLINDER_SIDE_LENGTH,
  MIN_ARC_ANGLE,
  MAX_ARC_ANGLE,
  MIN_CONINESS,
  MAX_CONINESS,
  DEFAULT_CYLINDER_CIRCUMFERENCE_TOP,
  DEFAULT_CYLINDER_CIRCUMFERENCE_BOTTOM,
  DEFAULT_CYLINDER_SIDE_LENGTH,
  DEFAULT_CONINESS,
  DEFAULT_ARC_ANGLE,
  DEFAULT_TARGET_CIRCUMFERENCE,
  DEFAULT_UNIT,
}
