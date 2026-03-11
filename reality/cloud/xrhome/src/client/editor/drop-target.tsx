import * as React from 'react'

interface IDropTarget {
  as?: 'li' | 'div'
  className?: string
  onHoverStart?: Function
  onHoverStop?: Function
  onDrop?: Function
  children?: React.ReactNode
}

export class DropTarget extends React.Component<IDropTarget> {
  hovering = false

  handleDrop = (e) => {
    e.preventDefault()
    e.stopPropagation()
    this.hovering = false
    if (this.props.onDrop) {
      this.props.onDrop(e)
    }
  }

  handleDragEnter = (e) => {
    e.preventDefault()
    e.stopPropagation()
    if (this.props.onHoverStart) {
      this.props.onHoverStart()
    }
  }

  handleStartEvent = (e) => {
    e.preventDefault()
    e.stopPropagation()

    if (this.hovering) {
      return
    }

    this.hovering = true

    if (this.props.onHoverStart) {
      this.props.onHoverStart()
    }
  }

  handleStopEvent = (e) => {
    e.preventDefault()
    e.stopPropagation()

    if (!this.hovering) {
      return
    }

    this.hovering = false

    if (this.props.onHoverStop) {
      this.props.onHoverStop()
    }
  }

  render() {
    const rest = {...this.props}

    delete rest.as
    delete rest.onDrop
    delete rest.onHoverStart
    delete rest.onHoverStop

    const Tag = this.props.as || 'div'
    return (
      <Tag
        {...rest}
        onDrop={this.handleDrop}
        onDragEnter={this.handleStartEvent}
        onDragOver={this.handleStartEvent}
        onDragLeave={this.handleStopEvent}
      >
        {this.props.children}
      </Tag>
    )
  }
}
