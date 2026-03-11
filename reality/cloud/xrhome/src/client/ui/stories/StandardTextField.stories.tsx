import React from 'react'
import type {StoryFn, Meta} from '@storybook/react'
import {useArgs} from '@storybook/client-api'

import {StandardTextField} from '../components/standard-text-field'

const SAMPLE_LABEL = 'This is a sample label.'
const SAMPLE_ERROR = 'This is an error message.'
const SAMPLE_PLACEHOLDER = 'This is a placeholder.'

export default {
  title: 'UI/StandardTextField',
  component: StandardTextField,
} as Meta<typeof StandardTextField>

const Template: StoryFn<typeof StandardTextField> = (args) => {
  const [, updateArgs] = useArgs()
  return (
    <StandardTextField
      {...args}
      onChange={e => updateArgs({value: e.target.value})}
    />
  )
}

export const Default = {
  render: Template,

  args: {
    label: SAMPLE_LABEL,
    placeholder: '',
  },
}

export const Bold = {
  render: Template,

  args: {
    label: SAMPLE_LABEL,
    boldLabel: true,
  },
}

export const BoldStarred = {
  render: Template,

  args: {
    label: SAMPLE_LABEL,
    boldLabel: true,
    starredLabel: true,
  },
}

export const ErrorMessage = {
  render: Template,

  args: {
    label: SAMPLE_LABEL,
    errorMessage: SAMPLE_ERROR,
  },
}

export const Placeholder = {
  render: Template,

  args: {
    label: SAMPLE_LABEL,
    placeholder: SAMPLE_PLACEHOLDER,
  },
}
