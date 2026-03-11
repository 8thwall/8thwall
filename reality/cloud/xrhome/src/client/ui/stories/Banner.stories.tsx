import type {Meta} from '@storybook/react'

import {StaticBanner} from '../components/banner'

const MESSAGE = 'This is a message'

export default {
  title: 'UI/Banner',
  component: StaticBanner,
} as Meta<typeof StaticBanner>

export const Warning = {
  args: {
    type: 'warning',
    message: MESSAGE,
  },
}

export const Info = {
  args: {
    type: 'info',
    message: MESSAGE,
  },
}

export const Success = {
  args: {
    type: 'success',
    message: MESSAGE,
  },
}

export const Danger = {
  args: {
    type: 'danger',
    message: MESSAGE,
  },
}
