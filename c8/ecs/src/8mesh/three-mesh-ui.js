import Block from './components/Block'
import Text from './components/Text'
import InlineBlock from './components/InlineBlock'
import Keyboard from './components/Keyboard'
import UpdateManager from './components/core/UpdateManager'
import pageFileName from './components/core/FontLibrary'
import * as TextAlign from './utils/inline-layout/TextAlign'
import * as Whitespace from './utils/inline-layout/Whitespace'
import * as JustifyContent from './utils/block-layout/JustifyContent'
import * as AlignItems from './utils/block-layout/AlignItems'
import * as ContentDirection from './utils/block-layout/ContentDirection'

const update = () => UpdateManager.update()

const ThreeMeshUI = {
  Block,
  Text,
  InlineBlock,
  Keyboard,
  FontLibrary: pageFileName,
  update,
  TextAlign,
  Whitespace,
  JustifyContent,
  AlignItems,
  ContentDirection,
}

if (typeof global !== 'undefined') global.ThreeMeshUI = ThreeMeshUI

export {
  Block,
  Text,
  InlineBlock,
  Keyboard,
  pageFileName as FontLibrary,
  update,
  TextAlign,
  Whitespace,
  JustifyContent,
  AlignItems,
  ContentDirection,
  ThreeMeshUI as default,
}
