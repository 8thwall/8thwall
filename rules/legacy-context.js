/*
Wraps plugin rules whose implementations still call ESLint APIs that were removed from rule
contexts and SourceCode (e.g. context.getFilename, sourceCode.getTokenOrCommentAfter).

NOTE(christoph): Some plugins don't fully support ESLint 10 yet. Wrap their rules with small
shims until upstream support lands.
*/

const LEGACY_SOURCE_FNS = sourceCode => ({
  getTokenOrCommentAfter: token => sourceCode.getTokenAfter(token, {includeComments: true}),
  getTokenOrCommentBefore: token => sourceCode.getTokenBefore(token, {includeComments: true}),
  getComments: node => ({
    leading: sourceCode.getCommentsBefore(node),
    trailing: sourceCode.getCommentsAfter(node),
  }),
})

const wrapRule = rule => {
  if (!rule || typeof rule.create !== 'function' || rule.__legacyContextCompat) {
    return
  }
  const {create} = rule
  rule.create = context => {
    const sourceProxy = new Proxy(context.sourceCode, {
      get(target, prop) {
        const fns = LEGACY_SOURCE_FNS(target)
        if (prop in fns && !(prop in target)) {
          return fns[prop]
        }
        return Reflect.get(target, prop)
      },
    })
    const contextProxy = new Proxy(context, {
      get(target, prop) {
        // NOTE(christoph): Intercept unconditionally since helpers like eslint-module-utils'
        // getSourceCode check `'sourceCode' in context` before falling back to the method.
        if (prop === 'sourceCode') {
          return sourceProxy
        }
        if (prop === 'getSourceCode') {
          return () => sourceProxy
        }
        if (prop === 'getFilename') {
          return () => target.filename
        }
        if (prop === 'getPhysicalFilename') {
          return () => target.physicalFilename
        }
        return Reflect.get(target, prop)
      },
    })
    return create.call(rule, contextProxy)
  }
  rule.__legacyContextCompat = true
}

const wrapLegacyContextRules = plugin => {
  for (const rule of Object.values(plugin.rules || {})) {
    wrapRule(rule)
  }
  return plugin
}

module.exports = {wrapLegacyContextRules}
