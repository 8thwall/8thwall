// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Akul Gupta (akulgupta@nianticlabs.com)

declare const THREE: any

// Convert raw geometry data to three.js geometry types
const rawGeoToThreejsBufferGeometry = (geometryData: Record<string, any>) => {
  const geometry = new THREE.BufferGeometry()

  if (geometryData.index) {
    geometry.setIndex(new THREE.BufferAttribute(geometryData.index.array, 1))
  }

  geometryData.attributes.forEach((attribute) => {
    const {name, array, itemSize} = attribute
    geometry.setAttribute(name, new THREE.BufferAttribute(array, itemSize))
  })

  geometry.computeVertexNormals()

  return geometry
}

export {
  rawGeoToThreejsBufferGeometry,
}
