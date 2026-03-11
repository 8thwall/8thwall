import React from 'react'
import type {Meta} from '@storybook/react'

import {Icon} from '../components/icon'
import {SecondaryButton} from '../components/secondary-button'

export default {
  title: 'UI/Button/Secondary',
  component: SecondaryButton,
} as Meta<typeof SecondaryButton>

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
