import React from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  bottom: {
    bottom: '0',
  },
  left: {
    left: '0',
    top: '0',
  },
  right: {
    right: '0',
    top: '0',
  },
  seCorner: {
    right: '0',
    bottom: '0',
    cursor: 'nwse-resize',
  },
  swCorner: {
    left: '0',
    bottom: '0',
    cursor: 'nesw-resize',
  },
  resizerCorner: {
    position: 'absolute',
    width: '20px',
    height: '20px',
    zIndex: '1105',
  },
  resizerUpDown: {
    position: 'absolute',
    width: '100%',
    height: '4px',
    zIndex: '1100',
    cursor: 'ns-resize',
  },
  resizerLeftRight: {
    position: 'absolute',
    width: '4px',
    height: '100%',
    zIndex: '1100',
    cursor: 'ew-resize',
  },
})

type ResizeMeasurements = {
  width: number
  height: number
  mouseX: number
  mouseY: number
  resizeWidth: boolean
  resizeHeight: boolean
  fromLeft: boolean
}

interface IResizablePanel {
  children: React.ReactNode
  left?: boolean
  bottom?: boolean
  right?: boolean
  bottomLeft?: boolean
  bottomRight?: boolean
  className?: string
  minWidth?: number
  minHeight?: number
  maxWidth?: string | number
  maxHeight?: string | number
  defaultWidth?: string | number
  defaultHeight?: string | number
  panelWidth?: number
  panelHeight?: number
  onResizeStart?: () => void
  onResize?: (width?: number, height?: number) => void
  onResizeEnd?: (width?: number, height?: number) => void
}

type PanelDimesions = {
  width?: number
  height?: number
}

