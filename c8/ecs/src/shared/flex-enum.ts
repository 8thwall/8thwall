// Copied from YogaLayout/dist/src/generated/YGEnums.d.ts
/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

enum Align {
  Auto = 0,
  FlexStart = 1,
  Center = 2,
  FlexEnd = 3,
  Stretch = 4,
  Baseline = 5,
  SpaceBetween = 6,
  SpaceAround = 7,
  SpaceEvenly = 8
}

enum Dimension {
  Width = 0,
  Height = 1
}

enum Direction {
  Inherit = 0,
  LTR = 1,
  RTL = 2
}

enum Display {
  Flex = 0,
  None = 1
}

enum Edge {
  Left = 0,
  Top = 1,
  Right = 2,
  Bottom = 3,
  Start = 4,
  End = 5,
  Horizontal = 6,
  Vertical = 7,
  All = 8
}

enum Errata {
  None = 0,
  StretchFlexBasis = 1,
  AbsolutePositioningIncorrect = 2,
  AbsolutePercentAgainstInnerSize = 4,
  All = 2147483647,
  Classic = 2147483646
}

enum ExperimentalFeature {
  WebFlexBasis = 0
}

enum FlexDirection {
  Column = 0,
  ColumnReverse = 1,
  Row = 2,
  RowReverse = 3
}

enum Gutter {
  Column = 0,
  Row = 1,
  All = 2
}

enum Justify {
  FlexStart = 0,
  Center = 1,
  FlexEnd = 2,
  SpaceBetween = 3,
  SpaceAround = 4,
  SpaceEvenly = 5
}

enum LogLevel {
  Error = 0,
  Warn = 1,
  Info = 2,
  Debug = 3,
  Verbose = 4,
  Fatal = 5
}

enum MeasureMode {
  Undefined = 0,
  Exactly = 1,
  AtMost = 2
}

enum NodeType {
  Default = 0,
  Text = 1
}

enum Overflow {
  Visible = 0,
  Hidden = 1,
  Scroll = 2
}

enum PositionType {
  Static = 0,
  Relative = 1,
  Absolute = 2
}

enum Unit {
  Undefined = 0,
  Point = 1,
  Percent = 2,
  Auto = 3
}

enum Wrap {
  NoWrap = 0,
  Wrap = 1,
  WrapReverse = 2
}

export {
  Align,
  Dimension,
  Direction,
  Display,
  Edge,
  Errata,
  ExperimentalFeature,
  FlexDirection,
  Gutter,
  Justify,
  LogLevel,
  MeasureMode,
  NodeType,
  Overflow,
  PositionType,
  Unit,
  Wrap,
}
