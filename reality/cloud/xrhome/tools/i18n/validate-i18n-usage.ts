import fs from 'fs'
import path from 'path'
import ts from 'typescript'

type TranslationSourceContext = {
  translatorNames: Set<string>
  variableInitializers: Map<string, ts.Expression>
}

type TranslationAnyAssertion = {
  boundary: 'key' | 'namespace'
  line: number
}

const getTypescriptFiles = (directory: string): string[] => fs.readdirSync(
  directory, {withFileTypes: true}
).flatMap((entry) => {
  const entryPath = path.join(directory, entry.name)
  if (entry.isDirectory()) {
    return getTypescriptFiles(entryPath)
  }
  return /\.tsx?$/.test(entry.name) ? [entryPath] : []
})

const getTranslationSourceContext = (
  sourceFile: ts.SourceFile
): TranslationSourceContext => {
  const translatorNames = new Set<string>()
  const variableInitializers = new Map<string, ts.Expression>()

  const collectTranslationBindings = (node: ts.Node) => {
    if (ts.isVariableDeclaration(node) && ts.isIdentifier(node.name) && node.initializer) {
      variableInitializers.set(node.name.text, node.initializer)
    }

    if (
      ts.isVariableDeclaration(node) &&
      ts.isObjectBindingPattern(node.name) &&
      node.initializer &&
      ts.isCallExpression(node.initializer) &&
      node.initializer.expression.getText(sourceFile) === 'useTranslation'
    ) {
      for (const element of node.name.elements) {
        const propertyName = element.propertyName?.getText(sourceFile)
        if (propertyName === 't' || (!propertyName && element.name.getText(sourceFile) === 't')) {
          translatorNames.add(element.name.getText(sourceFile))
        }
      }
    }

    if (
      (ts.isParameter(node) || ts.isVariableDeclaration(node)) &&
      node.type &&
      /\bTFunction(?:\s*<|\b)/.test(node.type.getText(sourceFile)) &&
      ts.isIdentifier(node.name)
    ) {
      translatorNames.add(node.name.text)
    }

    ts.forEachChild(node, collectTranslationBindings)
  }

  collectTranslationBindings(sourceFile)
  return {translatorNames, variableInitializers}
}

const isTranslationCall = (
  node: ts.CallExpression, translatorNames: Set<string>
): boolean => (
  (ts.isIdentifier(node.expression) && translatorNames.has(node.expression.text)) ||
  (ts.isPropertyAccessExpression(node.expression) && node.expression.name.text === 't')
)

const resolveIdentifier = (
  expression: ts.Expression,
  variableInitializers: Map<string, ts.Expression>,
  seen = new Set<string>()
): ts.Expression => {
  if (
    ts.isIdentifier(expression) &&
    variableInitializers.has(expression.text) &&
    !seen.has(expression.text)
  ) {
    seen.add(expression.text)
    return resolveIdentifier(variableInitializers.get(expression.text), variableInitializers, seen)
  }
  return expression
}

const findTranslationAnyAssertions = (
  fileContent: string, filePath = 'source.tsx'
): TranslationAnyAssertion[] => {
  const sourceFile = ts.createSourceFile(
    filePath,
    fileContent,
    ts.ScriptTarget.Latest,
    true,
    filePath.endsWith('x') ? ts.ScriptKind.TSX : ts.ScriptKind.TS
  )
  const {translatorNames, variableInitializers} = getTranslationSourceContext(sourceFile)
  const assertions: TranslationAnyAssertion[] = []

  const findAnyAssertion = (originalNode: ts.Node): ts.Node | undefined => {
    let assertion: ts.Node | undefined
    const seen = new Set<string>()
    const inspect = (child: ts.Node) => {
      if (assertion) {
        return
      }
      if (
        (ts.isAsExpression(child) || ts.isTypeAssertionExpression(child)) &&
        child.type.kind === ts.SyntaxKind.AnyKeyword
      ) {
        assertion = child
        return
      }
      if (
        ts.isIdentifier(child) &&
        variableInitializers.has(child.text) &&
        !seen.has(child.text)
      ) {
        seen.add(child.text)
        inspect(variableInitializers.get(child.text))
      }
      ts.forEachChild(child, inspect)
    }
    inspect(originalNode)
    return assertion
  }

  const addAssertion = (
    boundary: TranslationAnyAssertion['boundary'], node: ts.Node
  ) => {
    const assertion = findAnyAssertion(node)
    if (assertion) {
      const {line} = sourceFile.getLineAndCharacterOfPosition(assertion.getStart(sourceFile))
      assertions.push({boundary, line: line + 1})
    }
  }

  const inspectNamespaceOption = (expression: ts.Expression) => {
    const resolved = resolveIdentifier(expression, variableInitializers)
    if (
      (ts.isAsExpression(resolved) || ts.isTypeAssertionExpression(resolved)) &&
      resolved.type.kind === ts.SyntaxKind.AnyKeyword
    ) {
      addAssertion('namespace', resolved)
      return
    }
    if (!ts.isObjectLiteralExpression(resolved)) {
      return
    }
    const namespace = resolved.properties.find(property => (
      (ts.isPropertyAssignment(property) || ts.isShorthandPropertyAssignment(property)) &&
      property.name.getText(sourceFile) === 'ns'
    ))
    if (namespace) {
      addAssertion('namespace', namespace)
    }
  }

  const inspect = (node: ts.Node) => {
    if (ts.isCallExpression(node)) {
      if (node.expression.getText(sourceFile) === 'useTranslation' && node.arguments[0]) {
        addAssertion('namespace', node.arguments[0])
      } else if (isTranslationCall(node, translatorNames)) {
        if (node.arguments[0]) {
          addAssertion('key', node.arguments[0])
        }
        node.arguments.slice(1).forEach(inspectNamespaceOption)
      }
    }

    if (
      (ts.isJsxOpeningElement(node) || ts.isJsxSelfClosingElement(node)) &&
      node.tagName.getText(sourceFile) === 'Trans'
    ) {
      for (const attribute of node.attributes.properties) {
        if (
          ts.isJsxAttribute(attribute) &&
          (attribute.name.text === 'i18nKey' || attribute.name.text === 'ns') &&
          attribute.initializer &&
          ts.isJsxExpression(attribute.initializer) &&
          attribute.initializer.expression
        ) {
          addAssertion(
            attribute.name.text === 'i18nKey' ? 'key' : 'namespace',
            attribute.initializer.expression
          )
        }
      }
    }

    ts.forEachChild(node, inspect)
  }

  inspect(sourceFile)
  return assertions
}

export {
  findTranslationAnyAssertions,
  getTypescriptFiles,
}

export type {TranslationAnyAssertion}
