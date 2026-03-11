import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'
import type {Collider as SceneCollider} from '@ecs/shared/scene-graph'
import {
  DEFAULT_OFFSET_QUATERNION_X, DEFAULT_OFFSET_QUATERNION_Y,
  DEFAULT_OFFSET_QUATERNION_Z, DEFAULT_OFFSET_QUATERNION_W,
} from '@ecs/shared/physic-constants'

import {RowGroupFields} from './row-fields'
import {SliderInputAxisField} from '../../ui/components/slider-input-axis-field'
import {normalizeDegrees} from '../quaternion-conversion'
import {useEulerEdit} from './euler-edit'

type Collider = DeepReadonly<SceneCollider>

interface IRotationOffsetConfigurator {
  collider: Collider
  onChange: (updater: (current: Collider) => Collider) => void
}

const RotationOffsetConfigurator: React.FC<IRotationOffsetConfigurator> = ({
  collider, onChange,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const getCurrentQuat = (): [number, number, number, number] => [
    collider.offsetQuaternionX ?? DEFAULT_OFFSET_QUATERNION_X,
    collider.offsetQuaternionY ?? DEFAULT_OFFSET_QUATERNION_Y,
    collider.offsetQuaternionZ ?? DEFAULT_OFFSET_QUATERNION_Z,
    collider.offsetQuaternionW ?? DEFAULT_OFFSET_QUATERNION_W,
  ]

  const updateQuat = (newQuat: [number, number, number, number]) => {
    onChange(old => ({
      ...old,
      offsetQuaternionX: newQuat[0],
      offsetQuaternionY: newQuat[1],
      offsetQuaternionZ: newQuat[2],
      offsetQuaternionW: newQuat[3],
    }))
  }

  const rotationOffsetEdit = useEulerEdit(
    [getCurrentQuat()],
    newQuats => updateQuat(newQuats[0] as [number, number, number, number])
  )

  return (
    <RowGroupFields label={t('collider_configurator.rotation_offset.label')}>
      <>
        <SliderInputAxisField
          id='collider-rotation-offset-x'
          label='X'
          value={rotationOffsetEdit.x}
          onChange={rotationOffsetEdit.setX}
          onDrag={rotationOffsetEdit.setDragX}
          normalizer={normalizeDegrees}
          step={1}
          fullWidth
        />
        <SliderInputAxisField
          id='collider-rotation-offset-y'
          label='Y'
          value={rotationOffsetEdit.y}
          onChange={rotationOffsetEdit.setY}
          onDrag={rotationOffsetEdit.setDragY}
          normalizer={normalizeDegrees}
          step={1}
          fullWidth
        />
        <SliderInputAxisField
          id='collider-rotation-offset-z'
          label='Z'
          value={rotationOffsetEdit.z}
          onChange={rotationOffsetEdit.setZ}
          onDrag={rotationOffsetEdit.setDragZ}
          normalizer={normalizeDegrees}
          step={1}
          fullWidth
        />
      </>
    </RowGroupFields>
  )
}

export {
  RotationOffsetConfigurator,
}
