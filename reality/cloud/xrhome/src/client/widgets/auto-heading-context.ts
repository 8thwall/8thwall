import React from 'react'

import type {HeadingLevel} from './generic-heading'

type HeadingContext = {
  level: HeadingLevel
}

const AutoHeadingContext = React.createContext<HeadingContext>({level: 1})

export default AutoHeadingContext
