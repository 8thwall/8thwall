import {describe, it} from 'mocha'
import {assert} from 'chai'
import type {DeepReadonly} from 'ts-essentials'

import {
  handleTabClose, handleFileDelete, handleFileRename, handleFileSelect, handleReorder,
  handleStateChange, handleTabSelect, EditorTabs, EditorTab, getTopTabInStack, getAdjacentTab,
  getPaneTabCount, canCloseTabsInDirection, closeTabsInPane, closeOtherTabsInPane,
  closeTabsInDirection, getTopTabInPane,
} from '../src/client/editor/tab-state'

let tabId = 1
const tab = (v: Partial<EditorTab>): DeepReadonly<EditorTab> => (
  {id: tabId++, stackIndex: 0, customState: {}, filePath: 'default.txt', paneId: 0, ...v}
)

const state = (tabs: DeepReadonly<EditorTab[]>): EditorTabs => ({
  idCounter: 42,
  stackCounter: 314,
  tabs,
})

describe('Tab State', () => {
  describe('handleFileRename', () => {
    it('Renames tab that matches the file', () => {
      const tabA = tab({filePath: 'file.txt'})
      assert.deepEqual(
        handleFileRename(state([tabA]), 'file.txt', 'file2.txt'),
        state([{...tabA, filePath: 'file2.txt'}])
      )
    })
    it('Renames multiple tabs that match the file', () => {
      const tabA = tab({filePath: 'file.txt'})
      const tabB = tab({filePath: 'file.txt'})
      assert.deepEqual(
        handleFileRename(state([tabA, tabB]), 'file.txt', 'file2.txt'),
        state([{...tabA, filePath: 'file2.txt'}, {...tabB, filePath: 'file2.txt'}])
      )
    })
    it('Renames tabs within a folder that was renamed', () => {
      const tabA = tab({filePath: 'my-folder/file.txt'})
      const tabB = tab({filePath: 'my-folder/subfolder/file.txt'})
      assert.deepEqual(
        handleFileRename(state([tabA, tabB]), 'my-folder', 'new-folder'),
        state([
          {...tabA, filePath: 'new-folder/file.txt'},
          {...tabB, filePath: 'new-folder/subfolder/file.txt'},
        ])
      )
    })
    it('Leaves unrelated tabs unchanged', () => {
      const tabA = tab({filePath: 'my-folder/file.txt'})
      const tabB = tab({filePath: 'other-folder/my-folder/subfolder/file.txt'})
      assert.deepEqual(
        handleFileRename(state([tabA, tabB]), 'my-folder', 'new-folder'),
        state([
          {...tabA, filePath: 'new-folder/file.txt'},
          tabB,
        ])
      )
    })
    it('Leaves tabs unchanged that have the same filename in a different repo', () => {
      const tabA = tab({filePath: 'file.txt', repoId: 'something'})
      assert.deepEqual(
        handleFileRename(state([tabA]), 'my-folder', 'new-folder'),
        state([tabA])
      )
    })
    it('Leaves tabs unchanged within a folder in a different repo', () => {
      const tabA = tab({filePath: 'my-folder/file.txt'})
      assert.deepEqual(
        handleFileRename(state([tabA]), {filePath: 'my-folder', repoId: 'something'}, 'new-folder'),
        state([tabA])
      )
    })
    it('Supports moving files to a sub-repo', () => {
      const tabA = tab({filePath: 'my-folder/file.txt'})
      const prevLocation = {filePath: 'my-folder'}
      const newLocation = {filePath: 'new-folder', repoId: 'something'}
      assert.deepEqual(
        handleFileRename(state([tabA]), prevLocation, newLocation),
        state([{...tabA, filePath: 'new-folder/file.txt', repoId: 'something'}])
      )
    })
    it('Supports moving folders to a sub-repo', () => {
      const tabA = tab({filePath: 'my-folder/file.txt'})
      const prevLocation = {filePath: 'my-folder'}
      const newLocation = {filePath: 'new-folder', repoId: 'something'}
      assert.deepEqual(
        handleFileRename(state([tabA]), prevLocation, newLocation),
        state([{...tabA, filePath: 'new-folder/file.txt', repoId: 'something'}])
      )
    })
    it('Supports moving files between sub-repos', () => {
      const tabA = tab({filePath: 'my-folder/file.txt', repoId: 'repo-1'})
      const prevLocation = {filePath: 'my-folder', repoId: 'repo-1'}
      const newLocation = {filePath: 'new-folder', repoId: 'repo-2'}
      assert.deepEqual(
        handleFileRename(state([tabA]), prevLocation, newLocation),
        state([{...tabA, filePath: 'new-folder/file.txt', repoId: 'repo-2'}])
      )
    })
    it('Supports moving files out of a sub-repo', () => {
      const tabA = tab({filePath: 'my-folder/file.txt', repoId: 'repo-1'})
      const prevLocation = {filePath: 'my-folder', repoId: 'repo-1'}
      const newLocation = {filePath: 'new-folder', repoId: null}

      const expectedNextTab = {...tabA, filePath: 'new-folder/file.txt'}
      delete expectedNextTab.repoId
      assert.deepEqual(
        handleFileRename(state([tabA]), prevLocation, newLocation),
        state([expectedNextTab])
      )
    })
  })
  describe('handleFileDelete', () => {
    it('Closes tab that matches the file', () => {
      assert.deepEqual(
        handleFileDelete(state([tab({filePath: 'file.txt'})]), 'file.txt'),
        state([])
      )
    })
    it('Closes multiple tabs that match the file', () => {
      const before = state([tab({filePath: 'file.txt'}), tab({filePath: 'file.txt'})])
      assert.deepEqual(
        handleFileDelete(before, 'file.txt'),
        state([])
      )
    })
    it('Closes multiple tabs that are enclosed in a deleted folder', () => {
      const before = state([
        tab({filePath: 'my/folder/file.txt'}),
        tab({filePath: 'my/folder/other/file.txt'}),
      ])
      assert.deepEqual(
        handleFileDelete(before, 'my/folder'),
        state([])
      )
    })
    it('Leaves unrelated tabs open', () => {
      const tabA = tab({filePath: 'not/my/folder/other/file.txt'})
      const tabB = tab({filePath: 'my/folder/other/file.txt'})
      const tabC = tab({filePath: 'my/file.txt'})
      assert.deepEqual(
        handleFileDelete(state([tabA, tabB, tabC]), 'my/folder'),
        state([tabA, tabC])
      )
    })
    it('Leaves tabs open that have the same filename in a different repo', () => {
      const tabA = tab({filePath: 'file.txt', repoId: 'my-repo'})
      const tabB = tab({filePath: 'file.txt', repoId: null})
      const tabC = tab({filePath: 'file.txt', repoId: ''})
      const tabD = tab({filePath: 'file.txt'})

      assert.deepEqual(
        handleFileDelete(state([tabA, tabB, tabC, tabD]), 'file.txt'),
        state([tabA])
      )
    })
    it('Closes primary repo file with any falsy repoId', () => {
      const tabA = tab({filePath: 'file.txt', repoId: 'my-repo'})
      const tabB = tab({filePath: 'file.txt', repoId: null})
      const tabC = tab({filePath: 'file.txt', repoId: ''})
      const tabD = tab({filePath: 'file.txt'})

      const deletedFile = {filePath: 'file.txt'}

      assert.deepEqual(
        handleFileDelete(state([tabA, tabB, tabC, tabD]), deletedFile),
        state([tabA])
      )
    })
  })
  describe('handleFileSelect', () => {
    it('Opens a tab as ephemeral for the first time', () => {
      const tabA = tab({filePath: 'other-file.txt'})
      const before = state([tabA])
      const after = handleFileSelect(before, 'file.txt', 0)
      assert.deepEqual(after.tabs, [tabA, {
        filePath: 'file.txt',
        repoId: null,
        ephemeral: true,
        stackIndex: before.stackCounter,
        id: before.idCounter,
        customState: {},
        paneId: 0,
      }])
      assert.equal(before.stackCounter + 1, after.stackCounter)
      assert.equal(before.idCounter + 1, after.idCounter)
    })
    it('Marks a file as non-ephemeral if already open', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
        stackIndex: 3,
        customState: {preview: true},
      })
      const before = state([tabA])
      const after = handleFileSelect(before, 'file.txt', 0)
      assert.deepEqual(after.tabs, [{
        filePath: 'file.txt',
        ephemeral: false,
        stackIndex: before.stackCounter,
        id: tabA.id,
        customState: {preview: true},
        paneId: 0,
      }])
      assert.equal(before.stackCounter + 1, after.stackCounter)
      assert.equal(before.idCounter, after.idCounter)
    })
    it('Leaves a file as ephemeral if not already primary', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
        stackIndex: 3,
        customState: {preview: true},
      })
      const tabB = tab({
        filePath: 'file.txt',
        ephemeral: false,
        stackIndex: 5,  // Higher stackIndex means this file was more recently opened
        customState: {preview: true},
      })
      const before = state([tabA, tabB])
      const after = handleFileSelect(before, 'file.txt', 0)
      assert.deepEqual(after.tabs, [{
        ...tabA,
        stackIndex: before.stackCounter,
      }, tabB])
      assert.equal(before.stackCounter + 1, after.stackCounter)
      assert.equal(before.idCounter, after.idCounter)
    })
    it('Replaces the ephemeral tab instead of opening a new file', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
        customState: {preview: true},
      })
      const before = state([tabA])
      const after = handleFileSelect(before, 'other-file.txt', 0)
      assert.deepEqual(after.tabs, [{
        filePath: 'other-file.txt',
        repoId: null,
        ephemeral: true,
        stackIndex: before.stackCounter,
        id: before.idCounter,
        customState: {},
        paneId: 0,
      }])
      assert.equal(before.stackCounter + 1, after.stackCounter)
      assert.equal(before.idCounter + 1, after.idCounter)
    })
    it('Updates stackIndex when switching files', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: false,
        customState: {preview: true},
      })
      const tabB = tab({
        filePath: 'file2.txt',
        ephemeral: false,
        customState: {preview: true},
      })
      const before = state([tabA, tabB])
      const after = handleFileSelect(before, 'file2.txt', 0)
      assert.deepEqual(after.tabs, [tabA, {
        ...tabB,
        stackIndex: before.stackCounter,
      }])
      assert.equal(before.stackCounter + 1, after.stackCounter)
      assert.equal(before.idCounter, after.idCounter)
    })
    it('Leaves the ephemeral path if the file is already open', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
      })
      const tabB = tab({
        filePath: 'file2.txt',
        ephemeral: false,
      })
      const before = state([tabA, tabB])
      const after = handleFileSelect(before, 'file2.txt', 0)
      assert.deepEqual(after.tabs, [tabA, {
        ...tabB,
        stackIndex: before.stackCounter,
      }])
      assert.equal(before.stackCounter + 1, after.stackCounter)
      assert.equal(before.idCounter, after.idCounter)
    })
    it('Closes the "new tab" tab when switching to a file that is already open', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: false,
      })
      const tabB = tab({
        filePath: '',
        ephemeral: true,
      })
      const before = state([tabA, tabB])
      const after = handleFileSelect(before, 'file.txt', 0)
      assert.deepEqual(after.tabs, [{
        ...tabA,
        stackIndex: before.stackCounter,
      }])
    })
    it('Closes the "new tab" tab when opening a new file', () => {
      const tabA = tab({
        filePath: '',
        ephemeral: true,
      })
      const before = state([tabA])
      const after = handleFileSelect(before, 'other-file.txt', 0)
      assert.deepEqual(after.tabs, [{
        filePath: 'other-file.txt',
        repoId: null,
        id: before.idCounter,
        customState: {},
        ephemeral: true,
        stackIndex: before.stackCounter,
        paneId: 0,
      }])
    })
    it('Keeps the "new tab" tab as ephemeral', () => {
      const tabA = tab({
        filePath: '',
        ephemeral: true,
      })
      const before = state([tabA])
      const after = handleFileSelect(before, '', 0)
      assert.deepEqual(after.tabs, [{
        ...tabA,
        stackIndex: before.stackCounter,
      }])
    })
    it('Makes ephemeral tab persistent when opening the "new tab" tab', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
      })
      const before = state([tabA])
      const after = handleFileSelect(before, '', 0)

      const expectedA = {...tabA}
      delete expectedA.ephemeral

      assert.deepEqual(after.tabs, [
        expectedA,
        {
          filePath: '',
          id: before.idCounter,
          customState: {},
          ephemeral: true,
          repoId: null,
          stackIndex: before.stackCounter,
          paneId: 0,
        }])
    })
    it('Always opens the "new tab" tab at the end of the list', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
      })
      const tabB = tab({
        filePath: 'file2.txt',
        ephemeral: false,
      })
      const before = state([tabA, tabB])
      const after = handleFileSelect(before, '', 0)

      const expectedA = {...tabA}
      delete expectedA.ephemeral

      assert.deepEqual(after.tabs, [expectedA, tabB, {
        filePath: '',
        repoId: null,
        id: before.idCounter,
        customState: {},
        ephemeral: true,
        stackIndex: before.stackCounter,
        paneId: 0,
      }])
    })
  })

  describe('handleTabSelect', () => {
    it('Updates stackIndex when selecting a tab', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
        customState: {preview: true, mode: 'example'},
      })
      const before = state([tabA])
      const after = handleTabSelect(before, tabA.id)
      assert.deepEqual(after.tabs, [{
        ...tabA,
        stackIndex: before.stackCounter,
      }])
      assert.equal(before.stackCounter + 1, after.stackCounter)
      assert.equal(before.idCounter, after.idCounter)
    })
    it('Closes the "new tab" tab when selecting a different tab', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: false,
      })
      const tabB = tab({
        filePath: '',
        ephemeral: true,
      })
      const before = state([tabA, tabB])
      const after = handleTabSelect(before, tabA.id)
      assert.deepEqual(after.tabs, [{
        ...tabA,
        stackIndex: before.stackCounter,
      }])
    })
    it('Can select the "new tab" tab', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: false,
      })
      const tabB = tab({
        filePath: '',
        ephemeral: true,
      })
      const before = state([tabA, tabB])
      const after = handleTabSelect(before, tabB.id)
      assert.deepEqual(after.tabs, [tabA, {
        ...tabB,
        stackIndex: before.stackCounter,
      }])
    })
  })

  describe('handleReorder', () => {
    const tabA = tab({filePath: 'file1.txt'})
    const tabB = tab({filePath: 'file2.txt'})
    const tabC = tab({filePath: 'file3.txt'})
    const before = state([tabA, tabB, tabC])

    it('Updates stack index when moving a tab', () => {
      const after = handleReorder(before, 2, 1, 0, 0)
      assert.equal(before.stackCounter + 1, after.stackCounter)
      assert.equal(before.idCounter, after.idCounter)
    })
    it('Can move a tab to the selected index', () => {
      const after = handleReorder(before, 2, 1, 0, 0)
      const afterTabC = {...tabC, stackIndex: before.stackCounter}
      assert.deepEqual(after.tabs, [tabA, afterTabC, tabB])
    })
    it('Can move a tab to the front', () => {
      const after = handleReorder(before, 2, 0, 0, 0)
      const afterTabC = {...tabC, stackIndex: before.stackCounter}
      assert.deepEqual(after.tabs, [afterTabC, tabA, tabB])
    })
    it('Can move a tab to the end', () => {
      const after = handleReorder(before, 0, 2, 0, 0)
      const afterTabA = {...tabA, stackIndex: before.stackCounter}
      assert.deepEqual(after.tabs, [tabB, tabC, afterTabA])
    })
    it('Can move a tab in a pane', () => {
      const tabD = tab({filePath: 'file4.txt', paneId: 1})
      const tabE = tab({filePath: 'file5.txt', paneId: 1})
      const tabF = tab({filePath: 'file6.txt', paneId: 2})
      const tabG = tab({filePath: 'file7.txt', paneId: 1})
      const tabState = state([tabA, tabB, tabC, tabD, tabE, tabF, tabG])
      const after = handleReorder(tabState, 0, 2, 1, 1)
      const afterTabD = {...tabD, stackIndex: tabState.stackCounter}
      assert.deepEqual(after.tabs, [tabA, tabB, tabC, tabF, tabE, tabG, afterTabD])
    })
    it('Can move a tab across panes', () => {
      const tabD = tab({filePath: 'file4.txt', paneId: 1})
      const tabE = tab({filePath: 'file5.txt', paneId: 1})
      const tabF = tab({filePath: 'file6.txt', paneId: 2})
      const tabG = tab({filePath: 'file7.txt', paneId: 2})
      const tabState = state([tabA, tabB, tabC, tabD, tabE, tabF, tabG])
      const after = handleReorder(tabState, 0, 1, 2, 1)
      const afterTabF = {...tabF, stackIndex: tabState.stackCounter, paneId: 1}
      assert.deepEqual(after.tabs, [tabA, tabB, tabC, tabG, tabD, tabE, afterTabF])
    })
    it('Changes moved ephemeral tab to persistent', () => {
      const ephemeralTab = tab({filePath: 'file4.txt', ephemeral: true})
      const tabState = state([tabA, tabB, tabC, ephemeralTab])
      const after = handleReorder(tabState, 3, 1, 0, 0)
      const persistentTab = {...ephemeralTab, stackIndex: tabState.stackCounter}
      delete persistentTab.ephemeral
      assert.deepEqual(after.tabs, [tabA, persistentTab, tabB, tabC])
    })
    it('Replaces tab in target pane if file is already open', () => {
      const tabD = tab({filePath: 'file4.txt', paneId: 1})
      const tabE = tab({filePath: 'file5.txt', paneId: 1})
      const tabF = tab({filePath: 'file4.txt', paneId: 2})
      const tabG = tab({filePath: 'file7.txt', paneId: 2})
      const tabState = state([tabA, tabB, tabC, tabD, tabE, tabF, tabG])
      const after = handleReorder(tabState, 0, 1, 1, 2)
      const afterTabD = {...tabD, stackIndex: tabState.stackCounter, paneId: 2}
      assert.deepEqual(after.tabs, [tabA, tabB, tabC, tabE, afterTabD, tabG])
    })
    it('Moves tab with existing filename but different repoId to target pane', () => {
      const tabD = tab({filePath: 'file4.txt', paneId: 1})
      const tabE = tab({filePath: 'file5.txt', paneId: 1})
      const tabF = tab({filePath: 'file4.txt', paneId: 2, repoId: 'other'})
      const tabG = tab({filePath: 'file7.txt', paneId: 2})
      const tabState = state([tabA, tabB, tabC, tabD, tabE, tabF, tabG])
      const after = handleReorder(tabState, 0, 1, 2, 1)
      const afterTabF = {...tabF, stackIndex: tabState.stackCounter, paneId: 1}
      assert.deepEqual(after.tabs, [tabA, tabB, tabC, tabG, tabD, tabE, afterTabF])
    })
  })
  describe('handleTabClose', () => {
    it('Closes a tab', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
        stackIndex: 3,
        customState: {preview: true},
      })
      const tabB = tab({filePath: 'other-file.txt'})
      const before = state([tabA, tabB])
      assert.deepEqual(
        handleTabClose(before, tabA.id),
        handleTabSelect(state([tabB]), tabB.id)
      )
    })
    it('Keeps other tabs of the same file open', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
        stackIndex: 3,
        customState: {preview: true},
      })
      const tabB = tab({
        filePath: 'file.txt',
        ephemeral: true,
        stackIndex: 4,
        customState: {preview: true},
      })
      const before = state([tabA, tabB])
      assert.deepEqual(
        handleTabClose(before, tabA.id),
        handleTabSelect(state([tabB]), tabB.id)
      )
    })
    it('Selects next top tab in same pane if it exists', () => {
      const tabA = tab({filePath: 'file1.txt', stackIndex: 1})
      const tabB = tab({filePath: 'file2.txt', stackIndex: 2})
      const tabC = tab({filePath: 'file3.txt', stackIndex: 3, paneId: 1})
      const tabD = tab({filePath: 'file4.txt', stackIndex: 9, paneId: 1})
      const tabE = tab({filePath: 'file5.txt', stackIndex: 8, paneId: 2})
      const tabF = tab({filePath: 'file6.txt', stackIndex: 10, paneId: 2})
      const tabG = tab({filePath: 'file7.txt', stackIndex: 6, paneId: 2})
      const tabH = tab({filePath: 'file8.txt', stackIndex: 7, paneId: 2})
      const before = state([tabA, tabB, tabC, tabD, tabE, tabF, tabG, tabH])
      const after = handleTabClose(before, tabF.id)
      const afterTabE = {...tabE, stackIndex: before.stackCounter}
      assert.equal(before.stackCounter + 1, after.stackCounter)
      assert.deepEqual(after.tabs, [tabA, tabB, tabC, tabD, afterTabE, tabG, tabH])
    })
    it('Keeps tabs in other panes the same if last tab in a pane closes', () => {
      const tabA = tab({filePath: 'file1.txt', stackIndex: 1})
      const tabB = tab({filePath: 'file2.txt', stackIndex: 3})
      const tabC = tab({filePath: 'file3.txt', stackIndex: 2, paneId: 1})
      const tabD = tab({filePath: 'file4.txt', stackIndex: 4, paneId: 1})
      const tabE = tab({filePath: 'file5.txt', stackIndex: 5, paneId: 2})
      const before = state([tabA, tabB, tabC, tabD, tabE])
      const after = handleTabClose(before, tabE.id)
      assert.deepEqual(after, state([tabA, tabB, tabC, tabD]))
    })
  })
  describe('handleStateChange', () => {
    it('Updates custom state of a tab', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
        stackIndex: 3,
        customState: {preview: true, mode: 'example'},
      })
      const before = state([tabA])
      assert.deepEqual(
        handleStateChange(before, tabA.id, {preview: false}),
        state([{
          ...tabA,
          customState: {preview: false, mode: 'example'},
        }])
      )
    })
    it('Leaves state of other tabs with the same name unchanged', () => {
      const tabA = tab({
        filePath: 'file.txt',
        ephemeral: true,
        stackIndex: 3,
        customState: {preview: true, mode: 'example'},
      })
      const tabB = tab({
        filePath: 'file.txt',
        ephemeral: true,
        stackIndex: 3,
        customState: {preview: true, mode: 'example'},
      })
      const before = state([tabA, tabB])
      assert.deepEqual(
        handleStateChange(before, tabA.id, {preview: false}),
        state([{
          ...tabA,
          customState: {preview: false, mode: 'example'},
        }, tabB])
      )
    })
  })
  describe('getTopTabInStack', () => {
    it('Returns the most recently used tab', () => {
      const tabA = tab({filePath: 'file1.txt', stackIndex: 3})
      const tabB = tab({filePath: 'file2.txt', stackIndex: 4})
      const tabC = tab({filePath: 'file3.txt', stackIndex: 2})
      const tabD = tab({filePath: 'file4.txt', stackIndex: 1})
      assert.deepEqual(getTopTabInStack(state([tabA, tabB, tabC, tabD])), tabB)
    })
    it('Returns null for empty tabs', () => {
      assert.strictEqual(getTopTabInStack(state([])), null)
    })
  })
  describe('getTopTabInPane', () => {
    it('Returns the most recently used tab in pane', () => {
      const tabA = tab({filePath: 'file1.txt', id: 1, stackIndex: 1, paneId: 1})
      const tabB = tab({filePath: 'file2.txt', id: 2, stackIndex: 2, paneId: 1})
      const tabC = tab({filePath: 'file3.txt', id: 3, stackIndex: 3, paneId: 1})
      const tabD = tab({filePath: 'file4.txt', id: 4, stackIndex: 4, paneId: 1})
      const tabE = tab({filePath: 'file5.txt', id: 5, stackIndex: 5, paneId: 2})
      assert.deepEqual(getTopTabInPane(state([tabA, tabB, tabC, tabD, tabE]), 1), tabD)
    })
    it('Returns null for empty tabs', () => {
      assert.strictEqual(getTopTabInStack(state([])), null)
    })
  })
  describe('getAdjacentTab', () => {
    const tabA = tab({filePath: 'file1.txt', id: 1, stackIndex: 1, paneId: 1})
    const tabB = tab({filePath: 'file2.txt', id: 2, stackIndex: 2, paneId: 1})
    const tabC = tab({filePath: 'file3.txt', id: 3, stackIndex: 3, paneId: 1})
    const tabD = tab({filePath: 'file4.txt', id: 4, stackIndex: 4, paneId: 1})
    const tabE = tab({filePath: 'file5.txt', id: 5, stackIndex: 5, paneId: 2})
    it('Can switch to a right tab', () => {
      assert.deepEqual(getAdjacentTab(
        state([tabA, tabB, tabC, tabD]), tabC.id, tabC.paneId, 'right'
      ), tabD)
    })
    it('Can switch to a left tab', () => {
      assert.deepEqual(getAdjacentTab(
        state([tabA, tabB, tabC, tabD]), tabC.id, tabC.paneId, 'left'
      ), tabB)
    })
    it('When attempting to get right tab from rightmost tab, wrap to first tab', () => {
      assert.deepEqual(getAdjacentTab(
        state([tabA, tabB, tabC, tabD]), tabD.id, tabD.paneId, 'right'
      ), tabA)
    })
    it('When attempting to get right tab from rightmost tab, wrap to last tab', () => {
      assert.deepEqual(getAdjacentTab(
        state([tabA, tabB, tabC, tabD]), tabA.id, tabA.paneId, 'left'
      ), tabD)
    })
    it('Skip tabs not in pane', () => {
      assert.deepEqual(getAdjacentTab(
        state([tabA, tabB, tabE, tabC, tabD]), tabB.id, tabB.paneId, 'right'
      ), tabC)
    })
  })
  describe('getAdjacentTab', () => {
    const tabA = tab({filePath: 'file1.txt', id: 1, stackIndex: 1, paneId: 1})
    const tabB = tab({filePath: 'file2.txt', id: 2, stackIndex: 2, paneId: 1})
    const tabC = tab({filePath: 'file3.txt', id: 3, stackIndex: 3, paneId: 1})
    const tabD = tab({filePath: 'file4.txt', id: 4, stackIndex: 4, paneId: 1})
    const tabE = tab({filePath: 'file5.txt', id: 5, stackIndex: 5, paneId: 2})
    it('Can switch to a right tab', () => {
      assert.deepEqual(getAdjacentTab(
        state([tabA, tabB, tabC, tabD]), tabC.id, tabC.paneId, 'right'
      ), tabD)
    })
  })
  describe('getPaneTabCount', () => {
    const tabA = tab({filePath: 'file1.txt', id: 1, stackIndex: 1, paneId: 1})
    const tabB = tab({filePath: 'file2.txt', id: 2, stackIndex: 2, paneId: 1})
    const tabC = tab({filePath: 'file3.txt', id: 3, stackIndex: 3, paneId: 1})
    const tabD = tab({filePath: 'file4.txt', id: 4, stackIndex: 4, paneId: 1})
    const tabE = tab({filePath: 'file5.txt', id: 5, stackIndex: 5, paneId: 2})
    it('Gets the right tab count of pane', () => {
      assert.deepEqual(getPaneTabCount(
        state([tabA, tabB, tabC, tabD, tabE]), tabC.paneId
      ), 4)
    })
    it('Returns 0 for invalid pane id', () => {
      assert.deepEqual(getPaneTabCount(
        state([tabA, tabB, tabC, tabD, tabE]), 10
      ), 0)
    })
  })
  describe('canCloseTabsInDirection', () => {
    const tabA = tab({filePath: 'file1.txt', id: 1, stackIndex: 1, paneId: 1})
    const tabB = tab({filePath: 'file2.txt', id: 2, stackIndex: 2, paneId: 1})
    const tabC = tab({filePath: 'file3.txt', id: 3, stackIndex: 3, paneId: 1})
    const tabD = tab({filePath: 'file4.txt', id: 4, stackIndex: 4, paneId: 1})
    const tabE = tab({filePath: 'file5.txt', id: 5, stackIndex: 5, paneId: 2})
    it('Returns true when not on left or rightmost tab', () => {
      assert.deepEqual(canCloseTabsInDirection(
        state([tabA, tabB, tabC, tabD, tabE]), tabC.id, tabC.paneId, 'right'
      ), true)
    })
    it('Returns false when on leftmost tab and attempting to go left', () => {
      assert.deepEqual(canCloseTabsInDirection(
        state([tabA, tabB, tabC, tabD, tabE]), tabA.id, tabA.paneId, 'left'
      ), false)
    })
    it('Returns false when on rightmost tab and attempting to go right', () => {
      assert.deepEqual(canCloseTabsInDirection(
        state([tabA, tabB, tabC, tabD, tabE]), tabD.id, tabD.paneId, 'right'
      ), false)
    })
  })
  describe('closeTabsInPane', () => {
    const tabA = tab({filePath: 'file1.txt', id: 1, stackIndex: 1, paneId: 1})
    const tabB = tab({filePath: 'file2.txt', id: 2, stackIndex: 2, paneId: 1})
    const tabC = tab({filePath: 'file3.txt', id: 3, stackIndex: 3, paneId: 1})
    const tabD = tab({filePath: 'file4.txt', id: 4, stackIndex: 4, paneId: 1})
    const tabE = tab({filePath: 'file5.txt', id: 5, stackIndex: 5, paneId: 2})
    it('Closes all tabs in pane', () => {
      assert.deepEqual(closeTabsInPane(
        state([tabA, tabB, tabC, tabD, tabE]), tabC.paneId
      ), state([tabE]))
    })
    it('Does not close tabs for invalid pane id', () => {
      assert.deepEqual(closeTabsInPane(
        state([tabA, tabB, tabC, tabD, tabE]), 10
      ), state([tabA, tabB, tabC, tabD, tabE]))
    })
  })
  describe('closeOtherTabsInPane', () => {
    const tabA = tab({filePath: 'file1.txt', id: 1, stackIndex: 1, paneId: 1})
    const tabB = tab({filePath: 'file2.txt', id: 2, stackIndex: 2, paneId: 1})
    const tabC = tab({filePath: 'file3.txt', id: 3, stackIndex: 3, paneId: 1})
    const tabD = tab({filePath: 'file4.txt', id: 4, stackIndex: 4, paneId: 1})
    const tabE = tab({filePath: 'file5.txt', id: 5, stackIndex: 5, paneId: 2})
    it('Closes all tabs in pane except selected', () => {
      assert.deepEqual(closeOtherTabsInPane(
        state([tabA, tabB, tabC, tabD, tabE]), tabC.id, tabC.paneId
      ), state([tabC, tabE]))
    })
    it('Does not close any tabs for invalid pane id', () => {
      assert.deepEqual(closeOtherTabsInPane(
        state([tabA, tabB, tabC, tabD, tabE]), tabC.id, 10
      ), state([tabA, tabB, tabC, tabD, tabE]))
    })
  })
  describe('closeTabsInDirection', () => {
    const tabA = tab({filePath: 'file1.txt', id: 1, stackIndex: 1, paneId: 1})
    const tabB = tab({filePath: 'file2.txt', id: 2, stackIndex: 2, paneId: 1})
    const tabC = tab({filePath: 'file3.txt', id: 3, stackIndex: 3, paneId: 1})
    const tabD = tab({filePath: 'file4.txt', id: 4, stackIndex: 4, paneId: 1})
    const tabE = tab({filePath: 'file5.txt', id: 5, stackIndex: 5, paneId: 2})
    it('Closes all tabs to right', () => {
      assert.deepEqual(closeTabsInDirection(
        state([tabA, tabB, tabC, tabD, tabE]), tabB.id, tabB.paneId, 'right'
      ), state([tabA, tabB, tabE]))
    })
    it('Closes all tabs to left', () => {
      assert.deepEqual(closeTabsInDirection(
        state([tabA, tabB, tabC, tabD, tabE]), tabD.id, tabD.paneId, 'left'
      ), state([tabD, tabE]))
    })
  })
})
