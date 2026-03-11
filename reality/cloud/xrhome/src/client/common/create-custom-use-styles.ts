import {DefaultTheme, Styles, createUseStyles} from 'react-jss'
import type {Classes} from 'jss'

// NOTE(christoph): To allow for partial type inference we need to define
//   createCustomUseStyles as a nested function so you can specify the Props type argument
//   separately from the inferred types.
const createCustomUseStyles: {<Props>(): <C extends string = string, Theme = DefaultTheme>(
  styles: Styles<C, Props, Theme> | ((theme: Theme) => Styles<C, Props, undefined>),
) => (data?: Props & {theme?: Theme}) => Classes<C>} = () => createUseStyles

export {
  createCustomUseStyles,
}
