// Replace all dev-related strings with whether or not this is the dev environment.
const buildIfValueToBool = (value, env) => {
  switch (value) {
    case 'isQa':
    case 'isDev':
      return !!(env.isLocalDev || env.isRemoteDev)
    case 'isLocalDev':
    case 'isLocal':
      return !!env.isLocalDev
    case 'isRemoteDev':
      return !!env.isRemoteDev
    case 'isTest':
      return !!env.isTest
    case true:
    case false:
      return value
    case 'isExperimental':
      return env.flagLevel === 'experimental'
    case 'isMature':
      return env.flagLevel === 'mature' || env.flagLevel === 'experimental'
    default:
      throw new Error(`Unknown buildif value: "${value}"`)
  }
}

const extractBuildIf = (buildIf, env) => {
  if (!['experimental', 'mature', 'launch'].includes(env.flagLevel)) {
    throw new Error('Missing or invalid env.flagLevel')
  }

  if ([env.isLocalDev, env.isRemoteDev, env.isTest].some(e => typeof e !== 'boolean')) {
    throw new Error('Missing or invalid env property')
  }

  return Object.keys(buildIf).map(flag => ({
    flag, value: buildIfValueToBool(buildIf[flag], env),
  }))
}

module.exports = {
  extractBuildIf,
}
