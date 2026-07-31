if (!window.CoachingOverlay) {
  // eslint-disable-next-line no-console
  console.warn('CoachingOverlay not present on window, missing coaching-overlay.js script tag?')
}

if (!window.SkyCoachingOverlay) {
  // eslint-disable-next-line no-console
  console.warn('SkyCoachingOverlay not present on window, missing coaching-overlay.js script tag?')
}

module.exports = {
  AbsoluteScale: window.CoachingOverlay,
  Sky: window.SkyCoachingOverlay,
}
