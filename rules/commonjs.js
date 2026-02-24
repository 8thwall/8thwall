const isIdentifierNamed = (node, name) => node.type === 'Identifier' && node.name === name

module.exports = {
  name: 'commonjs',
  meta: {
    messages: {
      badImport: 'Expected es6 import in typescript file',
      badExport: 'Expected es6 export in typescript file',
    },
  },
  create(context) {
    if (!context.getFilename().match(/\.tsx?/)) {
      return {}
    }
    return {
      Identifier(node) {
        if (node.name === 'require') {
          context.report({
            node,
            messageId: 'badImport',
          })
        }
      },
      MemberExpression(node) {
        const isCommonJsExport = isIdentifierNamed(node.object, 'module') &&
                                 isIdentifierNamed(node.property, 'exports')
        if (isCommonJsExport) {
          context.report({
            node,
            messageId: 'badExport',
          })
        }
      },
    }
  },
}
