type AlignContent =
  | 'flex-start'
  | 'flex-end'
  | 'center'
  | 'stretch'
  | 'space-between'
  | 'space-around'
  | 'space-evenly'

type AlignItems =
  | 'flex-start'
  | 'flex-end'
  | 'center'
  | 'stretch'
  | 'baseline'

type JustifyContent =
  | 'flex-start'
  | 'flex-end'
  | 'center'
  | 'space-between'
  | 'space-around'
  | 'space-evenly'

type TextAlignContent =
  | 'left'
  | 'center'
  | 'right'
  | 'justify'

type VerticalTextAlignContent =
  | 'start'
  | 'center'
  | 'end'

type FlexWrap = 'nowrap' | 'wrap' | 'wrap-reverse'
type FlexDirection = 'row' | 'column' | 'row-reverse' | 'column-reverse'
type Direction = 'ltr' | 'rtl'
type Display = 'none' | 'flex'
type Overflow = 'visible' | 'hidden' | 'scroll'
type PositionMode = 'absolute' | 'relative' | 'static'

export type {
  FlexWrap,
  AlignContent,
  AlignItems,
  JustifyContent,
  FlexDirection,
  Direction,
  Display,
  Overflow,
  PositionMode,
  TextAlignContent,
  VerticalTextAlignContent,
}
