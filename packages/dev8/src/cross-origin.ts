const isCrossOriginWarnableScript = (script: HTMLScriptElement) => {
  // Not a remote script
  if (!script.src) {
    return false
  }

  // Already has it set
  if (script.hasAttribute('crossorigin')) {
    return false
  }

  // Ignore scripts on the same domain
  if (script.src.startsWith(window.location.origin)) {
    return false
  }

  // Doesn't apply to scripts that aren't setting a domain on the src
  if (['//', 'https://', 'http://'].every(protocol => !script.src.startsWith(protocol))) {
    return false
  }

  return true
}

let alreadyCheckedCrossOrigin = false

const maybeWarnCrossOriginError = (e: ErrorEvent) => {
  if (alreadyCheckedCrossOrigin) {
    return
  }

  // e.filename is the empty string if the script tag wasn't loaded with crossorigin
  if (e.filename) {
    return
  }

  alreadyCheckedCrossOrigin = true

  const warnScripts = Array.from(document.scripts).filter(isCrossOriginWarnableScript)

  if (warnScripts.length) {
    const plural = warnScripts.length > 1
    // eslint-disable-next-line no-console
    console.warn([
      `Detected script${plural ? 's' : ''} without the crossorigin attribute set:\n\n`,
      `    ${warnScripts.map(s => `<script src="${s.src}"></script>`).join('\n    ')}\n\n`,
      `This can cause the browser to limit the displayed error message to "${e.message}"\n\n`,
      'Add crossorigin="anonymous" to enable verbose script debugging. ',
      'Learn more at https://8th.io/crossorigin-script',
    ].join(''))
  }
}

export {
  maybeWarnCrossOriginError,
}
