import {useThree} from '@react-three/fiber'
import React, {useEffect, useRef} from 'react'
import {createUseStyles} from 'react-jss'
import {SelectionBox} from 'three/examples/jsm/interactive/SelectionBox'
import {Vector3} from 'three'

import {SelectionHelper} from '../../third_party/three/SelectionHelper'
import {useSceneContext} from './scene-context'
import {useStudioStateContext} from './studio-state-context'
import type {CanvasBounds} from './hooks/use-scene-camera'

const tempVector = new Vector3()

// NOTE(christoph): Drags that are smaller than this are ignored. This distance is in screen space
// been x/y -1 and 1.
const MINIMUM_SELECTION_BOX_DISTANCE = 0.01

const useStyles = createUseStyles({
  selectBox: {
    border: '1px solid rgba(128, 178, 255, 0.6)',
    backgroundColor: 'rgba(128, 178, 255, 0.2)',
    position: 'fixed',
  },
})

interface ISelectionBoxHelper {
  canvasBounds: CanvasBounds
}

const SelectionBoxHelper: React.FC<ISelectionBoxHelper> = ({canvasBounds}) => {
  const classes = useStyles()
  const three = useThree()
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const {objects} = ctx.scene

  const helperRef = useRef<SelectionHelper>(null)

  const dispose = () => {
    const {element} = helperRef.current
    const {parentElement} = element
    if (parentElement && parentElement.contains(element)) {
      parentElement.removeChild(element)
    }
    helperRef.current.dispose()
  }

  const initializeHelper = () => {
    helperRef.current = new SelectionHelper(three.gl, classes.selectBox, canvasBounds)
  }

  useEffect(() => {
    initializeHelper()
    return () => dispose()
  }, [])

  useEffect(() => {
    const selectionBox = new SelectionBox(three.camera, three.scene)

    const handlePointerDown = (event: PointerEvent) => {
      if (event.button !== 0 || ctx.isDraggingGizmoRef.current) {
        helperRef.current.isDown = false
        return
      }
      selectionBox.startPoint.set(
        ((event.clientX - canvasBounds.left) / canvasBounds.width) * 2 - 1,
        -((event.clientY - canvasBounds.top) / canvasBounds.height) * 2 + 1,
        0.5
      )
    }

    const handlePointerMove = (event: PointerEvent) => {
      if (helperRef.current.isDown && !ctx.isDraggingGizmoRef.current) {
        const ele = document.elementFromPoint(event.clientX, event.clientY)
        if (ele && ele !== three.gl.domElement) {
          // need to recreate the helper element to properly reset the selection state
          dispose()
          initializeHelper()
          return
        }

        selectionBox.endPoint.set(
          ((event.clientX - canvasBounds.left) / canvasBounds.width) * 2 - 1,
          -((event.clientY - canvasBounds.top) / canvasBounds.height) * 2 + 1,
          0.5
        )

        const dragDistance = tempVector
          .copy(selectionBox.startPoint)
          .sub(selectionBox.endPoint)
          .length()

        if (dragDistance < MINIMUM_SELECTION_BOX_DISTANCE) {
          return
        }

        const allSelectedIds = selectionBox.select()
          .filter(obj => !!objects[obj.name])
          .map(obj => obj.name)

        stateCtx.setSelection(...allSelectedIds)
      }
    }

    // NOTE(Carson): These should be separate, since we should take note of dragging the pointer
    // off of the canvas, to remove the box, but we don't want to start the selection box off of
    // the canvas.
    three.gl.domElement.addEventListener('pointerdown', handlePointerDown)
    document.addEventListener('pointermove', handlePointerMove)

    return () => {
      three.gl.domElement.removeEventListener('pointerdown', handlePointerDown)
      document.removeEventListener('pointermove', handlePointerMove)
    }
  }, [three.camera, three.scene, objects, canvasBounds])

  return null
}

export {SelectionBoxHelper}
