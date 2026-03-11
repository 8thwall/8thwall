import React from 'react'

import homeActions from './home-actions'
import {useSelector} from '../hooks'
import useActions from '../common/use-actions'
import {StaticBanner} from '../ui/components/banner'

const ErrorMessage: React.FC<{className?: string}> = ({className}) => {
  const error = useSelector(state => state.common.error)
  const message = useSelector(state => state.common.message)
  const success = useSelector(state => state.common.success)
  const {acknowledgeError, acknowledgeMessage, acknowledgeSuccess} = useActions(homeActions)

  if (!error && !message && !success) {
    return null
  }

  return (
    <div className={className}>
      {error && (
        <StaticBanner type='danger' onClose={acknowledgeError}> {`ERROR: ${error}`} </StaticBanner>
      )}
      {message && (
        <StaticBanner type='info' onClose={acknowledgeMessage}>{message}</StaticBanner>
      )}
      {success && (
        <StaticBanner type='success' onClose={acknowledgeSuccess}>{success}</StaticBanner>
      )}
    </div>
  )
}

export default ErrorMessage
