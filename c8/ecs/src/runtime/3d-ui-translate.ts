// three-mesh-ui use scene coordinates where 0,0 is the center of the plane with
// X going right and Y going up while layout system uses top-left corner as 0,0
// with Left going right and Top going down. These functions translate between the
// two coordinate systems.

/* Translates layout Left to scene X
 * parentWidth: width of the parent object
 * width: width of the object
 * left: left position of the object
 */
const translateX = (parentWidth?: number, width?: number, left?: number) => {
  // no parentWidth means it's a root node, center object at its position
  if (parentWidth === undefined) {
    return 0
  }
  // Offset based on top-left corner of parent, not center.
  // 0,0 is the center of the plane, X goes right
  return (-0.5 * parentWidth + (left ?? 0) + 0.5 * (width ?? 0))
}

/* Translates layout Top to scene Y
  * parentHeight: height of the parent object
  * height: height of the object
  * top: top position of the object
  */
const translateY = (parentHeight?: number, height?: number, top?: number) => {
  // no parentHeight means it's a root node, center object at its position
  if (parentHeight === undefined) {
    return 0
  }
  // Offset based on top-left corner of parent, not center.
  // 0,0 is the center of the plane, Y goes up
  return ((0.5 * parentHeight) - (top ?? 0) - 0.5 * (height ?? 0))
}

export {
  translateX,
  translateY,
}
