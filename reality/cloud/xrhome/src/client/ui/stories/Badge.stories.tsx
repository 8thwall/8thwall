import type {Meta} from '@storybook/react'

import {Badge} from '../components/badge'

export default {
  title: 'UI/Badge',
  component: Badge,
} as Meta<typeof Badge>

export const Primary = {
  args: {
    color: 'default',
    children: 'Default',
  },
}

export const Inverted = {
  args: {
    color: 'inverted',
    children: 'Inverted',
  },
}

export const Hightlight = {
  args: {
    color: 'highlight',
    children: 'Highlight',
  },
}

export const Blue = {
  args: {
    color: 'blue',
    children: 'Blue',
  },
}

export const Mango = {
  args: {
    color: 'mango',
    children: 'Mango',
  },
}

export const Mint = {
  args: {
    color: 'mint',
    children: 'Mint',
  },
}

export const Danger = {
  args: {
    color: 'danger',
    children: 'Danger',
  },
}
