import React from 'react'
import type {Meta} from '@storybook/react'
import {createUseStyles} from 'react-jss'

import {Tag, ITag} from '../components/tag'
import {SpaceBetween} from '../layout/space-between'

const useStyles = createUseStyles({
  grid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fill, minmax(calc(var(--grid-size) * 1px), 1fr))',
    gridGap: '2em',
    justifyItems: 'center',
  },
})

const TAG_VARIANTS: Array<ITag['height']> = ['medium', 'small', 'tiny']

type TagShowCaseProps = {size: string}

const TagShowCase: React.FC<TagShowCaseProps> = ({size}) => {
  const classes = useStyles()
  return (
    <SpaceBetween direction='vertical'>
      <h2>Normal</h2>
      <div className={classes.grid} style={{'--grid-size': size} as any}>
        {TAG_VARIANTS.map(variant => (
          <Tag key={variant} height={variant}>
            {variant}
          </Tag>
        ))}
      </div>
    </SpaceBetween>
  )
}

export default {
  title: 'Views/TagShowCase',
  render: TagShowCase,
  argTypes: {
    size: {
      control: 'select',
      options: ['64', '160', '400'],
    },
  },
} as Meta<typeof TagShowCase>

export const All = {
  args: {
    badgeSize: '160',
    badgeHeight: 'tiny',
    badgeSpacing: 'normal',
  },
}
