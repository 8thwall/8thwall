/* eslint-disable global-require */

const defaultRules = {
  'acronym-capitalization': require('./acronym-capitalization'),
  'commonjs': require('./commonjs'),
  'implicit-any': require('./implicit-any'),
  'inline-comment-spacing': require('./inline-comment-spacing'),
  'multiline-ternary': require('./multiline-ternary'),
  'prefer-await': require('./prefer-await'),
  'type-only-imports': require('./type-only-imports'),
  'typedef-separators': require('./typedef-separators'),
  'underscore-argument': require('./underscore-argument'),
  'untyped-array': require('./untyped-array'),
}

const optionalRules = {
  'export-request-handler': require('./export-request-handler'),
  'express-deprecated-send': require('./express-deprecated-send'),
  'hardcoded-copy': require('./hardcoded-copy'),
  'i18n-nesting': require('./i18n-nesting'),
  'reality-shared-imports': require('./reality-shared-imports'),
  'ui-component-styling': require('./ui-component-styling'),
  'zod-tuple': require('./zod-tuple'),
}

const plugin = {
  meta: {
    namespace: 'local-rules',
  },
  configs: {

  },
  rules: {...defaultRules, ...optionalRules},
  processors: {},
}

plugin.configs.default = {
  plugins: {[plugin.meta.namespace]: plugin},
  rules: Object.fromEntries(
    Object.keys(defaultRules)
      .map(e => ([`${plugin.meta.namespace}/${e}`, 'error']))
  ),
}

plugin.configs.overrides = {
  rules: require('./overrides.json'),
}

module.exports = plugin
