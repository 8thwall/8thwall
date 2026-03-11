import React from 'react'
import {useTranslation} from 'react-i18next'

import type {IImageTarget} from '../common/types/models'
import {createThemedStyles} from '../ui/theme'
import {Icon} from '../ui/components/icon'
import {combine} from '../common/styles'
import {SrOnly} from '../ui/components/sr-only'
import {typeToIcon, typeToLabel} from './image-target-list-item'
import {FloatingMenuButton} from '../ui/components/floating-menu-button'
import {useStyles as useRowFieldStyles} from './configuration/row-fields'

const useStyles = createThemedStyles(theme => ({
  imgContainer: {
    'width': '24px',
    'height': '24px',
    '& > img': {
      borderRadius: '3px',
      width: '100%',
      height: '100%',
      objectFit: 'cover',
    },
  },
  nameContainer: {
    textOverflow: 'ellipsis',
    overflow: 'hidden',
    whiteSpace: 'nowrap',
    width: '14.25rem',
    padding: '1px',
    margin: '0px 2px 0px 4px',
  },
  iconOptionContainer: {
    width: '16px',
    display: 'flex',
    marginRight: '0.5rem',
  },
  floatingMenuButton: {
    'display': 'flex',
    'gap': '0.25em',
    'alignItems': 'center',
    'padding': '0.15rem 0.5rem',
    'color': theme.fgMain,
    'cursor': 'pointer',
    'borderRadius': '0.25em',
    'userSelect': 'none',
    ':is(&:hover, &:focus-visible):not(&:disabled)': {
      color: theme.studioBtnHoverFg,
      background: theme.studioBtnHoverBg,
    },
    '& svg': {
      color: theme.fgMuted,
    },
    '&:hover svg': {
      color: theme.fgMain,
    },
  },
  noneOption: {
    padding: '0.15rem 0.75rem 0.15rem 0.5rem',
  },
  loading: {
    opacity: 0.5,
  },
  addTarget: {
    'alignItems': 'center',
    'padding': '0.15rem 0rem',
  },
  categoryChevron: {
    '& svg': {
      width: '0.75em',
      height: '0.75em',
    },
  },
}))

interface IImageTargetNoneMenuItem {
  onClick: () => void
  showCheckmark: boolean
}

const ImageTargetNoneMenuItem: React.FC<IImageTargetNoneMenuItem> = ({
  onClick, showCheckmark,
}) => {
  const classes = useStyles()
  const rowFieldStyles = useRowFieldStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  return (
    <FloatingMenuButton
      onClick={onClick}
    >
      <div className={combine(rowFieldStyles.selectOption, classes.noneOption)}>
        <div className={rowFieldStyles.selectText}>
          {t('image_target_configurator_menu.none.label')}
        </div>
        {showCheckmark &&
          <div className={rowFieldStyles.checkmark}>
            <Icon stroke='checkmark' color='highlight' block />
          </div>
        }
      </div>
    </FloatingMenuButton>
  )
}

interface IImageTargetMenuItem {
  imageTarget: IImageTarget
  showCheckmark: boolean
  onClick?: (it: IImageTarget) => void
  disabled?: boolean
}

const ImageTargetMenuItem: React.FC<IImageTargetMenuItem> = ({
  imageTarget, showCheckmark, onClick, disabled = false,
}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()

  return (
    <button
      className={combine('style-reset', classes.floatingMenuButton, disabled && classes.loading)}
      type='button'
      onClick={() => onClick && onClick(imageTarget)}
      disabled={disabled}
    >
      <div className={classes.imgContainer}>
        <img
          src={imageTarget.thumbnailImageSrc}
          alt={imageTarget.name}
        />
      </div>
      <div className={classes.nameContainer}>
        {imageTarget.name}
      </div>
      <div className={classes.iconOptionContainer}>
        {showCheckmark
          ? (
            <Icon stroke='checkmark' color='highlight' />
          )
          : (
            <>
              <SrOnly>{t(typeToLabel(imageTarget.type))}</SrOnly>
              <Icon stroke={typeToIcon(imageTarget.type)} />
            </>
          )}
      </div>
    </button>
  )
}

interface IImageTargetAddMenuItem {
  onClick?: () => void
}

const ImageTargetAddMenuItem: React.FC<IImageTargetAddMenuItem> = ({onClick}) => {
  const classes = useStyles()
  const rowFieldStyles = useRowFieldStyles()
  const {t} = useTranslation('cloud-studio-pages')

  return (
    <FloatingMenuButton
      onClick={onClick}
    >
      <div className={combine(rowFieldStyles.selectOption, classes.addTarget)}>
        <div className={rowFieldStyles.selectText}>
          {t('image_target_configurator_menu.add_new_target.label')}
        </div>
        <div className={classes.categoryChevron}>
          <Icon stroke='chevronRight' />
        </div>
      </div>
    </FloatingMenuButton>
  )
}

export {
  ImageTargetMenuItem,
  ImageTargetNoneMenuItem,
  ImageTargetAddMenuItem,
}
