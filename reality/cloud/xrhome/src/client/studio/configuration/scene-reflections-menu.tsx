import React, {useState} from 'react'
import {useTranslation} from 'react-i18next'

import type {Resource} from '@ecs/shared/scene-graph'

import {getReflectionsFromSceneGraph} from '@ecs/shared/get-environment-from-scene-graph'

import {extractResourceUrl} from '@ecs/shared/resource'

import {ENV_MAP_PRESETS} from '@ecs/shared/environment-maps'
import {createUseStyles} from 'react-jss'

import {useActiveSpace} from '../hooks/active-space'
import {MutateCallback, useSceneContext} from '../scene-context'
import {combine} from '../../common/styles'
import {RowTextField, useStyles as useRowStyles} from './row-fields'
import {basename} from '../../editor/editor-common'
import {useCurrentRepoId} from '../../git/repo-id-context'
import {useScopedGit} from '../../git/hooks/use-current-git'
import {getFilesByKind} from '../common/studio-files'
import {SubMenuSelectWithSearch} from '../ui/submenu-select-with-search'
import type {SubMenuCategory} from '../ui/submenu'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {useStyles as useRowFieldStyles} from './row-fields'

const useStyles = createUseStyles(
  {
    selectContainer: {
      flex: 1,
      fontSize: '12px',
    },
  }
)

const SceneReflectionsMenu: React.FC = () => {
  const ctx = useSceneContext()
  const classes = useStyles()
  const rowFieldStyles = useRowFieldStyles()
  const rowClasses = useRowStyles()
  const activeSpace = useActiveSpace()
  const [currentUrl, setCurrentUrl] = useState<string>('')

  const {t} = useTranslation(['cloud-studio-pages'])
  const reflections = getReflectionsFromSceneGraph(ctx.scene, activeSpace?.id)

  const updateSceneReflections = (cb: MutateCallback<Resource>) => {
    const updatedReflections = cb(reflections)
    ctx.updateScene((scene) => {
      if (activeSpace?.id) {
        return {
          ...scene,
          spaces: {
            ...scene.spaces,
            [activeSpace.id]: {
              ...scene.spaces[activeSpace.id],
              reflections: updatedReflections,
            },
          },
        }
      }
      return {
        ...scene,
        reflections: updatedReflections,
      }
    })
  }

  const repoId = useCurrentRepoId()
  const filesByPath = useScopedGit(repoId, git => git.filesByPath)
  const activeUrl = extractResourceUrl(reflections)
  const isPreset = reflections?.type === 'url' && !!ENV_MAP_PRESETS[reflections.url]
  const isUrl = reflections?.type === 'url' && !isPreset

  const presetCategory: SubMenuCategory = {
    parent: null,
    value: t('reflection.menu.label.presets'),
    options: Object.entries(ENV_MAP_PRESETS).map(([envUrl, displayKey]) => ({
      value: envUrl,
      content: t(displayKey),
      icon: activeUrl === envUrl ? 'checkmark' : undefined,
    })),
  }

  const assetCategory: SubMenuCategory = {
    parent: null,
    value: t('reflection.menu.label.assets'),
    options: getFilesByKind(filesByPath, 'special_image')
      .concat(getFilesByKind(filesByPath, 'image'))
      .map(asset => ({
        value: asset,
        content: basename(asset),
        icon: activeUrl === asset ? 'checkmark' : undefined,
      })),
  }

  const rootOptions: SubMenuCategory = {
    parent: null,
    value: null,
    options:
      [{
        value: '',
        content: t('reflection.menu.option.none'),
        icon: !reflections ? 'checkmark' : undefined,
      },
      {
        value: 'url',
        content: t('reflection.menu.label.url'),
        icon: isUrl ? 'checkmark' : undefined,
      },
      ],
  }

  const envMapCategories = [assetCategory, presetCategory, rootOptions]

  return (
    <>
      <div className={rowFieldStyles.row}>
        <div className={rowFieldStyles.flexItem}>
          <StandardFieldLabel
            label={t('space_configurator.label.reflections')}
            mutedColor
          />
        </div>
        <div className={classes.selectContainer}>
          <SubMenuSelectWithSearch
            trigger={(
              <button
                type='button'
                className={combine('style-reset', rowClasses.select, rowClasses.preventOverflow)}
              >
                <div className={rowClasses.selectText}>
                  {!reflections && t('reflection.menu.option.none')}
                  {!isPreset && reflections && (reflections.type === 'url'
                    ? t('reflection.menu.label.url')
                    : basename(reflections.asset))}
                  {isPreset && t(ENV_MAP_PRESETS[reflections.url])}
                </div>
                <div className={rowClasses.chevron} />
              </button>
            )}
            onChange={(newEnvMap) => {
              if (Object.keys(ENV_MAP_PRESETS).includes(newEnvMap)) {
                updateSceneReflections(() => ({type: 'url', url: newEnvMap}))
              } else if (newEnvMap === 'url') {
                updateSceneReflections(() => ({type: 'url', url: currentUrl}))
              } else if (newEnvMap) {
                updateSceneReflections(() => ({type: 'asset', asset: newEnvMap}))
              } else {
                updateSceneReflections(() => (null))
              }
            }}
            categories={envMapCategories}
          />
        </div>
      </div>

      {isUrl &&
        <RowTextField
          id='reflection-url'
          label={t('reflection.menu.label.url')}
          value={currentUrl}
          onChange={(event) => {
            setCurrentUrl(event.target.value)
            updateSceneReflections(() => ({type: 'url', url: event.target.value}))
          }}
        />}
    </>
  )
}

export {
  SceneReflectionsMenu,
}
