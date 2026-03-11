// @visibility(//visibility:public)

const SERVE_HOST_RE = /^(.+)\.(8thwall(|dev|staging|qa|int|dog)\.app|chickenprincess\.org)$/

// Domain looks like:
//   cmb-default-8w.dev.8thwall.app
//   master-8w.dev.8thwall.app
//   8w.staging.8thwall.app
//   8w.8thwall.app
//   user1-guestaccount-clientname-hostaccount.dev.8thwall.app
//   build-id-workspace.build.8thwall.app
const parseHostingDomain = (domain: string) => {
  const domainMatch = domain && domain.match(SERVE_HOST_RE)
  if (!domainMatch) {
    return null
  }

  const subdomains = domainMatch[1].split('.')
  const lastSubdomain = subdomains[subdomains.length - 1]

  if (subdomains.length === 1) {
    return {
      stage: 'production',
      branch: 'production',
      guestWorkspace: null,
      workspace: subdomains[0],
    }
  } else if (subdomains.length === 2 && lastSubdomain === 'staging') {
    return {
      stage: 'staging',
      branch: 'staging',
      guestWorkspace: null,
      workspace: subdomains[0],
    }
  } else if (subdomains.length === 2 && lastSubdomain === 'build') {
    const subdomainSplit = subdomains[0].split('-')
    return {
      stage: 'production',
      branch: `production:build:${subdomainSplit.slice(0, -1).join('-')}`,
      guestWorkspace: null,
      workspace: subdomainSplit[subdomainSplit.length - 1],
    }
  } else if (subdomains.length === 2 && lastSubdomain === 'dev') {
    const subdomainSplit = subdomains[0].split('-')
    return {
      stage: 'dev',
      branch: subdomainSplit.slice(0, -1).join('-'),
      guestWorkspace: subdomainSplit.length === 4 ? subdomainSplit[1] : null,
      workspace: subdomainSplit[subdomainSplit.length - 1],
    }
  } else {
    return null
  }
}

export {
  parseHostingDomain,
}
