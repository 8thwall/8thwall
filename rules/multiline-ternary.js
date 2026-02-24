const MAX_LINE_LENGTH = 100

module.exports = {
  meta: {
    type: 'layout',
    fixable: 'whitespace',
    messages: {
      improperTernary: 'Multiline ternary should have ? and : at the beginning of lines.',
    },
    schema: [],
  },
  create(context) {
    const sourceCode = context.getSourceCode()

    const checkTernary = (node) => {
      if (node.loc.start.line === node.loc.end.line) {
        const lineText = sourceCode.lines[node.loc.start.line - 1]
        if (lineText.length <= MAX_LINE_LENGTH) {
          return
        }
      }

      const questionToken = sourceCode.getTokenAfter(node.test, {
        filter: token => token.value === '?',
      })
      const colonToken = sourceCode.getTokenAfter(node.consequent, {
        filter: token => token.value === ':',
      })
      if (!questionToken || !colonToken) {
        return
      }

      const fixes = []

      const beforeQuestion = sourceCode.getTokenBefore(questionToken)
      if (beforeQuestion && beforeQuestion.loc.end.line === questionToken.loc.start.line) {
        fixes.push({
          token: questionToken,
          messageId: 'improperTernary',
        })
      }

      const beforeColon = sourceCode.getTokenBefore(colonToken)
      if (beforeColon && beforeColon.loc.end.line === colonToken.loc.start.line) {
        fixes.push({
          token: colonToken,
          messageId: 'improperTernary',
        })
      }

      fixes.forEach(({token}) => {
        context.report({
          node: token,
          messageId: 'improperTernary',
          fix(fixer) {
            return fixer.insertTextBefore(token, '\n')
          },
        })
      })
    }

    return {
      ConditionalExpression: checkTernary,
    }
  },
}
