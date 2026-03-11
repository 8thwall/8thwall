import React from 'react'
import {TransformControls} from '@react-three/drei'
import type {DeepReadonly} from 'ts-essentials'
import {Matrix4, Quaternion, Vector3} from 'three'
import type {GraphObject} from '@ecs/shared/scene-graph'
import {degreesToRadians} from '@ecs/shared/angle-conversion'

import {useSceneContext} from './scene-context'
import {useSelectedObjects} from './hooks/selected-objects'
import {useStudioStateContext} from './studio-state-context'
import {useChangeEffect} from '../hooks/use-change-effect'
import {useEvent} from '../hooks/use-event'
import {Keys} from './common/keys'
import {getRootAttributeIdFromScene} from './hooks/use-root-attribute-id'
import {shouldShowTransform} from './configuration/object-configurator'
import type {DerivedScene} from './derive-scene'
import {useDerivedScene} from './derived-scene-context'
import {computeGizmoCenter, getAncestorMatrix, getObjectInWorldMatrix} from './object-transforms'

const tempQuaternion = new Quaternion()
const tempPositionVector = new Vector3()
const tempScaleVector = new Vector3()

const TRANSLATION_SNAP = 1
const ROTATION_SNAP = degreesToRadians(15)
const SCALE_SNAP = 1

const decomposeIntoObject = (() => (
  matrix: Matrix4, object: DeepReadonly<GraphObject>
): DeepReadonly<GraphObject> => {
  matrix.decompose(tempPositionVector, tempQuaternion, tempScaleVector)
  return {
    ...object,
    position: tempPositionVector.toArray(),
    rotation: tempQuaternion.toArray() as GraphObject['rotation'],
    scale: tempScaleVector.toArray(),
  }
})()

const getDifferenceMatrix = (matrix1: Matrix4, matrix2: Matrix4) => {
  const inverseMatrix1 = matrix1.clone().invert()
  const transformationMatrix = new Matrix4().multiplyMatrices(inverseMatrix1, matrix2)
  return transformationMatrix
}

const getPivotTranform = (
  pivot: Matrix4,
  local: Matrix4
) => {
  const calculatedDL = getDifferenceMatrix(pivot, local)

  const toPivot = pivot.clone().invert()
  const fromPivot = toPivot.clone().invert()

  const pivotTransform = new Matrix4().multiply(fromPivot).multiply(calculatedDL)
    .multiply(toPivot)

  return pivotTransform
}

const getTransformedObjectMatrix = (
  object: DeepReadonly<GraphObject>,
  transformMatrix: Matrix4,
  derivedScene: DerivedScene
) => {
  const objInWorldMatrix = getObjectInWorldMatrix(object, derivedScene)
  const invertAncestorMatrix = getAncestorMatrix(object, derivedScene).invert()

  const transformWithAncestor = new Matrix4().multiplyMatrices(transformMatrix, objInWorldMatrix)
  const revertMatrix = new Matrix4().multiplyMatrices(invertAncestorMatrix, transformWithAncestor)
  return revertMatrix
}

const getRootIds = (
  ids: DeepReadonly<string[]>,
  derivedScene: DerivedScene
) => {
  const rootIds = new Set(ids)
  ids.forEach((id) => {
    let parent = derivedScene.getObject(id)
    while (parent) {
      if (rootIds.has(parent.parentId)) {
        rootIds.delete(id)
        return
      }
      parent = derivedScene.getObject(parent.parentId)
    }
  })
  return Array.from(rootIds)
}

const getRootObjects = (
  ids: DeepReadonly<string[]>,
  derivedScene: DerivedScene
) => {
  const rootIds = getRootIds(ids, derivedScene)
  return rootIds.map(id => derivedScene.getObject(id)).filter(Boolean)
}

const filterSceneObject = (
  obj: DeepReadonly<GraphObject>,
  derivedScene: DerivedScene
) => {
  const rootUiId = getRootAttributeIdFromScene(derivedScene, obj.id, 'ui')
  return shouldShowTransform(obj, rootUiId)
}

