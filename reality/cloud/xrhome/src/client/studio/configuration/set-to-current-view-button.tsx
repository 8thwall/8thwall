import {useTranslation} from 'react-i18next'
import React from 'react'
import {Euler, Quaternion, Vector3} from 'three'
import type {DeepReadonly} from 'ts-essentials'

import type {GraphObject} from '@ecs/shared/scene-graph'

import {useDerivedScene} from '../derived-scene-context'
import {useSceneContext} from '../scene-context'
import {sceneInterfaceContext} from '../scene-interface-context'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {getAncestorMatrix} from '../object-transforms'
import {useStudioStateContext} from '../studio-state-context'

interface ISetToCurrentViewButton {
  object: DeepReadonly<GraphObject>
}

const SetToCurrentViewButton: React.FC<ISetToCurrentViewButton> = ({
  object,
}) => {
  const {t} = useTranslation('cloud-studio-pages')

  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const sceneInterfaceCtx = React.useContext(sceneInterfaceContext)
  const stateCtx = useStudioStateContext()

  return (
    <FloatingPanelButton
      spacing='full'
      onClick={() => {
        const cameraMatrixWorld = sceneInterfaceCtx.getCameraTransform()
        const ancestorMatrix = getAncestorMatrix(object, derivedScene)
        const cameraMatrixLocal = ancestorMatrix.invert().multiply(cameraMatrixWorld)

        const position = new Vector3()
        const quaternion = new Quaternion()
        const scale = new Vector3()
        cameraMatrixLocal.decompose(position, quaternion, scale)

        // NOTE(christoph): Rotate 180 degrees around Y axis to account for our cameras aiming
        // on the positive Z
        quaternion.multiply(new Quaternion().setFromEuler(new Euler(0, Math.PI, 0))).normalize()

        ctx.updateObject(object.id, obj => ({
          ...obj,
          position: position.toArray(),
          rotation: [quaternion.x, quaternion.y, quaternion.z, quaternion.w],
          camera: obj.camera
            ? {
              ...obj.camera,
              type: stateCtx.state.cameraView,
            }
            : undefined,
        }))
      }}
    >{t('transform_configurator.button.set_to_current_view')}
    </FloatingPanelButton>
  )
}

export {
  SetToCurrentViewButton,
}
