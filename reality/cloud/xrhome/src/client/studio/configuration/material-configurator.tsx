import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'

import type {
  BasicMaterial, Material, MaterialBlending, Resource, TextureFiltering, Side, TextureWrap,
} from '@ecs/shared/scene-graph'
import {
  DEFAULT_EMISSIVE_COLOR, DEFAULT_MATERIAL_COLOR, MATERIAL_DEFAULTS,
} from '@ecs/shared/material-constants'

import {FloatingPanelCollapsible} from '../../ui/components/floating-panel-collapsible'
import {makeMaterial} from '../make-object'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {
  RowBooleanField, RowColorField, RowGroupFields, RowNumberField, NumberInput, RowSelectField,
  ColorPicker,
  RowJointToggleButton,
} from './row-fields'
import {SliderInputAxisField} from '../../ui/components/slider-input-axis-field'
import {CompactImagePicker} from '../ui/compact-image-picker'
import {DEFAULT_SHADOW_COLOR, DEFAULT_SHADOW_OPACITY} from './material-constants'
import {useSliderInput} from '../../ui/hooks/use-slider-input'
import {ScenePathScopeProvider} from '../scene-path-input-context'
import {
  makeRenderValueGroup,
  makeRenderValueNumber,
  makeRenderValueVisualResource,
  renderValueColor,
} from './diff-chip-default-renderers'
import type {ConfigDiffInfo, DiffRenderInfo} from './diff-chip-types'
import {bannerFromRenderValue} from './diff-chip'
import type {ExpanseField} from './expanse-field-types'

const hasColor = (material: Material): material is Extract<Material, {color: string}> => (
  material && 'color' in material
)

interface IMaterialTextureAndLevel {
  label: string
  level: number
  textureResource: string | Resource
  onChange: (newValue: number) => void
  onTextureChange: (resource: Resource) => void
  levelPath: string[]
  texturePath: string[]
  customExpanseField?: ExpanseField<ConfigDiffInfo<unknown, string[][]>>
  max?: number
  defaultLevel?: number
}

