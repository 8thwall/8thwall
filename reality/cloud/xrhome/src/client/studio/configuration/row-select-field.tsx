import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'

import {Icon} from '../../ui/components/icon'
import type {DropdownOption} from '../../ui/components/core-dropdown'
import {FloatingMenuButton} from '../../ui/components/floating-menu-button'
import {combine} from '../../common/styles'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {RowFieldLabel} from './row-field-label'
import {SelectMenu} from '../ui/select-menu'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {DeleteButton} from './row-delete-button'
import {useStyles} from './row-styles'
import {SearchBar} from '../ui/search-bar'
import type {DefaultableConfigDiffInfo} from './diff-chip-types'
import type {ExpanseField} from './expanse-field-types'
import {ExpanseFieldDiffChip} from './expanse-field-diff-chip'
import {useIdFallback} from '../use-id-fallback'

interface IRowSelectFieldBase<T extends string> {
  id?: string
  label: React.ReactNode
  icon?: React.ReactNode
  disabled?: boolean
  menuWrapperClassName?: string
  value: T
  options: DeepReadonly<DropdownOption<T>[]>
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  onChange: (newValue: T) => void
  minTriggerWidth?: boolean
  maxWidth?: number
  noMargin?: boolean
  rightContent?: React.ReactNode
  leftContent?: React.ReactNode
  placeholder?: string
  onDelete?: () => void
  formatLabel?: boolean
  renderExpandedOption?: (
    option: DeepReadonly<DropdownOption<T>>, isSelected: boolean
  ) => React.ReactNode
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<T, string[][]>>
}

interface IRowSelectField<T extends string> extends IRowSelectFieldBase<T> {
  withSearch?: boolean
}

function RowSelectFieldBase<T extends string>({
  id: idOverride, label, icon, disabled, menuWrapperClassName, options, value, onChange, maxWidth,
  minTriggerWidth = true, noMargin, rightContent, leftContent, placeholder, onDelete,
  formatLabel = true, renderExpandedOption, expanseField,
}: IRowSelectFieldBase<T>) {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()
  const [selectOpen, setSelectOpen] = React.useState(false)
  const menuStyles = useStudioMenuStyles()
  const displayPlaceholder = placeholder || t('row_fields.select.placeholder')
  const id = useIdFallback(idOverride)

  const selectMenu = (
    <SelectMenu
      id={id}
      grow={!!icon}
      menuWrapperClassName={
        combine(menuStyles.studioMenu, classes.selectMenuContainer, menuWrapperClassName)
      }
      trigger={(
        <button
          type='button'
          onClick={() => (disabled ? undefined : setSelectOpen(!selectOpen))}
          className={combine('style-reset', classes.select, classes.preventOverflow,
            disabled && classes.disabledSelect)}
        >
          <div className={classes.selectText}>
            {options.find(option => option.value === value)?.content ||
              displayPlaceholder}
          </div>
          {!!options.length && <div className={classes.chevron} />}
        </button>
      )}
      minTriggerWidth={minTriggerWidth}
      maxWidth={maxWidth}
      margin={4}
      placement='bottom-end'
      disabled={disabled}
    >
      {collapse => (
        options.map(option => (
          <FloatingMenuButton
            key={option.value}
            onClick={() => {
              onChange(option.value as T)
              collapse()
            }}
          >
            {renderExpandedOption
              ? renderExpandedOption(option, option.value === value)
              : (
                <div className={classes.selectOption}>
                  <div className={classes.selectText}>{option.content}</div>
                  {option.value === value &&
                    <div className={classes.checkmark}>
                      <Icon stroke='checkmark' color='highlight' block />
                    </div>
                }
                </div>
              )}
          </FloatingMenuButton>
        ))
      )}
    </SelectMenu>
  )

  return (
    <label htmlFor={id} className={combine(noMargin && classes.noMargin, classes.row)}>
      {expanseField &&
        <ExpanseFieldDiffChip
          field={expanseField}
          defaultRenderers={
            {defaultRenderValue: (v: string) => options.find(e => e.value === v)?.content}
          }
        />}
      {formatLabel
        ? (
          <div className={classes.flexItem}>
            <RowFieldLabel
              label={label}
              disabled={disabled}
              expanseField={expanseField}
            />
          </div>
        )
        : label}
      <div className={combine(classes.flexItem, classes.flexItemGroup)}>
        {leftContent}
        <div className={classes.flexItem}>
          <StandardFieldContainer disabled={disabled}>
            {icon
              ? (
                <div className={classes.selectIconInputContainer}>
                  {icon}
                  {selectMenu}
                </div>
              )
              : selectMenu}
          </StandardFieldContainer>
        </div>
        {rightContent}
        {onDelete && <DeleteButton onDelete={onDelete} />}
      </div>
    </label>
  )
}

