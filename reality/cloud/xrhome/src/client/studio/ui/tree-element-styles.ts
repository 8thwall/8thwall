import type React from 'react'

import type {GraphObject} from '@ecs/shared/scene-graph'

import type {DeepReadonly} from 'ts-essentials'

import {
  isMissingPrefab, isPrefab, isPrefabInstance, scopedObjectExists,
} from '@ecs/shared/object-hierarchy'

import {hexColorWithAlpha} from '../../../shared/colors'
import {createThemedStyles} from '../../ui/theme'
import {useSceneContext} from '../scene-context'
import {useRootAttributeId} from '../hooks/use-root-attribute-id'
import {blueberry, persimmon} from '../../static/styles/settings'
import {prefabNameExists} from '../common/studio-files'
import {useDerivedScene} from '../derived-scene-context'

interface IndentTreeElementCssProperties extends React.CSSProperties {
  '--indent-level'?: number
}

interface IndentFileBrowserCssProperties extends React.CSSProperties {
  '--file-level'?: number
}

const useTreeElementStyles = createThemedStyles(theme => ({
  treeElement: {
    padding: 0,
    margin: 0,
    display: 'flex',
    flexDirection: 'column',
  },
  hovered: {
    outline: '2px solid #44a',
  },
  treeElementButton: {
    'fontSize': '12px',
    'position': 'relative',
    'display': 'flex',
    'alignItems': 'center',
    'textAlign': 'left',
    'borderStyle': 'solid',
    'borderColor': 'transparent',
    'borderWidth': '1px 0 1px 0',
    'userSelect': 'none',
    'borderRadius': 'var(--tree-element-border-radius, 0)',
    '&:hover, &:has($treeElementBtnText:focus-visible)': {
      'background': theme.studioTreeHoverBg,
      'borderStyle': 'solid',
      'borderColor': hexColorWithAlpha(theme.sfcBorderFocus, 0.5),
      'borderWidth': '1px 0 1px 0',
      '& $treeElementIcon': {
        visibility: 'visible',
      },
    },
  },
  activeButton: {
    'background': theme.studioTreeHoverBg,
    'borderStyle': 'solid',
    'borderColor': hexColorWithAlpha(theme.sfcBorderFocus, 0.5),
    'borderWidth': '1px 0 1px 0',
    '& $treeElementIcon': {
      visibility: 'visible',
    },
  },
  selectedButton: {
    background: theme.studioTreeHoverBg,
    color: theme.fgMain,
  },
  treeElementGroup: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'stretch',
  },
  selectedGroup: {
    background: theme.studioTreeGroupBg,
    borderRadius: 'var(--tree-element-border-radius, 0)',
  },
  treeElementChildren: {
    display: 'flex',
    flexDirection: 'column',
  },
  treeElementNameRow: {
    alignItems: 'center',
    color: theme.fgMuted,
    flex: 1,
    position: 'relative',
    display: 'flex',
    width: '100%',
  },
  displayName: {
    color: theme.fgMain,
    alignItems: 'center',
    paddingLeft: 'calc(1.25em * var(--indent-level) + 1.5em)',
    paddingTop: '0.125em',
    paddingBottom: '0.125em',
    display: 'flex',
    width: '100%',
  },
  fileName: {
    color: theme.fgMain,
    alignItems: 'center',
    paddingLeft: 'calc(1.25em * var(--file-level) + 1em)',
    display: 'flex',
    width: '100%',
  },
  name: {
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
    paddingRight: '2.5em',
  },
  hideChildren: {
    paddingLeft: '0.5em',
  },
  treeElementBtnText: {
    width: '100%',
  },
  treeElementIconRow: {
    position: 'absolute',
    right: '0.75em',
    top: 0,
    padding: 0,
    height: '100%',
    display: 'flex',
    alignItems: 'center',
    color: theme.fgMuted,
    gap: '0.5em',
  },
  treeElementIcon: {
    'display': 'flex',
    'height': '16px',
    'visibility': 'hidden',
    '& > *': {
      padding: '0',
    },
  },
  visibleIcon: {
    visibility: 'visible',
  },
  chevron: {
    display: 'flex',
    alignItems: 'center',
    width: '0.75em',
    height: '100%',
    cursor: 'pointer',
    padding: 0,
    position: 'absolute',
    left: 'calc(1.25em * var(--indent-level) + 0.75em)',
    top: 0,
  },
  collapsedChevron: {
    transform: 'rotate(270deg)',
  },
  hidden: {
    '& $displayName': {
      color: theme.studioTreeHidden,
    },
    '& $typeIcon': {
      color: theme.studioTreeHidden,
    },
  },
  ephemeral: {
    '& $displayName': {
      color: theme.fgWarning,
    },
    '& $typeIcon': {
      color: theme.fgWarning,
    },
  },
  persistent: {
    '& $displayName': {
      color: theme.fgBlue,
    },
    '& $typeIcon': {
      color: theme.fgBlue,
    },
  },
  persistentAndHidden: {
    '& $displayName': {
      color: `${theme.fgBlue}80`,
    },
    '& $typeIcon': {
      color: `${theme.fgBlue}80`,
    },
  },
  ephemeralAndHidden: {
    '& $displayName': {
      color: `${theme.fgWarning}80`,
    },
    '& $typeIcon': {
      color: `${theme.fgWarning}80`,
    },
  },
  missingPrefab: {
    '& $displayName': {
      color: theme.fgError,
    },
    '& $typeIcon': {
      color: theme.fgError,
    },
  },
  instance: {
    '& $displayName': {
      color: `${persimmon}`,
    },
    '& $typeIcon': {
      color: `${persimmon}`,
    },
  },
  instanceAndHidden: {
    '& $displayName': {
      color: `${theme.fgSuccess}60`,
    },
    '& $typeIcon': {
      color: `${theme.fgSuccess}60`,
    },
  },
  typeIcon: {
    margin: '0 0.25rem',
    display: 'flex',
    alignItems: 'center',
    color: theme.fgMuted,
  },
  renaming: {
    outline: `1px solid ${theme.sfcBorderFocus}`,
  },
  overrideIcon: {
    position: 'relative',
    top: '-14px',
    right: '12px',
    width: '0',
    height: '0',
    color: blueberry,
  },
}))

