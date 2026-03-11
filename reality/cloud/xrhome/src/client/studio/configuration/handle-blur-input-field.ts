import type React from 'react'

const handleBlurInputField = (e: React.KeyboardEvent<HTMLInputElement>) => {
  // eslint-disable-next-line local-rules/hardcoded-copy
  if (e.key === 'Enter' || e.key === 'Escape') {
    const target = e.target as HTMLInputElement
    target.blur()
  }
}

export {handleBlurInputField}