// TODO(tri) wrap RowSelectField and this component in a common component
function RowSelectFieldWithSearch<T extends string>({
  id: idOverride, label, icon, disabled, menuWrapperClassName, options, value, onChange, maxWidth,
  minTriggerWidth = true, noMargin, rightContent, leftContent, placeholder, onDelete,
  formatLabel = true, renderExpandedOption, expanseField,
}: IRowSelectFieldBase<T>) {
  const id = useIdFallback(idOverride)
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()
  const [selectOpen, setSelectOpen] = React.useState(false)
  const menuStyles = useStudioMenuStyles()
  const displayPlaceholder = placeholder || t('row_fields.select.placeholder')
  const [rawSearchText, setSearchText] = React.useState('')
  const searchText = rawSearchText.toLowerCase()
  const [searchFocused, setSearchFocused] = React.useState(false)

  const searchResults = searchText
    ? options.filter((option) => {
      const optionContent = option.content
      return optionContent.toString().toLowerCase().includes(searchText.toLowerCase())
    })
    : null

  const renderOption = (
    option: DeepReadonly<DropdownOption<T>>,
    collapse: () => void,
    isActive: boolean = false
  ) => (
    <FloatingMenuButton
      key={option.value}
      onClick={() => {
        onChange(option.value as T)
        setSearchText('')
        collapse()
      }}
      isActive={isActive}
    >
      {renderExpandedOption
        ? renderExpandedOption(option, option.value === value)
        : (
          <div className={classes.selectOption}>
            <div className={classes.selectText}>{option.content}</div>
            {option.value === value &&
              <div className={classes.checkmark}>
                <Icon stroke='checkmark' color='highlight' block />
              </div>
          }
          </div>
        )}
    </FloatingMenuButton>
  )

  const selectMenu = (
    <SelectMenu
      id={id}
      grow={!!icon}
      menuWrapperClassName={
        combine(menuStyles.studioMenu, classes.selectMenuContainer, menuWrapperClassName)
      }
      trigger={(
        <button
          type='button'
          onClick={() => (disabled ? undefined : setSelectOpen(!selectOpen))}
          className={combine('style-reset', classes.select, classes.preventOverflow,
            disabled && classes.disabledSelect)}
        >
          <div className={classes.selectText}>
            {options.find(option => option.value === value)?.content ||
              displayPlaceholder}
          </div>
          {!!options.length && <div className={classes.chevron} />}
        </button>
      )}
      minTriggerWidth={minTriggerWidth}
      maxWidth={maxWidth}
      margin={4}
      staticWidth
      placement='bottom-end'
      disabled={disabled}
    >
      {collapse => (
        <div>
          <SearchBar
            searchText={searchText}
            setSearchText={setSearchText}
            focusOnOpen
            onFocus={() => setSearchFocused(true)}
            onBlur={() => setSearchFocused(false)}
            onSubmit={() => {
              const firstResult = searchResults?.[0]
              if (firstResult) {
                collapse()
                setSearchText('')
                onChange(firstResult.value as T)
              }
            }}
          />
          {searchResults && (
            searchResults.map((o, i) => renderOption(o, collapse, searchFocused && i === 0))
          )}
          {!searchResults && (
            options.map(o => renderOption(o, collapse))
          )}
        </div>
      )}
    </SelectMenu>
  )

  return (
    <label htmlFor={id} className={combine(noMargin && classes.noMargin, classes.row)}>
      {expanseField && <ExpanseFieldDiffChip
        field={expanseField}
        defaultRenderers={
          {defaultRenderValue: v => options.find(e => e.value === v)?.content}
        }
      />}
      {formatLabel
        ? (
          <div className={classes.flexItem}>
            <RowFieldLabel
              label={label}
              disabled={disabled}
              expanseField={expanseField}
            />
          </div>
        )
        : label}
      <div className={combine(classes.flexItem, classes.flexItemGroup)}>
        {leftContent}
        <div className={classes.flexItem}>
          <StandardFieldContainer disabled={disabled}>
            {icon
              ? (
                <div className={classes.selectIconInputContainer}>
                  {icon}
                  {selectMenu}
                </div>
              )
              : selectMenu}
          </StandardFieldContainer>
        </div>
        {rightContent}
        {onDelete && <DeleteButton onDelete={onDelete} />}
      </div>
    </label>
  )
}

function RowSelectField<T extends string>(props: IRowSelectField<T>) {
  const {withSearch, ...rest} = props
  return withSearch
    ? (
      <RowSelectFieldWithSearch {...rest} />
    )
    : (
      <RowSelectFieldBase {...rest} />
    )
}

export {RowSelectField, RowSelectFieldWithSearch}
