module.exports = {
  name: 'acronym-capitalization',
  meta: {
    type: 'suggestion',
    messages: {
      badCapitalization: '{{name}} should be capitalized like HtmlData, not HTMLData\').',
    },
  },
  create(context) {
    const identifierRegex = /[A-Z]{3,}([a-z])|[a-z].*[A-Z]{2}$/

    const checkName = (node, name) => {
      const matches = name.match(identifierRegex)
      if (matches) {
        context.report({
          node,
          messageId: 'badCapitalization',
          data: {
            name,
          },
        })
      }
    }

    return {
      VariableDeclarator: (node) => {
        if (node.id.type === 'Identifier') {
          checkName(node.id, node.id.name)
        }
      },
      // TypeScript: type FooBar = ...
      TSTypeAliasDeclaration: (node) => {
        checkName(node.id, node.id.name)
      },

      // TypeScript: interface FooBar { ... }
      TSInterfaceDeclaration: (node) => {
        checkName(node.id, node.id.name)
      },
    }
  },
}
