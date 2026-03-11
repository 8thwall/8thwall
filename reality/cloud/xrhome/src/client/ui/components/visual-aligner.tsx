import React from 'react'

import {combine} from '../../common/styles'
import type {UiTheme} from '../theme'
import {Icon, IconStroke} from './icon'
import {createCustomUseStyles} from '../../common/create-custom-use-styles'

const NUM_ROWS = 3
const NUM_COLS = 3
const CELL_WIDTH = 48
const CELL_HEIGHT = 36

enum LayoutMode {
  STACK = 'stack',
  GRID = 'grid',
}

enum VisualAlignerDirection {
  ROW = 'row',
  COLUMN = 'column',
}

enum AlignDirection {
  START = 'start',
  CENTER = 'center',
  END = 'end',
  SPACED = 'spaced',
}

type Align = [AlignDirection, AlignDirection]

const ROWS = [AlignDirection.START, AlignDirection.CENTER, AlignDirection.END] as const
const COLS = [
  AlignDirection.START, AlignDirection.CENTER, AlignDirection.END] as const

const IconOrientation = {
  DEFAULT: 'default',
  ROTATE_90: 'rotate90',
  FLIP_VERTICAL: 'flipVertical',
  FLIP_HORIZONTAL: 'flipHorizontal',
} as const

type IconOrientationType = typeof IconOrientation[keyof typeof IconOrientation]

const useStyles = createCustomUseStyles<{
  orientation?: IconOrientationType
}>()((theme: UiTheme) => ({
  container: {
    display: 'flex',
    flexDirection: 'column',
    backgroundColor: theme.mainEditorPane,
    borderRadius: '4px',
    width: `${CELL_WIDTH * NUM_COLS}px`,
    height: `${CELL_HEIGHT * NUM_ROWS}px`,
  },
  row: {
    display: 'flex',
    flex: 1,
    flexDirection: 'row',
    width: '100%',
  },
  cell: {
    'display': 'flex',
    'width': `${CELL_WIDTH}px`,
    'height': `${CELL_HEIGHT}px`,
    'justifyContent': 'center',
    'alignItems': 'center',
    'cursor': 'pointer',
    '&:disabled': {
      cursor: 'default',
    },
  },
  circle: {
    width: '6px',
    height: '6px',
    borderRadius: '50%',
    backgroundColor: theme.fgMuted,
  },
  disabled: {
    backgroundColor: theme.fgDisabled,
  },
  iconContainer: {
    transform: ({orientation}) => {
      switch (orientation) {
        case IconOrientation.ROTATE_90:
          return 'rotate(90deg)'
        case IconOrientation.FLIP_VERTICAL:
          return 'scaleY(-1)'
        case IconOrientation.FLIP_HORIZONTAL:
          return 'scaleX(-1) rotate(90deg)'
        case IconOrientation.DEFAULT:
        default:
          return 'none'
      }
    },
  },
}))

interface IAlignIcon {
  center: boolean
  position: number
  orientation: IconOrientationType
  stretch: boolean
}

const AlignIcon: React.FC<IAlignIcon> = ({center, position, orientation, stretch}) => {
  const classes = useStyles({orientation})
  let stroke: IconStroke = 'alignRowCenter'
  if (stretch) {
    switch (position) {
      case 0:
        stroke = center ? 'alignRowCenterFirst' : 'alignRowBottomFirst'
        break
      case 1:
        stroke = center ? 'alignRowCenterCenter' : 'alignRowBottomCenter'
        break
      default:
        stroke = center ? 'alignRowCenterLast' : 'alignRowBottomLast'
    }
  } else {
    stroke = center ? 'alignRowCenter' : 'alignRowBottom'
  }

  return (
    <div className={classes.iconContainer}>
      <Icon
        stroke={stroke}
        color='gray3'
        size={1.913}
      />
    </div>
  )
}

type VisualAlignerDirectionType = `${VisualAlignerDirection}`
interface IVisualAligner {
  layoutMode: LayoutMode
  direction: VisualAlignerDirectionType
  value: Align
  disabled?: boolean
  onChange: (value: Align) => void
}

