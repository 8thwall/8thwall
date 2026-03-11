import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {
  Geometry, GeometryType, GltfModel, Material, Splat, Shadow,
} from '@ecs/shared/scene-graph'
import {useTranslation} from 'react-i18next'

import {Quaternion, Vector3} from 'three'

import {
  DEFAULT_MATERIAL_COLOR, DEFAULT_SHADOW_COLOR, MATERIAL_DEFAULTS,
} from '@ecs/shared/material-constants'

import {
  useStyles as useRowStyles, RowGroupFields, ColorPicker, RowBooleanField, RowColorField,
  RowSelectField, RowTextField, RowNumberField,
} from './row-fields'
import {GeometryConfigurator} from './geometry-configurator'
import {MaterialConfigurator} from './material-configurator'
import {makeGeometry, makeGltfModel, makeMaterial, makeSplat} from '../make-object'
import {useResourceUrl} from '../hooks/resource-url'
import {useGltfWithDraco} from '../hooks/gltf'
import {GltfLoadBoundary} from '../gltf-load-boundary'
import {GltfAnimationConfigurator} from './gltf-animation-configurator'
import {copyDirectProperties} from './copy-component'
import {useStudioStateContext} from '../studio-state-context'
import {ComponentConfiguratorTray} from './component-configurator-tray'
import {MESH_COMPONENT} from '../hooks/available-components'
import {
  MODEL_URL_VALUE, MeshCategoryType, MeshConfiguratorMenu, SPLAT_URL_VALUE,
} from './mesh-configurator-menu'
import {
  convertConfiguredMaterial, convertThreeMaterial, findMaterialsInThreeScene,
} from './three-material-util'
import {CompactImagePicker} from '../ui/compact-image-picker'
import {
  GEOMETRY_COMPONENT, GLTF_MODEL_COMPONENT, MATERIAL_COMPONENT, MESH_COMPONENTS,
  SHADOW_COMPONENT, SPLAT_COMPONENT,
} from './direct-property-components'
import {
  makeRenderValueGroup,
  renderValueColor,
  makeRenderValueVisualResource,
} from './diff-chip-default-renderers'

interface IGltfMeshMaterialConfigurator {
  url: string
}

const undefinedIfDefaultShadow = (v: DeepReadonly<Shadow>) => {
  if (!v.receiveShadow && !v.castShadow) {
    return undefined
  }

  return v
}

const GltfMeshMaterialConfigurator: React.FC<IGltfMeshMaterialConfigurator> = ({url}) => {
  const scene = useGltfWithDraco(url)?.scene

  // First, build a set of all materials used in the GLTF model
  const materialsArray = Array.from(findMaterialsInThreeScene(scene))

  // Then, show a panel for each material
  // TODO (jperrins) Replace empty panels with actual configurators for each material
  // (will likely involve refactoring MaterialConfigurator to take an array of material interfaces)
  return (
    <>
      {materialsArray.map(material => (
        <MaterialConfigurator
          title={material.name}
          key={material.uuid}
          material={convertConfiguredMaterial(convertThreeMaterial(material))}
          defaultOpen={false}
          isAsset
          // TODO: handle modifications to the GLB/GLTF
          onChange={() => {}}
        />
      ))}
    </>
  )
}

interface MeshObject {
  gltfModel?: DeepReadonly<GltfModel>
  splat?: DeepReadonly<Splat>
  geometry?: DeepReadonly<Geometry>
  material?: DeepReadonly<Material>
  rotation?: DeepReadonly<[number, number, number, number]>
  shadow?: DeepReadonly<Shadow>
}

interface IMeshConfigurator {
  object: DeepReadonly<MeshObject>
  onChange: (updater: (current: MeshObject) => MeshObject) => void
  // eslint-disable-next-line react/no-unused-prop-types
  resetToPrefab?: (componentIds: string[], nonDirect?: boolean) => void
  // eslint-disable-next-line react/no-unused-prop-types
  applyOverridesToPrefab?: (componentIds: string[], nonDirect?: boolean) => void
}

