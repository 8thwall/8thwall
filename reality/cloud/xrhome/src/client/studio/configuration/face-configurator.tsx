import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {Face} from '@ecs/shared/scene-graph'
import {useTranslation} from 'react-i18next'

import {FloatingTrayCollapsible} from '../../ui/components/floating-tray-collapsible'
import {Icon} from '../../ui/components/icon'
import {RowSelectField} from './row-fields'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {createThemedStyles} from '../../ui/theme'
import {areComponentsOverridden} from './prefab'
import {useSceneContext} from '../scene-context'
import {useStudioStateContext} from '../studio-state-context'
import {FACE_COMPONENT} from './direct-property-components'

interface IFaceConfigurator {
  face: DeepReadonly<Face>
  maxDetections: number
  onChange: (updater: (currentFace: Face) => Face) => void
  onAddAttachment: () => void
  onAddMesh: () => void
}

const useStyles = createThemedStyles(() => ({
  row: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    gap: '0.5em',
  },
}))

const FaceConfigurator: React.FC<IFaceConfigurator> = (
  {face, maxDetections, onChange, onAddAttachment, onAddMesh}
) => {
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()

  const isOverridden = areComponentsOverridden(
    ctx.scene, stateCtx.state.selectedIds, [FACE_COMPONENT]
  )

  return (
    <FloatingTrayCollapsible
      title={t('face_configurator.title')}
      overridden={isOverridden}
    >
      <RowSelectField
        id='faceId'
        label={t('face_configurator.face_id.label')}
        value={face.id ? face.id.toString() : '1'}
        options={Array.from({length: maxDetections}, (_, i) => ({
          value: (i + 1).toString(),
          content: (i + 1).toString(),
        }))}
        onChange={(value) => {
          onChange(currentFace => ({
            ...currentFace,
            id: Number(value),
          }))
        }}
      />
      <div className={classes.row}>
        <FloatingPanelButton onClick={onAddAttachment} spacing='full'>
          <Icon stroke='plus' inline />
          {t('face_configurator.button.attachment_point')}
        </FloatingPanelButton>

        <FloatingPanelButton onClick={onAddMesh} spacing='full'>
          <Icon stroke='plus' inline />
          {t('face_configurator.button.face_mesh')}
        </FloatingPanelButton>
      </div>
    </FloatingTrayCollapsible>
  )
}

export {
  FaceConfigurator,
}
