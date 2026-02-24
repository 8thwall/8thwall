const CHAINING_METHODS = ['then', 'catch', 'finally']

module.exports = {
  name: 'prefer-await',
  meta: {
    type: 'suggestion',
    messages: {
      noPromiseChaining: 'Prefer await syntax over chaining using .{{method}}',
    },
  },
  create(context) {
    const checks = {}

    CHAINING_METHODS.forEach((method) => {
      checks[`MemberExpression > Identifier[name = ${method}]`] = (node) => {
        context.report({
          node,
          messageId: 'noPromiseChaining',
          data: {
            method,
          },
        })
      }
    })

    return checks
  },
}
