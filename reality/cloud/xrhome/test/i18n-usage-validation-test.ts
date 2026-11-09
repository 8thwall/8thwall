import {assert} from 'chai'
import fs from 'fs'
import path from 'path'
import ts from 'typescript'

import {
  findTranslationAnyAssertions,
  getTypescriptFiles,
} from '../tools/i18n/validate-i18n-usage'

describe('typed i18n usage', () => {
  it('finds explicit any assertions at translation boundaries', () => {
    const source = [
      'const namespace = runtimeNamespace as any',
      'const key = runtimeKey as any',
      'const options = {ns: runtimeNamespace as any}',
      'const unrelated = runtimeValue as any',
      'const {t: translate} = useTranslation(namespace)',
      'translate(key)',
      'translate(\'valid.key\', options)',
      'translate(\'valid.key\', \'fallback\', {ns: runtimeNamespace as any})',
      'i18n.t(\'valid.key\', {ns: runtimeNamespace as any})',
      'const component = <Trans i18nKey={runtimeKey as any} />',
      'const namespaced = <Trans ns={runtimeNamespace as any} i18nKey="valid.key" />',
    ].join('\n')

    assert.deepEqual(findTranslationAnyAssertions(source, 'source.tsx'), [
      {boundary: 'namespace', line: 1},
      {boundary: 'key', line: 2},
      {boundary: 'namespace', line: 3},
      {boundary: 'namespace', line: 8},
      {boundary: 'namespace', line: 9},
      {boundary: 'key', line: 10},
      {boundary: 'namespace', line: 11},
    ])
  })

  it('requires every TFunction use to select a namespace', () => {
    const errors: string[] = []
    for (const filePath of getTypescriptFiles(path.join(__dirname, '../src'))) {
      const sourceFile = ts.createSourceFile(
        filePath,
        fs.readFileSync(filePath, 'utf8'),
        ts.ScriptTarget.Latest,
        true,
        filePath.endsWith('x') ? ts.ScriptKind.TSX : ts.ScriptKind.TS
      )

      const inspect = (node: ts.Node) => {
        if (
          ts.isImportDeclaration(node) &&
          ts.isStringLiteral(node.moduleSpecifier) &&
          node.moduleSpecifier.text === 'i18next' &&
          node.importClause?.namedBindings &&
          ts.isNamedImports(node.importClause.namedBindings) &&
          // eslint-disable-next-line @stylistic/max-len
          node.importClause.namedBindings.elements.some(element => element.name.text === 'TFunction')
        ) {
          errors.push(`${filePath}: imports TFunction directly from i18next`)
        }
        if (
          ts.isTypeReferenceNode(node) &&
          node.typeName.getText(sourceFile) === 'TFunction' &&
          (!node.typeArguments?.length ||
            node.typeArguments[0].kind === ts.SyntaxKind.AnyKeyword ||
            node.typeArguments[0].kind === ts.SyntaxKind.StringKeyword)
        ) {
          const {line} = sourceFile.getLineAndCharacterOfPosition(node.getStart(sourceFile))
          errors.push(`${filePath}:${line + 1}: uses TFunction without an explicit namespace`)
        }
        ts.forEachChild(node, inspect)
      }

      inspect(sourceFile)
    }

    assert.deepEqual(errors, [])
  })

  it('does not cast translation keys or namespaces to any', () => {
    const errors: string[] = []
    for (const filePath of getTypescriptFiles(path.join(__dirname, '../src'))) {
      const source = fs.readFileSync(filePath, 'utf8')
      for (const assertion of findTranslationAnyAssertions(source, filePath)) {
        errors.push(
          `${filePath}:${assertion.line}: casts a translation ${assertion.boundary} to any`
        )
      }
    }

    assert.deepEqual(errors, [])
  })
})
