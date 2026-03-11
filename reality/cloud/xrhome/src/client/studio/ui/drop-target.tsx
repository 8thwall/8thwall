import React from 'react'

interface IDropTarget {
  as?: 'li' | 'div' | 'ul'
  className?: string
  onHoverStart?: () => void
  onHoverStop?: () => void
  onDrop?: (e: React.DragEvent) => void
  children?: React.ReactNode
}

const DropTarget: React.FC<IDropTarget> = ({
  as: Tag = 'div', onHoverStart, onHoverStop, onDrop, children, ...rest
}) => {
  const hoveringRef = React.useRef(false)

  const handleDrop = (e: React.DragEvent) => {
    e.preventDefault()
    e.stopPropagation()
    hoveringRef.current = false
    onDrop?.(e)
  }

  const handleStartEvent = (e: React.DragEvent) => {
    e.preventDefault()
    e.stopPropagation()

    if (hoveringRef.current) {
      return
    }

    hoveringRef.current = true

    onHoverStart?.()
  }

  const handleStopEvent = (e: React.DragEvent) => {
    e.preventDefault()
    e.stopPropagation()

    if (!hoveringRef.current) {
      return
    }

    hoveringRef.current = false

    onHoverStop?.()
  }

  return (
    <Tag
      {...rest}
      onDrop={handleDrop}
      onDragEnter={handleStartEvent}
      onDragOver={handleStartEvent}
      onDragLeave={handleStopEvent}
    >
      {children}
    </Tag>
  )
}

export {
  DropTarget,
}
