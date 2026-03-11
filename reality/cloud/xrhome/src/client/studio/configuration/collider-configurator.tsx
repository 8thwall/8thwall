import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation, Trans, type TFunction} from 'react-i18next'
import type {
  BoxGeometry, CapsuleGeometry, ColliderGeometryType, ConeGeometry, CylinderGeometry, GeometryType,
  PlaneGeometry, Collider as SceneCollider, SphereGeometry, ColliderType,
  SimplificationMode,
} from '@ecs/shared/scene-graph'
import {
  DEFAULT_ANGULAR_DAMPING, DEFAULT_FRICTION, DEFAULT_LINEAR_DAMPING, DEFAULT_MASS,
  DEFAULT_RESTITUTION, DEFAULT_ROLLING_FRICTION, DEFAULT_SPINNING_FRICTION, DEFAULT_GRAVITY_FACTOR,
  DEFAULT_OFFSET,
} from '@ecs/shared/physic-constants'
import {JOLT_FEATURE} from '@ecs/shared/features/jolt'
import {CONCAVE_COLLIDERS_FEATURE} from '@ecs/shared/features/concave-colliders'
import {
  ROTATIONAL_COLLIDER_OFFSETS_FEATURE,
} from '@ecs/shared/features/rotational-collider-offsets'

import {makeColliderGeometry, SUPPORTED_PRIMITIVE_COLLIDER_GEOMETRIES} from '../make-object'
import {
  RowBooleanField, RowBooleanFields, RowGroupFields, RowNumberField, RowSelectField, useStyles,
} from './row-fields'
import {RowJointToggleButton} from './row-joint-toggle-button'
import {Icon} from '../../ui/components/icon'
import {Tooltip} from '../../ui/components/tooltip'
import {SpaceBetween} from '../../ui/layout/space-between'
import {useStudioStateContext} from '../studio-state-context'
import {copyDirectProperties} from './copy-component'
import {ComponentConfiguratorTray} from './component-configurator-tray'
import {COLLIDER_COMPONENT} from '../hooks/available-components'
import {COLLIDER_COMPONENT as GRAPH_COLLIDER_COMPONENT} from './direct-property-components'
import {ScenePathScopeProvider} from '../scene-path-input-context'
import type {DefaultableConfigDiffInfo, DiffRenderInfo} from './diff-chip-types'
import type {ExpanseField} from './expanse-field-types'
import {bannerFromRenderValue} from './diff-chip'
import {renderValueDefault} from './diff-chip-default-renderers'
import {CURRENT_SCOPE} from './expanse-field-diff-chip'
import {SliderInputAxisField} from '../../ui/components/slider-input-axis-field'
import {useFeatureEnabled} from '../runtime-version/use-feature-enabled'
import {RotationOffsetConfigurator} from './rotation-offset-configurator'

type Collider = DeepReadonly<SceneCollider>

// NOTE(christoph): This is the initial mass for when we toggle static and need to set
// a positive mass.
const INITIAL_POSITIVE_MASS = 1

const MAXIMUM_FRICTION = 10

interface IColliderConfigurator {
  collider: Collider
  geometryType: GeometryType
  hasModel: boolean
  onChange: (updater: (current: Collider) => Collider) => void
  // eslint-disable-next-line react/no-unused-prop-types
  resetToPrefab?: (componentIds: string[], nonDirect?: boolean) => void
}

interface IColliderSlider {
  id: string
  label: string
  value: number
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<number, string[][]>>
  onChange: (n: number) => void
}

const ColliderSlider: React.FC<IColliderSlider> = ({
  id, label, value, onChange, expanseField,
}) => (
  <RowNumberField
    id={id}
    expanseField={expanseField}
    onChange={onChange}
    value={value || 0}
    min={0}
    step={0.1}
    label={label}
  />
)

