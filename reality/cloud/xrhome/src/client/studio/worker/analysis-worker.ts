import {parseComponentAst} from '../../../shared/ecs/shared/parse-component-ast'

// Mutates the docLines array by the contents of delta, so that the docLines array contains an
// updated version of a source file's contents."
const applyDelta = (docLines, delta) => {
  const {row} = delta.start
  const startColumn = delta.start.column
  const line = docLines[row] || ''
  switch (delta.action) {
    case 'insert':
      const {lines} = delta
      if (lines.length === 1) {
        docLines[row] = line.substring(0, startColumn) + delta.lines[0] + line.substring(startColumn)
      } else {
        const args = [row, 1].concat(delta.lines)
        docLines.splice.apply(docLines, args)
        docLines[row] = line.substring(0, startColumn) + docLines[row]
        docLines[row + delta.lines.length - 1] += line.substring(startColumn)
      }
      break
    case 'remove':
      const endColumn = delta.end.column
      const endRow = delta.end.row
      if (row === endRow) {
        docLines[row] = line.substring(0, startColumn) + line.substring(endColumn)
      } else {
        docLines.splice(
          row, endRow - row + 1,
          line.substring(0, startColumn) + docLines[endRow].substring(endColumn)
        )
      }
      break
    default:
      break
  }
}

// returns a range object which is compatible with some of the methods
// from ace builds document.
const range_ = (startRow, startColumn, endRow, endColumn) => ({
  start: {
    row: startRow,
    column: startColumn,
  },
  end: {
    row: endRow,
    column: endColumn,
  },
})

/**
 * Instantiates a new docuement.
 */
const doc_ = {
  lines: [''],
}

// returns the lines if the doc that correspond to the given range.
const getLinesForRange = (doc, range) => {
  let lines
  if (range.start.row === range.end.row) {
    // Handle a single-line range.
    lines = [(doc.lines[(range.start.row)] || '').substring(range.start.column, range.end.column)]
  } else {
    // Handle a multi-line range.
    lines = doc.lines.slice((range.start.row, range.end.row + 1))
    lines[0] = (lines[0] || '').substring(range.start.column)
    const l = lines.length - 1
    if (range.end.row - range.start.row === l) {
      lines[l] = lines[l].substring(0, range.end.column)
    }
  }
  return lines
}

/**
 * The purpose of this is to make sure there are no out of bounds errors.
 * If the given row and column is not in the bounds of the document it will
 * correct it to something that is.
 * @param {*} row - The row where the change starts.
 * @param {*} column - The column where the change starts.
 */
const clippedPos = (doc, row, column) => {
  const numLines = doc.lines.length
  if (row === undefined) {
    row = numLines
  } else if (row < 0) {
    row = 0
  } else if (row >= numLines) {
    row = numLines - 1
    column = undefined
  }
  const line = doc.lines[row] || ''
  if (column === undefined) {
    column = line.length
  }
  column = Math.min(Math.max(column, 0), line.length)
  return {row, column}
}

const insertMergedLines = (doc, pos, lines) => {
  const start = clippedPos(doc, pos.row, pos.column)
  const end = {
    row: start.row + lines.length - 1,
    column: (lines.length === 1 ? start.column : 0) + lines[lines.length - 1].length,
  }
  applyDelta(doc.lines, {
    start,
    end,
    action: 'insert',
    lines,
  })
}

/**
 * The initial value of the document (the code in the cloud editor) is passed in as a string.
 * The split function takes this single string and breaks it at the corresponding line
 * breaks. This allows the document to store the input in many different lines, rather than
 * just one line.
 * @param {*} text - any string to break into smaller pieces.
 */
// This is an ACE workaround for an IE bug in split, it's unknown what the issue is and
// how this works around it.
// TODO(rishi) figure out what version of split to use.
let split = text => text
if ('aaa'.split(/a/).length === 0) {
  split = text => text.replace(/\r\n|\r/g, '\n').split('\n')
} else {
  split = text => text.split(/\r\n|\r|\n/)
}

// TODO(rishi): Set pos to be a value generated from the linter.
const insert = (doc, pos, text) => insertMergedLines(doc, pos, split(text))

/**
 * This function removes lines from the document in a given range.
 * @param {*} doc The document being modified.
 * @param {*} range The range of lines to remove from the doc.
 */
const remove = (doc, range) => {
  const start = clippedPos(doc, range.start.row, range.start.column)
  const end = clippedPos(doc, range.end.row, range.end.column)
  applyDelta(doc.lines, {
    start,
    end,
    action: 'remove',
    lines: getLinesForRange(doc, {start, end}),
  })
}

/**
 * This function sets the initial value of the document and clears
 * any preexisting lines.
 */
const setValue = (doc, text) => {
  const len = doc.lines.length - 1
  remove(doc, range_(0, 0, len, doc.lines[len].length))
  insert(doc, {row: 0, column: 0}, text)
}

onmessage = (args) => {
  const {requestId, command, args: arg, data, event, val} = args.data
  if (command) {
    if (command === 'setValue') {
      setValue(doc_, arg[0])
    }
    if (command === 'parse-components') {
      const {componentData, errors} = parseComponentAst(val)

      postMessage({
        requestId,
        componentData,
        errors,
      })
    }
  }
  if (event) {
    if (event === 'change') {
      const changes = data.data
      if (changes[0] && changes[0].start) {
        changes.forEach(delta => applyDelta(doc_.lines, delta))
      } else {
        for (let i = 0; i < changes.length; i += 2) {
          let d
          if (Array.isArray(changes[i + 1])) {
            d = {action: 'insert', start: changes[i], lines: changes[i + 1]}
          } else {
            d = {action: 'remove', start: changes[i], end: changes[i + 1]}
          }
          applyDelta(doc_.lines, d)
        }
      }
    }
  }
}