// TODO(juliesoohoo): Allow resizing from top, topLeft, and topRight, or replace with a library
const ResizablePanel: React.FC<IResizablePanel> = ({
  className, children, left = false, bottom = false, right = false,
  bottomLeft = false, bottomRight = false,
  minWidth = 0, minHeight = 0, maxWidth = '100%', maxHeight = '100%',
  defaultWidth = 100, defaultHeight = 100, panelWidth, panelHeight,
  onResizeStart, onResize, onResizeEnd,
}) => {
  const {t} = useTranslation(['common'])
  const classes = useStyles()
  const panelRef = React.useRef<HTMLDivElement>(null)
  const originalMeasurementsRef = React.useRef<ResizeMeasurements | null>(null)
  const defaultWidthVal = typeof defaultWidth === 'number' ? `${defaultWidth}px` : defaultWidth
  const defaultHeightVal = typeof defaultHeight === 'number' ? `${defaultHeight}px` : defaultHeight

  const getPanelDimensions = (): PanelDimesions => {
    if (!panelRef.current) {
      return {width: undefined, height: undefined}
    }
    const panelStyles = getComputedStyle(panelRef.current, null)
    return {
      width: parseFloat(panelStyles.getPropertyValue('width').replace('px', '')),
      height: parseFloat(panelStyles.getPropertyValue('height').replace('px', '')),
    }
  }

  const prevDimensions = React.useRef<PanelDimesions>({width: panelWidth, height: panelHeight})
  React.useLayoutEffect(() => {
    const dimensions = getPanelDimensions()
    if (dimensions.width && dimensions.height &&
       (prevDimensions.current.width !== dimensions.width ||
        prevDimensions.current.height !== dimensions.height)
    ) {
      onResize?.(dimensions.width, dimensions.height)
      prevDimensions.current = dimensions
    }
  })

  const getValidWidth = (inputWidth: number) => {
    const width = inputWidth !== undefined ? `${inputWidth}px` : defaultWidthVal
    const maxWidthValue = typeof maxWidth === 'number' ? `${maxWidth}px` : maxWidth
    return `min(${maxWidthValue}, max(${minWidth}px, ${width}))`
  }
  const getValidHeight = (inputHeight: number) => {
    const height = inputHeight !== undefined ? `${inputHeight}px` : defaultHeightVal
    const maxHeightValue = typeof maxHeight === 'number' ? `${maxHeight}px` : maxHeight
    return `min(${maxHeightValue}, max(${minHeight}px, ${height}))`
  }

  React.useEffect(() => {
    if (panelRef.current) {
      const {width, height} = getPanelDimensions()
      onResizeEnd?.(width, height)
    }
  }, [minHeight, minWidth, maxHeight, maxWidth])

  const resize = (e) => {
    if (!panelRef.current || !originalMeasurementsRef.current) {
      return
    }
    const {
      width: originalWidth, height: originalHeight,
      mouseX: originalMouseX, mouseY: originalMouseY,
      resizeWidth, resizeHeight, fromLeft,
    } = originalMeasurementsRef.current

    if (resizeWidth) {
      const dX = e.pageX - originalMouseX
      const width = fromLeft ? originalWidth - dX : originalWidth + dX
      if (width >= minWidth) {
        panelRef.current.style.width = getValidWidth(width)
      }
    }
    // TODO(juliesoohoo): Include case for resizing from the top
    if (resizeHeight) {
      const height = originalHeight + (e.pageY - originalMouseY)
      if (height >= minHeight) {
        panelRef.current.style.height = getValidHeight(height)
      }
    }

    if (resizeWidth || resizeHeight) {
      const {width, height} = getPanelDimensions()
      onResize?.(width, height)
    }
  }

  const stopResize = () => {
    document.removeEventListener('mousemove', resize)
    document.removeEventListener('mouseup', stopResize)
    originalMeasurementsRef.current = null
    const {width, height} = getPanelDimensions()
    onResizeEnd?.(width, height)
  }

  const startResize = (
    e: React.MouseEvent, resizeWidth: boolean, resizeHeight: boolean, fromLeft: boolean
  ) => {
    if (!panelRef.current) {
      return
    }

    const {width, height} = getPanelDimensions()
    originalMeasurementsRef.current = {
      width,
      height,
      mouseX: e.pageX,
      mouseY: e.pageY,
      resizeWidth,
      resizeHeight,
      fromLeft,
    }

    document.addEventListener('mousemove', resize)
    document.addEventListener('mouseup', stopResize)
    onResizeStart?.()
  }

  return (
    <div
      className={className}
      ref={panelRef}
      style={{width: getValidWidth(panelWidth), height: getValidHeight(panelHeight)}}
    >
      {left && (
        <div
          className={combine(classes.resizerLeftRight, classes.left)}
          aria-label={t('resizer.left.label')}
          role='button'
          tabIndex={-1}
          onMouseDown={e => startResize(e, true, false, true)}
        />
      )}
      {right && (
        <div
          className={combine(classes.resizerLeftRight, classes.right)}
          aria-label={t('resizer.right.label')}
          role='button'
          tabIndex={-1}
          onMouseDown={e => startResize(e, true, false, false)}
        />
      )}
      {bottom && (
        <div
          className={combine(classes.resizerUpDown, classes.bottom)}
          aria-label={t('resizer.bottom.label')}
          role='button'
          tabIndex={-1}
          onMouseDown={e => startResize(e, false, true, false)}
        />
      )}
      {bottomLeft && (
        <div
          className={combine(classes.resizerCorner, classes.swCorner)}
          aria-label={t('resizer.bottom_left.label')}
          role='button'
          tabIndex={-1}
          onMouseDown={e => startResize(e, true, true, true)}
        />
      )}
      {bottomRight && (
        <div
          className={combine(classes.resizerCorner, classes.seCorner)}
          aria-label={t('resizer.bottom_right.label')}
          role='button'
          tabIndex={-1}
          onMouseDown={e => startResize(e, true, true, false)}
        />
      )}
      {children}
    </div>
  )
}

export {
  ResizablePanel,
}
