import React from 'react'
import {useTranslation} from 'react-i18next'

import {combine} from '../../common/styles'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {SelectMenu} from '../ui/select-menu'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {useStyles} from './row-styles'
import {RowBooleanField} from './row-boolean-field'

interface MultiSelectOption {
  id: string
  label: React.ReactNode
  disabled?: boolean
}

interface IMultiSelect {
  options: MultiSelectOption[]
  selectedIds: readonly string[]
  onChange: (selectedId: string) => void
  placeholders?: {all: string; none: string; custom: string}
  formIdLabel?: string
  displayText?: string
}

const MultiSelect: React.FC<IMultiSelect> = ({
  options, selectedIds, onChange, placeholders, formIdLabel, displayText,
}) => {
  const classes = useStyles()
  const menuStyles = useStudioMenuStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  const defaultPlaceholders = {
    all: t('multi_select_menu.default_placeholders.all'),
    none: t('multi_select_menu.default_placeholders.none'),
    custom: t('multi_select_menu.default_placeholders.custom'),
  }

  const getSelectionDisplayText = (selectedPlaceholders) => {
    if (displayText) {
      return displayText
    }

    const selectableOptions = options.filter(opt => !opt.disabled)
    const validSelectedIds = selectedIds.filter(id => selectableOptions.some(opt => opt.id === id))

    if (validSelectedIds.length === 0) {
      return selectedPlaceholders.none
    }

    if (validSelectedIds.length === selectableOptions.length) {
      return selectedPlaceholders.all
    }

    return selectedPlaceholders.custom
  }

  return (
    <div className={classes.flexItem}>
      <StandardFieldContainer>
        <SelectMenu
          id={`${formIdLabel}-select-menu`}
          menuWrapperClassName={combine(menuStyles.studioMenu, classes.selectMenuContainer)}
          trigger={(
            <button
              type='button'
              className={combine('style-reset', classes.select, classes.preventOverflow)}
            >
              <div className={classes.selectText}>
                {getSelectionDisplayText(placeholders ?? defaultPlaceholders)}
              </div>
              <div className={classes.chevron} />
            </button>
          )}
          matchTriggerWidth
          margin={4}
          placement='bottom-end'
        >
          {() => (
            <>
              {options.map(option => (
                <div key={option.id} className={classes.subMenuOption}>
                  <RowBooleanField
                    checkBoxAlign='right'
                    disabled={option.disabled}
                    id={`include-${option.id}`}
                    label={<span className={classes.optionText}>{option.label}</span>}
                    checked={selectedIds.includes(option.id) && !option.disabled}
                    onChange={() => onChange(option.id)}
                    noMargin
                  />
                </div>
              ))}
            </>
          )}
        </SelectMenu>
      </StandardFieldContainer>
    </div>
  )
}

interface IRowMultiSelect extends IMultiSelect {
  label: React.ReactNode
}

const RowMultiSelect: React.FC<IRowMultiSelect> = ({
  label, options, selectedIds, onChange, placeholders, formIdLabel, displayText,
}) => {
  const classes = useStyles()

  return (
    /* eslint-disable-next-line jsx-a11y/label-has-associated-control */
    <label htmlFor={formIdLabel} className={classes.row}>
      <div className={classes.flexItem}>
        <StandardFieldLabel
          label={label}
          mutedColor
        />
      </div>
      <div className={classes.flexItem}>
        <MultiSelect
          options={options}
          selectedIds={selectedIds}
          onChange={onChange}
          placeholders={placeholders}
          formIdLabel={formIdLabel}
          displayText={displayText}
        />
      </div>
    </label>
  )
}

export {RowMultiSelect}
export type {IMultiSelect, MultiSelectOption}
