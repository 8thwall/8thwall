const resolveName = (node) => {
  if (!node) {
    return null
  }
  if (node.type === 'TSTypeAliasDeclaration') {
    return node.id.name
  }

  if (node.type === 'TSInterfaceDeclaration') {
    return node.id.name
  }

  return resolveName(node.parent)
}

const reportInconsistency = (context, node, fix) => {
  context.report({
    node,
    messageId: 'inconsistentTrailing',
    data: {
      declarationName: resolveName(node) || 'Type definition',
    },
    fix,
  })
}

const checkDeclaration = (context, node) => {
  const isMultiline = node.loc.start.line !== node.loc.end.line

  if (!isMultiline) {
    return
  }

  const unneededSeparatorLocations = []

  const sourceCode = context.getSourceCode().getText()

  // Handle TSTypeLiteral or TSInterfaceDeclaration
  const parts = node.members || node.body.body

  parts.forEach((member, i) => {
    const nextMember = parts[i + 1]
    if (nextMember && nextMember.loc.start.line === member.loc.end.line) {
      // Ignore members that are not the last on the line
      return
    }
    const lastCharLocation = member.range[1] - 1
    const lastChar = sourceCode[lastCharLocation]
    switch (lastChar) {
      case ',':
      case ';':
        unneededSeparatorLocations.push(lastCharLocation)
        break
      default:
        break
    }
  })

  if (unneededSeparatorLocations.length) {
    reportInconsistency(
      context,
      node,
      fixer => unneededSeparatorLocations.map(loc => fixer.replaceTextRange([loc, loc + 1], ''))
    )
  }
}

module.exports = {
  name: 'typedef-separators',
  meta: {
    fixable: 'code',
    messages: {
      inconsistentTrailing: '{{declarationName}} should not use , or ; as a line separator.',
    },
  },
  create(context) {
    return {
      TSTypeLiteral(node) {
        checkDeclaration(context, node)
      },
      TSInterfaceDeclaration(node) {
        checkDeclaration(context, node)
      },
    }
  },
}