const VisualAligner: React.FC<IVisualAligner> = ({
  layoutMode, direction, value, disabled, onChange,
}) => {
  const classes = useStyles({})

  const [horizontalAlign, verticalAlign] = value
  const isCellSelected = (row: number, col: number) => {
    let isHorizontalMatch = false
    let isVerticalMatch = false

    if (horizontalAlign === AlignDirection.SPACED) {
      isHorizontalMatch = true
    } else if (col === 0) {
      isHorizontalMatch = horizontalAlign === AlignDirection.START
    } else if (col === 1) isHorizontalMatch = horizontalAlign === AlignDirection.CENTER
    else if (col === 2) isHorizontalMatch = horizontalAlign === AlignDirection.END

    if (verticalAlign === AlignDirection.SPACED) {
      isVerticalMatch = true
    } else if (row === 0) isVerticalMatch = verticalAlign === AlignDirection.START
    else if (row === 1) isVerticalMatch = verticalAlign === AlignDirection.CENTER
    else if (row === 2) isVerticalMatch = verticalAlign === AlignDirection.END

    return isHorizontalMatch && isVerticalMatch
  }

  const renderCell = (row: number, col: number) => {
    if (!isCellSelected(row, col)) {
      return (
        <div className={combine(classes.circle, disabled && classes.disabled)} />
      )
    }

    if (layoutMode === LayoutMode.GRID) {
      return (
        <Icon stroke='roundedSquare' color='gray3' size={1.25} />
      )
    }

    let isCenter: boolean = false
    let orientation: IconOrientationType = IconOrientation.DEFAULT
    if (direction === VisualAlignerDirection.ROW) {
      if (row === 0) {
        orientation = IconOrientation.FLIP_VERTICAL
      } else if (row === 1) {
        isCenter = true
      }
    } else if (direction === VisualAlignerDirection.COLUMN) {
      if (col === 0) {
        orientation = IconOrientation.ROTATE_90
      } else if (col === 1) {
        isCenter = true
        orientation = IconOrientation.FLIP_HORIZONTAL
      } else if (col === 2) {
        orientation = IconOrientation.FLIP_HORIZONTAL
      }
    } else {
      throw new Error(`Invalid direction: ${direction}`)
    }

    return (
      <AlignIcon
        center={isCenter}
        position={direction === VisualAlignerDirection.ROW ? col : row}
        orientation={orientation}
        stretch={horizontalAlign === AlignDirection.SPACED ||
          verticalAlign === AlignDirection.SPACED}
      />
    )
  }

  const handleCellClick = (row: number, col: number) => {
    if (disabled) {
      return
    }

    let newHorizontalAlign: AlignDirection = horizontalAlign
    let newVerticalAlign: AlignDirection = verticalAlign

    if (horizontalAlign === AlignDirection.SPACED) {
      newHorizontalAlign = AlignDirection.SPACED
    } else if (col === 0) newHorizontalAlign = AlignDirection.START
    else if (col === 1) newHorizontalAlign = AlignDirection.CENTER
    else if (col === 2) newHorizontalAlign = AlignDirection.END

    if (verticalAlign === AlignDirection.SPACED) {
      newVerticalAlign = AlignDirection.SPACED
    } else if (row === 0) newVerticalAlign = AlignDirection.START
    else if (row === 1) newVerticalAlign = AlignDirection.CENTER
    else if (row === 2) newVerticalAlign = AlignDirection.END

    onChange([newHorizontalAlign, newVerticalAlign])
  }

  return (
    <div className={classes.container}>
      {
        ROWS.map((rowKey, i) => (
          <div key={`row-${rowKey}`} className={classes.row}>
            {
              COLS.map((colKey, j) => (
                <button
                  key={`col-${rowKey}-${colKey}`}
                  type='button'
                  className={combine('style-reset', classes.cell)}
                  onClick={() => handleCellClick(i, j)}
                  disabled={disabled}
                >
                  {renderCell(i, j)}
                </button>
              ))
            }
          </div>
        ))
      }
    </div>
  )
}

export {
  VisualAligner,
  IVisualAligner,
  Align,
  VisualAlignerDirection,
  LayoutMode,
  AlignDirection,
}
