/* eslint-disable no-continue, prefer-destructuring, @typescript-eslint/no-use-before-define, func-names, no-restricted-syntax, max-len */

// Adapted from https://github.com/typescript-eslint/typescript-eslint/blob/main/packages/eslint-plugin/src/rules/consistent-type-imports.ts

/*
TypeScript ESLint

Originally extracted from:

TypeScript ESLint Parser
Copyright JS Foundation and other contributors, https://js.foundation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
const {AST_TOKEN_TYPES, AST_NODE_TYPES} = require('@typescript-eslint/types')
const util = require('@typescript-eslint/eslint-plugin/dist/util')

const ENABLE_AUTOFIX = true

function isImportToken(token) {
  return token.type === AST_TOKEN_TYPES.Keyword && token.value === 'import'
}
function isImportSpecifierToken(token) {
  return token.type === AST_NODE_TYPES.ImportSpecifier
}

module.exports = {
  name: 'type-only-imports',
  meta: {
    messages: {
      typeOverValue: 'All imports in the declaration are only used as types. Use `import type`.',
    },
    fixable: 'code',
  },
  create(context) {
    const sourceCode = context.getSourceCode()
    const sourceImportsMap = {}
    return {
      // prefer type imports
      ImportDeclaration(node) {
        const source = node.source.value
        const sourceImports = sourceImportsMap[source] ||
                    (sourceImportsMap[source] = {
                      source,
                      reportValueImports: [],
                      typeOnlyNamedImport: null,
                      valueOnlyNamedImport: null,
                    })
        if (node.importKind === 'type') {
          if (!sourceImports.typeOnlyNamedImport && node.specifiers.every(isImportSpecifierToken)) {
            sourceImports.typeOnlyNamedImport = node
          }
        } else if (!sourceImports.valueOnlyNamedImport &&
                        node.specifiers.every(isImportSpecifierToken)) {
          sourceImports.valueOnlyNamedImport = node
        }
        const typeSpecifiers = []
        const valueSpecifiers = []
        const unusedSpecifiers = []
        for (const specifier of node.specifiers) {
          const [variable] = context.getDeclaredVariables(specifier)
          if (variable.references.length === 0) {
            unusedSpecifiers.push(specifier)
          } else {
            const onlyHasTypeReferences = variable.references.every((ref) => {
              /**
                              * keep origin import kind when export
                              * export { Type }
                              * export default Type;
                              */
              if (ref.identifier.parent && (ref.identifier.parent.type ===
                                AST_NODE_TYPES.ExportSpecifier ||
                                ref.identifier.parent.type ===
                                    AST_NODE_TYPES.ExportDefaultDeclaration)) {
                if (ref.isValueReference && ref.isTypeReference) {
                  return node.importKind === 'type'
                }
              }
              if (ref.isValueReference) {
                let {parent} = ref.identifier
                let child = ref.identifier
                while (parent) {
                  switch (parent.type) {
                    // CASE 1:
                    // `type T = typeof foo` will create a value reference because "foo" must be a value type
                    // however this value reference is safe to use with type-only imports
                    case AST_NODE_TYPES.TSTypeQuery:
                      return true
                    case AST_NODE_TYPES.TSQualifiedName:
                      // TSTypeQuery must have a TSESTree.EntityName as its child, so we can filter here and break early
                      if (parent.left !== child) {
                        return false
                      }
                      child = parent
                      parent = parent.parent
                      continue
                      // END CASE 1
                      /// ///////////
                      // CASE 2:
                      // `type T = { [foo]: string }` will create a value reference because "foo" must be a value type
                      // however this value reference is safe to use with type-only imports.
                      // Also this is represented as a non-type AST - hence it uses MemberExpression
                    case AST_NODE_TYPES.TSPropertySignature:
                      return parent.key === child
                    case AST_NODE_TYPES.MemberExpression:
                      if (parent.object !== child) {
                        return false
                      }
                      child = parent
                      parent = parent.parent
                      continue
                      // END CASE 2
                    default:
                      return false
                  }
                }
              }
              return ref.isTypeReference
            })
            if (onlyHasTypeReferences) {
              typeSpecifiers.push(specifier)
            } else {
              valueSpecifiers.push(specifier)
            }
          }
        }
        if ((node.importKind === 'value' && typeSpecifiers.length) ||
                    (node.importKind === 'type' && valueSpecifiers.length)) {
          sourceImports.reportValueImports.push({
            node,
            typeSpecifiers,
            valueSpecifiers,
            unusedSpecifiers,
          })
        }
      },
      'Program:exit': function () {
        for (const sourceImports of Object.values(sourceImportsMap)) {
          for (const report of sourceImports.reportValueImports) {
            if (report.valueSpecifiers.length === 0 &&
                            report.unusedSpecifiers.length === 0) {
              // import is all type-only, convert the entire import to `import type`
              context.report({
                node: report.node,
                messageId: 'typeOverValue',
                fix: ENABLE_AUTOFIX && (function* fix(fixer) {
                  yield* fixToTypeImport(fixer, report, sourceImports)
                }),
              })
            }
          }
        }
      },
    }
    function classifySpecifier(node) {
      const defaultSpecifier = node.specifiers[0].type === AST_NODE_TYPES.ImportDefaultSpecifier
        ? node.specifiers[0]
        : null
      const namespaceSpecifier = node.specifiers.find(specifier => specifier.type === AST_NODE_TYPES.ImportNamespaceSpecifier) || null
      const namedSpecifiers = node.specifiers.filter(specifier => specifier.type === AST_NODE_TYPES.ImportSpecifier)
      return {
        defaultSpecifier,
        namespaceSpecifier,
        namedSpecifiers,
      }
    }
    /**
         * Returns information for fixing named specifiers.
         */
    function getFixesNamedSpecifiers(fixer, node, typeNamedSpecifiers, allNamedSpecifiers) {
      if (allNamedSpecifiers.length === 0) {
        return {
          typeNamedSpecifiersText: '',
          removeTypeNamedSpecifiers: [],
        }
      }
      const typeNamedSpecifiersTexts = []
      const removeTypeNamedSpecifiers = []
      if (typeNamedSpecifiers.length === allNamedSpecifiers.length) {
        // e.g.
        // import Foo, {Type1, Type2} from 'foo'
        // import DefType, {Type1, Type2} from 'foo'
        const openingBraceToken = util.nullThrows(sourceCode.getTokenBefore(typeNamedSpecifiers[0], util.isOpeningBraceToken), util.NullThrowsReasons.MissingToken('{', node.type))
        const commaToken = util.nullThrows(sourceCode.getTokenBefore(openingBraceToken, util.isCommaToken), util.NullThrowsReasons.MissingToken(',', node.type))
        const closingBraceToken = util.nullThrows(sourceCode.getFirstTokenBetween(openingBraceToken, node.source, util.isClosingBraceToken), util.NullThrowsReasons.MissingToken('}', node.type))
        // import DefType, {...} from 'foo'
        //               ^^^^^^^ remove
        removeTypeNamedSpecifiers.push(fixer.removeRange([commaToken.range[0], closingBraceToken.range[1]]))
        typeNamedSpecifiersTexts.push(sourceCode.text.slice(openingBraceToken.range[1], closingBraceToken.range[0]))
      } else {
        const typeNamedSpecifierGroups = []
        let group = []
        for (const namedSpecifier of allNamedSpecifiers) {
          if (typeNamedSpecifiers.includes(namedSpecifier)) {
            group.push(namedSpecifier)
          } else if (group.length) {
            typeNamedSpecifierGroups.push(group)
            group = []
          }
        }
        if (group.length) {
          typeNamedSpecifierGroups.push(group)
        }
        for (const namedSpecifiers of typeNamedSpecifierGroups) {
          const {removeRange, textRange} = getNamedSpecifierRanges(namedSpecifiers, allNamedSpecifiers)
          removeTypeNamedSpecifiers.push(fixer.removeRange(removeRange))
          typeNamedSpecifiersTexts.push(sourceCode.text.slice(...textRange))
        }
      }
      return {
        typeNamedSpecifiersText: typeNamedSpecifiersTexts.join(','),
        removeTypeNamedSpecifiers,
      }
    }
    /**
         * Returns ranges for fixing named specifier.
         */
    function getNamedSpecifierRanges(namedSpecifierGroup, allNamedSpecifiers) {
      const first = namedSpecifierGroup[0]
      const last = namedSpecifierGroup[namedSpecifierGroup.length - 1]
      const removeRange = [first.range[0], last.range[1]]
      const textRange = [...removeRange]
      const before = sourceCode.getTokenBefore(first)
      textRange[0] = before.range[1]
      if (util.isCommaToken(before)) {
        removeRange[0] = before.range[0]
      } else {
        removeRange[0] = before.range[1]
      }
      const isFirst = allNamedSpecifiers[0] === first
      const isLast = allNamedSpecifiers[allNamedSpecifiers.length - 1] === last
      const after = sourceCode.getTokenAfter(last)
      textRange[1] = after.range[0]
      if (isFirst || isLast) {
        if (util.isCommaToken(after)) {
          removeRange[1] = after.range[1]
        }
      }
      return {
        textRange,
        removeRange,
      }
    }
    /**
         * insert specifiers to named import node.
         * e.g.
         * import type { Already, Type1, Type2 } from 'foo'
         *                        ^^^^^^^^^^^^^ insert
         */
    function insertToNamedImport(fixer, target, insertText) {
      const closingBraceToken = util.nullThrows(sourceCode.getFirstTokenBetween(sourceCode.getFirstToken(target), target.source, util.isClosingBraceToken), util.NullThrowsReasons.MissingToken('}', target.type))
      const before = sourceCode.getTokenBefore(closingBraceToken)
      if (!util.isCommaToken(before) && !util.isOpeningBraceToken(before)) {
        // eslint-disable-next-line no-param-reassign
        insertText = `,${insertText}`
      }
      return fixer.insertTextBefore(closingBraceToken, insertText)
    }
    function* fixToTypeImport(fixer, report, sourceImports) {
      const {node} = report
      const {defaultSpecifier, namespaceSpecifier, namedSpecifiers} = classifySpecifier(node)
      if (namespaceSpecifier && !defaultSpecifier) {
        // e.g.
        // import * as types from 'foo'
        yield* fixToTypeImportByInsertType(fixer, node, false)
        return
      } else if (defaultSpecifier) {
        if (report.typeSpecifiers.includes(defaultSpecifier) &&
                    namedSpecifiers.length === 0 &&
                    !namespaceSpecifier) {
          // e.g.
          // import Type from 'foo'
          yield* fixToTypeImportByInsertType(fixer, node, true)
          return
        }
      } else if (namedSpecifiers.every(specifier => report.typeSpecifiers.includes(specifier)) &&
                    !namespaceSpecifier) {
        // e.g.
        // import {Type1, Type2} from 'foo'
        yield* fixToTypeImportByInsertType(fixer, node, false)
        return
      }
      const typeNamedSpecifiers = namedSpecifiers.filter(specifier => report.typeSpecifiers.includes(specifier))
      const fixesNamedSpecifiers = getFixesNamedSpecifiers(fixer, node, typeNamedSpecifiers, namedSpecifiers)
      const afterFixes = []
      if (typeNamedSpecifiers.length) {
        if (sourceImports.typeOnlyNamedImport) {
          const insertTypeNamedSpecifiers = insertToNamedImport(fixer, sourceImports.typeOnlyNamedImport, fixesNamedSpecifiers.typeNamedSpecifiersText)
          if (sourceImports.typeOnlyNamedImport.range[1] <= node.range[0]) {
            yield insertTypeNamedSpecifiers
          } else {
            afterFixes.push(insertTypeNamedSpecifiers)
          }
        } else {
          yield fixer.insertTextBefore(node, `import type {${fixesNamedSpecifiers.typeNamedSpecifiersText}} from ${sourceCode.getText(node.source)};\n`)
        }
      }
      const fixesRemoveTypeNamespaceSpecifier = []
      if (namespaceSpecifier &&
                report.typeSpecifiers.includes(namespaceSpecifier)) {
        // e.g.
        // import Foo, * as Type from 'foo'
        // import DefType, * as Type from 'foo'
        // import DefType, * as Type from 'foo'
        const commaToken = util.nullThrows(sourceCode.getTokenBefore(namespaceSpecifier, util.isCommaToken), util.NullThrowsReasons.MissingToken(',', node.type))
        // import Def, * as Ns from 'foo'
        //           ^^^^^^^^^ remove
        fixesRemoveTypeNamespaceSpecifier.push(fixer.removeRange([commaToken.range[0], namespaceSpecifier.range[1]]))
        // import type * as Ns from 'foo'
        // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ insert
        yield fixer.insertTextBefore(node, `import type ${sourceCode.getText(namespaceSpecifier)} from ${sourceCode.getText(node.source)};\n`)
      }
      if (defaultSpecifier &&
                report.typeSpecifiers.includes(defaultSpecifier)) {
        if (report.typeSpecifiers.length === node.specifiers.length) {
          const importToken = util.nullThrows(sourceCode.getFirstToken(node, isImportToken), util.NullThrowsReasons.MissingToken('import', node.type))
          // import type Type from 'foo'
          //        ^^^^ insert
          yield fixer.insertTextAfter(importToken, ' type')
        } else {
          const commaToken = util.nullThrows(sourceCode.getTokenAfter(defaultSpecifier, util.isCommaToken), util.NullThrowsReasons.MissingToken(',', defaultSpecifier.type))
          // import Type , {...} from 'foo'
          //        ^^^^^ pick
          const defaultText = sourceCode.text
            .slice(defaultSpecifier.range[0], commaToken.range[0])
            .trim()
          yield fixer.insertTextBefore(node, `import type ${defaultText} from ${sourceCode.getText(node.source)};\n`)
          const afterToken = util.nullThrows(sourceCode.getTokenAfter(commaToken, {includeComments: true}), util.NullThrowsReasons.MissingToken('any token', node.type))
          // import Type , {...} from 'foo'
          //        ^^^^^^^ remove
          yield fixer.removeRange([
            defaultSpecifier.range[0],
            afterToken.range[0],
          ])
        }
      }
      yield* fixesNamedSpecifiers.removeTypeNamedSpecifiers
      yield* fixesRemoveTypeNamespaceSpecifier
      yield* afterFixes
    }
    function* fixToTypeImportByInsertType(fixer, node, isDefaultImport) {
      // import type Foo from 'foo'
      //       ^^^^^ insert
      const importToken = util.nullThrows(sourceCode.getFirstToken(node, isImportToken), util.NullThrowsReasons.MissingToken('import', node.type))
      yield fixer.insertTextAfter(importToken, ' type')
      if (isDefaultImport) {
        // Has default import
        const openingBraceToken = sourceCode.getFirstTokenBetween(importToken, node.source, util.isOpeningBraceToken)
        if (openingBraceToken) {
          // Only braces. e.g. import Foo, {} from 'foo'
          const commaToken = util.nullThrows(sourceCode.getTokenBefore(openingBraceToken, util.isCommaToken), util.NullThrowsReasons.MissingToken(',', node.type))
          const closingBraceToken = util.nullThrows(sourceCode.getFirstTokenBetween(openingBraceToken, node.source, util.isClosingBraceToken), util.NullThrowsReasons.MissingToken('}', node.type))
          // import type Foo, {} from 'foo'
          //                  ^^ remove
          yield fixer.removeRange([
            commaToken.range[0],
            closingBraceToken.range[1],
          ])
          const specifiersText = sourceCode.text.slice(commaToken.range[1], closingBraceToken.range[1])
          if (node.specifiers.length > 1) {
            // import type Foo from 'foo'
            // import type {...} from 'foo' // <- insert
            yield fixer.insertTextAfter(node, `\nimport type${specifiersText} from ${sourceCode.getText(node.source)};`)
          }
        }
      }
    }
  },
}
