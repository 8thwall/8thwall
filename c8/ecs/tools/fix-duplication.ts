// @rule(js_cli)
// @attr(esnext = True)

// bazel run //c8/ecs/tools:fix-duplication -- path/to/input.json  > path/to/output.json

import {promises as fs} from 'fs'

// https://github.com/automerge/automerge/issues/951

// Convert "duplicatedduplicated" into "duplicated"
const maybeFixDuplicatedString = (str: string): string => {
  const len = str.length
  if (len % 2 > 0) {
    return str
  }
  const half = len / 2
  const firstPart = str.slice(0, half)
  const secondPart = str.slice(half)
  // NOTE(christoph): This doesn't cover strings that are are supposed to be duplicated,
  // for example 'haha' would be rewritten to 'ha'. Better would be to compare with a previous
  // state if available.
  if (firstPart === secondPart) {
    return firstPart
  }
  return str
}

// Convert {"something", "duplicatedduplicated"} into {"something", "duplicated"}
const fixDuplication = (data: any): any => {
  switch (typeof data) {
    case 'string':
      return maybeFixDuplicatedString(data)
    case 'object': {
      if (!data) {
        return data
      }
      if (Array.isArray(data)) {
        return data.map(item => fixDuplication(item))
      }
      const newData = {}
      Object.keys(data).forEach((key) => {
        newData[key] = fixDuplication(data[key])
      })
      return newData
    }
    default:
      return data
  }
}

const file = process.argv[2]

const content = await fs.readFile(file, 'utf-8')

const fileData = JSON.parse(content)

const deduplicated = fixDuplication(fileData)

// eslint-disable-next-line no-console
console.log(JSON.stringify(deduplicated, null, 2))
