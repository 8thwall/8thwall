import React from 'react'
import type {StoryFn, Meta} from '@storybook/react'
import {createUseStyles} from 'react-jss'

import {StandardRadioButton, StandardRadioGroup} from '../components/standard-radio-group'

const useStyles = createUseStyles({
  standardRadioGroup: {
    display: 'flex',
    flexDirection: 'row',
    gap: '1em',
  },
})
export default {
  title: 'UI/StandardRadioGroup',
  component: StandardRadioButton,
} as Meta<typeof StandardRadioButton>

const Template: StoryFn<typeof StandardRadioButton> = (args) => {
  const classes = useStyles()

  return (
    <StandardRadioGroup label='Standard Radio Group'>
      <div className={classes.standardRadioGroup}>
        <StandardRadioButton {...args} label='Checked Radio Button' checked />
        <StandardRadioButton {...args} label='Unchecked Radio Button' checked={false} />
      </div>
    </StandardRadioGroup>
  )
}

export const DisabledRadioButton = {
  render: Template,
  args: {
    disabled: true,
  },
}

export const EnabledRadioButton = {
  render: Template,
  args: {
    disabled: false,
  },
}
