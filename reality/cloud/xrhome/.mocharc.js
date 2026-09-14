'use strict'

module.exports = {
  diff: true,
  extension: ['js', 'ts'],
  "node-option": ["import=tsx"],
  slow: 75,
  timeout: 2000,
  colors: true,
  parallel: true,
  'watch-files': ['test/*test.js', 'test/*test.ts'],
}
