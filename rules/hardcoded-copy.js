const nonTranslatedStrings = new Set([
  '',
  '8th Wall',
  'Ace',
  'AAB (Android App Bundle)',
  'Android',
  'APK (Android Package)',
  'Apple',
  'Authorization',
  'Content-Type',
  'Emacs',
  'Facebook',
  'GitHub',
  'Google',
  'Instagram',
  'LinkedIn',
  'Monaco',
  'Niantic Lightship',
  'noopener noreferrer',
  'Samsung',
  'Stripe',
  'Twitter',
  'Verizon',
  'Vim',
  'YouTube',
])

const looksLikeCopy = str => (
  !nonTranslatedStrings.has(str.trim()) &&
  str.match(/((\b[A-Z][a-z]+\b)|[a-zA-Z] [a-zA-Z])/) &&  // Sentence case or has spaces in it
  !str.match(/\d(r?em|px)\b|M ?\d/)  // Ignore pixel/em values in css and SVG paths
)

// Ignore
//   className='some classes'
//   <Button icon='example icon name' />
//   <Icon name='example icon name' />
const nodeIsIgnoredAttribute = node => (
  node.type === 'JSXAttribute' && (
    node.name.name === 'className' ||
    node.name.name === 'icon' ||
    node.name.name === 'viewBox' ||
    (node.name.name === 'name' && node.parent.name.name === 'Icon')
  )
)

// Ignore console.log('Blah')
const nodeIsLog = node => (
  node.type === 'CallExpression' &&
  node.callee.type === 'MemberExpression' &&
  node.callee.object.name === 'console'
)

// Ignore <Trans>This is a placeholder</Trans>
const nodeIsTrans = node => (
  node.type === 'JSXElement' &&
  node.openingElement.name.name === 'Trans'
)

// Ignore new Error('This is an error')
const nodeIsErrorConstructor = node => (
  node.type === 'NewExpression' &&
  node.callee.name?.endsWith('Error')
)

const nodeIsPath = node => (
  node.type === 'JSXElement' &&
  node.openingElement.name.name === 'path'
)

// Ignore createUseStyles({myClass: {padding: '0.5rem 1rem'}})
const nodeIsJss = node => (
  node.type === 'CallExpression' &&
  (!!node.callee.name?.match(/^create.*Styles$/) || nodeIsJss(node.callee))
)

const IGNORED_TS_NODES = [
  'TypeAliasDeclaration', 'InterfaceDeclaration', 'TSTypeAliasDeclaration', 'TSTypeAnnotation',
]

// Ignore type Direction = 'top left' | 'bottom right'
const nodeIsTypeDefinition = node => (
  IGNORED_TS_NODES.includes(node.type)
)

// These props are surfaced to the user and should always be translated.
const userVisibleProps = new Set([
  'alt',
  'aria-label',
  'content',
  'text',
])

const nodeIsUserVisibleAttribute = node => (
  node.type === 'JSXAttribute' && node.name && userVisibleProps.has(node.name.name)
)

const nodeIsIgnored = node => (
  nodeIsIgnoredAttribute(node) ||
  nodeIsLog(node) ||
  nodeIsTrans(node) ||
  nodeIsJss(node) ||
  nodeIsTypeDefinition(node) ||
  nodeIsErrorConstructor(node) ||
  nodeIsPath(node) ||
  (node.parent && !nodeIsUserVisibleAttribute(node.parent) && nodeIsIgnored(node.parent))
)

module.exports = {
  name: 'hardcoded-copy',
  meta: {
    messages: {
      hardCodedCopyDetected: 'Hardcoded copy: {{value}}',
    },
  },
  create(context) {
    const report = (node, text) => {
      const trimmedText = text.trim().replace(/\s+/g, ' ')
      const clippedText = trimmedText.length > 20 ? `${trimmedText.substr(0, 17)}...` : trimmedText
      context.report({
        node,
        messageId: 'hardCodedCopyDetected',
        data: {
          value: clippedText,
        },
      })
    }

    return {
      Literal(node) {
        if (typeof node.value !== 'string' || nodeIsIgnored(node)) {
          return
        }
        if (looksLikeCopy(node.value)) {
          report(node, node.value)
        }
      },
      JSXAttribute(node) {
        if (nodeIsUserVisibleAttribute(node)) {
          if (node.value.type === 'Literal' && !nonTranslatedStrings.has(node.value.value)) {
            report(node.value, node.value.value)
          }
        }
      },
      JSXText(node) {
        if (node.containsOnlyTriviaWhiteSpaces || nodeIsIgnored(node)) {
          return
        }
        if (looksLikeCopy(node.value)) {
          report(node, node.value)
        }
      },
      TemplateLiteral(node) {
        if (nodeIsIgnored(node)) {
          return
        }
        const copyQuasi = node.quasis.find(q => looksLikeCopy(q.value.raw))
        if (copyQuasi) {
          report(node, copyQuasi.value.raw)
        }
      },
    }
  },
}
