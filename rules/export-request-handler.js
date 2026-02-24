// Returns true if the function seems to be a handler function.
// Middleware is exempt.
const isRequestHandler = node => (
  node.params.length === 2 &&
  ['_', 'req'].includes(node.params[0].name) &&
 node.params[1].name === 'res'
)

module.exports = {
  name: 'export-request-handler',
  meta: {
    docs: {
      // eslint-disable-next-line max-len
      url: 'https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/wiki/spaces/AR/pages/2633531527/Code+Organization#Express-Handlers',
    },
    messages: {
      noExportHandler: 'Handler "{{name}}" should attached to a router before exporting.',
    },
  },
  create(context) {
    const handlerNames = new Set()

    return {
      VariableDeclaration(node) {
        const isTopLevel = node.parent.type === 'Program'
        if (!isTopLevel) {
          return
        }

        node.declarations.forEach((declaration) => {
          const isFunction = declaration.init?.type === 'ArrowFunctionExpression'
          if (!isFunction) {
            return
          }

          if (!isRequestHandler(declaration.init)) {
            return
          }

          handlerNames.add(declaration.id.name)
        })
      },
      FunctionDeclaration(node) {
        const isTopLevel = node.parent.type === 'Program'
        if (!isTopLevel) {
          return
        }

        if (!isRequestHandler(node)) {
          return
        }

        handlerNames.add(node.id.name)
      },
      ExportNamedDeclaration(node) {
        node.specifiers.forEach((specifier) => {
          const localName = specifier.local.name
          if (handlerNames.has(localName)) {
            context.report({
              node: specifier,
              messageId: 'noExportHandler',
              data: {
                name: localName,
              },
            })
          }
        })
      },
    }
  },
}
