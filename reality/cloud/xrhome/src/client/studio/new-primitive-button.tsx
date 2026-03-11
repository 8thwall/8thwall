import React from 'react'
import type {GeometryType, GraphObject, LightType} from '@ecs/shared/scene-graph'
import {TFunction, useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import type {StudioStateContext} from './studio-state-context'
import type {SceneContext} from './scene-context'
import {SelectMenu} from './ui/select-menu'
import {
  PrimaryGeometryTypes, SecondaryGeometryTypes, LightTypes, makeEmptyObject, makePrimitive,
  makeCamera, makeFace, makeMap, makeImageTarget,
} from './make-object'
import {Icon, IconStroke} from '../ui/components/icon'
import {SpaceBetween} from '../ui/layout/space-between'
import {makeObjectName} from './common/studio-files'
import {useSceneContext} from './scene-context'
import {useStudioMenuStyles} from './ui/studio-menu-styles'
import {useStudioStateContext} from './studio-state-context'
import {FloatingPanelIconButton} from '../ui/components/floating-panel-icon-button'
import {getActiveSpaceFromCtx, useActiveSpace} from './hooks/active-space'
import {MenuOption, MenuOptions} from './ui/option-menu'
import {spawnUiPreset, UI_PRESETS} from './ui-presets'
import {useDerivedScene} from './derived-scene-context'
import type {DerivedScene} from './derive-scene'
import {getNextChildOrder} from './reparent-object'

const useStyles = createUseStyles({
  primitiveOption: {
    width: '100%',
    padding: '0.1em 0.0em',
  },
  uiPresetOption: {
    display: 'flex',
    alignItems: 'center',
  },
})

interface IPrimitiveOption {
  label: string
  stroke: IconStroke
}

const PrimitiveOption: React.FC<IPrimitiveOption> = ({label, stroke}) => {
  const classes = useStyles()
  return (
    <div className={classes.primitiveOption}>
      <SpaceBetween narrow>
        {stroke ? <Icon stroke={stroke} /> : null }
        {label}
      </SpaceBetween>
    </div>
  )
}

const createAndSelectObject = (
  object: GraphObject,
  ctx: SceneContext,
  stateCtx: StudioStateContext,
  makePrefab?: boolean
) => {
  const spaceId = getActiveSpaceFromCtx(ctx, stateCtx)?.id
  const prefab = stateCtx.state.selectedPrefab
  ctx.updateScene(s => ({
    ...s,
    objects: {
      ...s.objects,
      [object.id]: {
        ...object,
        name: makeObjectName(object.name, s, spaceId, prefab, makePrefab),
        order: object.order ?? getNextChildOrder(s, object.parentId),
        ...(makePrefab ? {prefab: true} : {}),
      },
    },
  }))
  stateCtx.setSelection(object.id)
}

const createPrimitiveOption = (label: string, onClick: () => void, stroke?: IconStroke) => ({
  content: (<PrimitiveOption label={label} stroke={stroke} />),
  onClick,
})

const createNewPrimitiveOptions = (
  newObjectParentId: string, ctx: SceneContext, stateCtx: StudioStateContext,
  derivedScene: DerivedScene, t: TFunction, makePrefab?: boolean
) => {
  const handleNewPrimitive = (type: string, label?: string) => {
    let newEntity: GraphObject
    if (type === 'empty') {
      newEntity = makeEmptyObject(newObjectParentId)
      newEntity.name = t('new_primitive_button.option.empty')
    } else if (type === 'face') {
      newEntity = makeFace(newObjectParentId)
      newEntity.name = t('new_primitive_button.option.face')
    } else if (type === 'imageTarget') {
      newEntity = makeImageTarget(newObjectParentId)
      newEntity.name = t('new_primitive_button.option.image_target')
    } else if (type) {
      newEntity = makePrimitive(newObjectParentId, type as GeometryType | LightType)
      newEntity.name = t(label)
    } else {
      return
    }
    createAndSelectObject(newEntity, ctx, stateCtx, makePrefab)
  }

  const primaryGeometryOptions: MenuOption[] = PrimaryGeometryTypes.map(value => (
    createPrimitiveOption(
      t(value.label),
      () => handleNewPrimitive(value.type, value.label),
      value.stroke as IconStroke
    )
  ))

  const secondaryGeometryOptions: MenuOption = {
    type: 'menu',
    content: (
      <SpaceBetween narrow>
        <Icon stroke='primitives' />
        {t('new_primitive_button.option.more')}
      </SpaceBetween>
    ),
    options: SecondaryGeometryTypes.map(value => (
      createPrimitiveOption(
        t(value.label),
        () => handleNewPrimitive(value.type, value.label),
        value.stroke as IconStroke
      )
    )),
  }

  const emptyOption: MenuOption = createPrimitiveOption(
    t('new_primitive_button.option.empty'),
    () => handleNewPrimitive('empty'), 'empty'
  )

  const faceOption: MenuOption = createPrimitiveOption(
    t('new_primitive_button.option.face'),
    () => handleNewPrimitive('face'), 'face'
  )

  const lightMenuOptions: MenuOption = {
    type: 'menu',
    content: (
      <SpaceBetween narrow>
        <Icon stroke='pointLight' />
        {t('new_primitive_button.option.light')}
      </SpaceBetween>
    ),
    options: LightTypes.map(light => (
      createPrimitiveOption(
        t(light.label),
        () => handleNewPrimitive(light.type, light.label),
        light.stroke as IconStroke
      )
    )),
  }

  const cameraOption: MenuOption = createPrimitiveOption(
    t('new_primitive_button.option.camera'),
    () => {
      createAndSelectObject(
        makeCamera(newObjectParentId, t('new_primitive_button.option.camera')),
        ctx, stateCtx, makePrefab
      )
    },
    'camera'
  )

  const uiMenuOptions: MenuOption = {
    type: 'menu',
    content: (
      <SpaceBetween narrow>
        <Icon stroke='ui' />
        {t('new_primitive_button.option.ui')}
      </SpaceBetween>
    ),
    options: UI_PRESETS.map(({nameKey, type}) => (
      createPrimitiveOption(
        t(nameKey),
        () => spawnUiPreset(ctx, stateCtx, derivedScene, newObjectParentId, type, t),
        'ui'
      )
    )),
  }

  const mapOption: MenuOption = createPrimitiveOption(
    t('new_primitive_button.option.map'),
    () => createAndSelectObject(
      makeMap(newObjectParentId, t('new_primitive_button.option.map')),
      ctx, stateCtx, makePrefab
    ), 'mapOutline'
  )

  const imageTargetOption: MenuOption = createPrimitiveOption(
    t('new_primitive_button.option.image_target'),
    () => handleNewPrimitive('imageTarget'), 'imageTarget'
  )

  const divider: MenuOption = {type: 'divider'}

  return [
    emptyOption,
    divider,
    ...primaryGeometryOptions,
    secondaryGeometryOptions,
    divider,
    lightMenuOptions,
    divider,
    cameraOption,
    divider,
    faceOption,
    divider,
    imageTargetOption,
    divider,
    uiMenuOptions,
    divider,
    mapOption,
  ]
}

const useNewPrimitiveOptions = (newObjectParentId: string, makePrefab?: boolean): MenuOption[] => {
  const {t} = useTranslation('cloud-studio-pages')
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()

  const options = React.useMemo(() => (
    createNewPrimitiveOptions(newObjectParentId, ctx, stateCtx, derivedScene, t, makePrefab)
  ), [newObjectParentId, ctx, stateCtx, derivedScene, t, makePrefab])
  return options
}

interface INewPrimitiveButton {
  makePrefab?: boolean
}

const NewPrimitiveButton: React.FC<INewPrimitiveButton> = ({makePrefab}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const stateCtx = useStudioStateContext()
  const classes = useStudioMenuStyles()
  const activeSpace = useActiveSpace()?.id
  const prefab = stateCtx.state.selectedPrefab
  const newObjectParentId = makePrefab ? undefined : (prefab || activeSpace)

  const primitiveOptions = useNewPrimitiveOptions(newObjectParentId, makePrefab)

  return (
    <SelectMenu
      id='add-component-select-menu'
      menuWrapperClassName={classes.studioMenu}
      trigger={(
        <FloatingPanelIconButton
          a8='click;studio;new-object-menu-button'
          text={t('new_primitive_button.button.new_object')}
          stroke='plus'
        />
      )}
      placement='right-start'
      margin={17}
      returnFocus={false}
    >
      {collapse => (
        <MenuOptions
          options={primitiveOptions}
          collapse={collapse}
          returnFocus={false}
        />
      )}
    </SelectMenu>
  )
}

export {
  NewPrimitiveButton,
  createAndSelectObject,
  useNewPrimitiveOptions,
}
