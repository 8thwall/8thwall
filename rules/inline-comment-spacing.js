/*
Ensure consistency in spacing with inline comments that share a line with code

Example allowed code:

  // This comment is not preceded by code

  console.log(1 + 1)  // This has two spaces

  console.log({
    a: true,                    // These
    b: false,
    c: false,
    d: 'long long long string'  // are
    e false,                    // aligned (skipping one or two lines is allowed)
  })

  console.log(1 + 1)  // These are
  console.log(1 + 1000)  // alignable (but since they already have two spaces, we leave them)

Example fixable code:

  console.log(1 + 1)// No spaces
  console.log(1 + 1) // One space
  console.log(1 + 1)     // More spaces (unless surrounded by aligned comments)

  console.log({
    a: true,                 // These
    b: 'long long long string'  // are
    c false,                       // alignable (the first and third will move to match the second)
  })
*/

const MINIMUM_SPACE_COUNT = 2  // Minimum space between code and comment
const MAX_NEIGHBOR_DISTANCE = 3  // Maximum line offset between two "aligned" comments

const isGrouped = (previousComment, nextComment, sourceCode) => {
  if (nextComment.line - previousComment.line > MAX_NEIGHBOR_DISTANCE) {
    return false
  }

  // If there are any lines with no code (comment or empty line) between two comments,
  // they can't be grouped.
  for (let line = previousComment.line + 1; line < nextComment.line; line++) {
    const index = sourceCode.getIndexFromLoc({line, column: 0})
    const node = sourceCode.getNodeByRangeIndex(index)
    const tokens = sourceCode.getTokens(node)
    const linesWithTokens = tokens.map(({range}) => sourceCode.getLocFromIndex(range[0]).line)
    if (!linesWithTokens.includes(line)) {
      return false
    }
  }

  return true
}

const fixInlineCommentSpacing = (context, sourceCode) => {
  const comments = sourceCode.getAllComments()

  const inlineComments = comments.filter((comment) => {
    if (comment.type !== 'Line') {
      return false
    }
    const tokenBefore = sourceCode.getTokenBefore(comment, {includeComments: true})
    const hasCodeOnLine = tokenBefore && tokenBefore.loc.end.line === comment.loc.start.line
    return hasCodeOnLine
  })

  // Read AST into a structure to process
  const commentStates = inlineComments.map((comment) => {
    const previous = sourceCode.getTokenBefore(comment, {includeComments: true})

    return {
      line: comment.loc.start.line,
      codeEnd: previous.loc.end.column,
      commentStart: comment.loc.start.column,
      range: [previous.range[1], comment.range[0]],
    }
  })

  // Group comments into groups that can share horizontal alignment
  const alignableGroups = []

  commentStates.forEach((c) => {
    const lastGroup = alignableGroups.length && alignableGroups[alignableGroups.length - 1]
    if (lastGroup && isGrouped(lastGroup[lastGroup.length - 1], c, sourceCode)) {
      lastGroup.push(c)
    } else {
      alignableGroups.push([c])
    }
  })

  const finalStates = []

  alignableGroups.forEach((group) => {
    const anyHasExtraSpace = group.some(c => c.commentStart - c.codeEnd > MINIMUM_SPACE_COUNT)

    if (anyHasExtraSpace) {
      const maxCodeEnd = group.reduce((acc, c) => Math.max(acc, c.codeEnd), 0)

      group.forEach((c) => {
        c.finalCommentStart = maxCodeEnd + MINIMUM_SPACE_COUNT
      })
    } else {
      group.forEach((c) => {
        c.finalCommentStart = c.codeEnd + MINIMUM_SPACE_COUNT
      })
    }

    group.forEach(c => finalStates.push(c))
  })

  const statesToFix = finalStates.filter(c => c.commentStart !== c.finalCommentStart)

  statesToFix.forEach(({line, codeEnd, commentStart, finalCommentStart, range}) => {
    context.report({
      node: null,
      loc: {start: {line, column: codeEnd}, end: {line, column: commentStart}},
      messageId: 'incorrectSpaceCount',
      fix(fixer) {
        const correctSpacing = ' '.repeat(finalCommentStart - codeEnd)
        return fixer.replaceTextRange(range, correctSpacing)
      },
    })
  })
}

module.exports = {
  meta: {
    type: 'layout',
    fixable: 'whitespace',
    messages: {
      incorrectSpaceCount: 'Inline comment not aligned or preceded by two spaces.',
    },
  },
  create(context) {
    const sourceCode = context.getSourceCode()
    return {
      Program() {
        fixInlineCommentSpacing(context, sourceCode)
      },
    }
  },
}
