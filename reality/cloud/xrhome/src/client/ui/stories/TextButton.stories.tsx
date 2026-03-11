import React from 'react'
import type {Meta} from '@storybook/react'

import {TextButton} from '../components/text-button'
import {Icon} from '../components/icon'

export default {
  title: 'UI/Button/Text',
  component: TextButton,
} as Meta<typeof TextButton>

export const Basic = {
  args: {
    loading: false,
    disabled: false,
    children: 'Text Button',
  },
}

export const WithIcon = {
  args: {
    loading: false,
    disabled: false,
    children: (
      <>
        <Icon stroke='plus' inline /> Add Item
      </>
    ),
  },
}

export const Loading = {
  args: {
    loading: true,
    children: 'Loading...',
  },
}
