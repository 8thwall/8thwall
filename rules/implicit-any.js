const TS_EXTENSIONS = ['.ts', '.tsx']

module.exports = {
  name: 'implicit-any',
  meta: {
    type: 'suggestion',
    messages: {
      missingArgumentType: 'Function argument "{{name}}" is missing a type annotation',
    },
  },
  create(context) {
    const fileName = context.getFilename()
    if (!TS_EXTENSIONS.some(ext => fileName.endsWith(ext))) {
      return {}
    }

    const checkParam = (param) => {
      if (param.type === 'AssignmentPattern') {
        // We're ok with default arguments like:
        //   const add = (arg = 1) => arg + 1
        return
      }
      if (!param.typeAnnotation) {
        context.report({
          node: param,
          messageId: 'missingArgumentType',
          data: {
            name: param.name,
          },
        })
      }
    }

    return {
      'VariableDeclarator > ArrowFunctionExpression': (node) => {
        // We're ok with types on the identifier like:
        //   const add: (arg: number) => number = arg => arg + 1
        if (node.parent.id.typeAnnotation) {
          return
        }
        node.params.forEach(checkParam)
      },
      'FunctionDeclaration': (node) => {
        node.params.forEach(checkParam)
      },
    }
  },
}
