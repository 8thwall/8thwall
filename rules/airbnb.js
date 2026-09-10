/*
Merges the rule modules of eslint-config-airbnb / eslint-config-airbnb-base.

NOTE(christoph): Those packages are stuck on eslintrc format, which ESLint 10 dropped, so
"extends: airbnb" no longer works. This collects their rules directly instead. Rules that no
longer exist in ESLint 10 are dropped, and anything stylistic is neutralized by spreading
eslint-config-prettier after this object.
*/

const {builtinRules} = require('eslint/use-at-your-own-risk')
const react = require('eslint-plugin-react')
const reactHooks = require('eslint-plugin-react-hooks')
const jsxA11y = require('eslint-plugin-jsx-a11y')
const importPlugin = require('eslint-plugin-import')
const eslintConfigAirbnbBase = require('eslint-config-airbnb-base')
const eslintConfigAirbnb = require('eslint-config-airbnb')

const AIRBNB_RULE_FILES = [...eslintConfigAirbnbBase.extends, ...eslintConfigAirbnb.extends]

const PLUGIN_RULE_SOURCES = {
  'import/': importPlugin,
  'react-hooks/': reactHooks,
  'jsx-a11y/': jsxA11y,
  'react/': react,
}

const collectAirbnbRules = () => {
  const merged = {}
  for (const file of AIRBNB_RULE_FILES) {
    // eslint-disable-next-line global-require, import/no-dynamic-require
    for (const [id, config] of Object.entries(require(file).rules)) {
      const source = Object.entries(PLUGIN_RULE_SOURCES).find(([prefix]) => id.startsWith(prefix))
      const exists = source ? !!source[1].rules[id.slice(source[0].length)] : builtinRules.has(id)
      if (exists) {
        merged[id] = config
      }
    }
  }
  return merged
}

module.exports = collectAirbnbRules()
