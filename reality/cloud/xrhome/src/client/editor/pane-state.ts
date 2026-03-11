import type {DeepReadonly} from 'ts-essentials'

import {EditorTabs, getTopTabInStack} from './tab-state'

interface Pane {
  id: number
  parentId: number  // null for root pane
}

interface RootPane extends Pane {
  type: 'root'
  id: 0
  childrenIds: [number]
}

interface TabbedEditorPane extends Pane {
  type: 'tabbed-editor'
  // TabbedEditor knows which tab is active based on the highest stackIndex.
  // Tabs keep track of which pane they belong to.
}

interface SplitPane extends Pane {
  type: 'split'
  direction: 'vertical' | 'horizontal'
  ratio: number  // double between (0,1)
  childrenIds: [number, number]
}

type EditorPane = RootPane | TabbedEditorPane | SplitPane

type EditorPanes = DeepReadonly<{
  idCounter: number
  rootId: number
  panes: Record<number, EditorPane>
}>

const handleSplitPane = (
  state: EditorPanes,
  paneIdToSplit: number
): EditorPanes => {
  const paneToSplit = state.panes[paneIdToSplit]
  if (!paneToSplit) {
    throw new Error(`pane not found ${paneIdToSplit}`)
  }
  const self = {...paneToSplit}
  if (self.type === 'root') {
    throw new Error('cannot split the root pane')
  }

  let {idCounter} = state

  const newParentId = idCounter++
  const newSiblingId = idCounter++

  const oldParent = {...state.panes[self.parentId]}
  if (oldParent.type !== 'root' && oldParent.type !== 'split') {
    throw new Error(`unable to split: parent pane type ${oldParent.type} does not have children`)
  }

  // @ts-ignore we are mapping the same pane type 1:1, we know that the tuple size is preserved.
  oldParent.childrenIds = oldParent.childrenIds.map(id => (id === paneIdToSplit ? newParentId : id))

  const newSiblingPane: TabbedEditorPane = {
    type: 'tabbed-editor',
    id: newSiblingId,
    parentId: newParentId,
  }

  const newParent: SplitPane = {
    type: 'split',
    id: newParentId,
    parentId: self.parentId,
    direction: 'vertical',
    ratio: 0.5,
    childrenIds: [paneIdToSplit, newSiblingId],
  }

  self.parentId = newParentId

  return {
    ...state,
    idCounter,
    panes: {
      ...state.panes,
      [oldParent.id]: oldParent,
      [newParent.id]: newParent,
      [self.id]: self,
      [newSiblingPane.id]: newSiblingPane,
    },
  }
}

const handleClosePane = (
  state: DeepReadonly<EditorPanes>,
  paneIdToClose: number
): EditorPanes => {
  const self = {...state.panes[paneIdToClose]}
  if (!self) {
    throw new Error(`pane not found ${self}`)
  }
  if (self.type !== 'tabbed-editor') {
    throw new Error(`cannot directly close pane type ${self.type}`)
  }
  // Only tabbed editor panes can be closed for the time being.
  // The parent is replaced by the sibling.
  const parent = state.panes[self.parentId]
  if (parent.type !== 'split') {
    throw new Error(`unknown parent type ${parent.type}`)
  }

  const [siblingId, ...otherSiblingIds] = parent.childrenIds.filter(id => id !== paneIdToClose)
  if (otherSiblingIds.length !== 0) {
    throw new Error(`unable to close pane: too many siblings on pane type ${parent.type}`)
  }

  const sibling = {...state.panes[siblingId]}
  const grandparent = {...state.panes[parent.parentId]}

  // @ts-ignore we are mapping the same pane type 1:1, we know that the tuple size is preserved.
  grandparent.childrenIds = grandparent.childrenIds.map(id => (id === parent.id ? siblingId : id))
  sibling.parentId = grandparent.id

  const updatedPanes = {...state.panes}
  delete updatedPanes[self.id]
  delete updatedPanes[parent.id]

  return {
    ...state,
    panes: {
      ...updatedPanes,
      [grandparent.id]: grandparent,
      [sibling.id]: sibling,
    },
  }
}

const getNewestTabbedPaneId = (state: DeepReadonly<EditorPanes>) => {
  if (!state) {
    throw new Error(`pane not found ${state}`)
  }
  const newestTab = Object.values(state.panes).filter(pane => pane.type === 'tabbed-editor')
    .reduce((acc, pane) => Math.max(acc, pane.id || -1), -1)
  if (newestTab === -1) {
    throw new Error(`no tabbed editor found ${state}`)
  }
  return newestTab
}

const getTabbedPaneCount = (state: DeepReadonly<EditorPanes>) => {
  if (!state) {
    throw new Error(`pane not found ${state}`)
  }
  return Object.values(state.panes).filter(e => e.type === 'tabbed-editor').length
}

const getTabbedPanes = (state: DeepReadonly<EditorPanes>) => (
  Object.values(state.panes).filter(e => e.type === 'tabbed-editor')
)

const getCurrentTabbedPaneId = (
  tabState: EditorTabs,
  paneState: EditorPanes
) => getTopTabInStack(tabState)?.paneId ||
  Object.values(paneState.panes).find(pane => pane.type === 'tabbed-editor').id

const flattenPanes = (
  paneId: number,
  panes: DeepReadonly<Record<number, EditorPane>>,
  elements: number[] = []
) => {
  const pane = panes[paneId]
  if (pane.type === 'split') {
    // TODO(Dale): Update this to handle different split orientations
    flattenPanes(pane.childrenIds[0], panes, elements)
    flattenPanes(pane.childrenIds[1], panes, elements)
  } else {
    elements.push(paneId)
  }
  return elements
}

export {
  handleSplitPane,
  handleClosePane,
  getNewestTabbedPaneId,
  getTabbedPaneCount,
  getTabbedPanes,
  getCurrentTabbedPaneId,
  flattenPanes,
}

export type {
  RootPane,
  TabbedEditorPane,
  SplitPane,
  EditorPane,
  EditorPanes,
}
