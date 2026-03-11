const path = require('path')

const DISALLOWED_PREFIX = '@repo/reality/shared/'

// NOTE(christoph): This list reflects what is present in the reality/shared directory
// eslint-disable-next-line max-len
// Get all the subpaths with: ls reality/shared  | sed 's|\..*||g' | sort | grep -v "BUILD" | grep -v test | sed -E "s|(.*)|'\1',|g"
const ALLOWED_SUBPATHS = [
  'asset-pointer',
  'convert-case',
  'desktop',
  'dynamodb',
  'dynamodb-impl',
  'gateway',
  'html',
  'integration',
  'module',
  'nae',
  'partition-info',
  'run-queue',
  's3',
  's3-impl',
  'studio',
  'typed-attributes',
]

const fixImportPath = (filename, pathWithinShared) => {
  const pathParts = filename.split(path.sep)
  const srcIndex = pathParts.lastIndexOf('src')
  if (srcIndex === -1) {
    return null
  }
  const srcPath = pathParts.slice(0, srcIndex + 1).join(path.sep)
  const targetPath = path.join(srcPath, 'shared', pathWithinShared)
  return path.relative(path.dirname(filename), targetPath)
}

const checkImport = (node, context) => {
  const source = node.source.value
  if (source.startsWith(DISALLOWED_PREFIX)) {
    const pathWithinShared = source.substring(DISALLOWED_PREFIX.length)
    const subpath = pathWithinShared.split('/')[0]
    if (!ALLOWED_SUBPATHS.includes(subpath)) {
      const fixedPath = fixImportPath(context.getFilename(), pathWithinShared)
      context.report({
        node: node.source,
        messageId: 'invalidMessage',
        fix: fixedPath && (fixer => fixer.replaceText(node.source, `'${fixedPath}'`)),
      })
    }
  }
}

module.exports = {
  name: 'reality-shared-imports',
  meta: {
    fixable: 'code',
    messages: {
      invalidMessage:
        'Imports of @repo/reality/shared are only valid if the file exists in reality/shared',
    },
  },
  create(context) {
    return {
      ImportDeclaration: node => checkImport(node, context),
    }
  },
}
