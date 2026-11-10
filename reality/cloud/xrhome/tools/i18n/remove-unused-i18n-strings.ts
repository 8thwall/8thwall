/* eslint-disable no-console */
import * as fs from 'fs'
import * as path from 'path'

// Usage: ./remove-unused-i18n-strings.ts <json-file-path> <dry-run>

const VALID_FILE_EXTENSIONS = ['.ts', '.js', '.jsx', '.tsx']

// Path to the codebase directory
const XRHOME_SRC_PATH = './src'
const EXCLUDED_TRANSLATION_FILES = new Set([
  'studio-tooltips.json',
  'common.json',
])

type JsonData = Record<string, string>

// Function to load JSON and extract all keys
const loadJsonKeys = (jsonFilePath: string): {jsonData: JsonData, keys: string[]} => {
  const jsonData = JSON.parse(fs.readFileSync(jsonFilePath, 'utf-8'))
  return {jsonData, keys: Object.keys(jsonData)}
}

// Function to search for JSON keys in the codebase
const searchKeysInCodebase = (keys: string[], codebaseDir: string): Set<string> => {
  const matchedKeys = new Set<string>()

  const searchFileForKeys = (filePath: string): void => {
    const fileContent = fs.readFileSync(filePath, 'utf-8')
    for (const key of keys) {
      const base = key.replace(/_(one|other)$/, '')
      const lastDot = key.lastIndexOf('.')
      const templatePrefix = lastDot === -1 ? undefined : `${key.slice(0, lastDot + 1)}\${`
      if (fileContent.includes(base) || (templatePrefix && fileContent.includes(templatePrefix))) {
        matchedKeys.add(key)
      }
    }
  }

  const walkDirectory = (dir: string) => {
    const files = fs.readdirSync(dir)
    for (const file of files) {
      const fullPath = path.join(dir, file)
      const stat = fs.lstatSync(fullPath)

      if (stat.isDirectory()) {
        // eslint-disable-next-line no-await-in-loop
        walkDirectory(fullPath)  // Recursively go through directories
      } else if (VALID_FILE_EXTENSIONS.includes(path.extname(file))) {
        // eslint-disable-next-line no-await-in-loop
        searchFileForKeys(fullPath)
      }
    }
  }

  walkDirectory(codebaseDir)
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
  jsonFilePath: string, codebaseDir: string, isDryRun: boolean
) => {
  if (EXCLUDED_TRANSLATION_FILES.has(path.basename(jsonFilePath))) {
    console.log(`Skipping generated translation file ${jsonFilePath}.`)
    return
  }

  if (isDryRun) {
    console.log('\n***********************')
    console.log('** THIS IS A DRY RUN **')
    console.log('***********************\n')
  }

  // Load JSON keys and the original data
  const {jsonData, keys} = loadJsonKeys(jsonFilePath)

  // Search for keys in the codebase
  const matchedKeys = searchKeysInCodebase(keys, codebaseDir)

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
    fs.writeFileSync(jsonFilePath, `${JSON.stringify(updatedJsonData, null, 2)}\n`, 'utf-8')
    console.log(`Unmatched keys have been removed from ${jsonFilePath}.`)
  } else {
    console.log('All JSON keys are referenced in the codebase.')
  }
}

const isDryRun = process.argv.includes('--dry-run')
process.argv.slice(2).filter(e => !e.startsWith('--')).map(
  e => removeUnusedI18nStrings(e, XRHOME_SRC_PATH, isDryRun)
)
