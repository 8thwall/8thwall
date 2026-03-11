import React from 'react'
import type {Meta} from '@storybook/react'

import {createUseStyles} from 'react-jss'

import {IIcon, Icon, STROKES} from '../components/icon'
import {StandardDropdownField} from '../components/standard-dropdown-field'
import {SpaceBetween} from '../layout/space-between'
import {combine} from '../../common/styles'

const hasGrid = (size: string) => size === '400'

const useStyles = createUseStyles({
  grid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fill, minmax(calc(var(--grid-size) * 1px), 1fr))',
    gridGap: '2em',
    justifyItems: 'center',
  },
  icon: {
    'position': 'relative',
    'backgroundColor': '#e5e5f7',
    'box-shadow': '0 0 0 1px #444cf7',
    'backgroundSize': '10px 10px',
    'width': 'calc(var(--grid-size) * 1px)',
    'height': 'calc(var(--grid-size) * 1px)',
    '& > :not($label)': {
      transform: 'scale(calc(var(--grid-size) / 16))',
      transformOrigin: 'top left',
    },
  },
  withPixelGrid: {
    backgroundImage: `linear-gradient(#444cf7 1px, transparent 1px)
    , linear-gradient(to right, #444cf7 1px, #e5e5f7 1px)`,
    backgroundSize: '25px 25px',
  },
  label: {
    zIndex: 1,
    background: 'white',
    color: 'black',
    position: 'absolute',
    top: 'calc(100% + 2px)',
    left: '0',
  },
})

const GridEntry = ({stroke, withGrid}: {stroke: IIcon['stroke'], withGrid?: boolean}) => {
  const classes = useStyles()
  return (
    <div className={combine(classes.icon, withGrid && classes.withPixelGrid)} key={stroke}>
      <div className={classes.label}>{stroke}</div>
      <Icon stroke={stroke} color='black' />
    </div>
  )
}

const IconShowcase: React.FC = () => {
  const classes = useStyles()
  const [size, setSize] = React.useState('160')

  return (
    <SpaceBetween direction='vertical'>
      <StandardDropdownField
        id='size-select'
        value={size}
        onChange={setSize}
        options={['16', '64', '160', '400'].map(e => ({
          value: e,
          content: e + (hasGrid(e) ? ' (with grid)' : ''),
        }))}
        label='Size'
      />
      <div className={classes.grid} style={{'--grid-size': size} as any}>
        {Object.keys(STROKES).map(s => (
          <GridEntry
            key={s}
            stroke={s as IIcon['stroke']}
            withGrid={hasGrid(size)}
          />
        ))}
      </div>
    </SpaceBetween>
  )
}

export default {
  title: 'Views/IconShowcase',
  component: IconShowcase,
} as Meta<typeof IconShowcase>

export const All = {
  args: {},
}
