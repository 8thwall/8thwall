import React from 'react'

import {useTranslation} from 'react-i18next'

import {combine} from '../../common/styles'
import {Icon} from '../../ui/components/icon'
import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  learnMoreText: {
    'display': 'inline-flex',
    'alignItems': 'start',
    'gap': '0.5rem',
    'color': theme.fgMuted,
    'fontSize': '12px',
    'lineHeight': '20px',
  },
  iconContainer: {
    height: '100%',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
  },
}))

interface ILearnMoreText {
  text?: string
  iconSize?: number
  className?: string
  a8?: string
}

const LearnMoreText: React.FC<ILearnMoreText> = ({
  text,
  iconSize = 0.75,
  className,
  a8,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['common'])
  return (
    <div
      className={combine(classes.learnMoreText, className, 'learn-more-text')}
      a8={a8}
    >
      <div className={classes.iconContainer}>
        <Icon
          stroke='external'
          size={iconSize}
        />
      </div>
      <span>{text || t('button.learn_more')}</span>
    </div>
  )
}

export {
  LearnMoreText,
}

export type {
  ILearnMoreText,
}
