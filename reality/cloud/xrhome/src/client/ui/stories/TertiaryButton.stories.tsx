import React from 'react'
import type {Meta} from '@storybook/react'

import {TertiaryButton} from '../components/tertiary-button'
import {Icon} from '../components/icon'

export default {
  title: 'UI/Button/Tertiary',
  component: TertiaryButton,
} as Meta<typeof TertiaryButton>

export const Submit = {
  args: {
    loading: false,
    children: 'Submit',
  },
}

export const WithIcon = {
  args: {
    loading: false,
    children: (
      <>
        <Icon stroke='plus' inline /> Create new thing
      </>
    ),
  },
}

export const Loading = {
  args: {
    loading: true,
    children: 'Test content',
  },
}

export const WithIconLoading = {
  args: {
    loading: true,
    children: (
      <>
        <Icon stroke='plus' inline /> Hello world
      </>
    ),
  },
}
