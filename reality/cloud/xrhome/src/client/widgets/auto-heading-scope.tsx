import React from 'react'

import AutoHeadingContext from './auto-heading-context'
import type {HeadingLevel} from './generic-heading'

interface IAutoHeadingScope {
  level?: HeadingLevel
  children?: React.ReactNode
}

const AutoHeadingScope: React.FC<IAutoHeadingScope> = ({children, level}) => {
  const context = React.useContext(AutoHeadingContext)

  return (
    <AutoHeadingContext.Provider
      value={{level: level !== undefined ? level : (context.level + 1 as HeadingLevel)}}
    >
      {children}
    </AutoHeadingContext.Provider>
  )
}

export default AutoHeadingScope
