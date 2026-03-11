import React from 'react'

import {createThemedStyles} from '../../ui/theme'

const GRID_OF_SQUARES_ASSET_SIZE = 450
const ONE_COLUMN_ASSET_GRID_SIZE = GRID_OF_SQUARES_ASSET_SIZE * 2

const useStyles = createThemedStyles(() => ({
  grid: {
    display: 'grid',
    gap: '4px',
  },
}))

type IGridOfSquares = {
  numColumns?: number  // e.g. 3. If exists, autoForCellSize is ignored.
  // When numColumns is not set, minmax with autoForCellSize, e.g. '1fr', '200px'
  columnSize?: string
  autoForCellSize?: string  // e.g. '200px'
  children?: React.ReactNode
}

const columnBasedRepeat = (numColumns: number): string => (
  numColumns > 1 ? `repeat(${numColumns}, 1fr)` : '1fr'
)

const GridOfSquares: React.FC<IGridOfSquares> = ({
  numColumns, autoForCellSize, columnSize = '1fr', children,
}) => {
  // Organize the children into rows based on the number of columns
  const gridTemplateColumns = numColumns
    ? columnBasedRepeat(numColumns)
    : `repeat(auto-fill, minmax(${autoForCellSize}, ${columnSize}))`
  const classes = useStyles()
  // Generate grid template columns based on the number of columns
  return (
    <div className={classes.grid} style={{gridTemplateColumns}}>
      {children}
    </div>
  )
}

export {
  GridOfSquares,
  GRID_OF_SQUARES_ASSET_SIZE,
  ONE_COLUMN_ASSET_GRID_SIZE,
}
