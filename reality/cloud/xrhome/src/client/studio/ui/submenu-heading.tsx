import React from 'react'
import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../../ui/theme'
import {IconButton} from '../../ui/components/icon-button'
import {combine} from '../../common/styles'

const useStyles = createThemedStyles(theme => ({
  pageTitleContainer: {
    display: 'flex',
    alignContent: 'center',
    justifyContent: 'left',
    borderBottom: theme.studioSectionBorder,
    padding: '1em 0.5em',
    position: 'relative',
  },
  small: {
    padding: '0.5em',
  },
  title: {
    color: theme.fgMuted,
    width: '100%',
    fontSize: '12px',
    textAlign: 'center',
  },
  backButton: {
    'position': 'absolute',
    'top': '0',
    'bottom': '0',
    'display': 'flex',
    'alignItems': 'center',
    'justifyContent': 'center',
    'color': theme.fgMain,
    '& svg': {
      width: '0.75em',
      height: '0.75em',
    },
  },
}))

interface ISubMenuHeading {
  title: string
  onBackClick: () => void
  compact?: boolean
}

const SubMenuHeading: React.FC<ISubMenuHeading> = ({
  title, onBackClick, compact = false,
}) => {
  const classes = useStyles()
  const {t} = useTranslation('common')

  return (
    <div className={combine(classes.pageTitleContainer, compact && classes.small)}>
      <div className={classes.backButton}>
        <IconButton
          stroke='chevronLeft'
          onClick={onBackClick}
          text={t('button.back')}
        />
      </div>
      <span className={classes.title}>{title}</span>
    </div>
  )
}

export {
  SubMenuHeading,
}
