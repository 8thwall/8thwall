const getXrWebScript = () => [].find.call(document.scripts, s => /xrweb(\?.*)?$/.test(s.src))

const isStudio = () => {
  try {
    return new URL(getXrWebScript().src).searchParams.get('s') === '1'
  } catch (err) {
    return false
  }
}

// Use the domain that the xrweb script was loaded from to verify
const verifyDomain = () => {
  try {
    return new URL(getXrWebScript().src).host
  } catch (err) {
    return 'apps.8thwall.com'
  }
}

// There is a c++ path for sending logs, but it broke when we switched from asm.js to WebAssembly
// because fetch was not implemented.  By default, we are now using the JS fetch.
const useJSFetch = () => !(window._c8 && window._c8.hasFetch)

const xrConstants = {
  isStudio,
  useJSFetch,
  verifyDomain,
}

export {
  xrConstants,
}
