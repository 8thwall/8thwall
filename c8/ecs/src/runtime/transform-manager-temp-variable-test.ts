// @package(npm-ecs)
// @attr[](data = "transform-manager.ts")
// @attr(esnext = 1)
// @attr(externalize_npm = 1)
import {describe, it, assert} from '@repo/bzl/js/chai-js'

import {parse} from '@babel/parser'
import type {ArrowFunctionExpression, VariableDeclaration} from '@babel/types'

import path from 'path'
import fs from 'fs'

// NOTE(christoph) The purpose of this test is to make sure the transform manager uses temporary
// variables safely. If functionA uses tempVariable1, and calls functionB that also
// uses tempVariable1, that's an error.

// For each function within the manager, we store:
type PerDeclarationInfo = {
  code: string  // Source code
  usedTemps: Set<string>  // Temp variables used directly
  callsFunctions: Set<string>  // Functions called directly
  recursiveCallsFunctions: Set<string>  // Functions called directly or indirectly
  recursiveUsedTemps: Set<string>  // Temp variables used by any of the recursiveCallsFunctions
}

const sourcePath = path.join(
  process.env.RUNFILES_DIR!,
  '_main/c8/ecs/src/runtime/transform-manager.ts'
)

describe('transform-manager.ts', () => {
  const code = fs.readFileSync(sourcePath, 'utf8')
  const parsed = parse(code, {
    sourceType: 'module',
    plugins: ['typescript'],
  })

  const managerFactoryNode = parsed.program.body.find(
    (node): node is VariableDeclaration => node.type === 'VariableDeclaration' &&
      node.declarations[0].id.type === 'Identifier' &&
      node.declarations[0].id.name === 'createTransformManager'
  )

  if (!managerFactoryNode) {
    throw new Error('Could not find createTransformManager')
  }

  const factoryFunction = managerFactoryNode.declarations[0].init as ArrowFunctionExpression

  if (factoryFunction.body.type !== 'BlockStatement') {
    throw new Error('Expected a block statement')
  }

  const tempVariables = new Set<string>()
  parsed.program.body.forEach((node) => {
    if (node.type === 'VariableDeclaration') {
      node.declarations.forEach((declaration) => {
        if (declaration.id.type === 'Identifier' && declaration.id.name.startsWith('temp')) {
          tempVariables.add(declaration.id.name)
        }
      })
    }
  })

  assert.isNotEmpty(tempVariables)

  const info = new Map<string, PerDeclarationInfo>()

  factoryFunction.body.body.filter(e => e.type === 'VariableDeclaration').forEach((node) => {
    if (node.declarations[0].id.type === 'Identifier' &&
        node.declarations[0].init && node.declarations[0].init.type === 'ArrowFunctionExpression') {
      const {name} = node.declarations[0].id

      const functionCode = code.slice(
        node.declarations[0].init.body.start!,
        node.declarations[0].init.body.end!
      )

      info.set(name, {
        usedTemps: new Set<string>(),
        code: functionCode,
        callsFunctions: new Set<string>(),
        recursiveCallsFunctions: new Set<string>(),
        recursiveUsedTemps: new Set<string>(),
      })
    }
  })

  const functionNames = new Set(info.keys())

  assert.isNotEmpty(functionNames)

  // Parse the code to see directly used temp variables and called functions
  info.forEach((value) => {
    tempVariables.forEach((temp) => {
      if (value.code.includes(temp)) {
        value.usedTemps.add(temp)
      }
    })

    functionNames.forEach((key) => {
      if (value.code.includes(`${key}(`)) {
        value.callsFunctions.add(key)
      }
    })
  })

  // Recurse into all functions to populate recursiveCallsFunctions
  info.forEach((value) => {
    const toVisit = [...value.callsFunctions]

    while (toVisit.length > 0) {
      const current = toVisit.pop()!
      if (!value.recursiveCallsFunctions.has(current)) {
        value.recursiveCallsFunctions.add(current)
        info.get(current)!.callsFunctions.forEach((f) => {
          toVisit.push(f)
        })
      }
    }
  })

  // Populate recursiveUsedTemps based on recursiveCallsFunctions
  info.forEach((value) => {
    value.recursiveCallsFunctions.forEach((f) => {
      info.get(f)!.usedTemps.forEach((temp) => {
        value.recursiveUsedTemps.add(temp)
      })
    })
  })

  info.forEach((value, key) => {
    describe(key, () => {
      for (const temp of tempVariables) {
        const usedDirectly = value.usedTemps.has(temp)
        const usedRecursively = value.recursiveUsedTemps.has(temp)

        let message: string
        if (usedDirectly) {
          message = `${temp} is used directly`
        } else if (usedRecursively) {
          message = `${temp} is used recursively`
        } else {
          message = `${temp} is not used`
        }

        it(message, () => {
          assert.isFalse(usedDirectly && usedRecursively)
        })
      }
    })
  })
})