const GeometrySpecificConfig: React.FC<IMeshConfigurator> = ({object, onChange}) => {
  const gltfModelSrcUrl = useResourceUrl(object.gltfModel?.src)
  const {t} = useTranslation(['cloud-studio-pages'])
  const rowClasses = useRowStyles()

  const materialTypeOptions = [
    {
      content: t('mesh_configurator.material_type.option.normal'),
      value: 'basic',
    },
    {
      content: t('mesh_configurator.material_type.option.hider'),
      value: 'hider',
    },
    {
      content: t('mesh_configurator.material_type.option.shadow'),
      value: 'shadow',
    },
    {
      content: t('mesh_configurator.material_type.option.video'),
      value: 'video',
    },
  ] as const

  const shadowConfig = (
    <>
      <RowBooleanField
        label={t('shadow_configurator.cast_shadow.label')}
        id='castShadow'
        expanseField={{
          leafPaths: [['shadow', 'castShadow']],
          diffOptions: {
            defaults: [false],
          },
        }}
        checked={!!object.shadow?.castShadow}
        onChange={(e) => {
          const {checked} = e.target
          onChange(o => ({
            ...o,
            shadow: undefinedIfDefaultShadow({...o.shadow, castShadow: checked}),
          }))
        }}
      />
      <RowBooleanField
        label={t('shadow_configurator.receive_shadow.label')}
        id='receiveShadow'
        expanseField={{
          leafPaths: [['shadow', 'receiveShadow']],
          diffOptions: {
            defaults: [false],
          },
        }}
        checked={!!object.shadow?.receiveShadow}
        onChange={(e) => {
          const {checked} = e.target
          onChange(o => ({
            ...o,
            shadow: undefinedIfDefaultShadow({...o.shadow, receiveShadow: checked}),
          }))
        }}
      />
    </>
  )

  if (object.gltfModel && gltfModelSrcUrl) {
    return (
      <>
        <RowSelectField
          id='material-type-selector'
          expanseField={{
            leafPaths: [['material', 'type']],
            diffOptions: {
              defaults: ['basic'],
            },
          }}
          label={t('mesh_configurator.material_type.label')}
          options={materialTypeOptions}
          value={object.material?.type ?? 'basic'}
          onChange={((value) => {
            let color: string | undefined
            if (value === 'shadow') {
              color = DEFAULT_SHADOW_COLOR
            } else if (value !== 'hider') {
              color = DEFAULT_MATERIAL_COLOR
            }
            onChange(o => ({
              ...o,
              material: {
                ...o.material,
                type: value,
                color,
                textureSrc: undefined,
                opacity: value === 'video' ? MATERIAL_DEFAULTS.opacity : undefined,
              },
            }))
          })}
        />

        {object.material?.type === 'shadow' &&
          <RowColorField
            id='shadowColorField'
            expanseField={{
              leafPaths: [['material', 'color']],
              diffOptions: {
                defaults: [DEFAULT_SHADOW_COLOR],
              },
            }}
            label={t('mesh_configurator.shadow_material.label.color')}
            value={object.material?.color ?? DEFAULT_SHADOW_COLOR}
            onChange={((value) => {
              onChange(o => ({
                ...o,
                material: {
                  ...o.material,
                  color: value,
                },
              }))
            })}
          />
        }

        {object.material?.type === 'video' &&
          <div className={rowClasses.indent}>
            <RowGroupFields
              label={t('mesh_configurator.video_material.label.color')}
              expanseField={{
                leafPaths: [['material', 'color'], ['material', 'textureSrc']],
                diffOptions: {
                  renderValue: makeRenderValueGroup(
                    [t('mesh_configurator.video_material.value'),
                      t('mesh_configurator.video_material.map')],
                    [
                      renderValueColor,
                      makeRenderValueVisualResource({
                        altText: t('scene_diff.image_alt_text'),
                      }),
                    ]
                  ),
                  defaults: [DEFAULT_MATERIAL_COLOR, undefined],
                },
              }}
            >
              <ColorPicker
                id='videoColorField'
                ariaLabel={t('material_configurator.fieldset.value')}
                value={object.material?.color ?? DEFAULT_MATERIAL_COLOR}
                onChange={color => onChange(o => ({...o, material: {...o.material, color}}))}
              />
              <CompactImagePicker
                assetKind='video'
                ariaLabel={t('material_configurator.fieldset.map')}
                resource={object.material?.textureSrc}
                onChange={src => onChange(o => ({
                  ...o, material: {...o.material, textureSrc: src},
                }))}
              />
            </RowGroupFields>
            <RowNumberField
              id='videoOpacityField'
              label={t('material_configurator.opacity.label')}
              value={object.material?.opacity ?? MATERIAL_DEFAULTS.opacity}
              onChange={opacity => onChange(o => ({...o, material: {...o.material, opacity}}))}
              step={0.01}
              max={1}
              min={0}
            />
          </div>
        }

        {shadowConfig}
        <GltfLoadBoundary key={gltfModelSrcUrl}>
          {BuildIf.STUDIO_MESH_MATERIAL_CONFIGURATOR_20240828
            ? <GltfMeshMaterialConfigurator url={gltfModelSrcUrl} />
            : null}
          <GltfAnimationConfigurator
            url={gltfModelSrcUrl}
            gltfModel={object.gltfModel}
            onChange={e => onChange(o => ({...o, gltfModel: e(object.gltfModel)}))}
          />
        </GltfLoadBoundary>
      </>
    )
  } else if (object.geometry) {
    return (
      <>
        {shadowConfig}
        <MaterialConfigurator
          material={object.material}
          onChange={e => onChange(o => ({...o, material: e(object.material)}))}
        />
      </>
    )
  }
  return null
}

