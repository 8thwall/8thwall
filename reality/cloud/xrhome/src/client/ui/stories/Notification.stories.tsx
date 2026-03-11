import type {Meta} from '@storybook/react'

import {TextNotification} from '../components/text-notification'

const MESSAGE = 'This is a message'

export default {
  title: 'UI/Notification',
  component: TextNotification,
} as Meta<typeof TextNotification>

export const Warning = {
  args: {
    type: 'warning',
    children: MESSAGE,
  },
}

export const Info = {
  args: {
    type: 'info',
    children: MESSAGE,
  },
}

export const Success = {
  args: {
    type: 'success',
    children: MESSAGE,
  },
}

export const Danger = {
  args: {
    type: 'danger',
    children: MESSAGE,
  },
}
