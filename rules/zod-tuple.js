// NOTE(kyle): Zod tuples are not supported by zod-to-json-schema.
// They should be replaced by Zod arrays.
module.exports = {
  name: 'zod-tuple',
  meta: {
    type: 'problem',
    messages: {
      noZodTuple: 'Zod tuple not allowed, use Zod array.',
    },
  },
  create(context) {
    return {
      CallExpression(node) {
        if (node.callee.type === 'MemberExpression') {
          const {object} = node.callee
          const {property} = node.callee
          if (object.name === 'z' && property.name === 'tuple') {
            context.report({
              node,
              messageId: 'noZodTuple',
            })
          }
        }
      },
    }
  },
}
