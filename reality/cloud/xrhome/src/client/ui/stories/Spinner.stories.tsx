import React from 'react'
import type {Meta} from '@storybook/react'

import {Loader} from '../components/loader'

export default {
  title: 'UI/Spinner',
  component: Loader,
  argTypes: {
    inline: {control: 'boolean'},
    centered: {control: 'boolean'},
  },
  decorators: [
    Story => (
      <div style={{height: '10em'}}>
        <Story />
      </div>
    ),
  ],
} as Meta<typeof Loader>

export const TinyNoMsg = {
  args: {
    size: 'tiny',
  },
}

export const SmallNoMsg = {
  args: {
    size: 'small',
  },
}

export const DefaultNoMsg = {
  args: {},
}

export const LargeNoMsg = {
  args: {
    size: 'large',
  },
}

export const Tiny = {
  args: {
    size: 'tiny',
    children: 'Loading...',
  },
}

export const Small = {
  args: {
    size: 'small',
    children: 'Loading...',
  },
}

export const Default = {
  args: {
    children: 'Loading...',
  },
}

export const Large = {
  args: {
    size: 'large',
    children: 'Loading...',
  },
}

export const TinyInline = {
  args: {
    size: 'tiny',
    inline: true,
    children: 'Loading...',
  },
}

export const SmallInline = {
  args: {
    size: 'small',
    inline: true,
    children: 'Loading...',
  },
}

export const MediumInline = {
  args: {
    inline: true,
    children: 'Loading...',
  },
}

export const LargeInline = {
  args: {
    size: 'large',
    inline: true,
    children: 'Loading...',
  },
}

export const TinyCentered = {
  args: {
    size: 'tiny',
    centered: true,
    children: 'Loading...',
  },
}

export const DefaultCentered = {
  args: {
    centered: true,
    children: 'Loading...',
  },
}

export const MediumCentered = {
  args: {
    size: 'medium',
    centered: true,
    children: 'Loading...',
  },
}

export const LargeCentered = {
  args: {
    size: 'large',
    centered: true,
    children: 'Loading...',
  },
}
