import React from 'react'
import type {TFunction} from 'react-i18next'
import {Trans, useTranslation} from 'react-i18next'

// NOTE(christoph): This file contains demo code that validates that our react-i18next types
// enforce the desired rules.

const BUILD_TYPE_KEYS = {
  apk: 'editor_page.export_modal.build_type.apk_description',
  aab: 'editor_page.export_modal.build_type.aab_description',
} as const

const ReactI18nTypeTest: React.FC = () => {
  const {t: desktopT} = useTranslation(['studio-desktop-pages'])
  const {t: editorT} = useTranslation(['cloud-editor-pages', 'common'])
  const {t: studioT} = useTranslation(['cloud-studio-pages'])
  const {t: studioCommonT} = useTranslation(['cloud-studio-pages', 'common'])
  const {t: assetLabT} = useTranslation('asset-lab')
  const {t: commonT} = useTranslation(['common'])
  const namespaceAsString = 'cloud-studio-pages' as string
  // @ts-expect-error A namespace selection is required.
  useTranslation()
  // @ts-expect-error A broad string is not a namespace selection.
  useTranslation(namespaceAsString)
  const buildType: keyof typeof BUILD_TYPE_KEYS = 'apk'

  // @ts-expect-error cannot assign incompatible TFunction
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  const _myTFunction: TFunction<'account-pages'> = commonT

  // Can assign out optional namespaces
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  const _justCommonT: TFunction<'cloud-studio-pages'> = studioCommonT

  return (
    <>
      {/* The first selected namespace is implicit. */}
      {editorT('editor_page.export_modal.build_type')}
      {editorT('button.delete', {ns: 'common'})}
      {/* @ts-expect-error common is not the implicit namespace. */}
      {editorT('button.delete')}

      {/* Single and string namespace selections. */}
      {studioT('mesh_configurator.title')}
      {/* @ts-expect-error string is not assignable. */}
      {studioT('mesh_configurator.title' as string)}
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

      {/* Default namespace and Trans coverage. */}
      {desktopT('project_list_item.menu.option.reveal_finder')}
      <Trans i18nKey='button.learn_more' />
      <Trans ns='common' i18nKey='button.learn_more' />
      <Trans
        ns='studio-desktop-pages'
        i18nKey='project_list_item.menu.option.reveal_finder'
      />

      {/* @ts-expect-error common was not selected. */}
      {desktopT('button.learn_more', {ns: 'common'})}
      {/* @ts-expect-error common is not the implicit namespace. */}
      {desktopT('project_list_item.menu.option.reveal_finder', {ns: 'common'})}
      {/* @ts-expect-error Namespace-prefixed keys are not supported. */}
      {desktopT('studio-desktop-pages:project_list_item.menu.option.reveal_finder')}
      {/* @ts-expect-error Namespace-prefixed keys are not supported. */}
      <Trans i18nKey='common:button.learn_more' />
      {/* @ts-expect-error The key does not belong to the default namespace. */}
      <Trans i18nKey='project_list_item.menu.option.reveal_finder' />
      <Trans
        ns='studio-desktop-pages'
        // @ts-expect-error Namespace-prefixed keys are not supported.
        i18nKey='studio-desktop-pages:project_list_item.menu.option.reveal_finder'
      />
      <Trans
        // @ts-expect-error The namespace does not exist.
        ns='studio-desktop-pages2'
        i18nKey='project_list_item.menu.option.reveal_finder'
      />
    </>
  )
}

export {ReactI18nTypeTest}
