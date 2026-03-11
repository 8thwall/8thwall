import React, {Component, ErrorInfo, ReactNode} from 'react'

import {ErrorDisplay} from '../ui/components/error-display'
import {StandardLink} from '../ui/components/standard-link'
import {PrimaryButton} from '../ui/components/primary-button'

// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

interface Props {
  children: ReactNode
}

interface State {
  hasError: boolean
  errorData: Error | null
}

class ErrorBoundary extends Component<Props, State> {
  public state: State = {
    hasError: false,
    errorData: null,
  }

  public static getDerivedStateFromError(err: Error): State {
    return {hasError: true, errorData: err}
  }

  public componentDidCatch(error: Error, errorInfo: ErrorInfo) {
    // eslint-disable-next-line no-console
    console.error('\n Uncaught error:', error, errorInfo)
  }

  public render() {
    if (this.state.hasError) {
      return (
        <ErrorDisplay error={this.state.errorData}>
          <p>
            An unexpected error occurred. See the console for more error details.
            Please fix the error and try again, or revert to the last known good state.
            If this keeps happening, please contact support at {' '}
            <StandardLink href='mailto:support@8thwall.com' underline>
              support@8thwall.com
            </StandardLink>
            .
          </p>
          <PrimaryButton
            onClick={() => this.setState({hasError: false, errorData: null})}
          >
            Try again
          </PrimaryButton>
        </ErrorDisplay>
      )
    }

    return this.props.children
  }
}

export default ErrorBoundary