const renderBoxSliders = (
  geometry: DeepReadonly<BoxGeometry>,
  onChange: (update: BoxGeometry) => void,
  t: TFunction<'cloud-editor-pages'[]>
) => (
  <div>
    <ColliderSlider
      id='collider-geometry-width'
      expanseField='width'
      onChange={n => onChange({...geometry, width: n})}
      value={geometry.width}
      label={t('collider_configurator.width.label')}
    />
    <ColliderSlider
      id='collider-geometry-height'
      expanseField='height'
      onChange={n => onChange({...geometry, height: n})}
      value={geometry.height}
      label={t('collider_configurator.height.label')}
    />
    <ColliderSlider
      id='collider-geometry-depth'
      expanseField='depth'
      onChange={n => onChange({...geometry, depth: n})}
      value={geometry.depth}
      label={t('collider_configurator.depth.label')}
    />
  </div>
)

const renderSphereSliders = (
  geometry: DeepReadonly<SphereGeometry>,
  onChange: (update: SphereGeometry) => void,
  t: TFunction<'cloud-editor-pages'[]>
) => (
  <div>
    <ColliderSlider
      id='collider-geometry-radius'
      expanseField='radius'
      onChange={n => onChange({...geometry, radius: n})}
      value={geometry.radius}
      label={t('collider_configurator.radius.label')}
    />
  </div>
)

const renderPlaneSliders = (
  geometry: DeepReadonly<PlaneGeometry>,
  onChange: (update: PlaneGeometry) => void,
  t: TFunction<'cloud-editor-pages'[]>
) => (
  <div>
    <ColliderSlider
      id='collider-geometry-width'
      expanseField='width'
      onChange={n => onChange({...geometry, width: n})}
      value={geometry.width}
      label={t('collider_configurator.width.label')}
    />
    <ColliderSlider
      id='collider-geometry-height'
      expanseField='height'
      onChange={n => onChange({...geometry, height: n})}
      value={geometry.height}
      label={t('collider_configurator.height.label')}
    />
  </div>
)

const renderCapsuleSliders = (
  geometry: DeepReadonly<CapsuleGeometry>,
  onChange: (update: CapsuleGeometry) => void,
  t: TFunction<'cloud-editor-pages'[]>
) => (
  <div>
    <ColliderSlider
      id='collider-geometry-radius'
      expanseField='radius'
      onChange={n => onChange({...geometry, radius: n})}
      value={geometry.radius}
      label={t('collider_configurator.radius.label')}
    />
    <ColliderSlider
      id='collider-geometry-height'
      expanseField='height'
      onChange={n => onChange({...geometry, height: n})}
      value={geometry.height}
      label={t('collider_configurator.height.label')}
    />
  </div>
)

const renderConeSliders = (
  geometry: DeepReadonly<ConeGeometry>,
  onChange: (update: ConeGeometry) => void,
  t: TFunction<'cloud-editor-pages'[]>
) => (
  <div>
    <ColliderSlider
      id='collider-geometry-radius'
      expanseField='radius'
      onChange={n => onChange({...geometry, radius: n})}
      value={geometry.radius}
      label={t('collider_configurator.radius.label')}
    />
    <ColliderSlider
      id='collider-geometry-height'
      expanseField='height'
      onChange={n => onChange({...geometry, height: n})}
      value={geometry.height}
      label={t('collider_configurator.height.label')}
    />
  </div>
)

const renderCylinderSliders = (
  geometry: DeepReadonly<CylinderGeometry>,
  onChange: (update: CylinderGeometry) => void,
  t: TFunction<'cloud-editor-pages'[]>
) => (
  <div>
    <ColliderSlider
      id='collider-geometry-radius'
      expanseField='radius'
      onChange={n => onChange({...geometry, radius: n})}
      value={geometry.radius}
      label={t('collider_configurator.radius.label')}
    />
    <ColliderSlider
      id='collider-geometry-height'
      expanseField='height'
      onChange={n => onChange({...geometry, height: n})}
      value={geometry.height}
      label={t('collider_configurator.height.label')}
    />
  </div>
)

const renderSliders = (
  geometry: DeepReadonly<SceneCollider['geometry']>,
  onChange: (update: SceneCollider['geometry']) => void,
  t: TFunction<'cloud-editor-pages'[]>
) => {
  switch (geometry.type) {
    case 'box':
      return renderBoxSliders(geometry, onChange, t)
    case 'sphere':
      return renderSphereSliders(geometry, onChange, t)
    case 'plane':
      return renderPlaneSliders(geometry, onChange, t)
    case 'capsule':
      return renderCapsuleSliders(geometry, onChange, t)
    case 'cone':
      return renderConeSliders(geometry, onChange, t)
    case 'cylinder':
      return renderCylinderSliders(geometry, onChange, t)
    default:
      return null
  }
}

