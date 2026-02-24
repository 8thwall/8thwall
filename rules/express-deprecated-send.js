const isIdentifierNamed = (node, name) => node.type === 'Identifier' && node.name === name

module.exports = {
  name: 'express-deprecated-send',
  meta: {
    messages: {
      badSend: 'res.send({{value}}) is deprecated in favor of res.sendStatus({{value}})',
    },
  },
  create(context) {
    return {
      CallExpression(node) {
        // Only match something looking like "res.send(xxx)"
        const isResSend = node.callee.type === 'MemberExpression' &&
                          isIdentifierNamed(node.callee.object, 'res') &&
                          isIdentifierNamed(node.callee.property, 'send')

        if (!isResSend) {
          return
        }

        if (node.arguments.length !== 1) {
          return
        }

        const argument = node.arguments[0]

        const isPassingNumber = argument.type === 'Literal' && typeof argument.value === 'number'

        if (isPassingNumber) {
          context.report({
            node,
            messageId: 'badSend',
            data: {
              value: argument.value,
            },
          })
        }
      },
    }
  },
}
