import React from 'react'

type FallbackComponent = React.ComponentType<{onReset: () => void, error: Error}>

interface IErrorBoundary {
  fallback: FallbackComponent | null
  children: React.ReactNode
}

class ErrorBoundary extends React.Component<IErrorBoundary> {
  state = {hasError: false, error: null}

  static getDerivedStateFromError(error: Error) {
    return {hasError: true, error}
  }

  componentDidCatch(error: Error, errorInfo: React.ErrorInfo) {
    // eslint-disable-next-line no-console
    console.error(error)
    // eslint-disable-next-line no-console
    console.error('Thrown from:', errorInfo.componentStack)
  }

  reset = () => {
    this.setState({hasError: false})
  }

  render() {
    if (this.state.hasError) {
      if (!this.props.fallback) {
        return null
      }
      const Fallback = this.props.fallback
      return <Fallback onReset={this.reset} error={this.state.error} />
    }

    return (
      this.props.children
    )
  }
}

export {
  ErrorBoundary,
}
