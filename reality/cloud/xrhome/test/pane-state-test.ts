import {describe, it} from 'mocha'
import {expect, assert} from 'chai'
import type {DeepReadonly, MarkRequired} from 'ts-essentials'

import {
  handleSplitPane, handleClosePane, getNewestTabbedPaneId,
  getTabbedPaneCount, getCurrentTabbedPaneId, getTabbedPanes, flattenPanes,
} from '../src/client/editor/pane-state'
import type {
  EditorPanes, EditorPane, RootPane, SplitPane, TabbedEditorPane,
} from '../src/client/editor/pane-state'
import type {EditorTab, EditorTabs} from '../src/client/editor/tab-state'

let tabId = 1
const tab = (v: Partial<EditorTab>): DeepReadonly<EditorTab> => (
  {id: tabId++, stackIndex: 0, customState: {}, filePath: 'default.txt', paneId: undefined, ...v}
)

const tabState = (tabs: DeepReadonly<EditorTab[]>): EditorTabs => ({
  idCounter: 42,
  stackCounter: 314,
  tabs,
})

const reducePanes = (panes: EditorPane[]): Record<number, EditorPane> => (
  panes.reduce((acc, val) => {
    acc[val.id] = val
    return acc
  }, {})
)

const state = (panes: EditorPane[]): EditorPanes => ({
  idCounter: 42,
  rootId: 0,
  panes: reducePanes(panes),
})

const rootPane = (v: MarkRequired<Partial<RootPane>, 'childrenIds'>): EditorPane => ({
  type: 'root',
  id: 0,
  parentId: null,
  ...v,
})

const tabbedEditorPane = (
  v: MarkRequired<Partial<TabbedEditorPane>, 'id' | 'parentId'>
): EditorPane => ({
  type: 'tabbed-editor',
  ...v,
})

const splitPane = (
  v: MarkRequired<Partial<SplitPane>, 'id' | 'parentId' | 'childrenIds'>
): EditorPane => ({
  type: 'split',
  direction: 'vertical',
  ratio: 0.5,
  ...v,
})