const ColliderConfigurator: React.FC<IColliderConfigurator> = ({
  collider, geometryType, hasModel, onChange, resetToPrefab,
}) => {
  const [massFocused, setMassFocused] = React.useState(false)
  const classes = useStyles()
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const stateCtx = useStudioStateContext()

  const handleGeometryChange = (newGeometry: DeepReadonly<SceneCollider['geometry']>) => {
    onChange(() => ({
      ...collider,
      geometry: newGeometry,
    }))
  }

  const unsupportedGeometry = collider.geometry?.type === 'auto' &&
    !SUPPORTED_PRIMITIVE_COLLIDER_GEOMETRIES.includes(geometryType) && !hasModel

  const modelCollider = collider.geometry?.type === 'auto' && hasModel && !geometryType

  // NOTE(christoph): Rolling and spinning friction are not supported with Jolt.
  const joltEnabled = useFeatureEnabled(JOLT_FEATURE)
  const concaveEnabled = useFeatureEnabled(CONCAVE_COLLIDERS_FEATURE)
  const rotationalOffsetsEnabled = useFeatureEnabled(ROTATIONAL_COLLIDER_OFFSETS_FEATURE)

  const topContent = (
    <SpaceBetween extraNarrow>
      {unsupportedGeometry &&
        <Tooltip
          content={t('collider_configurator.tooltip.unsupported_geometry')}
          position='top-end'
        >
          <Icon stroke='danger' color='danger' block />
        </Tooltip>
        }
      {modelCollider &&
        <Tooltip
          content={(
            <Trans
              ns='cloud-studio-pages'
              i18nKey='collider_configurator.tooltip.model_collider'
              components={{1: <br />}}
            />
          )}
        >
          <Icon stroke='info' color='info' block />
        </Tooltip>
        }
    </SpaceBetween>
  )

  const visibleMass = collider.mass ?? DEFAULT_MASS

  const colliderShapeOptions = [
    {value: 'none', content: t('collider_configurator.shape.option.none')},
    {value: 'auto', content: t('collider_configurator.shape.option.auto')},
    {value: 'box', content: t('collider_configurator.shape.option.box')},
    {value: 'sphere', content: t('collider_configurator.shape.option.sphere')},
    {value: 'plane', content: t('collider_configurator.shape.option.plane')},
    {value: 'capsule', content: t('collider_configurator.shape.option.capsule')},
    {value: 'cone', content: t('collider_configurator.shape.option.cone')},
    {value: 'cylinder', content: t('collider_configurator.shape.option.cylinder')},
    {value: 'circle', content: t('collider_configurator.shape.option.circle')},
  ]

  const rigidBodyOptions = [
    {value: 'dynamic', content: t('collider_configurator.rigidbody.option.dynamic')},
    {value: 'static', content: t('collider_configurator.rigidbody.option.static')},
    ...(joltEnabled
      ? [{value: 'kinematic', content: t('collider_configurator.rigidbody.option.kinematic')}]
      : []),
  ]

  const type = collider.type ?? (collider.mass > 0 ? 'dynamic' : 'static')

  return (
    <ScenePathScopeProvider path={['collider']}>
      <ComponentConfiguratorTray
        title={t('collider_configurator.title')}
        description={t('collider_configurator.description')}
        expanseField={{
          leafPaths: CURRENT_SCOPE,
        }}
        sectionId={COLLIDER_COMPONENT}
        topContent={topContent}
        onRemove={() => onChange(() => undefined)}
        onCopy={() => copyDirectProperties(stateCtx, {collider})}
        onResetToPrefab={resetToPrefab
          ? () => resetToPrefab([GRAPH_COLLIDER_COMPONENT])
          : undefined}
        componentData={[GRAPH_COLLIDER_COMPONENT]}
      >
        <RowSelectField
          expanseField={{
            leafPaths: [['geometry', 'type']],
          }}
          id='collider-shape-select'
          label={t('collider_configurator.shape.label')}
          value={collider.geometry?.type ?? 'none'}
          options={colliderShapeOptions}
          onChange={(value) => {
            if (value === 'none') {
              onChange(current => ({
                ...current,
                geometry: undefined,
              }))
              return
            }
            const newValue = value as ColliderGeometryType
            onChange(current => ({
              ...current,
              geometry: makeColliderGeometry(newValue),
            }))
          }}
        />

        {collider.geometry?.type === 'auto' && hasModel && concaveEnabled &&
          <RowJointToggleButton
            id='collider-gltf-method'
            label={t('collider_configurator.gltf_collider_method.label')}
            value={collider.simplificationMode ?? 'convex'}
            options={[
              {
                value: 'convex',
                content: t('collider_configurator.gltf_collider_method.convex'),
                tooltip: t('collider_configurator.gltf_collider_method.convex_tooltip'),
              },
              {
                value: 'concave',
                content: t('collider_configurator.gltf_collider_method.concave'),
                tooltip: t('collider_configurator.gltf_collider_method.concave_tooltip'),
              },
            ] as const}
            onChange={(value: SimplificationMode) => {
              onChange(current => ({
                ...current,
                simplificationMode: value,
              }))
            }}
          />
        }

        {collider.geometry && collider.geometry.type !== 'auto' &&
          <ScenePathScopeProvider path={['geometry']}>
            {renderSliders(collider.geometry, handleGeometryChange, t)}
          </ScenePathScopeProvider>
        }

        {!joltEnabled
          ? <RowSelectField
              id='collider-rigid-body-type'
              label={t('collider_configurator.rigidbody.label')}
              value={type}
              expanseField={{
                leafPaths: [['mass']],
                diffOptions: {
                  defaults: [0],
                  renderDiff: ((prevMass: number, newMass: number) => {
                    const dynamicString = t('collider_configurator.rigidbody.option.dynamic')
                    const staticString = t('collider_configurator.rigidbody.option.static')
                    const prevString = prevMass > 0 ? dynamicString : staticString
                    const newString = newMass > 0 ? dynamicString : staticString
                    if (prevString === newString) {
                      return {
                        type: 'unchanged',
                        bannerContent: null,
                      }
                    } else {
                      return {
                        type: 'changed',
                        bannerContent:
                          bannerFromRenderValue(renderValueDefault, prevString, newString),
                      }
                    }
                    // Edge case where SelectField actually listens to a number value
                  }) as unknown as (prev: string, now: string) => DiffRenderInfo,
                },
              }}
              options={rigidBodyOptions}
              onChange={value => onChange(current => ({
                ...current,
                mass: value === 'static' ? 0 : INITIAL_POSITIVE_MASS,
              }))}
          />
          : <RowSelectField
              id='collider-rigid-body-type'
              label={t('collider_configurator.rigidbody.label')}
              value={type}
              expanseField={{
                leafPaths: [['type', 'mass']],
                diffOptions: {
                  defaults: [0, 0],
                  // TODO: write a proper scene diff logic with
                  // ([prevType, prevMass], [newType, newMass])
                  renderDiff: (() => ({
                    type: 'unchanged',
                    bannerContent: null,
                  })
                  ) as unknown as () => DiffRenderInfo,
                },
              }}
              options={rigidBodyOptions}
              onChange={(value) => {
                onChange(current => ({
                  ...current,
                  mass: value === 'static' || value === 'kinematic' ? 0 : INITIAL_POSITIVE_MASS,
                  type: value as ColliderType,
                }))
              }}
          />}

        {(massFocused || type === 'dynamic') &&
          <div className={classes.indent}>

            <RowNumberField
              id='gravity-factor'
              expanseField={{
                leafPaths: [['gravityFactor']],
                diffOptions: {
                  defaults: [DEFAULT_GRAVITY_FACTOR],
                },
              }}
              onChange={(value) => {
                onChange(current => ({
                  ...current,
                  gravityFactor: value,
                }))
              }}
              value={collider.gravityFactor ?? DEFAULT_GRAVITY_FACTOR}
              step={0.01}
              label={t('collider_configurator.gravity_factor.label')}
            />

            <RowNumberField
              id='collider-mass'
              onChange={value => onChange(current => ({
                ...current,
                mass: value,
              }))}
              expanseField={{
                leafPaths: [['mass']],
                diffOptions: {
                  defaults: [DEFAULT_MASS],
                },
              }}
              value={visibleMass}
              min={0}
              step={1}
              label={t('collider_configurator.mass.label')}
              onFocus={() => setMassFocused(true)}
              onBlur={() => setMassFocused(false)}
            />

            <RowNumberField
              id='linear-damping'
              onChange={value => onChange(current => ({
                ...current,
                linearDamping: value,
              }))}
              expanseField={{
                leafPaths: [['linearDamping']],
                diffOptions: {
                  defaults: [DEFAULT_LINEAR_DAMPING],
                },
              }}
              value={collider.linearDamping ?? DEFAULT_LINEAR_DAMPING}
              min={0}
              max={1}
              step={0.01}
              label={t('collider_configurator.linear_damping.label')}
            />

            <RowNumberField
              id='angular-damping'
              onChange={value => onChange(current => ({
                ...current,
                angularDamping: value,
              }))}
              expanseField={{
                leafPaths: [['angularDamping']],
                diffOptions: {
                  defaults: [DEFAULT_ANGULAR_DAMPING],
                },
              }}
              value={collider.angularDamping ?? DEFAULT_ANGULAR_DAMPING}
              min={0}
              max={1}
              step={0.01}
              label={t('collider_configurator.angular_damping.label')}
            />
            <RowBooleanFields
              label={t('collider_configurator.lock_position.label')}
              expanseField={{
                leafPaths: [
                  ['lockXPosition'], ['lockYPosition'], ['lockZPosition'],
                ],
                diffOptions: {
                  defaults: [false, false, false],
                },
              }}
              fields={[
                {
                  id: 'lock-x-position',
                  label: 'X',
                  checked: !!collider.lockXPosition,
                  onChange: (e) => {
                    onChange(current => ({
                      ...current,
                      lockXPosition: e.target.checked ? true : undefined,
                    }))
                  },
                },
                {
                  id: 'lock-y-position',
                  label: 'Y',
                  checked: !!collider.lockYPosition,
                  onChange: (e) => {
                    onChange(current => ({
                      ...current,
                      lockYPosition: e.target.checked ? true : undefined,
                    }))
                  },
                },
                {
                  id: 'lock-z-position',
                  label: 'Z',
                  checked: !!collider.lockZPosition,
                  onChange: (e) => {
                    onChange(current => ({
                      ...current,
                      lockZPosition: e.target.checked ? true : undefined,
                    }))
                  },
                },
              ]}
            />
            <RowBooleanFields
              label={t('collider_configurator.lock_rotation.label')}
              expanseField={{
                leafPaths: [
                  ['lockXAxis'], ['lockYAxis'], ['lockZAxis'],
                ],
                diffOptions: {
                  defaults: [false, false, false],
                },
              }}
              fields={[
                {
                  id: 'lock-x-axis',
                  label: 'X',
                  checked: !!collider.lockXAxis,
                  onChange: (e) => {
                    onChange(current => ({
                      ...current,
                      lockXAxis: e.target.checked ? true : undefined,
                    }))
                  },
                },
                {
                  id: 'lock-y-axis',
                  label: 'Y',
                  checked: !!collider.lockYAxis,
                  onChange: (e) => {
                    onChange(current => ({
                      ...current,
                      lockYAxis: e.target.checked ? true : undefined,
                    }))
                  },
                },
                {
                  id: 'lock-z-axis',
                  label: 'Z',
                  checked: !!collider.lockZAxis,
                  onChange: (e) => {
                    onChange(current => ({
                      ...current,
                      lockZAxis: e.target.checked ? true : undefined,
                    }))
                  },
                },
              ]}
            />

            <RowBooleanField
              id='collider-high-precision'
              label={t('collider_configurator.high_precision.label')}
              checked={!!collider.highPrecision}
              onChange={event => onChange(current => ({
                ...current,
                highPrecision: !!event.target.checked,
              }))}
            />

          </div>
}

        {!!collider.geometry &&
          <RowBooleanField
            id='collider-eventOnly'
            label={t('collider_configurator.event_only.label')}
            expanseField={{
              leafPaths: [['eventOnly']],
              diffOptions: {
                defaults: [false],
              },
            }}
            checked={!!collider.eventOnly}
            onChange={event => onChange(current => ({
              ...current,
              eventOnly: event.target.checked ? true : undefined,
            }))}
          />
      }

        {!collider.eventOnly &&
          <>
            <RowNumberField
              id='friction'
              expanseField={{
                leafPaths: [['friction']],
                diffOptions: {
                  defaults: [DEFAULT_FRICTION],
                },
              }}
              onChange={value => onChange(current => ({
                ...current,
                friction: value,
              }))}
              value={collider.friction ?? DEFAULT_FRICTION}
              min={0}
              max={MAXIMUM_FRICTION}
              step={0.01}
              label={t('collider_configurator.friction.label')}
            />
            {!joltEnabled &&
              <>
                <RowNumberField
                  id='rolling-friction'
                  expanseField={{
                    leafPaths: [['rollingFriction']],
                    diffOptions: {
                      defaults: [DEFAULT_ROLLING_FRICTION],
                    },
                  }}
                  onChange={value => onChange(current => ({
                    ...current,
                    rollingFriction: value,
                  }))}
                  value={collider.rollingFriction ?? DEFAULT_ROLLING_FRICTION}
                  min={0}
                  max={MAXIMUM_FRICTION}
                  step={0.01}
                  label={t('collider_configurator.rolling_friction.label')}
                />

                <RowNumberField
                  id='spinning-friction'
                  expanseField={{
                    leafPaths: [['spinningFriction']],
                    diffOptions: {
                      defaults: [DEFAULT_SPINNING_FRICTION],
                    },
                  }}
                  onChange={value => onChange(current => ({
                    ...current,
                    spinningFriction: value,
                  }))}
                  value={collider.spinningFriction ?? DEFAULT_SPINNING_FRICTION}
                  min={0}
                  max={MAXIMUM_FRICTION}
                  step={0.01}
                  label={t('collider_configurator.spinning_friction.label')}
                />
              </>
            }

            <RowNumberField
              id='bounciness'
              expanseField={{
                leafPaths: [['restitution']],
                diffOptions: {
                  defaults: [DEFAULT_RESTITUTION],
                },
              }}
              onChange={value => onChange(current => ({
                ...current,
                restitution: value,
              }))}
              value={collider.restitution ?? DEFAULT_RESTITUTION}
              min={joltEnabled ? -1 : 0}
              max={1}
              step={0.01}
              label={t('collider_configurator.bounciness.label')}
            />
          </>
      }
        {/* TODO(Dale): Add expanseField for offset */}
        <RowGroupFields label={t('collider_configurator.offset.label')}>
          <SliderInputAxisField
            id='collider-offset-x'
            label='X'
            value={collider.offsetX ?? DEFAULT_OFFSET}
            onChange={offsetX => onChange(old => ({...old, offsetX}))}
            step={0.1}
            fullWidth
          />
          <SliderInputAxisField
            id='collider-offset-y'
            label='Y'
            value={collider.offsetY ?? DEFAULT_OFFSET}
            onChange={offsetY => onChange(old => ({...old, offsetY}))}
            step={0.1}
            fullWidth
          />
          <SliderInputAxisField
            id='collider-offset-z'
            label='Z'
            value={collider.offsetZ ?? DEFAULT_OFFSET}
            onChange={offsetZ => onChange(old => ({...old, offsetZ}))}
            step={0.1}
            fullWidth
          />
        </RowGroupFields>

        {rotationalOffsetsEnabled &&
          <RotationOffsetConfigurator collider={collider} onChange={onChange} />
        }
      </ComponentConfiguratorTray>
    </ScenePathScopeProvider>
  )
}

export {
  ColliderConfigurator,
}
