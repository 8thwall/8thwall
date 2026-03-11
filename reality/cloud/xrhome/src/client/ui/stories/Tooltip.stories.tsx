import React from 'react'
import type {StoryFn, Meta} from '@storybook/react'

import {createThemedStyles} from '../theme'

import {Tooltip} from '../components/tooltip'
import {Icon} from '../components/icon'
import {PrimaryButton} from '../components/primary-button'

const useStyles = createThemedStyles(theme => ({
  icon: {
    border: 'none',
    verticalAlign: 'middle',
    background: 'none',
    padding: '0',
    color: theme.fgMain,
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
  title: 'UI/Tooltip',
  component: Tooltip,
} as Meta<typeof Tooltip>

const TooltipIconTemplate: StoryFn<typeof Tooltip> = (args) => {
  const classes = useStyles()

  return (
    <div className={classes.container}>
      <Tooltip {...args}>
        <button type='button' className={classes.icon}>
          <Icon stroke='info' />
        </button>
      </Tooltip>
    </div>
  )
}

const ButtonTemplate: StoryFn<typeof Tooltip> = (args) => {
  const classes = useStyles()

  return (
    <div className={classes.container}>
      <Tooltip {...args}>
        <PrimaryButton> Button </PrimaryButton>
      </Tooltip>
    </div>
  )
}

export const TooltipIcon = {
  render: TooltipIconTemplate,

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
