module.exports = {
  rules: {
    'local-rules/hardcoded-copy': 'warn',
    'local-rules/i18n-nesting': 'error',
  },
  overrides: [
    {
      files: ['arcade/**/*.*'],
      rules: {
        'local-rules/hardcoded-copy': 'off',
        'local-rules/i18n-nesting': 'off',
      },
    },
  ],
}
