import React from 'react'
import type {Meta} from '@storybook/react'
import {createUseStyles} from 'react-jss'

import {Trans, useTranslation} from 'react-i18next'

import {Tag, ITag} from '../components/tag'
import {SpaceBetween} from '../layout/space-between'

const useStyles = createUseStyles({
  grid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fill, minmax(calc(var(--grid-size) * 1px), 1fr))',
    gridGap: '2em',
    justifyItems: 'center',
  },
})

const TAG_VARIANTS: Array<ITag['height']> = ['medium', 'small', 'tiny']
const BUILD_TYPE_KEYS = {
  apk: 'editor_page.export_modal.build_type.apk_description',
  aab: 'editor_page.export_modal.build_type.aab_description',
} as const

type TagShowCaseProps = {size: string}

const TagShowCase: React.FC<TagShowCaseProps> = ({size}) => {
  const {t} = useTranslation(['studio-desktop-pages'])
  const {t: editorT} = useTranslation(['cloud-editor-pages', 'common'])
  const {t: studioT} = useTranslation(['cloud-studio-pages'])
  const {t: studioCommonT} = useTranslation(['cloud-studio-pages', 'common'])
  const {t: assetLabT} = useTranslation('asset-lab')
  const {t: commonT} = useTranslation(['common'])
  // @ts-expect-error A namespace selection is required.
  const _missingNamespace = useTranslation()
  const classes = useStyles()
  const buildType: keyof typeof BUILD_TYPE_KEYS = 'apk'
  return (
    <SpaceBetween direction='vertical'>
      <h2>Normal</h2>

      {/* eslint-disable-next-line no-constant-binary-expression */}
      {false &&
        <>
          {/* The first selected namespace is implicit. */}
          {editorT('editor_page.export_modal.build_type')}
          {editorT('button.delete', {ns: 'common'})}
          {/* @ts-expect-error common is not the implicit namespace. */}
          {editorT('button.delete')}

          {/* Single and string namespace selections. */}
          {studioT('mesh_configurator.title')}
          {assetLabT('asset_lab.title')}
          {commonT('button.delete')}
          {/* @ts-expect-error common was not selected. */}
          {studioT('button.delete', {ns: 'common'})}
          {/* @ts-expect-error common is not the implicit namespace. */}
          {studioCommonT('button.delete')}
          {studioCommonT('button.delete', {ns: 'common'})}

          {/* Explicit namespaces must be selected and contain the key. */}
          {/* @ts-expect-error button.delete belongs to common. */}
          {studioCommonT('button.delete', {ns: 'cloud-studio-pages'})}
          {/* @ts-expect-error asset-lab was not selected. */}
          {studioCommonT('button.delete', {ns: 'asset-lab'})}
          {/* @ts-expect-error missing is not a known namespace. */}
          {studioCommonT('button.delete', {ns: 'missing'})}

          {/* Keys are the bare, flat JSON keys. */}
          {editorT('editor_page.export_modal.build_type.apk_description')}
          {/* @ts-expect-error Namespace-prefixed keys are not supported. */}
          {editorT('cloud-editor-pages:editor_page.export_modal.build_type')}

          {/* Plural suffixes and their inferred base key are supported. */}
          {studioT('tree_hierarchy_search_results.results_length_one')}
          {studioT('tree_hierarchy_search_results.results_length_other')}
          {studioT('tree_hierarchy_search_results.results_length')}

          {/* Invalid and cross-namespace keys are rejected. */}
          {/* @ts-expect-error Typo. */}
          {editorT('editor_page.export_modal.build_typ')}
          {/* @ts-expect-error asset_lab.title belongs to asset-lab. */}
          {studioT('asset_lab.title')}
          {/* @ts-expect-error Typo in an explicitly selected namespace. */}
          {studioCommonT('button.deleet', {ns: 'common'})}

          {/* Finite dynamic keys retain their literal types. */}
          {editorT(BUILD_TYPE_KEYS[buildType])}

          {/* Legacy TFunction and Trans coverage. */}
          {t('project_list_item.menu.option.reveal_finder')}
          <Trans i18nKey='button.learn_more' />
          <Trans ns='common' i18nKey='button.learn_more' />
          <Trans ns='studio-desktop-pages' i18nKey='project_list_item.menu.option.reveal_finder' />

          {/* @ts-expect-error */}
          {t('button.learn_more', {ns: 'common'})}
          {/* @ts-expect-error */}
          {t('project_list_item.menu.option.reveal_finder', {ns: 'common'})}
          {/* @ts-expect-error */}
          {t('studio-desktop-pages:project_list_item.menu.option.reveal_finder')}
          {/* @ts-expect-error */}
          <Trans i18nKey='common:button.learn_more' />
          {/* @ts-expect-error */}
          <Trans i18nKey='project_list_item.menu.option.reveal_finder' />
          <Trans
            ns='studio-desktop-pages'
            // @ts-expect-error
            i18nKey='studio-desktop-pages:project_list_item.menu.option.reveal_finder'
          />
          <Trans
            // @ts-expect-error
            ns='studio-desktop-pages2'
            i18nKey='project_list_item.menu.option.reveal_finder'
          />
        </>}
      <div className={classes.grid} style={{'--grid-size': size} as any}>
        {TAG_VARIANTS.map(variant => (
          <Tag key={variant} height={variant}>
            {variant}
          </Tag>
        ))}
      </div>
    </SpaceBetween>
  )
}

export default {
  title: 'Views/TagShowCase',
  render: TagShowCase,
  argTypes: {
    size: {
      control: 'select',
      options: ['64', '160', '400'],
    },
  },
} as Meta<typeof TagShowCase>

export const All = {
  args: {
    badgeSize: '160',
    badgeHeight: 'tiny',
    badgeSpacing: 'normal',
  },
}
