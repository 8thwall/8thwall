import React from 'react'

const useIdFallback = (idOverride?: string): string => {
  const generatedId = React.useId()
  return idOverride || generatedId
}

export {
  useIdFallback,
}
