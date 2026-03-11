import React from 'react'

import {BufferGeometry, BufferAttribute} from 'three'

import {
  FaceMeshPositions,
  FaceMeshNormals,
  FaceMeshUvs,
  FaceMeshIndices,
  FaceMeshEyeIndices,
  FaceMeshMouthIndices,
  FaceMeshIrisIndices,
  EarLMeshPositions,
  EarLMeshIndices,
} from '@ecs/shared/face-mesh-data'

const getPartialFaceMeshIndices = (
  hasFace: boolean, hasEyes: boolean, hasIris: boolean, hasMouth: boolean
): Uint32Array => {
  let arrayCount = 0
  if (hasFace) {
    arrayCount += FaceMeshIndices.length
  }
  if (hasEyes) {
    arrayCount += FaceMeshEyeIndices.length
  }
  if (hasIris) {
    arrayCount += FaceMeshIrisIndices.length
  }
  if (hasMouth) {
    arrayCount += FaceMeshMouthIndices.length
  }

  const indexData = new Uint32Array(arrayCount)
  let fi = 0
  if (hasFace) {
    for (let i = 0; i < FaceMeshIndices.length; i++) {
      indexData[fi + i] = FaceMeshIndices[i]
    }
    fi += FaceMeshIndices.length
  }
  if (hasEyes) {
    for (let i = 0; i < FaceMeshEyeIndices.length; i++) {
      indexData[fi + i] = FaceMeshEyeIndices[i]
    }
    fi += FaceMeshEyeIndices.length
  }
  if (hasIris) {
    for (let i = 0; i < FaceMeshIrisIndices.length; i++) {
      indexData[fi + i] = FaceMeshIrisIndices[i]
    }
    fi += FaceMeshIrisIndices.length
  }
  if (hasMouth) {
    for (let i = 0; i < FaceMeshMouthIndices.length; i++) {
      indexData[fi + i] = FaceMeshMouthIndices[i]
    }
    fi += FaceMeshMouthIndices.length
  }

  return indexData
}

interface IFaceMeshGeometry {
  hasFace: boolean
  hasEyes: boolean
  hasIris: boolean
  hasMouth: boolean
  includeUv: boolean
}

const FaceMeshGeometry: React.FC<IFaceMeshGeometry> = (
  {hasFace, hasEyes, hasIris, hasMouth, includeUv}
) => {
  const bufferGeometryRef = React.useRef<BufferGeometry>(null)

  const meshIndices = React.useMemo(() => {
    const indices = getPartialFaceMeshIndices(hasFace, hasEyes, hasIris, hasMouth)
    const geometry = bufferGeometryRef.current
    if (geometry) {
      geometry.setIndex(new BufferAttribute(indices, 1))
      geometry.index.needsUpdate = true
    }
    return indices
  }, [hasFace, hasEyes, hasIris, hasMouth])

  return (
    <bufferGeometry ref={bufferGeometryRef}>
      <bufferAttribute
        attach='attributes-position'
        array={FaceMeshPositions}
        count={FaceMeshPositions.length / 3}
        itemSize={3}
      />
      <bufferAttribute
        attach='attributes-normal'
        array={FaceMeshNormals}
        count={FaceMeshNormals.length / 3}
        itemSize={3}
      />
      {includeUv &&
        <bufferAttribute
          attach='attributes-uv'
          array={FaceMeshUvs}
          count={FaceMeshUvs.length / 2}
          itemSize={2}
        />}
      <bufferAttribute
        attach='index'
        array={meshIndices}
        count={meshIndices.length}
        itemSize={1}
      />
    </bufferGeometry>
  )
}

const makeEarLMeshGeometry = () => (
  <bufferGeometry>
    <bufferAttribute
      attach='attributes-position'
      array={EarLMeshPositions}
      count={EarLMeshPositions.length / 3}
      itemSize={3}
    />
    <bufferAttribute
      attach='index'
      array={EarLMeshIndices}
      count={EarLMeshIndices.length}
      itemSize={1}
    />
  </bufferGeometry>
)

const makeEarRMeshGeometry = () => {
  const mirrorVertices = new Float32Array(EarLMeshPositions.length)
  for (let i = 0; i < EarLMeshPositions.length / 3; i++) {
    mirrorVertices[3 * i] = -EarLMeshPositions[3 * i]
    mirrorVertices[3 * i + 1] = EarLMeshPositions[3 * i + 1]
    mirrorVertices[3 * i + 2] = EarLMeshPositions[3 * i + 2]
  }

  return (
    <bufferGeometry>
      <bufferAttribute
        attach='attributes-position'
        array={mirrorVertices}
        count={mirrorVertices.length / 3}
        itemSize={3}
      />
      <bufferAttribute
        attach='index'
        array={EarLMeshIndices}
        count={EarLMeshIndices.length}
        itemSize={1}
      />
    </bufferGeometry>
  )
}

export {
  makeEarLMeshGeometry,
  makeEarRMeshGeometry,
  FaceMeshGeometry,
}
