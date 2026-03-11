import React from 'react'
import type {StoryFn, Meta} from '@storybook/react'

import {createThemedStyles} from '../theme'

import {Popup} from '../components/popup'
import {Icon} from '../components/icon'
import {PrimaryButton} from '../components/primary-button'
import {StandardLink} from '../components/standard-link'
import {TextNotification} from '../components/text-notification'

const useStyles = createThemedStyles(theme => ({
  icon: {
    border: 'none',
    verticalAlign: 'middle',
    background: 'none',
    padding: '0',
    color: theme.fgMuted,
  },
  container: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    height: '10rem',
  },
}))

const MESSAGE = 'This is a message'

export default {
  title: 'UI/Popup',
  component: Popup,
} as Meta<typeof Popup>

const TooltipTemplate: StoryFn<typeof Popup> = (args) => {
  const classes = useStyles()

  return (
    <div className={classes.container}>
      <Popup {...args}>
        <button type='button' className={classes.icon} title='Info'>
          <Icon stroke='info' />
        </button>
      </Popup>
    </div>
  )
}

const ButtonTemplate: StoryFn<typeof Popup> = (args) => {
  const classes = useStyles()

  return (
    <div className={classes.container}>
      <Popup {...args}>
        <PrimaryButton> Button </PrimaryButton>
      </Popup>
    </div>
  )
}

export const Tooltip = {
  render: TooltipTemplate,

  args: {
    content: MESSAGE,
  },
}

export const Button = {
  render: ButtonTemplate,

  args: {
    content: MESSAGE,
  },
}

export const LinkContent = {
  render: TooltipTemplate,

  args: {
    content: <StandardLink>https:/example.com</StandardLink>,
  },
}

export const TextNotificationContent = {
  render: TooltipTemplate,

  args: {
    content: <TextNotification type='danger'>https:/example.com</TextNotification>,
  },
}
