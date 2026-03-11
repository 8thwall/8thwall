import memoize from 'memoizee'
import path from 'path'
import fs from 'fs'

import type {BuildIfEnv, DefinitionKey, DefinitionRecord} from '../../src/shared/buildif'
import {extractBuildIf} from '../../src/shared/extract-buildif'

const DEFINITION_VALUE_LOOKUP = {
  'true': true,
  'false': false,
}

const getCurrentBuildIfValues = (): DefinitionRecord => {
  const content = fs.readFileSync(path.resolve(__dirname, '../../src/shared/buildif.ts'), 'utf8')
  const definitionPart = content.split('const definitions = {')[1].split('} as const')[0]

  const res = {} as DefinitionRecord

  definitionPart.split('\n')
    .map(e => e.trim().replace(/,.*$/, ''))
    .filter(e => e && !e.startsWith('//'))
    .forEach((line) => {
      const [key, stringValue] = line.split(':').map(e => e.trim())
      const value = DEFINITION_VALUE_LOOKUP[stringValue] ?? stringValue.replace(/'/g, '')
      res[key] = value
    })

  return res
}

const createLiveBuildIfReplacements = (env: BuildIfEnv, overrides: DefinitionRecord) => {
  const getReplacement = memoize((key: DefinitionKey) => {
    if (overrides[key] !== undefined) {
      return overrides[key]
    }
    return extractBuildIf(getCurrentBuildIfValues(), env).find(e => e.flag === key)?.value
  }, {
    maxAge: 1000,
  })

  return [
    {
      search: /BuildIf\.(\w+)/g,
      replace: (match: string, flag: DefinitionKey) => getReplacement(flag) ?? match,
    },
  ]
}

export {
  createLiveBuildIfReplacements,
}
