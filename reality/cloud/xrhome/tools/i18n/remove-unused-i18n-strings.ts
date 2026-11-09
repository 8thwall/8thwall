/* eslint-disable no-console */
import * as fs from 'fs'
import * as path from 'path'
import ts from 'typescript'

// Usage: ./remove-unused-i18n-strings.ts <json-file-path> <dry-run>

const VALID_FILE_EXTENSIONS = ['.ts', '.js', '.jsx', '.tsx', '.json']

// Paths containing xrhome translation consumers.
const TRANSLATION_SOURCE_PATHS = ['./src', '../../../c8/ecs/src']

type JsonData = Record<string, string>
type TranslationKeyReferences = {
  keys: Set<string>
  prefixes: Set<string>
}

type TranslationSourceContext = {
  translatorNames: Set<string>
  variableInitializers: Map<string, ts.Expression>
}

type TranslationAnyAssertion = {
  boundary: 'key' | 'namespace'
  line: number
}

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
        if (node.arguments[1]) {
          inspectNamespaceOption(node.arguments[1])
        }
      }
    }

    if (
      (ts.isJsxOpeningElement(node) || ts.isJsxSelfClosingElement(node)) &&
      node.tagName.getText(sourceFile) === 'Trans'
    ) {
      const keyAttribute = node.attributes.properties.find(attribute => (
        ts.isJsxAttribute(attribute) && attribute.name.text === 'i18nKey'
      )) as ts.JsxAttribute | undefined
      if (
        keyAttribute?.initializer &&
        ts.isJsxExpression(keyAttribute.initializer) &&
        keyAttribute.initializer.expression
      ) {
        addAssertion('key', keyAttribute.initializer.expression)
      }
    }

    ts.forEachChild(node, inspect)
  }

  inspect(sourceFile)
  return assertions
}

const findPotentialTranslationKeys = (
  fileContent: string, filePath = 'source.tsx'
): TranslationKeyReferences => {
  const sourceFile = ts.createSourceFile(
    filePath,
    fileContent,
    ts.ScriptTarget.Latest,
    true,
    filePath.endsWith('x') ? ts.ScriptKind.TSX : ts.ScriptKind.TS
  )
  const {translatorNames, variableInitializers} = getTranslationSourceContext(sourceFile)
  const keys = new Set<string>()
  const prefixes = new Set<string>()

  const finiteStrings = (
    originalExpression: ts.Expression, seen = new Set<string>()
  ): string[] | undefined => {
    const expression = resolveIdentifier(originalExpression, variableInitializers, seen)
    if (ts.isStringLiteral(expression) || ts.isNoSubstitutionTemplateLiteral(expression)) {
      return [expression.text]
    }
    if (ts.isParenthesizedExpression(expression)) {
      return finiteStrings(expression.expression, seen)
    }
    if (ts.isConditionalExpression(expression)) {
      const whenTrue = finiteStrings(expression.whenTrue, new Set(seen))
      const whenFalse = finiteStrings(expression.whenFalse, new Set(seen))
      return whenTrue && whenFalse
        ? [...new Set([...whenTrue, ...whenFalse])]
        : undefined
    }
    if (
      ts.isBinaryExpression(expression) &&
      expression.operatorToken.kind === ts.SyntaxKind.PlusToken
    ) {
      const left = finiteStrings(expression.left, new Set(seen))
      const right = finiteStrings(expression.right, new Set(seen))
      if (!left || !right || left.length * right.length > 100) {
        return undefined
      }
      return left.flatMap(leftPart => right.map(rightPart => leftPart + rightPart))
    }
    if (ts.isTemplateExpression(expression)) {
      let values = [expression.head.text]
      for (const span of expression.templateSpans) {
        const expressionValues = finiteStrings(span.expression, new Set(seen))
        if (!expressionValues || values.length * expressionValues.length > 100) {
          return undefined
        }
        values = values.flatMap(value => expressionValues.map(
          expressionValue => value + expressionValue + span.literal.text
        ))
      }
      return values
    }
    return undefined
  }

  const leadingStrings = (
    originalExpression: ts.Expression, seen = new Set<string>()
  ): string[] => {
    const expression = resolveIdentifier(originalExpression, variableInitializers, seen)
    const values = finiteStrings(expression, new Set(seen))
    if (values) {
      return values
    }
    if (ts.isParenthesizedExpression(expression)) {
      return leadingStrings(expression.expression, seen)
    }
    if (ts.isTemplateExpression(expression)) {
      return expression.head.text ? [expression.head.text] : []
    }
    if (
      ts.isBinaryExpression(expression) &&
      expression.operatorToken.kind === ts.SyntaxKind.PlusToken
    ) {
      return finiteStrings(expression.left, new Set(seen)) ||
        leadingStrings(expression.left, seen)
    }
    return []
  }

  const addExpression = (expression: ts.Expression) => {
    const values = finiteStrings(expression)
    if (values) {
      values.forEach(value => keys.add(value))
      return
    }
    leadingStrings(expression)
      .filter(prefix => prefix.includes('.'))
      .forEach(prefix => prefixes.add(prefix))
  }

  const collectTranslationCalls = (node: ts.Node) => {
    if (ts.isCallExpression(node) && node.arguments[0]) {
      if (isTranslationCall(node, translatorNames)) {
        addExpression(node.arguments[0])
      }
    }

    if (
      (ts.isJsxOpeningElement(node) || ts.isJsxSelfClosingElement(node)) &&
      node.tagName.getText(sourceFile) === 'Trans'
    ) {
      const keyAttribute = node.attributes.properties.find(attribute => (
        ts.isJsxAttribute(attribute) && attribute.name.text === 'i18nKey'
      )) as ts.JsxAttribute | undefined
      if (keyAttribute?.initializer) {
        if (ts.isStringLiteral(keyAttribute.initializer)) {
          keys.add(keyAttribute.initializer.text)
        } else if (
          ts.isJsxExpression(keyAttribute.initializer) &&
          keyAttribute.initializer.expression
        ) {
          addExpression(keyAttribute.initializer.expression)
        }
      }
    }

    ts.forEachChild(node, collectTranslationCalls)
  }

  collectTranslationCalls(sourceFile)
  return {keys, prefixes}
}

