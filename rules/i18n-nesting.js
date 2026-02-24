module.exports = {
  name: 'i18n-nesting',
  meta: {
    messages: {
      childrenIsForbidden: 'Prefer <Trans components={{...}} /> rather than wrapping children.',
    },
  },
  create(context) {
    return {
      JSXElement(node) {
        const nameNode = node.openingElement.name
        const componentName = nameNode.name
        const isBreakingRule = componentName === 'Trans' && !!node.children.length

        if (isBreakingRule) {
          context.report({node: nameNode, messageId: 'childrenIsForbidden'})
        }
      },
    }
  },
}
