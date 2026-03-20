if (!window.XRExtras) {
  console.warn('XRExtras not present on window, missing xrextras.js script tag?')
}

module.exports = window.XRExtras
