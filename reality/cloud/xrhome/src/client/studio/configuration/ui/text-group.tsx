import React from 'react'
import type {DeepReadonly} from 'ts-essentials'

import type {FontResource, UiGraphSettings} from '@ecs/shared/scene-graph'
import {extractFontResourceUrl} from '@ecs/shared/resource'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'
import {useTranslation} from 'react-i18next'
import {FONTS} from '@ecs/shared/fonts'
import type {TextAlignContent, VerticalTextAlignContent} from '@ecs/shared/flex-style-types'

import {useStyles as useRowStyles} from '../row-fields'
import {useUiConfiguratorStyles} from './ui-configurator-styles'
import {StandardTextAreaField} from '../../../ui/components/standard-text-area-field'
import {
  DeleteGroupButton, RowColorField, RowGroupFields, RowNumberField, RowSelectField,
} from '../row-fields'
import {SrOnly} from '../../../ui/components/sr-only'
import {useCurrentRepoId} from '../../../git/repo-id-context'
import {useScopedGit} from '../../../git/hooks/use-current-git'
import {getFilesByKind} from '../../common/studio-files'
import {basename} from '../../../editor/editor-common'
import {ToggleButtonGroup} from '../../../ui/components/toggle-button-group'
import {useUiFont} from '../../hooks/use-ui-font'
import type {DropdownOption} from '../../../ui/components/core-dropdown'

const toFontResource = (value: string): FontResource => (
  FONTS.has(value) ? {type: 'font', font: value} : {type: 'asset', asset: value}
)
interface IFontOption {
  option: DeepReadonly<DropdownOption<string>>
}
const FontOption: React.FC<IFontOption> = ({option}) => {
  const uiFont = useUiFont(toFontResource(option.value))
  if (!uiFont) {
    return <span>{option.value}</span>
  }
  // Font name cannot have special chars used in our asset path (for font8 files). We use option
  // value so there is no collision if the font has the same name but a different path in assets/.
  // e.g. assets_PixelifySans-VariableFont_wght_font8 is ok but the original version is not.
  const fontName = option.value.replace(/[/.]/g, '_')

  // Even though style is removed and added when unmounted, network tab showed that the ttf file is
  // redownloaded.
  return (
    <div>
      <style>{`
        @font-face {
          font-family: '${fontName}';
          src: local('${fontName}'), url('${uiFont.ttf}');
      `}
      </style>

      <span style={{fontFamily: fontName}}>{option.content}</span>
    </div>
  )
}

interface ITextGroup {
  settings: UiGraphSettings
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
  onDeleteGroup: () => void
}

const TextGroup: React.FC<ITextGroup> = ({settings, onChange, onDeleteGroup}) => {
  const classes = useUiConfiguratorStyles()
  const rowClasses = useRowStyles()
  const {t} = useTranslation(['cloud-studio-pages'])
  const repoId = useCurrentRepoId()
  const filesByPath = useScopedGit(repoId, git => git.filesByPath)
  const fontFiles = getFilesByKind(filesByPath, 'font8')
  const fontOptions = [...FONTS.keys()]
    .concat(fontFiles).map(font => ({value: font, content: basename(font)}))

  return (
    <>
      <div className={classes.groupHeaderRow}>
        {t('ui_configurator.ui_text.label')}
        <DeleteGroupButton
          onClick={onDeleteGroup}
        />
      </div>
      <div className={rowClasses.row}>
        <StandardTextAreaField
          id='ui-text'
          label={<SrOnly>{t('ui_configurator.ui_text.label')}</SrOnly>}
          value={settings.text ?? UI_DEFAULTS.text}
          onChange={(e) => {
            e.stopPropagation()
            onChange(current => ({...current, text: e.target.value}))
          }}
          rows={3}
          placeholder={t('ui_configurator.ui_text.label')}
        />
      </div>
      <RowSelectField
        id='ui-font'
        label={t('ui_configurator.ui_font.label')}
        value={extractFontResourceUrl(settings.font ?? UI_DEFAULTS.font)}
        options={fontOptions}
        onChange={value => onChange(current => ({
          ...current,
          font: toFontResource(value),
        }))}
        renderExpandedOption={option => <FontOption option={option} />}
      />
      <RowNumberField
        id='ui-font-size'
        label={t('ui_configurator.ui_font_size.label')}
        value={settings.fontSize ?? UI_DEFAULTS.fontSize}
        onChange={value => onChange(current => ({...current, fontSize: value}))}
        min={0}
        step={1}
      />
      <RowColorField
        id='ui-color'
        label={t('ui_configurator.ui_color.label')}
        value={settings.color ?? UI_DEFAULTS.color}
        onChange={(newValue) => {
          onChange(current => ({...current, color: newValue}))
        }}
      />
      <RowGroupFields
        label={t('ui_configurator.ui_text_align.label')}
        contentFlexDirection='column'
      >
        <ToggleButtonGroup
          onChange={value => onChange(current => ({
            ...current,
            textAlign: value as TextAlignContent,
          }))}
          options={[
            {
              text: t('ui_configurator.ui_text_align.option.left'),
              value: 'left',
              icon: 'leftAlign',
            },
            {
              text: t('ui_configurator.ui_text_align.option.center'),
              value: 'center',
              icon: 'centerAlign',
            },
            {
              text: t('ui_configurator.ui_text_align.option.right'),
              value: 'right',
              icon: 'rightAlign',
            },
          ]}
          value={settings.textAlign ?? UI_DEFAULTS.textAlign}
        />
        <ToggleButtonGroup
          onChange={value => onChange(current => ({
            ...current,
            verticalTextAlign: value as VerticalTextAlignContent,
          }))}
          options={[
            {
              text: t('ui_configurator.ui_text_align.option.center'),
              value: 'start',
              icon: 'topVerticalAlignment',
            },
            {
              text: t('ui_configurator.ui_text_align.option.top'),
              value: 'center',
              icon: 'centerVerticalAlignment',
            },
            {
              text: t('ui_configurator.ui_text_align.option.bottom'),
              value: 'end',
              icon: 'bottomVerticalAlignment',
            },
          ]}
          value={settings.verticalTextAlign ?? UI_DEFAULTS.verticalTextAlign}
        />
      </RowGroupFields>
    </>
  )
}

export {
  TextGroup,
}
