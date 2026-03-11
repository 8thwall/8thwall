import React from 'react'

import AutoHeadingContext from './auto-heading-context'
import GenericHeading from './generic-heading'

const AutoHeading: React.FunctionComponent<React.HTMLProps<HTMLHeadingElement>> = React.forwardRef(
  ({children, ...rest}, ref) => {
    const context = React.useContext(AutoHeadingContext)
    return <GenericHeading {...rest} ref={ref} level={context.level}>{children}</GenericHeading>
  }
)

export default AutoHeading
