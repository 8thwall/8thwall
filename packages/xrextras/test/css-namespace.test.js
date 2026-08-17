const fs = require('fs')
const path = require('path')

const cssPath = path.join(__dirname, '../src/loadingmodule/loading-module.css')
const css = fs.readFileSync(cssPath, 'utf8')

const failures = []

const requiredScopedSelectors = [
  '#loadingContainer .spin',
  '#loadingContainer .scale',
  '#loadingContainer .pulse',
  '#loadingContainer .fade-out',
  '#loadingContainer .highlight',
  '#loadingContainer #loadImage',
  '#loadingContainer #loadBackground',
]

for (const selector of requiredScopedSelectors) {
  if (!css.includes(selector)) {
    failures.push(`Missing scoped selector: ${selector}`)
  }
}

const forbiddenKeyframes = [
  '@keyframes spin',
  '@keyframes scale',
  '@keyframes pulse',
  '@keyframes fade-out',
]

for (const keyframe of forbiddenKeyframes) {
  if (css.includes(keyframe)) {
    failures.push(`Unscoped keyframe remains: ${keyframe}`)
  }
}

const requiredKeyframes = [
  '@keyframes xrextras-loading-spin',
  '@keyframes xrextras-loading-scale',
  '@keyframes xrextras-loading-pulse',
  '@keyframes xrextras-loading-fade-out',
]

for (const keyframe of requiredKeyframes) {
  if (!css.includes(keyframe)) {
    failures.push(`Missing namespaced keyframe: ${keyframe}`)
  }
}

if (failures.length) {
  process.stderr.write('XRExtras loading CSS namespace regression check failed:\n')
  failures.forEach(failure => process.stderr.write(`- ${failure}\n`))
  process.exit(1)
}

process.stdout.write('XRExtras loading CSS namespace regression check passed\n')