const TransformGizmo: React.FC<{disabled?: boolean}> = ({disabled = false}) => {
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const ids = stateCtx.state.selectedIds
  const objects = useSelectedObjects().filter(obj => filterSceneObject(obj, derivedScene))
  const rootObjectsRef = React.useRef(getRootObjects(ids, derivedScene))
  const pendingDragRef = React.useRef(new Matrix4())
  const pivotControlRef = React.useRef(null)
  const [gizmoPosition, setGizmoPosition] = React.useState(
    computeGizmoCenter(objects, derivedScene)
  )
  const [gizmoQuaternion, setGizmoQuaternion] = React.useState(new Quaternion())
  const [gizmoScale, setGizmoScale] = React.useState(new Vector3(1, 1, 1))
  const [enableTransformSnap, setEnableTransformSnap] = React.useState(false)

  React.useEffect(() => () => {
    ctx.setIsDraggingGizmo(false)
  }, [])

  React.useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key === Keys.SHIFT) {
        setEnableTransformSnap(true)
      }
    }

    const handleKeyUp = (event: KeyboardEvent) => {
      if (event.key === Keys.SHIFT) {
        setEnableTransformSnap(false)
      }
    }

    window.addEventListener('keydown', handleKeyDown)
    window.addEventListener('keyup', handleKeyUp)

    return () => {
      window.removeEventListener('keydown', handleKeyDown)
      window.removeEventListener('keyup', handleKeyUp)
    }
  }, [])

  useChangeEffect(([prevIds]) => {
    rootObjectsRef.current = getRootObjects(ids, derivedScene)
    const hasSameIds = ids.length === prevIds?.length &&
      ids.every((id, index) => id === prevIds[index])
    if (!hasSameIds) {
      setGizmoPosition(computeGizmoCenter(objects, derivedScene))
      setGizmoQuaternion(new Quaternion())
      setGizmoScale(new Vector3(1, 1, 1))
    }
  }, [ids])

  React.useEffect(() => {
    if (!ctx.isDraggingGizmo) {
      rootObjectsRef.current = getRootObjects(ids, derivedScene)
      setGizmoPosition(computeGizmoCenter(objects, derivedScene))
    }
  }, [ctx.scene])

  const handleOnDrag = useEvent((start: Matrix4, local: Matrix4) => {
    const pivotTransform = getPivotTranform(start, local)
    rootObjectsRef.current.forEach((obj) => {
      const transformedMatrix = getTransformedObjectMatrix(obj, pivotTransform, derivedScene)
      ctx.updateObject(obj.id, o => decomposeIntoObject(transformedMatrix, o))
    })
  })

  if (objects.length < 1 || !stateCtx.state.transformMode) {
    return null
  }

  return (
    <TransformControls
      ref={pivotControlRef}
      enabled={!disabled}
      mode={stateCtx.state.transformMode}
      position={gizmoPosition}
      quaternion={gizmoQuaternion}
      scale={gizmoScale}
      translationSnap={enableTransformSnap ? TRANSLATION_SNAP : 0}
      rotationSnap={enableTransformSnap ? ROTATION_SNAP : 0}
      scaleSnap={enableTransformSnap ? SCALE_SNAP : 0}
      onMouseDown={() => {
        ctx.setIsDraggingGizmo(true)
        pendingDragRef.current = null
      }}
      onObjectChange={() => {
        const start = new Matrix4().compose(
          pivotControlRef.current.worldPositionStart,
          pivotControlRef.current.worldQuaternionStart,
          pivotControlRef.current.worldScaleStart
        )
        pendingDragRef.current = new Matrix4().compose(
          pivotControlRef.current.worldPosition,
          pivotControlRef.current.worldQuaternion,
          pivotControlRef.current.worldScale
        )
        handleOnDrag(start, pendingDragRef.current)
      }}
      onMouseUp={() => {
        ctx.setIsDraggingGizmo(false)
        if (pendingDragRef.current) {
          rootObjectsRef.current = getRootObjects(ids, derivedScene)
          pendingDragRef.current.decompose(
            tempPositionVector,
            tempQuaternion,
            tempScaleVector
          )
          setGizmoPosition(tempPositionVector)
          setGizmoQuaternion(tempQuaternion)
          setGizmoScale(tempScaleVector)
        }
      }}
    />
  )
}

export {
  TransformGizmo,
}
