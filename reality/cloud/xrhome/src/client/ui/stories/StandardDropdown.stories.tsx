import React from 'react'
import type {StoryFn, Meta} from '@storybook/react'
import {useArgs} from '@storybook/client-api'

import {StandardDropdownField} from '../components/standard-dropdown-field'

const SAMPLE_LABEL = 'This is a sample label.'
// const SAMPLE_ERROR = 'This is an error message.'
const SAMPLE_PLACEHOLDER = 'This is a placeholder.'
const SAMPLE_OPTIONS = [
  {value: 'option1', content: 'Option 1'},
  {value: 'option2', content: 'Option 2'},
  {value: 'option3', content: 'Option 3'},
]
const DEFAULT_VALUE = SAMPLE_OPTIONS[0].value

export default {
  title: 'UI/StandardDropdownField',
  component: StandardDropdownField,
} as Meta<typeof StandardDropdownField>

const Template: StoryFn<typeof StandardDropdownField> = (args) => {
  const [arg, updateArgs] = useArgs()
  return (
    <StandardDropdownField
      {...args}
      options={SAMPLE_OPTIONS}
      value={arg.value}
      onChange={value => updateArgs({value})}
    />
  )
}

// Default story
export const Default: StoryFn<typeof StandardDropdownField> = Template.bind({})
Default.args = {
  id: 'dropdown',
  label: SAMPLE_LABEL,
  placeholder: '',
  value: DEFAULT_VALUE,
  height: 'auto',
}

// Bold story
export const Bold: StoryFn<typeof StandardDropdownField> = Template.bind({})
Bold.args = {
  label: SAMPLE_LABEL,
  boldLabel: true,
}

// BoldStarred story
export const BoldStarred: StoryFn<typeof StandardDropdownField> = Template.bind({})
BoldStarred.args = {
  label: SAMPLE_LABEL,
  boldLabel: true,
  starredLabel: true,
}

// Disabled story
export const Disabled: StoryFn<typeof StandardDropdownField> = Template.bind({})
Disabled.args = {
  label: SAMPLE_LABEL,
  value: DEFAULT_VALUE,
  disabled: true,
}

// Placeholder story
export const Placeholder: StoryFn<typeof StandardDropdownField> = Template.bind({})
Placeholder.args = {
  label: SAMPLE_LABEL,
  placeholder: SAMPLE_PLACEHOLDER,
}