// Function to load JSON and extract all keys
const loadJsonKeys = (jsonFilePath: string): {jsonData: JsonData, keys: string[]} => {
  const jsonData = JSON.parse(fs.readFileSync(jsonFilePath, 'utf-8'))

  const extractKeys = (data: JsonData): string[] => {
    const keys: string[] = []
    for (const key of Object.keys(data)) {
      keys.push(key)
    }
    return keys
  }

  const keys = extractKeys(jsonData)
  return {jsonData, keys}
}

// Function to search for JSON keys in the codebase
const searchKeysInCodebase = (
  keys: string[], codebaseDirs: string | string[]
): Set<string> => {
  const matchedKeys = new Set<string>()
  const dynamicPrefixes = new Set<string>()

  const pluralBase = (key: string): string | undefined => {
    const match = key.match(/^(.*)_(one|other)$/)
    return match?.[1]
  }

  const searchFileForKeys = (filePath: string): void => {
    const fileContent = fs.readFileSync(filePath, 'utf-8')
    for (const key of keys) {
      const base = pluralBase(key)
      if (fileContent.includes(key) || (base && fileContent.includes(base))) {
        matchedKeys.add(key)
      }
    }

    if (path.extname(filePath) !== '.json') {
      const references = findPotentialTranslationKeys(fileContent, filePath)
      references.keys.forEach((key) => {
        if (keys.includes(key)) {
          matchedKeys.add(key)
        }
      })
      references.prefixes.forEach(prefix => dynamicPrefixes.add(prefix))
    }
  }

  const walkDirectory = (dir: string, translationDirectory: string) => {
    const files = fs.readdirSync(dir)
    for (const file of files) {
      const fullPath = path.join(dir, file)
      const stat = fs.lstatSync(fullPath)

      if (stat.isDirectory()) {
        // eslint-disable-next-line no-await-in-loop
        walkDirectory(fullPath, translationDirectory)  // Recursively go through directories
      } else if (
        VALID_FILE_EXTENSIONS.includes(path.extname(file)) &&
        !fullPath.startsWith(`${translationDirectory}${path.sep}`)
      ) {
        // eslint-disable-next-line no-await-in-loop
        searchFileForKeys(fullPath)
      }
    }
  }

  for (const codebaseDir of [codebaseDirs].flat()) {
    walkDirectory(codebaseDir, path.join(codebaseDir, 'client', 'i18n'))
  }

  for (const prefix of dynamicPrefixes) {
    for (const key of keys) {
      if (key.startsWith(prefix)) {
        matchedKeys.add(key)
      }
    }
  }

  return matchedKeys
}

// Function to remove unmatched keys from the JSON data
const removeUnmatchedKeys = (
  jsonData: Record<string, string>, unmatchedKeys: string[]
): Record<string, string> => (
  Object.entries(jsonData)
    .reduce((acc, [key, value]) => {
      if (!unmatchedKeys.includes(key)) {
        acc[key] = value
      }
      return acc
    }, {} as Record<string, string>)
)

// Main function to find unmatched keys and remove them from the JSON file
const removeUnusedI18nStrings = async (
  jsonFilePath: string, codebaseDirs: string | string[], dryRun: string
) => {
  const isDryRun = dryRun === undefined || dryRun === 'true' || dryRun === 'dry-run'
  if (isDryRun) {
    console.log('\n***********************')
    console.log('** THIS IS A DRY RUN **')
    console.log('***********************\n')
  }

  // Load JSON keys and the original data
  const {jsonData, keys} = loadJsonKeys(jsonFilePath)

  // Search for keys in the codebase
  const matchedKeys = searchKeysInCodebase(keys, codebaseDirs)

  // Find unmatched keys by subtracting matched keys from all keys
  const unmatchedKeys = keys.filter(key => !matchedKeys.has(key))

  // Output the unmatched keys
  if (unmatchedKeys.length > 0) {
    console.log('The following JSON keys are NOT referenced in the codebase and will be removed:')
    unmatchedKeys.forEach(key => console.log(key))

    if (isDryRun) {
      return
    }

    const updatedJsonData = removeUnmatchedKeys(jsonData, unmatchedKeys)

    // Write the updated JSON back to the file
    fs.writeFileSync(jsonFilePath, JSON.stringify(updatedJsonData, null, 2), 'utf-8')
    console.log(`Unmatched keys have been removed from ${jsonFilePath}.`)
  } else {
    console.log('All JSON keys are referenced in the codebase.')
  }
}

if (process.argv[1] === __filename) {
  removeUnusedI18nStrings(process.argv[2], TRANSLATION_SOURCE_PATHS, process.argv[3])
}

export {
  findPotentialTranslationKeys,
  findTranslationAnyAssertions,
  searchKeysInCodebase,
}
