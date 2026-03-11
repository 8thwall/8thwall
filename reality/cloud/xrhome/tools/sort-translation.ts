import fs from 'fs'

// `  "foo.bar.baz": "blah"` => `foo.bar` (the last element is not considered for sorting purposes)
// `  "foo": "blah"` => `foo`
const getSortable = (line: string) => line.match(/"([^"]+)"/)[1].replace(/\.[^.]+$/, '')

const sortTranslationFile = (filePath: string) => {
  const data = fs.readFileSync(filePath, 'utf8')
  const keys: Record<string, string> = JSON.parse(data)
  const isExpectedFormat = keys &&
                           typeof keys === 'object' &&
                           Object.values(keys).every(value => typeof value === 'string')

  if (!isExpectedFormat) {
    throw new Error(`File is not in expected format: ${filePath}`)
  }

  const lines = data.split('\n').filter(e => e.match(/^ *"/)).map(e => e.replace(/(,$|^ +)/g, ''))

  const sortedLines = lines.sort((left, right) => {
    const sortableLeft = getSortable(left)
    const sortableRight = getSortable(right)
    if (sortableLeft < sortableRight) {
      return -1
    } else if (sortableLeft > sortableRight) {
      return 1
    } else {
      return 0
    }
  })

  const sortedText = `{
  ${sortedLines.join(',\n  ')}
}
`

  fs.writeFileSync(filePath, sortedText)
}

process.argv.slice(2).forEach(sortTranslationFile)
