import React from 'react'
import {useTranslation} from 'react-i18next'

import type {DropdownOption} from '../../ui/components/standard-dropdown-field'
import {
  ImageModelOptions, MeshModelOptions, AnimModelOptions, ModelDropdownOption,
} from '../constants'
import {createThemedStyles} from '../../ui/theme'
import {StandardChip} from '../../ui/components/standard-chip'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {CoreDropdown} from '../../ui/components/core-dropdown'
import {combine} from '../../common/styles'
import {useStyles as standardDropdownFieldStyles} from '../../ui/components/standard-dropdown-field'

const useStyles = createThemedStyles(theme => ({
  row: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
  },
  modelName: {
    fontWeight: 'bold',
  },
  anOption: {
    'display': 'flex',
    'flexDirection': 'column',
    'paddingRight': '1rem',
  },
  // These items are specialized for model dropdown, see their version in standard-dropdown-field
  focusedOption: {
    background: theme.studioBtnActiveBg,
  },
  menuOpen: {
    display: 'block',
    padding: '0.5rem 0 0.5rem 0.5rem',
  },
  menuOption: {
    padding: '0.5rem',
    overflow: 'auto',
  },
}))

interface IModelOption {
  nameKey: string
  descriptionKey: string
  costRange: string
  selected?: boolean
}

const ModelOption: React.FC<IModelOption> = ({nameKey, descriptionKey, costRange, selected}) => {
  const {t} = useTranslation('asset-lab')
  const classes = useStyles()
  return (
    <div className={classes.anOption}>
      <div className={classes.row}>
        <span className={classes.modelName}>{t(nameKey)}</span>
        {!selected && <StandardChip iconStroke='credits' inline text={costRange} />}
      </div>
      {t(descriptionKey)}
    </div>
  )
}

const getDropdownOptions = (
  modelOptions: ModelDropdownOption[]
): DropdownOption[] => modelOptions.map((option) => {
  const nameKey = `${option.translationKey}.name`
  const descriptionKey = `${option.translationKey}.description`
  return {
    value: option.modelId,
    content: (
      <ModelOption nameKey={nameKey} descriptionKey={descriptionKey} costRange={option.costRange} />
    ),
    extra: {
      nameKey,
      descriptionKey,
      costRange: option.costRange,
    },
  }
})

const formatVisibleContent = (option: DropdownOption): React.ReactNode => {
  const {nameKey, descriptionKey, costRange} = option.extra
  return (
    <ModelOption nameKey={nameKey} descriptionKey={descriptionKey} costRange={costRange} selected />
  )
}

interface IAssetLabModelDropdown {
  model: string
  setModel: (model: string) => void
  label: string
  id?: string
  disabled?: boolean
  boldLabel?: boolean
  starredLabel?: boolean
}
interface IAssetModelDropdownWithModel extends IAssetLabModelDropdown {
  modelOptions: ModelDropdownOption[]
}

const AssetLabModelDropdown: React.FC<IAssetModelDropdownWithModel> = ({
  model, setModel, modelOptions, label,
  id = 'asset-lab-model', disabled = false, boldLabel = false, starredLabel = false,
}) => {
  const options = getDropdownOptions(modelOptions)
  const classes = useStyles()
  const sdfClasses = standardDropdownFieldStyles()

  const labelId = `${id}-labeltext`
  return (
    <div>
      <label htmlFor={labelId}>
        <StandardFieldLabel
          id={labelId}
          label={label}
          disabled={disabled}
          bold={boldLabel}
          starred={starredLabel}
        />
        <StandardFieldContainer disabled={disabled}>
          <CoreDropdown
            id={id}
            labelId={labelId}
            value={model}
            options={options}
            disabled={disabled}
            onChange={value => setModel(value)}
            renderChevron={menuOpen => (
              <span
                className={combine(sdfClasses.chevron, menuOpen && sdfClasses.chevronOpen)}
                aria-hidden
              />
            )}
            targetClassName={combine(sdfClasses.target, sdfClasses.auto)}
            menuClassName={sdfClasses.menu}
            menuTopClassName={sdfClasses.menuTop}
            menuBottomClassName={sdfClasses.menuBottom}
            menuOpenClassName={classes.menuOpen}
            optionClassName={combine(sdfClasses.menuOption, classes.menuOption)}
            optionGroupClassName={sdfClasses.menuOptionGroup}
            optionGroupLabelClassName={sdfClasses.menuOptionGroupLabel}
            focusedOptionClassName={classes.focusedOption}
            formatVisibleContent={formatVisibleContent}
          />
        </StandardFieldContainer>
      </label>
    </div>
  )
}

const AssetLabImageModelDropdown: React.FC<IAssetLabModelDropdown> = props => (
  <AssetLabModelDropdown
    {...props}
    id='image-gen-model'
    modelOptions={ImageModelOptions}
    starredLabel
  />
)

const AssetLab3dModelDropdown: React.FC<IAssetLabModelDropdown> = props => (
  <AssetLabModelDropdown
    {...props}
    id='3d-gen-model'
    modelOptions={MeshModelOptions}
    starredLabel
  />
)

const AssetLabAnimModelDropdown: React.FC<IAssetLabModelDropdown> = props => (
  <AssetLabModelDropdown
    {...props}
    id='anim-gen-model'
    modelOptions={AnimModelOptions}
    starredLabel
  />
)

export {AssetLabImageModelDropdown, AssetLab3dModelDropdown, AssetLabAnimModelDropdown}
