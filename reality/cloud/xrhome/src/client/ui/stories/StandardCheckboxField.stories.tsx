import React from 'react'
import type {StoryFn, Meta} from '@storybook/react'
import {useArgs} from '@storybook/client-api'

import {StandardCheckboxField} from '../components/standard-checkbox-field'

const SAMPLE_LABEL = 'This is a sample label.'

export default {
  title: 'UI/StandardCheckboxField',
  component: StandardCheckboxField,
} as Meta<typeof StandardCheckboxField>

const Template: StoryFn<typeof StandardCheckboxField> = (args) => {
  const [{checked}, updateArgs] = useArgs()
  return <StandardCheckboxField {...args} onChange={() => updateArgs({checked: !checked})} />
}

export const Wrap = {
  render: Template,

  args: {
    checked: true,
    label: SAMPLE_LABEL,
  },
}

export const NoWrap = {
  render: Template,

  args: {
    checked: true,
    label: SAMPLE_LABEL,
    nowrap: true,
  },
}
