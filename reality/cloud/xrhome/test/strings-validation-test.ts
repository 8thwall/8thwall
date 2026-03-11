import chai, {assert} from 'chai'
import fs from 'fs'
import path from 'path'

import {getSupportedLocales8w, SupportedLocale8w} from '../src/shared/i18n/i18n-locales'

chai.should()

enum I18nEnvironment {
  CLIENT = 'client',
  SERVER = 'server',
}

const buildNamespaceToLocaleMap = (env: I18nEnvironment): Record<string, SupportedLocale8w[]> => {
  const map: Record<string, SupportedLocale8w[]> = {}

  const locales = getSupportedLocales8w() as SupportedLocale8w[]
  const localesDirectories = locales.map(loc => (
    path.join(__dirname, `../src/${env}/i18n/${loc}`)
  ))

  localesDirectories.forEach((localeDir) => {
    const files = fs.readdirSync(localeDir)
    const namespaces = files.map((file) => {
      if (path.extname(file) !== '.json') {
        return null
      }
      return file.replace('.json', '')
    }).filter(ns => ns !== null) as string[]

    namespaces.forEach((ns) => {
      // Return a list of locales that have existing files.
      map[ns] = locales.filter((locale) => {
        const filePath = path.join(
          __dirname, `../src/${env}/i18n/${locale}/${ns}.json`
        )
        return fs.existsSync(filePath)
      })
    })
  })
  return map
}

const getTagName = (tag: string): string | null => {
  const match = tag.match(/<\/?\s*([a-zA-Z0-9]+)/)
  return match ? match[1] : null
}

const areBracketsBalanced = (str: string) => {
  const stack = [] as string[]
  const brackets = {
    '{': '}',
  }

  for (const char of str) {
    if (brackets[char]) {
      stack.push(char)
    } else if (Object.values(brackets).includes(char)) {
      const last = stack.pop()
      if (brackets[last] !== char) {
        return false
      }
    }
  }

  if (stack.length !== 0) {
    return false
  }

  return true
}

const areTagsComplete = (str: string) => {
  const tags = str.match(/<\/?[a-zA-Z0-9]+\s*[^>]*>/g) || []
  const stack: string[] = []

  for (const tag of tags) {
    const isClosingTag = tag.startsWith('</')
    const isSelfClosingTag = tag.endsWith('/>')

    if (isClosingTag) {
      const tagName = getTagName(tag)  // Extract the tag name
      if (stack.length === 0 || stack[stack.length - 1] !== tagName) {
        return false  // Either no matching opening tag or incorrect nesting
      }
      stack.pop()  // Remove the matching opening tag
    } else if (isSelfClosingTag) {
      // If self closing tag do not add to stack
    } else {
      const tagName = getTagName(tag)  // Extract the tag name
      stack.push(tagName)
    }
  }

  return stack.length === 0
}

describe('Strings Validation Test - Verify all i18n strings are valid', () => {
  describe('Client side translations', () => {
    const NAMESPACE_TO_LOCALES_MAP = buildNamespaceToLocaleMap(I18nEnvironment.CLIENT)

    Object.keys(NAMESPACE_TO_LOCALES_MAP).forEach((namespace: string) => {
      describe(`For the namespace ${namespace}`, () => {
        let localeToString = {}

        before(() => {
          NAMESPACE_TO_LOCALES_MAP[namespace].forEach((locale) => {
            const filePath = path.join(
              __dirname, `../src/client/i18n/${locale}/${namespace}.json`
            )

            try {
              const content = fs.readFileSync(filePath, 'utf8')
              localeToString[locale] = JSON.parse(content)
            } catch (err) {
              // if file doesn't exist or fails to read, do not add anything to localeToString.
            }
          })
        })

        after(() => {
          localeToString = {}
        })

        NAMESPACE_TO_LOCALES_MAP[namespace].forEach((locale) => {
          // Test for unbalanced brackets
          describe(`For locale ${locale}`, () => {
            it('should have balanced brackets', () => {
              const translations: Record<string, string> = localeToString[locale]

              Object.entries(translations).forEach(([key, translation]) => {
                assert.isTrue(
                  areBracketsBalanced(translation), `Invalid translation. Associated key: ${key}`
                )
              })
            })

            it('should have complete and balanced tags', () => {
              const translations: Record<string, string> = localeToString[locale]

              Object.entries(translations).forEach(([key, translation]) => {
                assert.isTrue(
                  areTagsComplete(translation),
                  `Invalid translation. Associated key: ${key}`
                )
              })
            })

            it('should have a similar structure of placeholders in all locales', () => {
              const translations: Record<string, string> = localeToString[locale]

              NAMESPACE_TO_LOCALES_MAP[namespace]
                .filter(loc => loc !== locale)
                .forEach((otherLocale) => {
                  const otherTranslations: Record<string, string> = localeToString[otherLocale]

                  Object.entries(translations).forEach(([key, translation]) => {
                    const otherTranslation = otherTranslations[key]
                    if (otherTranslation === undefined) {
                      return
                    }
                    const tagPattern = /<\/?[a-zA-Z0-9]+\s*[^>]*>/g
                    const placeholders = translation.match(tagPattern) || []
                    const otherPlaceholders = otherTranslation.match(tagPattern) || []

                    assert.sameMembers(
                      placeholders, otherPlaceholders,
                      // eslint-disable-next-line max-len
                      `Placeholders mismatch for key: ${key} in locales ${locale} and ${otherLocale}`
                    )
                  })
                })
            })
          })
        })
      })
    })
  })

  // TODO(Brandon): Create Server side translation tests
})
