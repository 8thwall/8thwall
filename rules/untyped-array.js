module.exports = {
  name: 'untyped-array',
  meta: {
    type: 'suggestion',
    messages: {
      noUntypedArray: 'Empty array declarations should be typed to avoid defaulting to any[]',
    },
  },
  create(context) {
    // Only ts/tsx
    if (!context.getFilename().endsWith('.ts') && !context.getFilename().endsWith('.tsx')) {
      return {}
    }

    return {
      'VariableDeclarator > ArrayExpression': (arrayExpression) => {
        if (arrayExpression.elements.length > 0) {
          return
        }

        if (arrayExpression.typeAnnotation) {
          return
        }

        if (arrayExpression.parent.id.typeAnnotation) {
          return
        }

        context.report({
          node: arrayExpression.parent,
          messageId: 'noUntypedArray',
        })
      },

    }
  },
}
