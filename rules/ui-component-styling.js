const path = require('path')

module.exports = {
  name: 'ui-component-styling',
  meta: {
    messages: {
      propIsForbidden: '{{prop}} should be used sparingly, if ever, on {{component}}',
    },
  },
  create(context) {
    const uiComponentNames = new Set()
    const disallowedProps = new Set(['className', 'style'])

    return {
      ImportDeclaration(node) {
        const importPath = node.source.type === 'Literal' && node.source.value
        if (!importPath || !importPath.startsWith('.')) {
          return
        }

        const resolvedPath = path.resolve(path.dirname(context.getFilename()), importPath)
        if (resolvedPath.includes('/xrhome/src/client/ui/')) {
          node.specifiers.forEach((specifier) => {
            uiComponentNames.add(specifier.local.name)
          })
        }
      },
      JSXAttribute(propNode) {
        const prop = propNode.name.name
        const componentNameNode = propNode.parent.name
        const component = componentNameNode.name || componentNameNode.property.name
        const isBreakingRule = disallowedProps.has(prop) && uiComponentNames.has(component)

        if (isBreakingRule) {
          context.report({
            node: propNode,
            messageId: 'propIsForbidden',
            data: {
              prop,
              component,
            },
          })
        }
      },
    }
  },
}