describe('Pane State', () => {
  describe('handleSplitPane', () => {
    it('splits pane when parent is root', () => {
      const before = state([
        rootPane({childrenIds: [1]}),
        tabbedEditorPane({id: 1, parentId: 0}),
      ])
      const after = handleSplitPane(before, 1)
      expect(after.panes).to.deep.equal(reducePanes([
        rootPane({childrenIds: [42]}),
        splitPane({id: 42, parentId: 0, childrenIds: [1, 43]}),
        tabbedEditorPane({id: 1, parentId: 42}),
        tabbedEditorPane({id: 43, parentId: 42}),
      ]))
    })
    it('splits pane when parent is another split', () => {
      const before = state([
        rootPane({childrenIds: [1]}),
        splitPane({id: 5, parentId: 0, childrenIds: [10, 20]}),
        tabbedEditorPane({id: 10, parentId: 5}),
        tabbedEditorPane({id: 20, parentId: 5}),
      ])
      const after = handleSplitPane(before, 20)
      expect(after.panes).to.deep.equal(reducePanes([
        rootPane({childrenIds: [1]}),
        tabbedEditorPane({id: 10, parentId: 5}),
        // update order: oldParent, newParent, self, newSibling
        splitPane({id: 5, parentId: 0, childrenIds: [10, 42]}),
        splitPane({id: 42, parentId: 5, childrenIds: [20, 43]}),
        tabbedEditorPane({id: 20, parentId: 42}),
        tabbedEditorPane({id: 43, parentId: 42}),
      ]))
    })
  })
  describe('handleClosePane', () => {
    it('closes pane when grandparent is root', () => {
      const before = state([
        rootPane({childrenIds: [1]}),
        splitPane({id: 1, parentId: 0, childrenIds: [2, 3]}),
        tabbedEditorPane({id: 2, parentId: 1}),
        tabbedEditorPane({id: 3, parentId: 1}),
      ])
      const afterClose2 = handleClosePane(before, 2)
      const afterClose3 = handleClosePane(before, 3)
      expect(afterClose2.panes).to.deep.equal(reducePanes([
        rootPane({childrenIds: [3]}),
        tabbedEditorPane({id: 3, parentId: 0}),
      ]))
      expect(afterClose3.panes).to.deep.equal(reducePanes([
        rootPane({childrenIds: [2]}),
        tabbedEditorPane({id: 2, parentId: 0}),
      ]))
    })
    it('closes when grandparent is another split pane', () => {
      const before = state([
        rootPane({childrenIds: [1]}),
        splitPane({id: 1, parentId: 0, childrenIds: [10, 20]}),  // grandparent
        tabbedEditorPane({id: 10, parentId: 1}),

        splitPane({id: 20, parentId: 1, childrenIds: [50, 60]}),  // parent
        tabbedEditorPane({id: 50, parentId: 20}),  // close this guy

        splitPane({id: 60, parentId: 20, childrenIds: [600, 700]}),  // sibling
        tabbedEditorPane({id: 600, parentId: 60}),
        tabbedEditorPane({id: 700, parentId: 60}),
      ])
      const afterClose50 = handleClosePane(before, 50)
      expect(afterClose50.panes).to.deep.equal(reducePanes([
        rootPane({childrenIds: [1]}),
        tabbedEditorPane({id: 10, parentId: 1}),
        tabbedEditorPane({id: 600, parentId: 60}),
        tabbedEditorPane({id: 700, parentId: 60}),
        splitPane({id: 1, parentId: 0, childrenIds: [10, 60]}),  // grandparent
        splitPane({id: 60, parentId: 1, childrenIds: [600, 700]}),  // sibling
      ]))
    })
  })

  describe('getNewestTabbedPaneId', () => {
    it('returns newest tabbed pane id with 1 editor', () => {
      const testState = state([
        rootPane({childrenIds: [1]}),
        tabbedEditorPane({id: 10, parentId: 0}),
      ])
      assert.deepEqual(getNewestTabbedPaneId(testState), 10)
    })
    it('returns newest tabbed pane id with 2 editors', () => {
      const testState = state([
        rootPane({childrenIds: [1]}),
        splitPane({id: 5, parentId: 0, childrenIds: [10, 20]}),
        tabbedEditorPane({id: 10, parentId: 5}),
        tabbedEditorPane({id: 20, parentId: 5}),
      ])
      assert.deepEqual(getNewestTabbedPaneId(testState), 20)
    })
  })

  describe('getTabbedPaneCount', () => {
    it('returns number of tabbed editors with 1 editor', () => {
      const testState = state([
        rootPane({childrenIds: [1]}),
        tabbedEditorPane({id: 10, parentId: 0}),
      ])
      assert.deepEqual(getTabbedPaneCount(testState), 1)
    })
    it('returns number of tabbed editors with 2 editors', () => {
      const testState = state([
        rootPane({childrenIds: [1]}),
        splitPane({id: 5, parentId: 0, childrenIds: [10, 20]}),
        tabbedEditorPane({id: 10, parentId: 5}),
        tabbedEditorPane({id: 20, parentId: 5}),
      ])
      assert.deepEqual(getTabbedPaneCount(testState), 2)
    })
  })

  describe('getTabbedPanes', () => {
    it('returns 1 tabbed editor with 1 editor', () => {
      const testState = state([
        rootPane({childrenIds: [1]}),
        tabbedEditorPane({id: 10, parentId: 0}),
      ])
      assert.deepEqual(getTabbedPanes(testState), [tabbedEditorPane({id: 10, parentId: 0})])
    })
    it('returns 2 tabbed editors with 2 editors', () => {
      const testState = state([
        rootPane({childrenIds: [1]}),
        splitPane({id: 5, parentId: 0, childrenIds: [10, 20]}),
        tabbedEditorPane({id: 10, parentId: 5}),
        tabbedEditorPane({id: 20, parentId: 5}),
      ])
      assert.deepEqual(getTabbedPanes(testState),
        [
          tabbedEditorPane({id: 10, parentId: 5}),
          tabbedEditorPane({id: 20, parentId: 5}),
        ])
    })
  })

  describe('getCurrentTabbedPaneId', () => {
    it('returns a paneId if there are no tabs in the tabState', () => {
      const testTabState = tabState([])
      const testPaneState = state([
        rootPane({childrenIds: [1]}),
        tabbedEditorPane({id: 10, parentId: 0}),
      ])
      assert.deepEqual(getCurrentTabbedPaneId(testTabState, testPaneState), 10)
    })
    it('returns the paneId associated with the tab with the highest stackIndex', () => {
      const testTabState = tabState([
        tab({stackIndex: 1, filePath: 'a.txt', paneId: 20}),
        tab({stackIndex: 10, filePath: 'a.txt', paneId: 20}),
        tab({stackIndex: 100, filePath: 'a.txt', paneId: 10}),
      ])
      const testPaneState = state([
        rootPane({childrenIds: [1]}),
        splitPane({id: 5, parentId: 0, childrenIds: [10, 20]}),
        tabbedEditorPane({id: 10, parentId: 5}),
        tabbedEditorPane({id: 20, parentId: 5}),
      ])
      assert.deepEqual(getCurrentTabbedPaneId(testTabState, testPaneState), 10)
    })
  })
  describe('getCurrentTabbedPaneId', () => {
    it('returns a paneId if there are no tabs in the tabState', () => {
      const testTabState = tabState([])
      const testPaneState = state([
        rootPane({childrenIds: [1]}),
        tabbedEditorPane({id: 10, parentId: 0}),
      ])
      assert.deepEqual(getCurrentTabbedPaneId(testTabState, testPaneState), 10)
    })
    it('returns the paneId associated with the tab with the highest stackIndex', () => {
      const testTabState = tabState([
        tab({stackIndex: 1, filePath: 'a.txt', paneId: 20}),
        tab({stackIndex: 10, filePath: 'a.txt', paneId: 20}),
        tab({stackIndex: 100, filePath: 'a.txt', paneId: 10}),
      ])
      const testPaneState = state([
        rootPane({childrenIds: [1]}),
        splitPane({id: 5, parentId: 0, childrenIds: [10, 20]}),
        tabbedEditorPane({id: 10, parentId: 5}),
        tabbedEditorPane({id: 20, parentId: 5}),
      ])
      assert.deepEqual(getCurrentTabbedPaneId(testTabState, testPaneState), 10)
    })
  })
  describe('flattenPanes', () => {
    it('Flattens a pane state with a split', () => {
      const testPaneState = state([
        rootPane({childrenIds: [1]}),
        splitPane({id: 5, parentId: 0, childrenIds: [10, 20]}),
        tabbedEditorPane({id: 10, parentId: 5}),
        tabbedEditorPane({id: 20, parentId: 5}),
      ])
      assert.deepEqual(flattenPanes(5, testPaneState.panes), [10, 20])
    })
    it('Flattens a pane state without a split', () => {
      const testPaneState = state([
        rootPane({childrenIds: [1]}),
        tabbedEditorPane({id: 20, parentId: 5}),
      ])
      assert.deepEqual(flattenPanes(20, testPaneState.panes), [20])
    })
  })
})
