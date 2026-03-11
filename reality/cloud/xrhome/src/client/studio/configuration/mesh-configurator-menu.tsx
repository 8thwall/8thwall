import React from 'react'
import {useTranslation} from 'react-i18next'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../../ui/theme'
import {useStyles as useRowFieldStyles} from './row-fields'
import {SubMenuSelectWithSearch} from '../ui/submenu-select-with-search'
import type {SubMenuCategory, SubMenuItem} from '../ui/submenu'
import {basename} from '../../editor/editor-common'
import {getFilesByKind} from '../common/studio-files'
import {useScopedGit} from '../../git/hooks/use-current-git'
import {useCurrentRepoId} from '../../git/repo-id-context'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {RowLayout} from './row-layout'
import type {ExpanseField} from './expanse-field-types'
import type {ConfigDiffInfo} from './diff-chip-types'

const useStyles = createThemedStyles(theme => ({
  floatingMenu: {
    padding: 0,
    gap: 0,
  },
  searchResultsContainer: {
    padding: '0.5em',
    maxHeight: '200px',
    overflow: 'auto',
  },
  selectContainer: {
    flex: 1,
    fontSize: '12px',
  },
  button: {
    display: 'flex',
    gap: '1em',
    alignItems: 'center',
    justifyContent: 'space-between',
    overflow: 'hidden',
    whiteSpace: 'nowrap',
    padding: '0.325em 0.5em',
  },
  searchInputContainer: {
    margin: '0.5em 0.5em 0 0.5em',
  },
  placeholder: {
    color: theme.fgMuted,
    fontStyle: 'italic',
    padding: '0.125rem 0',
  },
  selectedBinding: {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5em',
  },
}))

enum MeshCategoryType {
  PRIMITIVE = 'mesh_configurator_menu.category.primitive',
  MODEL = 'mesh_configurator_menu.category.gltf_model',
  SPLAT = 'mesh_configurator_menu.category.splat',
}

const MODEL_URL_VALUE = 'model-url'
const SPLAT_URL_VALUE = 'splat-url'

interface IMeshConfiguratorMenu {
  value: string
  onChange: (newValue: string, category: MeshCategoryType) => void
  disabled: boolean
}

const MeshConfiguratorMenu: React.FC<IMeshConfiguratorMenu> = (
  {value, onChange, disabled}
) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()
  const rowFieldStyles = useRowFieldStyles()

  const meshOptions: SubMenuItem[] = [
    {value: 'box', content: t('mesh_configurator.geometry_type.option.box'), icon: 'box'},
    {value: 'sphere', content: t('mesh_configurator.geometry_type.option.sphere'), icon: 'sphere'},
    {value: 'plane', content: t('mesh_configurator.geometry_type.option.plane'), icon: 'plane'},
    {
      value: 'capsule',
      content: t('mesh_configurator.geometry_type.option.capsule'),
      icon: 'capsule',
    },
    {value: 'cone', content: t('mesh_configurator.geometry_type.option.cone'), icon: 'cone'},
    {
      value: 'cylinder',
      content: t('mesh_configurator.geometry_type.option.cylinder'),
      icon: 'cylinder',
    },
    {
      value: 'polyhedron',
      content: t('mesh_configurator.geometry_type.option.polyhedron'),
      icon: 'tetrahedron',
    },
    {value: 'circle', content: t('mesh_configurator.geometry_type.option.circle'), icon: 'circle'},
    {value: 'ring', content: t('mesh_configurator.geometry_type.option.ring'), icon: 'ring'},
    {value: 'torus', content: t('mesh_configurator.geometry_type.option.torus'), icon: 'torus'},
  ]

  const repoId = useCurrentRepoId()
  const filesByPath = useScopedGit(repoId, git => git.filesByPath)
  const gltfFiles = getFilesByKind(filesByPath, 'model')
  const splatFiles = getFilesByKind(filesByPath, 'splat')

  const getFilteredOptions = (options: SubMenuItem[]) => options
    .filter(option => option.value !== value)

  const rootOptions = [
    {
      value: MODEL_URL_VALUE,
      content: t('mesh_configurator.geometry_type.option.model_url'),
    },
    {
      value: SPLAT_URL_VALUE,
      content: t('mesh_configurator.geometry_type.option.splat_url'),
    },
    {
      value: '',
      content: t('mesh_configurator.geometry_type.option.none'),
    },
  ]
  const modelOptions = [
    ...gltfFiles.map(file => ({value: file, content: basename(file)})),
  ]
  const splatOptions = [
    ...splatFiles.map(file => ({value: file, content: basename(file)})),
  ]
  const allOptions = [...rootOptions, ...meshOptions, ...modelOptions, ...splatOptions]

  const meshCategories = [
    {
      value: null,
      parent: null,
      options: rootOptions,
    },
    {
      value: MeshCategoryType.PRIMITIVE,
      parent: null,
      options: getFilteredOptions(meshOptions),
    },
    modelOptions.length > 0 && {
      value: MeshCategoryType.MODEL,
      parent: null,
      options: getFilteredOptions(modelOptions),
    },
    splatOptions.length > 0 && {
      value: MeshCategoryType.SPLAT,
      parent: null,
      options: getFilteredOptions(splatOptions),
    },
  ].filter(Boolean) as SubMenuCategory[]

  const menuExpanseField: ExpanseField<ConfigDiffInfo<string, string[][]>> = {
    leafPaths: [['geometry', 'type'], ['gltfModel'], ['splat']],
    diffOptions: {
      renderValue: ([geometryType, gltf, splat]) => {
        if (gltf) {
          return t('mesh_configurator.geometry_type.option.model_url')
        }
        if (splat) {
          return t('mesh_configurator.geometry_type.option.splat_url')
        }
        if (geometryType) {
          return t(`mesh_configurator.geometry_type.option.${geometryType}`)
        }
        return t('mesh_configurator.geometry_type.option.none')
      },
    },
  }

  return (
    <RowLayout
      expanseField={menuExpanseField}
    >
      <div className={rowFieldStyles.flexItem}>
        <StandardFieldLabel
          label={t('mesh_configurator.geometry_type.label')}
          disabled={disabled}
          mutedColor
        />
      </div>
      <div className={classes.selectContainer}>
        <SubMenuSelectWithSearch
          onChange={onChange}
          categories={meshCategories}
          disabled={disabled}
          a8='click;studio-right-panel;mesh-geometry-type-click'
          trigger={(
            <button
              className={combine('style-reset', rowFieldStyles.select,
                rowFieldStyles.preventOverflow, disabled && rowFieldStyles.disabledSelect)}
              type='button'
            >
              <div className={rowFieldStyles.selectText}>
                {allOptions.find(option => option.value === value)?.content}
              </div>
              <div className={rowFieldStyles.chevron} />
            </button>
          )}
        />
      </div>
    </RowLayout>
  )
}

export {
  MeshCategoryType,
  MeshConfiguratorMenu,
  MODEL_URL_VALUE,
  SPLAT_URL_VALUE,
}