const MeshConfigurator: React.FC<IMeshConfigurator> = (
  {object, onChange, resetToPrefab, applyOverridesToPrefab}
) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const stateCtx = useStudioStateContext()
  const isFaceGeometry = object.geometry && object.geometry.type === 'face'

  const geometryValue = () => {
    if (object.gltfModel?.src) {
      if (object.gltfModel.src.type === 'asset') {
        return object.gltfModel.src.asset
      } else {
        return MODEL_URL_VALUE
      }
    }
    if (object.splat?.src) {
      if (object.splat.src.type === 'asset') {
        return object.splat.src.asset
      } else {
        return SPLAT_URL_VALUE
      }
    }
    if (object.geometry) {
      return object.geometry.type
    }
    return ''
  }

  const rotateQuaternion = (
    quaternion: DeepReadonly<[number, number, number, number]>, angle: number
  ) => {
    const q = new Quaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3])
    const rotationQuaternion = new Quaternion()
    rotationQuaternion.setFromAxisAngle(new Vector3(1, 0, 0), angle)
    q.multiply(rotationQuaternion)
    return [q.x, q.y, q.z, q.w] as DeepReadonly<[number, number, number, number]>
  }

  const updateRotation = (
    geometryType: GeometryType, prevGeometryType: GeometryType,
    rotation: DeepReadonly<[number, number, number, number]>
  ) => {
    const currentRotation = rotation || [0, 0, 0, 1]

    if (geometryType === 'plane') {
      return rotateQuaternion(currentRotation, -Math.PI / 2)
    } else if (prevGeometryType === 'plane') {
      return rotateQuaternion(currentRotation, Math.PI / 2)
    }
    return currentRotation
  }

  const handleGeometrySelect = (value: string, category: MeshCategoryType | null) => {
    if (!value) {
      onChange(o => ({
        ...o,
        geometry: undefined,
        gltfModel: undefined,
        splat: undefined,
        material: o.material || makeMaterial(),
      }))
    } else if (category === MeshCategoryType.PRIMITIVE) {
      onChange(o => ({
        ...o,
        geometry: makeGeometry(value as GeometryType),
        gltfModel: undefined,
        splat: undefined,
        material: o.material || makeMaterial(),
        // Note: plane is rotated -90 degrees in the x axis for convenience
        rotation: updateRotation(value as GeometryType, o.geometry?.type, o.rotation),
      }))
    } else if (category === MeshCategoryType.MODEL || value === MODEL_URL_VALUE) {
      onChange(o => ({
        ...o,
        geometry: undefined,
        gltfModel: makeGltfModel(value === MODEL_URL_VALUE ? '' : value),
        splat: undefined,
        material: undefined,
      }))
    } else if (category === MeshCategoryType.SPLAT || value === SPLAT_URL_VALUE) {
      onChange(o => ({
        ...o,
        geometry: undefined,
        gltfModel: undefined,
        splat: makeSplat(value === SPLAT_URL_VALUE ? '' : value),
        material: undefined,
      }))
    }
  }

  return (
    <ComponentConfiguratorTray
      a8='click;studio;mesh-dropdown-button'
      title={t('mesh_configurator.title')}
      description={t('mesh_configurator.description')}
      sectionId={MESH_COMPONENT}
      expanseField={{
        leafPaths: [['geometry'], ['gltfModel'], ['splat'], ['material']],
      }}
      onRemove={() => onChange(() => ({
        ...object,
        geometry: undefined,
        splat: undefined,
        material: undefined,
        gltfModel: undefined,
      }))}
      onCopy={() => copyDirectProperties(stateCtx, {
        geometry: object.geometry,
        material: object.material,
        splat: object.splat,
        gltfModel: object.gltfModel,
      })}
      onResetToPrefab={resetToPrefab ? () => resetToPrefab(MESH_COMPONENTS) : undefined}
      onApplyOverridesToPrefab={applyOverridesToPrefab
        ? () => applyOverridesToPrefab(MESH_COMPONENTS)
        : undefined}
      componentData={[
        GEOMETRY_COMPONENT, MATERIAL_COMPONENT, SPLAT_COMPONENT,
        GLTF_MODEL_COMPONENT, SHADOW_COMPONENT,
      ]}
    >
      <MeshConfiguratorMenu
        value={geometryValue()}
        onChange={handleGeometrySelect}
        disabled={isFaceGeometry}
      />
      {object.gltfModel?.src?.type === 'url' &&
        <RowTextField
          id='geometry-url'
          expanseField={{
            leafPaths: [['gltfModel', 'src', 'url']],
          }}
          label={t('mesh_configurator.model_url.label')}
          value={object.gltfModel.src.url}
          onChange={e => onChange(o => ({
            ...o,
            gltfModel: {...o.gltfModel, src: {type: 'url', url: e.target.value}},
          }))}
        /> }
      {!!object.splat?.src &&
        <RowBooleanField
          id='splat-skybox'
          expanseField={{
            leafPaths: [['splat', 'skybox']],
          }}
          label={t('mesh_configurator_splat_skybox.label')}
          checked={!!object.splat.skybox}
          onChange={(e) => {
            onChange(o => ({...o, splat: {...o.splat, skybox: e.target.checked}}))
          }}
        />}
      <GeometryConfigurator
        geometry={object.geometry}
        onChange={e => onChange(o => ({...o, geometry: object.geometry && e(object.geometry)}))}
      />
      <GeometrySpecificConfig object={object} onChange={onChange} />
    </ComponentConfiguratorTray>
  )
}

export {
  MeshConfigurator,
}
