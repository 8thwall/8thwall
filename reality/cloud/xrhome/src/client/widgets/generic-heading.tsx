import React from 'react'

// A generic heading is a heading like an h1, h2, etc, but the level is specified as a prop, with 1
// as the largest, and 6 as the smallest.

type HeadingLevel = 1|2|3|4|5|6

const toHeadingLevel = (level: number): HeadingLevel => {
  if (level < 1) {
    return 1
  }
  if (level > 6) {
    return 6
  }
  return level as HeadingLevel
}

type HeadingLevelElement = 'h1'|'h2'|'h3'|'h4'|'h5'|'h6'

interface IGenericHeading extends React.HTMLProps<HTMLHeadingElement> {
  level: HeadingLevel
}

const GenericHeading: React.FC<IGenericHeading> = React.forwardRef(
  ({children, level, ...rest}, ref) => {
    const Element = `h${Math.max(1, Math.min(level, 6))}` as HeadingLevelElement
    return <Element {...rest} data-level={level} ref={ref}>{children}</Element>
  }
)

export default GenericHeading

export {
  toHeadingLevel,
}

export type {
  HeadingLevel,
}
