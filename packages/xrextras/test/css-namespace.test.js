const fs = require('fs')
const path = require('path')

const failures = []

const htmlPath = path.join(__dirname, '../src/loadingmodule/loading-module.html')
const html = fs.readFileSync(htmlPath, 'utf8')

const forbiddenDomIds = [
  'id="loadImage"',
  'id="loadBackground"',
]

for (const id of forbiddenDomIds) {
  if (html.includes(id)) {
    failures.push(`Unnamespaced loading DOM id remains: ${id}`)
  }
}

const requiredDomIds = [
  'id="xrextras-load-image"',
  'id="xrextras-load-background"',
]

for (const id of requiredDomIds) {
  if (!html.includes(id)) {
    failures.push(`Missing namespaced loading DOM id: ${id}`)
  }
}

const cssPath = path.join(__dirname, '../src/loadingmodule/loading-module.css')
const css = fs.readFileSync(cssPath, 'utf8')

const requiredScopedSelectors = [
  '#loadingContainer .xrextras-loading-spin',
  '#loadingContainer .xrextras-loading-scale',
  '#loadingContainer .xrextras-loading-pulse',
  '#loadingContainer .xrextras-loading-fade-out',
  '#loadingContainer .xrextras-loading-highlight',
  '#loadingContainer #xrextras-load-image',
  '#loadingContainer #xrextras-load-background',
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
