import * as React from 'react'

import {createThemedStyles} from '../../ui/theme'
import {combine} from '../../common/styles'
import {ITertiaryButton, TertiaryButton} from '../../ui/components/tertiary-button'

interface IPublishingPrimaryButton extends ITertiaryButton {
}

const useStyles = createThemedStyles(theme => ({
  publishingPrimaryButton: {
    'display': 'flex',
    'alignItems': 'center',
    'justifyContent': 'center',
    'gap': '0.5rem',
    'background': theme.studioPanelBtnBg,
    'color': theme.fgMain,
    '&:hover:not(:disabled)': {
      background: theme.studioPanelBtnHoverBg,
    },
    '&:disabled': {
      cursor: 'default',
      color: theme.tertiaryBtnDisabledFg,
      background: theme.tertiaryBtnDisabledBg,
    },
  },
}))

const PublishingPrimaryButton: React.FC<IPublishingPrimaryButton> = ({
  children, className, ...props
}) => {
  const classes = useStyles()

  return (
    <TertiaryButton
      {...props}
      // eslint-disable-next-line local-rules/ui-component-styling
      className={combine(className, classes.publishingPrimaryButton)}
    >
      {children}
    </TertiaryButton>
  )
}

export type {IPublishingPrimaryButton}
export {PublishingPrimaryButton}
