import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {GraphObject} from '@ecs/shared/scene-graph'
import {useTranslation} from 'react-i18next'

import {useSceneContext} from '../scene-context'
import {SliderInputAxisField} from '../../ui/components/slider-input-axis-field'
import {IconButton} from '../../ui/components/icon-button'
import {createThemedStyles} from '../../ui/theme'

import {useSelectedObjects} from '../hooks/selected-objects'
import {useStudioStateContext} from '../studio-state-context'
import {copyDirectProperties} from './copy-component'
import {ComponentConfiguratorTray} from './component-configurator-tray'
import {TRANSFORM_COMPONENT} from '../hooks/available-components'
import {useLayoutChangeEffect} from '../../hooks/use-change-effect'
import {RowGroupFields} from './row-group-fields'
import {POSITION_COMPONENT, ROTATION_COMPONENT, SCALE_COMPONENT} from './direct-property-components'
import {resetPrefabInstanceComponents} from './prefab'
import {makeRenderDiffVector3} from './diff-chip-default-renderers'
import type {DiffRenderInfo} from './diff-chip-types'
import {Tooltip} from '../../ui/components/tooltip'
import {SetToCurrentViewButton} from './set-to-current-view-button'
import {RowContent} from './row-content'
import {
  normalizeDegrees, quaternionToEulerDegrees,
} from '../quaternion-conversion'
import {useEulerEdit} from './euler-edit'

const useStyles = createThemedStyles(theme => ({
  transformGrid: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0em',
  },
  transformButtonRow: {
    'display': 'flex',
    'gap': '0.5em',
    'color': theme.fgMuted,
    '& > *:hover': {
      color: theme.fgMain,
    },
    'alignItems': 'center',
  },
  transformInput: {
    display: 'flex',
    gap: '0.25em',
    flex: 2,
  },
}))

const renderQuaternionDiff = (
  before: [number, number, number, number] | undefined,
  after: [number, number, number, number] | undefined
): DiffRenderInfo => {
  if (before === undefined || after === undefined) {
    return {
      type: null,
      bannerContent: null,
    }
  }
  const beforeAngle = quaternionToEulerDegrees(before)
  const afterAngle = quaternionToEulerDegrees(after)
  const precisionVec3Renderer = makeRenderDiffVector3({step: 1})

  return precisionVec3Renderer(
    [beforeAngle.x, beforeAngle.y, beforeAngle.z],
    [afterAngle.x, afterAngle.y, afterAngle.z]
  )
}

const isScaleLocked = (objects: DeepReadonly<GraphObject[]>) => {
  if (!objects) {
    return false
  }

  return objects.every(
    object => object.scale[0] === object.scale[1] && object.scale[0] === object.scale[2]
  )
}

interface ITransformConfiguratorProps {
  isRotationLocked?: { x: boolean, y: boolean, z: boolean }
  onApplyOverridesToPrefab?: (componentIds: string[], nonDirect?: boolean) => void
}

