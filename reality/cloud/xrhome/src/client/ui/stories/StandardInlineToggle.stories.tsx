import React from 'react'
import type {StoryFn, Meta} from '@storybook/react'
import {useArgs} from '@storybook/client-api'

import {StandardInlineToggleField} from '../components/standard-inline-toggle-field'

export default {
  title: 'UI/StandardInlineToggle',
  component: StandardInlineToggleField,
} as Meta<typeof StandardInlineToggleField>

const Template: StoryFn<typeof StandardInlineToggleField> = (args) => {
  const [{checked}, updateArgs] = useArgs()
  return <StandardInlineToggleField {...args} onChange={() => updateArgs({checked: !checked})} />
}

export const Toggle = {
  render: Template,

  args: {
    id: 'toggle',
    label: 'Toggle',
    checked: true,
    reverse: false,
  },
}

export const ReverseToggle = {
  render: Template,

  args: {
    id: 'reverse-toggle',
    label: 'Reverse toggle',
    checked: true,
    reverse: true,
  },
}

export const ListToggle = {
  render: Template,

  args: {
    id: 'list-toggle',
    label: (
      <div style={{display: 'flex', flexDirection: 'column'}}>
        <span>Standard</span>
        <span>Inline</span>
        <span>Toggle</span>
      </div>
    ),
    checked: true,
    reverse: false,
  },
}
