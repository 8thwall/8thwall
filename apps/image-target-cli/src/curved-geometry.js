/**
 * @param {number} topRadius
 * @param {number} bottomRadius
 * @returns {number}
 */
const getCircumferenceRatio = (topRadius, bottomRadius) => {
  if (topRadius < 0) {
    return -bottomRadius / topRadius
  } else {
    return topRadius / bottomRadius
  }
}

/**
 * @param {number} topRadius
 * @param {number} bottomRadius
 * @returns {number}
 */
const getConinessForRadii = (topRadius, bottomRadius) => {
  const baseConiness = Math.log2(Math.abs(topRadius / bottomRadius))

  const signedConiness = topRadius < 0 ? -baseConiness : baseConiness

  return signedConiness
}

/**
 * @param {number} targetCircumferenceTop
 * @param {number} cylinderCircumferenceTop
 * @param {number} cylinderCircumferenceBottom
 * @returns {number}
 */
const getTargetCircumferenceBottom = (
  targetCircumferenceTop, cylinderCircumferenceTop, cylinderCircumferenceBottom
) => (targetCircumferenceTop * cylinderCircumferenceBottom) / cylinderCircumferenceTop

export {
  getCircumferenceRatio,
  getConinessForRadii,
  getTargetCircumferenceBottom,
}
