module.exports = {
  name: 'underscore-argument',
  meta: {
    messages: {
      noUnderscoreNames: 'Function argument lists should not end with _ (it doesn\'t do anything)',
    },
  },
  create(context) {
    const reject = (node) => {
      context.report({
        node,
        messageId: 'noUnderscoreNames',
      })
    }

    const UNDERSCORE_IDENTIFIER_FILTER = 'Identifier[name=/^_+$/]:last-child'

    return {
      [`ArrowFunctionExpression > ${UNDERSCORE_IDENTIFIER_FILTER}`]: reject,
      [`FunctionDeclaration > ${UNDERSCORE_IDENTIFIER_FILTER}`]: reject,
    }
  },
}