const MaterialTextureAndLevel: React.FC<IMaterialTextureAndLevel> = ({
  label, level, textureResource, onChange, onTextureChange, levelPath, texturePath,
  defaultLevel, customExpanseField, max = 1,
// eslint-disable-next-line arrow-body-style
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const id = React.useId()
  const levelId = `material-level-${id}`
  const draggableProps = useSliderInput({
    value: level,
    min: 0,
    max,
    step: 0.001,
    onChange,
  })

  const expanseField = (levelPath && texturePath && !customExpanseField)
    ? {
      leafPaths: [levelPath, texturePath],
      diffOptions: {
        renderValue: makeRenderValueGroup(
          [t('material_configurator.fieldset.value'), t('material_configurator.fieldset.map')],
          [
            makeRenderValueNumber({step: 0.1}),
            makeRenderValueVisualResource({altText: t('scene_diff.image_alt_text')}),
          ]
        ),
        defaults: [defaultLevel, undefined],
      },
    }
    : customExpanseField

  return (
    <RowGroupFields
      label={<div {...draggableProps}>{label}</div>}
      expanseField={expanseField}
    >
      <NumberInput
        id={levelId}
        ariaLabel={t('material_configurator.fieldset.value')}
        value={level}
        onChange={onChange}
        min={0}
        max={max}
        step={0.1}
      />
      <CompactImagePicker
        assetKind={['image', 'video']}
        ariaLabel={t('material_configurator.fieldset.map')}
        resource={textureResource}
        onChange={onTextureChange}
      />
    </RowGroupFields>
  )
}

interface IEmissiveField {
  material: DeepReadonly<BasicMaterial>
  onChange: (updater: (current: DeepReadonly<Material>) => Material) => void
}

const EmissiveField: React.FC<IEmissiveField> = ({material, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const id = React.useId()
  const emissiveValue = material.emissiveIntensity ?? MATERIAL_DEFAULTS.emissiveIntensity

  const handleEmissiveChange = (value: number) => {
    onChange(old => ({...old, emissiveIntensity: value}))
  }

  const draggableProps = useSliderInput({
    value: emissiveValue,
    step: 0.01,
    min: 0,
    max: 1,
    onChange: handleEmissiveChange,
  })
  return (
    <RowGroupFields
      label={(
        <div {...draggableProps}>
          {t('material_configurator.emissive.label')}
        </div>
      )}
      expanseField={{
        leafPaths: [['emissiveIntensity'], ['emissiveColor'], ['emissiveMap']],
        diffOptions: {
          renderValue: makeRenderValueGroup(
            [t('material_configurator.fieldset.value'),
              t('material_configurator.emissive_color.label'),
              t('material_configurator.fieldset.map')],
            [
              makeRenderValueNumber({step: 0.1}),
              renderValueColor,
              makeRenderValueVisualResource({altText: t('scene_diff.image_alt_text')}),
            ]
          ),
          defaults: [MATERIAL_DEFAULTS.emissiveIntensity, DEFAULT_EMISSIVE_COLOR, undefined],
        },
      }}
    >
      <NumberInput
        id={`material-emissive-intensity-${id}`}
        ariaLabel={t('material_configurator.fieldset.value')}
        value={emissiveValue}
        onChange={handleEmissiveChange}
        min={0}
        max={1}
        step={0.1}
      />
      <ColorPicker
        id={`material-emissive-color-${id}`}
        ariaLabel={t('material_configurator.emissive_color.label')}
        value={material.emissiveColor ?? DEFAULT_EMISSIVE_COLOR}
        onChange={(emissiveColor) => {
          onChange(old => ({...old, emissiveColor}))
        }}
        hideInput
      />
      <CompactImagePicker
        assetKind={['image', 'video']}
        ariaLabel={t('material_configurator.fieldset.map')}
        resource={material.emissiveMap}
        onChange={(src) => {
          onChange(old => ({...old, emissiveMap: src}))
        }}
      />
    </RowGroupFields>
  )
}

const COLOR_EXPANSE_FIELD = {
  leafPaths: [['color']],
  diffOptions: {
    renderValue: renderValueColor,
    defaults: [DEFAULT_MATERIAL_COLOR],
  },
}

interface IMaterialConfigurator {
  title?: string
  material: DeepReadonly<Material>
  defaultOpen?: boolean
  isAsset?: boolean
  onChange: (updater: (current: DeepReadonly<Material>) => DeepReadonly<Material>) => void
}

type MaterialTypeOption = { value: Material['type']; content: string; a8: string; }

const MAT_TYPES: {[type: string]: MaterialTypeOption} = {
  basic: {
    value: 'basic',
    content: 'material_configurator.type.option.standard',
    a8: 'click;studio;material-type-standard-button',
  },
  unlit: {
    value: 'unlit',
    content: 'material_configurator.type.option.unlit',
    a8: 'click;studio;material-type-unlit-button',
  },
  shadow: {
    value: 'shadow',
    content: 'material_configurator.type.option.shadow',
    a8: 'click;studio;material-type-shadow-button',
  },
  hider: {
    value: 'hider',
    content: 'material_configurator.type.option.hider',
    a8: 'click;studio;material-type-standard-button',
  },
}

const getMaterialTypeOptions = (isAsset: boolean): MaterialTypeOption[] => {
  if (isAsset) {
    return [
      MAT_TYPES.basic,
      MAT_TYPES.unlit,
    ]
  }

  return [
    MAT_TYPES.basic,
    MAT_TYPES.shadow,
    MAT_TYPES.unlit,
    MAT_TYPES.hider,
  ]
}

const getDefaultOpacity = (type: Material['type']): number => {
  if (type === 'shadow') {
    return DEFAULT_SHADOW_OPACITY
  } else {
    return MATERIAL_DEFAULTS.opacity
  }
}

const renderShadowChangeOpacity = (
  prev: [number, Material['type']], now: [number, Material['type']]
) => {
  const [prevOpacity, prevType] = prev
  const [newOpacity, nowType] = now

  // If either type is undefined, we shouldn't render
  // a diff.
  if (prevType === undefined || nowType === undefined) {
    return {
      type: null,
      bannerContent: null,
    }
  }

  const truePrevOpacity = prevOpacity ?? getDefaultOpacity(prevType)
  const trueNowOpacity = newOpacity ?? getDefaultOpacity(nowType)

  const precisionRenderer = makeRenderValueNumber({
    step: 0.01,
  })

  if (truePrevOpacity !== trueNowOpacity) {
    return {
      type: 'changedDirectly',
      bannerContent: bannerFromRenderValue(
        precisionRenderer, truePrevOpacity, trueNowOpacity
      ),
    }
  } else {
    return {
      type: 'unchanged',
      bannerContent: null,
    }
  }
}

const MaterialConfigurator: React.FC<IMaterialConfigurator> = ({
  title, material, defaultOpen, onChange, isAsset = false,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const typeOptions = getMaterialTypeOptions(isAsset)
  const id = React.useId()

  const customOpacityMapRenderDiff = (
    prev: [Resource, number, Material['type']], now: [Resource, number, Material['type']]
  ): DiffRenderInfo => {
    const [prevResource, prevOpacity, prevType] = prev
    const [nowResource, nowOpacity, nowType] = now

    if (nowType === undefined || prevType === undefined) {
    // Can't render a meaningful diff here.
      return {
        type: null,
        bannerContent: null,
      }
    }

    const truePrevOpacity = prevOpacity ?? getDefaultOpacity(prevType)
    const trueNowOpacity = nowOpacity ?? getDefaultOpacity(nowType)

    if (
      JSON.stringify(prevResource) !== JSON.stringify(nowResource) ||
      truePrevOpacity !== trueNowOpacity
    ) {
      const resourceRenderer = makeRenderValueVisualResource({
        altText: t('scene_diff.image_alt_text'),
      })
      const precisionRenderer = makeRenderValueNumber({
        step: 0.01,
      })

      const groupRenderer = makeRenderValueGroup(
        [t('material_configurator.fieldset.map'), t('material_configurator.opacity.label')],
        [resourceRenderer, precisionRenderer]
      )

      return {
        type: 'changedDirectly',
        bannerContent: bannerFromRenderValue(
          groupRenderer, [prevResource, truePrevOpacity], [nowResource, trueNowOpacity]
        ),
      }
    }
    return {
      type: 'unchanged',
      bannerContent: null,
    }
  }

  const customOpacityMapExpanseField: ExpanseField<ConfigDiffInfo<unknown, string[][]>> = {
    leafPaths: [['opacityMap'], ['opacity'], ['type']],
    diffOptions: {
      renderDiff: customOpacityMapRenderDiff,
    },
  }

  const handleDrag = (propertyKey: string, difference: number) => {
    onChange((old) => {
      if (old.type === 'basic') {
        const currentValue = old[propertyKey] ?? MATERIAL_DEFAULTS[propertyKey]
        return {
          ...old,
          [propertyKey]: currentValue + difference,
        }
      }
      return old
    })
  }

  const getTitle = () => {
    switch (material?.type) {
      case 'shadow': return t('material_configurator.title.shadow')
      case 'unlit': return t('material_configurator.title.unlit')
      case 'hider': return t('material_configurator.title.hider')
      default: return t('material_configurator.title.standard')
    }
  }

  const hasColorImage = material?.type === 'basic' || material?.type === 'unlit'
  const hasPhysicalProperties = material?.type === 'basic'
  const hasOpacity = material?.type === 'basic' || material?.type === 'unlit'
  const hasEmissive = material?.type === 'basic'
  const hasSide = material && material?.type !== 'hider' && material?.type !== 'video'
  const hasBlending = material?.type === 'basic' || material?.type === 'unlit'

  const hasColorTexture = hasColorImage && material.textureSrc
  const hasOpacityTexture = !isAsset && hasOpacity && material.opacityMap
  const hasOtherTextures = hasPhysicalProperties && (
    material.roughnessMap || material.metalnessMap || material.normalMap || material.emissiveMap)
  const hasTextureSettings = hasColorTexture || hasOpacityTexture || hasOtherTextures
  const hasDepthSettings = material && material?.type !== 'hider' && material?.type !== 'video'
  const hasWireframe = material?.type === 'basic' || material?.type === 'unlit'
  const isShadow = material?.type === 'shadow'
  const hasForceTransparent = (
    hasColorImage && (material.opacity ?? MATERIAL_DEFAULTS.opacity) === 1 && !material.opacityMap
  )

  const lastColor = React.useRef<string>()

  const handleMaterialChange = (
    value: Material['type'],
    current: DeepReadonly<Material>
  ): DeepReadonly<Material> => {
    if (isAsset) {
      return {...current, type: value} as Material
    }

    if (value === 'hider') {
      lastColor.current = hasColor(current) ? current.color : ''
      return {type: 'hider'}
    }

    // Shadow material has different defaults (for color and opacity)
    if (value === 'shadow') {
      lastColor.current = hasColor(current) ? current.color : ''
      return {...current, color: DEFAULT_SHADOW_COLOR, opacity: DEFAULT_SHADOW_OPACITY, type: value}
    }

    const useDefaults = current.type === 'shadow' || current.type === 'hider'
    let color = lastColor.current || DEFAULT_MATERIAL_COLOR
    if (!useDefaults && hasColor(current)) {
      color = current.color
    }

    const newBase = {
      color,
      opacity: useDefaults ? undefined : current.opacity,
    }

    return {...current, ...newBase, type: value}
  }

  return (
    <FloatingPanelCollapsible
      a8={typeOptions.find(option => option.value === material?.type)?.a8}
      title={title ?? getTitle()}
      defaultOpen={defaultOpen}
      // FloatingPanelCollapsible always defaults to open if sectionId is defined
      sectionId={defaultOpen === false ? undefined : 'material'}
    >
      <ScenePathScopeProvider path={['material']}>
        {material &&
          <RowSelectField
            label={t('material_configurator.type.label')}
            options={typeOptions.map(type => ({value: type.value, content: t(type.content)}))}
            expanseField='type'
            onChange={(value) => {
              onChange(current => handleMaterialChange(value, current))
            }}
            value={material.type}
          />}
        {hasColorImage &&
          <RowGroupFields
            label={t('material_configurator.color.label')}
            expanseField={COLOR_EXPANSE_FIELD}
          >
            <ColorPicker
              id={`material-color-${id}`}
              ariaLabel={t('material_configurator.fieldset.value')}
              value={material.color}
              onChange={(color) => {
                onChange(old => ({...old, color}))
              }}
            />
            <CompactImagePicker
              assetKind={['image', 'video']}
              ariaLabel={t('material_configurator.fieldset.map')}
              resource={material.textureSrc}
              onChange={(src) => {
                onChange(old => ({...old, textureSrc: src}))
              }}
            />
          </RowGroupFields>
        }
        {hasPhysicalProperties &&
          <>
            <MaterialTextureAndLevel
              label={t('material_configurator.roughness.label')}
              levelPath={['roughness']}
              defaultLevel={MATERIAL_DEFAULTS.roughness}
              texturePath={['roughnessMap']}
              level={material.roughness ?? MATERIAL_DEFAULTS.roughness}
              textureResource={material.roughnessMap}
              onChange={(newValue) => {
                onChange(old => ({...old, roughness: newValue}))
              }}
              onTextureChange={(src) => {
                onChange(old => ({...old, roughnessMap: src}))
              }}
            />
            <MaterialTextureAndLevel
              label={t('material_configurator.metalness.label')}
              levelPath={['metalness']}
              defaultLevel={MATERIAL_DEFAULTS.metalness}
              texturePath={['metalnessMap']}
              level={material.metalness ?? MATERIAL_DEFAULTS.metalness}
              textureResource={material.metalnessMap}
              onChange={(newValue) => {
                onChange(old => ({...old, metalness: newValue}))
              }}
              onTextureChange={(src) => {
                onChange(old => ({...old, metalnessMap: src}))
              }}
            />
            <MaterialTextureAndLevel
              label={t('material_configurator.normal.label')}
              levelPath={['normalScale']}
              defaultLevel={MATERIAL_DEFAULTS.normalScale}
              texturePath={['normalMap']}
              level={material.normalScale ?? MATERIAL_DEFAULTS.normalScale}
              textureResource={material.normalMap}
              onChange={(newValue) => {
                onChange(old => ({...old, normalScale: newValue}))
              }}
              onTextureChange={(src) => {
                onChange(old => ({...old, normalMap: src}))
              }}
              max={Infinity}
            />
          </>
        }
        {hasOpacity && !isAsset &&
          <MaterialTextureAndLevel
            label={t('material_configurator.opacity.label')}
            levelPath={['opacity']}
            defaultLevel={isShadow ? DEFAULT_SHADOW_OPACITY : MATERIAL_DEFAULTS.opacity}
            texturePath={['opacityMap']}
            // TODO(Carson): Fix edge case by expanding diffRenderer to listen to
            // type of material as well.
            level={material.opacity ?? (
              isShadow ? DEFAULT_SHADOW_OPACITY : MATERIAL_DEFAULTS.opacity)}
            textureResource={material.opacityMap}
            customExpanseField={customOpacityMapExpanseField}
            onChange={(newValue) => {
              onChange(old => ({...old, opacity: newValue}))
            }}
            onTextureChange={(src) => {
              onChange(old => ({...old, opacityMap: src}))
            }}
          />
        }
        {hasOpacity && isAsset &&
          <RowNumberField
            expanseField={{
              leafPaths: [['opacity'], ['type']],
              diffOptions: {
                renderDiff: (
                  renderShadowChangeOpacity as unknown as (
                    (prev: number, now: number) => DiffRenderInfo
                  )
                ),
              },
            }}
            label={t('material_configurator.opacity.label')}
            value={material.opacity ?? (
              isShadow ? DEFAULT_SHADOW_OPACITY : MATERIAL_DEFAULTS.opacity)}
            onChange={(newValue) => {
              onChange(old => ({...old, opacity: newValue}))
            }}
            step={0.01}
            max={1}
            min={0}
          />
        }
        {hasEmissive &&
          <EmissiveField
            material={material}
            onChange={onChange}
          />
        }
        {material?.type === 'shadow' &&
          <>
            <RowColorField
              expanseField='color'
              label={t('material_configurator.color.label')}
              value={material.color}
              onChange={(color) => {
                onChange(old => ({...old, color}))
              }}
            />
            <RowNumberField
              expanseField={{
                leafPaths: [['opacity'], ['type']],
                diffOptions: {
                  renderDiff: renderShadowChangeOpacity as unknown as (
                    (prev: number, now: number) => DiffRenderInfo
                  ),
                },
              }}
              label={t('material_configurator.opacity.label')}
              value={material.opacity ?? DEFAULT_SHADOW_OPACITY}
              onChange={(opacity) => {
                onChange(old => ({...old, opacity}))
              }}
              min={0}
              max={1}
              step={0.1}
            />
          </>
          }
        {hasSide &&
          <RowSelectField
            expanseField={{
              leafPaths: [['side']],
              diffOptions: {
                defaults: [MATERIAL_DEFAULTS.side],
              },
            }}
            label={t('material_configurator.side.label')}
            options={[
              {value: 'front', content: t('material_configurator.side.option.front')},
              {value: 'back', content: t('material_configurator.side.option.back')},
              {value: 'double', content: t('material_configurator.side.option.double')},
            ]}
            value={material.side ?? MATERIAL_DEFAULTS.side}
            onChange={(value) => {
              const newValue = value as Side
              onChange(old => ({...old, side: newValue}))
            }}
          />}
        {hasBlending &&
          <RowSelectField
            expanseField={{
              leafPaths: [['blending']],
              diffOptions: {
                defaults: [MATERIAL_DEFAULTS.blending],
              },
            }}
            label={t('material_configurator.blending.label')}
            value={material.blending ?? MATERIAL_DEFAULTS.blending}
            options={[
              {value: 'no', content: t('material_configurator.blending.option.no')},
              {value: 'normal', content: t('material_configurator.blending.option.normal')},
              {value: 'additive', content: t('material_configurator.blending.option.additive')},
              {
                value: 'subtractive',
                content: t('material_configurator.blending.option.subtractive'),
              },
              {value: 'multiply', content: t('material_configurator.blending.option.multiply')},
            ]}
            onChange={(value) => {
              onChange(old => ({...old, blending: value as MaterialBlending}))
            }}
          />
        }
        {hasColorImage &&
          <>
            <RowJointToggleButton
              label={t('material_configurator.texture_filtering.label')}
              value={material.textureFiltering ?? MATERIAL_DEFAULTS.textureFiltering}
              expanseField={{
                leafPaths: [['textureFiltering']],
                diffOptions: {
                  defaults: [MATERIAL_DEFAULTS.textureFiltering],
                },
              }}
              options={[
                {
                  value: 'smooth',
                  content: t('material_configurator.texture_filtering.option.smooth'),
                },
                {
                  value: 'sharp',
                  content: t('material_configurator.texture_filtering.option.sharp'),
                },
              ]}
              onChange={(value) => {
                const newValue = value as TextureFiltering
                onChange(old => ({...old, textureFiltering: newValue}))
              }}
            />
            <RowBooleanField
              expanseField={{
                leafPaths: [['mipmaps']],
                diffOptions: {
                  defaults: [MATERIAL_DEFAULTS.mipmaps],
                },
              }}
              label={t('material_configurator.use_mipmaps.label')}
              checked={material.mipmaps ?? MATERIAL_DEFAULTS.mipmaps}
              onChange={e => onChange(old => old && ({...old, mipmaps: e.target.checked}))}
            />
          </>
      }
        {hasTextureSettings &&
          <>
            <RowSelectField
              expanseField={{
                leafPaths: [['wrap']],
                diffOptions: {
                  defaults: [MATERIAL_DEFAULTS.wrap],
                },
              }}
              label={t('material_configurator.wrap.label')}
              options={[
                {value: 'repeat', content: t('material_configurator.wrap.option.repeat')},
                {value: 'clamp', content: t('material_configurator.wrap.option.clamp')},
                {
                  value: 'mirroredRepeat',
                  content: t('material_configurator.wrap.option.mirrored_repeat'),
                },
              ]}
              value={material.wrap ?? MATERIAL_DEFAULTS.wrap}
              onChange={(value) => {
                const newValue = value as TextureWrap
                onChange(old => ({...old, wrap: newValue}))
              }}
            />
            <RowGroupFields
              label={t('material_configurator.repeat.label')}
              expanseField={{
                leafPaths: [['repeatX'], ['repeatY']],
                diffOptions: {
                  renderValue: makeRenderValueGroup(
                    ['X', 'Y'],
                    [makeRenderValueNumber({step: 0.1}), makeRenderValueNumber({step: 0.1})]
                  ),
                  defaults: [MATERIAL_DEFAULTS.repeatX, MATERIAL_DEFAULTS.repeatY],
                },
              }}
            >
              <SliderInputAxisField
                label='X'
                value={material.repeatX ?? MATERIAL_DEFAULTS.repeatX}
                onChange={repeatX => onChange(old => ({...old, repeatX}))}
                onDrag={difference => handleDrag('repeatX', difference)}
                min={0}
                step={0.1}
                fullWidth
              />
              <SliderInputAxisField
                label='Y'
                value={material.repeatY ?? MATERIAL_DEFAULTS.repeatY}
                onChange={repeatY => onChange(old => ({...old, repeatY}))}
                onDrag={difference => handleDrag('repeatY', difference)}
                min={0}
                step={0.1}
                fullWidth
              />
            </RowGroupFields>
            <RowGroupFields
              label={t('material_configurator.offset.label')}
              expanseField={{
                leafPaths: [['offsetX'], ['offsetY']],
                diffOptions: {
                  renderValue: makeRenderValueGroup(
                    ['X', 'Y'],
                    [makeRenderValueNumber({step: 0.1}), makeRenderValueNumber({step: 0.1})]
                  ),
                  defaults: [MATERIAL_DEFAULTS.offsetX, MATERIAL_DEFAULTS.offsetY],
                },
              }}
            >
              <SliderInputAxisField
                label='X'
                value={material.offsetX ?? MATERIAL_DEFAULTS.offsetX}
                onChange={offsetX => onChange(old => ({...old, offsetX}))}
                onDrag={difference => handleDrag('offsetX', difference)}
                step={0.1}
                fullWidth
              />
              <SliderInputAxisField
                label='Y'
                value={material.offsetY ?? MATERIAL_DEFAULTS.offsetY}
                onChange={offsetY => onChange(old => ({...old, offsetY}))}
                onDrag={difference => handleDrag('offsetY', difference)}
                step={0.1}
                fullWidth
              />
            </RowGroupFields>
          </>}
        {!isAsset && hasDepthSettings &&
          <>
            <RowBooleanField
              expanseField={{
                leafPaths: [['depthTest']],
                diffOptions: {
                  defaults: [MATERIAL_DEFAULTS.depthTest],
                },
              }}
              label={t('material_configurator.depth_test.label')}
              checked={material.depthTest ?? MATERIAL_DEFAULTS.depthTest}
              onChange={e => onChange(current => current && ({
                ...current,
                depthTest: e.target.checked,
              }))}
            />
            <RowBooleanField
              expanseField={{
                leafPaths: [['depthWrite']],
                diffOptions: {
                  defaults: [MATERIAL_DEFAULTS.depthWrite],
                },
              }}
              label={t('material_configurator.depth_write.label')}
              checked={material.depthWrite ?? MATERIAL_DEFAULTS.depthWrite}
              onChange={e => onChange(current => current && ({
                ...current,
                depthWrite: e.target.checked,
              }))}
            />
          </>}
        {!isAsset && hasWireframe &&
          <RowBooleanField
            expanseField={{
              leafPaths: [['wireframe']],
              diffOptions: {
                defaults: [MATERIAL_DEFAULTS.wireframe],
              },
            }}
            label={t('material_configurator.wireframe.label')}
            checked={material.wireframe ?? MATERIAL_DEFAULTS.wireframe}
            onChange={e => onChange(current => current && ({
              ...current,
              wireframe: e.target.checked,
            }))}
          />}
        {hasForceTransparent &&
          <RowBooleanField
            expanseField={{
              leafPaths: [['forceTransparent']],
              diffOptions: {
                defaults: [MATERIAL_DEFAULTS.forceTransparent],
              },
            }}
            label={t('material_configurator.force_transparent.label')}
            checked={material.forceTransparent ?? MATERIAL_DEFAULTS.forceTransparent}
            onChange={e => onChange(current => current && ({
              ...current,
              forceTransparent: e.target.checked,
            }))}
          />}
        {!material &&
          <>
            <p>{t('material_configurator.no_material')}</p>
            <FloatingPanelButton
              type='button'
              onClick={() => {
                onChange(() => makeMaterial())
              }}
            >{t('material_configurator.button.add_material')}
            </FloatingPanelButton>
          </>}
      </ScenePathScopeProvider>
    </FloatingPanelCollapsible>
  )
}

export {
  MaterialConfigurator,
}