const TransformConfigurator: React.FC<ITransformConfiguratorProps> = (
  {isRotationLocked, onApplyOverridesToPrefab}
) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()
  const ctx = useSceneContext()
  const objects = useSelectedObjects()
  const stateCtx = useStudioStateContext()

  const description = (objects.length === 1 && objects[0]?.parentId)
    ? t('transform_configurator.description_parent')
    : t('transform_configurator.description')

  const eulerEdit = useEulerEdit(objects.map((obj => obj.rotation)), (newRotations) => {
    objects.forEach((object, index) => {
      ctx.updateObject(object.id, oldObject => ({...oldObject, rotation: newRotations[index]}))
    })
  })

  const [scaledLocked, setScaleLocked] = React.useState(isScaleLocked(objects))

  useLayoutChangeEffect(([previousSelectedIds]) => {
    if (!!stateCtx.state.selectedIds !== !!previousSelectedIds ||
        stateCtx.state.selectedIds.length !== previousSelectedIds.length ||
        stateCtx.state.selectedIds.some((id, index) => id !== previousSelectedIds[index])) {
      setScaleLocked(isScaleLocked(objects))
    }
  }, [stateCtx.state.selectedIds])

  if (objects.length < 1) {
    return null
  }

  const makeChangeHandler = <T extends 'position' | 'scale'>(
    key: T, key2: number
  ) => (newValue: number) => {
      objects.forEach((object) => {
        ctx.updateObject(object.id, (oldObject) => {
          const cloned = {...oldObject}
          const oldSubObject: readonly [number, number, number] = oldObject[key]
          let newSubObject: [number, number, number]

          if (key === 'scale' && scaledLocked) {
            const oldValue = object[key][key2]
            if (oldValue === 0) {
              newSubObject = oldSubObject.map(
                previous => (previous === 0 ? newValue : previous)
              ) as [number, number, number]
            } else {
              const ratio = newValue / oldValue
              newSubObject = oldSubObject.map(
                (previous, j) => (j === key2 ? newValue : previous * ratio)
              ) as [number, number, number]
            }
          } else {
            newSubObject = [...oldSubObject]
            newSubObject[key2] = newValue
          }
          cloned[key] = newSubObject
          return cloned
        })
      })
    }

  const makeDragHandler = <T extends 'position' | 'scale'>(
    key: T, key2: number, min?: number
  ) => (newDiff: number) => {
      objects.forEach((obj) => {
        ctx.updateObject(obj.id, (oldObject) => {
          const cloned = {...oldObject}
          const oldSubObject: readonly [number, number, number] = obj[key]
          let newSubObject: [number, number, number]

          if (key === 'scale' && scaledLocked) {
            const oldValue = oldSubObject[key2] as number
            const newSummedValue = oldValue + newDiff
            const newValue = newSummedValue < min ? min : newSummedValue
            if (oldValue === 0) {
              newSubObject = oldSubObject.map(
                previous => (previous === 0 ? newValue : previous)
              ) as [number, number, number]
            } else {
              const ratio = newValue / oldValue
              newSubObject = oldSubObject.map((num) => {
                const newProduct = num * ratio
                return newProduct < min ? min : newProduct
              }) as [number, number, number]
            }
          } else {
            newSubObject = [...oldSubObject]
            const newSummedValue = newSubObject[key2 as number] + newDiff
            newSubObject[key2] = newSummedValue < min ? min : newSummedValue
          }
          cloned[key] = newSubObject
          return cloned
        })
      })
    }

  const propIsMixed = (
    propertyAccessor: (obj: DeepReadonly<GraphObject>) => number
  ) => objects.length > 1 &&
    objects.some(obj => propertyAccessor(obj) !== propertyAccessor(objects[0]))

  const isPositionMixed = Array.from({length: 3}, (_, i) => propIsMixed(obj => obj.position[i]))
  const isScaleMixed = Array.from({length: 3}, (_, i) => propIsMixed(obj => obj.scale[i]))

  // Note(owenmech): any difference in rotation is considered mixed when displaying euler angles
  const isRotationMixed = Array.from({length: 4}, (_, i) => propIsMixed(obj => obj.rotation[i]))
    .some(Boolean)

  const isTransformDisabled = objects.some(obj => !!obj.mapPoint)

  // TODO[Yuling]: Better to have a static field which doesn't accept changes
  const rotationXField = (
    <SliderInputAxisField
      id='rotation-x'
      label='X'
      step={1}
      value={isRotationMixed ? 0 : eulerEdit.x}
      onChange={eulerEdit.setX}
      onDrag={eulerEdit.setDragX}
      normalizer={normalizeDegrees}
      mixed={isRotationMixed}
      disabled={isTransformDisabled || isRotationLocked?.x}
    />
  )
  const rotationYField = (
    <SliderInputAxisField
      id='rotation-y'
      label='Y'
      step={1}
      value={isRotationMixed ? 0 : eulerEdit.y}
      onChange={eulerEdit.setY}
      onDrag={eulerEdit.setDragY}
      normalizer={normalizeDegrees}
      mixed={isRotationMixed}
      disabled={isTransformDisabled || isRotationLocked?.y}
    />
  )
  const rotationZField = (
    <SliderInputAxisField
      id='rotation-z'
      label='Z'
      step={1}
      value={isRotationMixed ? 0 : eulerEdit.z}
      onChange={eulerEdit.setZ}
      onDrag={eulerEdit.setDragZ}
      normalizer={normalizeDegrees}
      mixed={isRotationMixed}
      disabled={isTransformDisabled || isRotationLocked?.z}
    />
  )
  return (
    <ComponentConfiguratorTray
      a8='click;studio;transforms-dropdown-button'
      title={t('transform_configurator.title')}
      description={description}
      sectionId={TRANSFORM_COMPONENT}
      onReset={() => {
        objects.forEach((object) => {
          ctx.updateObject(object.id, oldObject => ({
            ...oldObject,
            position: [0, 0, 0],
            rotation: [0, 0, 0, 1],
            scale: [1, 1, 1],
          }))
        })
        setScaleLocked(true)
      }}
      // TODO (owenmech): handle copy for multiple objects once logic is in to display mixed
      onCopy={objects.length === 1
        ? () => copyDirectProperties(stateCtx, {
          position: objects[0].position,
          rotation: objects[0].rotation,
          scale: objects[0].scale,
        })
        : undefined}
      onResetToPrefab={() => {
        const object = objects[0]
        ctx.updateScene(s => resetPrefabInstanceComponents(s, object.id,
          [POSITION_COMPONENT, ROTATION_COMPONENT, SCALE_COMPONENT]))
      }}
      onApplyOverridesToPrefab={onApplyOverridesToPrefab
        ? () => {
          onApplyOverridesToPrefab([POSITION_COMPONENT, ROTATION_COMPONENT, SCALE_COMPONENT])
        }
        : undefined}
      expanseField={{
        leafPaths: [['position'], ['rotation'], ['scale']],
      }}
       // Note(dale): prefab instances are instantiated with position values
      componentData={[ROTATION_COMPONENT, SCALE_COMPONENT]}
    >
      <div className={classes.transformGrid}>
        <RowGroupFields
          label={t('transform_configurator.position.label')}
          expanseField={{
            leafPaths: [['position']],
            diffOptions: {
              renderDiff: makeRenderDiffVector3({step: 0.1}),
            },
          }}
        >
          <div className={classes.transformInput}>
            <SliderInputAxisField
              id='position-x'
              label='X'
              step={0.1}
              value={isPositionMixed[0] ? 0 : objects[0].position[0]}
              onChange={makeChangeHandler('position', 0)}
              onDrag={makeDragHandler('position', 0)}
              mixed={isPositionMixed[0]}
              disabled={isTransformDisabled}
            />
            <SliderInputAxisField
              id='position-y'
              label='Y'
              step={0.1}
              value={isPositionMixed[1] ? 0 : objects[0].position[1]}
              onChange={makeChangeHandler('position', 1)}
              onDrag={makeDragHandler('position', 1)}
              mixed={isPositionMixed[1]}
              disabled={isTransformDisabled}
            />
            <SliderInputAxisField
              id='position-z'
              label='Z'
              step={0.1}
              value={isPositionMixed[2] ? 0 : objects[0].position[2]}
              onChange={makeChangeHandler('position', 2)}
              onDrag={makeDragHandler('position', 2)}
              mixed={isPositionMixed[2]}
              disabled={isTransformDisabled}
            />
          </div>
        </RowGroupFields>
        <RowGroupFields
          label={t('transform_configurator.rotation.label')}
          expanseField={{
            leafPaths: [['rotation']],
            diffOptions: {
              renderDiff: renderQuaternionDiff,
            },
          }}
        >
          <div className={classes.transformInput}>
            {isRotationLocked?.x
              ? (
                <Tooltip content={t('transform_configurator.rotation_locked_tooltip')}>
                  {rotationXField}
                </Tooltip>
              )
              : rotationXField}
            {isRotationLocked?.y
              ? (
                <Tooltip content={t('transform_configurator.rotation_locked_tooltip')}>
                  {rotationYField}
                </Tooltip>
              )
              : rotationYField}
            {isRotationLocked?.z
              ? (
                <Tooltip content={t('transform_configurator.rotation_locked_tooltip')}>
                  {rotationZField}
                </Tooltip>
              )
              : rotationZField}
          </div>
        </RowGroupFields>
        <RowGroupFields
          label={t('transform_configurator.scale.label')}
          expanseField={{
            leafPaths: [['scale']],
            diffOptions: {
              renderDiff: makeRenderDiffVector3({step: 0.1}),
            },
          }}
        >
          <div className={classes.transformButtonRow}>
            <IconButton
              text={t('transform_configurator.button.lock_scale')}
              onClick={() => {
                setScaleLocked(!scaledLocked)
              }}
              stroke={scaledLocked ? 'chain' : 'chainBroken'}
            />
          </div>
          <div className={classes.transformInput}>
            <SliderInputAxisField
              id='scale-x'
              label='X'
              min={0}
              step={0.1}
              value={isScaleMixed[0] ? 1 : objects[0].scale[0]}
              onChange={makeChangeHandler('scale', 0)}
              onDrag={makeDragHandler('scale', 0, 0)}
              mixed={isScaleMixed[0]}
              disabled={isTransformDisabled}
            />
            <SliderInputAxisField
              id='scale-y'
              label='Y'
              min={0}
              step={0.1}
              value={isScaleMixed[1] ? 1 : objects[0].scale[1]}
              onChange={makeChangeHandler('scale', 1)}
              onDrag={makeDragHandler('scale', 1, 0)}
              mixed={isScaleMixed[1]}
              disabled={isTransformDisabled}
            />
            <SliderInputAxisField
              id='scale-z'
              label='Z'
              min={0}
              step={0.1}
              value={isScaleMixed[2] ? 1 : objects[0].scale[2]}
              onChange={makeChangeHandler('scale', 2)}
              onDrag={makeDragHandler('scale', 2, 0)}
              mixed={isScaleMixed[2]}
              disabled={isTransformDisabled}
            />
          </div>
        </RowGroupFields>
        {objects.length === 1 && objects[0].camera &&
          <RowContent>
            <SetToCurrentViewButton object={objects[0]} />
          </RowContent>
        }
      </div>
    </ComponentConfiguratorTray>
  )
}

export {
  TransformConfigurator,
}