const useTreeElementStatusClass = (id: string) => {
  const classes = useTreeElementStyles()
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const object = derivedScene.getObject(id)

  const hidden = !!useRootAttributeId(id, obj => obj.hidden === true)
  const disabled = !!useRootAttributeId(id, obj => obj.disabled === true)

  const missing = isMissingPrefab(ctx.scene, id)
  const instance = (ctx.scene.objects[id] && isPrefabInstance(ctx.scene.objects[id]))
  const instanceOrChild = instance || scopedObjectExists(ctx.scene, id)

  const duplicatedPrefabName = object && isPrefab(object)
    ? prefabNameExists(derivedScene, object.name, id)
    : false

  if (missing || duplicatedPrefabName) {
    return classes.missingPrefab
  }
  if (disabled || hidden) {
    if (object.persistent) {
      return classes.persistentAndHidden
    }
    if (object.ephemeral) {
      return classes.ephemeralAndHidden
    }
    if (instanceOrChild) {
      return classes.instanceAndHidden
    }
    return classes.hidden
  }

  if (object.ephemeral) {
    return classes.ephemeral
  }

  if (object.persistent) {
    return classes.persistent
  }

  if (instanceOrChild) {
    return classes.instance
  }

  return ''
}

const getTreeElementA8Tag = (object: DeepReadonly<GraphObject>) => {
  if (object.camera) {
    return `click;studio;object-camera-${object.camera.type}`
  }

  if (object.light) {
    return `click;studio;object-light-${object.light.type}`
  }

  if (object.geometry) {
    return `click;studio;object-geometry-${object.geometry.type}`
  }

  if (object.location) {
    return 'click;studio;object-location'
  }

  if (object.face) {
    return 'click;studio;object-face'
  }

  return 'click;studio;object-empty'
}

export {
  useTreeElementStyles,
  useTreeElementStatusClass,
  getTreeElementA8Tag,
  IndentTreeElementCssProperties,
  IndentFileBrowserCssProperties,
}
