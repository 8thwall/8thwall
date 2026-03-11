import type {Meta} from '@storybook/react'

import {VisualAligner} from '../components/visual-aligner'

export default {
  title: 'UI/VisualAligner',
  component: VisualAligner,
} as Meta<typeof VisualAligner>

export const StackRowStart = {
  args: {
    layoutMode: 'stack',
    direction: 'row',
    value: ['flex-start', 'flex-start'],
  },
}

export const StackRowCenter = {
  args: {
    layoutMode: 'stack',
    direction: 'row',
    value: ['flex-start', 'center'],
  },
}

export const StackRowEnd = {
  args: {
    layoutMode: 'stack',
    direction: 'row',
    value: ['flex-start', 'flex-end'],
  },
}

export const StackColumnTop = {
  args: {
    layoutMode: 'stack',
    direction: 'column',
    value: ['flex-start', 'flex-start'],
  },
}

export const StackColumnCenter = {
  args: {
    layoutMode: 'stack',
    direction: 'column',
    value: ['center', 'flex-start'],
  },
}

export const StackColumnEnd = {
  args: {
    layoutMode: 'stack',
    direction: 'column',
    value: ['flex-end', 'flex-start'],
  },
}

export const StackRowStretchStart = {
  args: {
    layoutMode: 'stack',
    direction: 'row',
    value: ['space-between', 'flex-start'],
  },
}

export const StackRowStretchCenter = {
  args: {
    layoutMode: 'stack',
    direction: 'row',
    value: ['space-between', 'center'],
  },
}

export const StackRowStretchEnd = {
  args: {
    layoutMode: 'stack',
    direction: 'row',
    value: ['space-between', 'flex-end'],
  },
}

export const StackColumnStretchStart = {
  args: {
    layoutMode: 'stack',
    direction: 'column',
    value: ['flex-start', 'space-between'],
  },
}

export const StackColumnStretchCenter = {
  args: {
    layoutMode: 'stack',
    direction: 'column',
    value: ['center', 'space-between'],
  },
}

export const StackColumnStretchEnd = {
  args: {
    layoutMode: 'stack',
    direction: 'column',
    value: ['flex-end', 'space-between'],
  },
}

export const Grid = {
  args: {
    layoutMode: 'grid',
    direction: 'row',
    value: ['flex-start', 'flex-start'],
  },
}
