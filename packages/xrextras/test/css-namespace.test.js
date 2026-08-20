const fs = require('fs')
const path = require('path')
const postcss = require('postcss')
const selectorParser = require('postcss-selector-parser')

const srcDir = path.join(__dirname, '../src')
const failures = []

// CSS files that have not been migrated to the xrextras- namespace yet.
// Remove a file from this set in the PR that migrates it.
const skippedCssFiles = new Set([
  'almosttheremodule/almost-there-module.css',
  'common.css',
  'mediarecorder/media-preview.css',
  'mediarecorder/record-button.css',
  'pwainstallermodule/pwa-installer-module.css',
  'runtimeerrormodule/runtime-error-module.css',
])

const cssFiles = []

const collectCssFiles = (dir) => {
  for (const entry of fs.readdirSync(dir, {withFileTypes: true})) {
    const fullPath = path.join(dir, entry.name)

    if (entry.isDirectory()) {
      collectCssFiles(fullPath)
    } else if (entry.isFile() && entry.name.endsWith('.css')) {
      cssFiles.push(fullPath)
    }
  }
}

collectCssFiles(srcDir)

for (const cssPath of cssFiles) {
  const relativePath = path.relative(srcDir, cssPath).split(path.sep).join('/')

  if (skippedCssFiles.has(relativePath)) {
    continue
  }

  const css = fs.readFileSync(cssPath, 'utf8')
  const root = postcss.parse(css, {from: cssPath})

  root.walkRules((rule) => {
    selectorParser((selectors) => {
      selectors.walkIds((node) => {
        if (!node.value.startsWith('xrextras-')) {
          failures.push(`${relativePath}: unnamespaced id "#${node.value}"`)
        }
      })

      selectors.walkClasses((node) => {
        if (!node.value.startsWith('xrextras-')) {
          failures.push(`${relativePath}: unnamespaced class ".${node.value}"`)
        }
      })
    }).processSync(rule.selector)
  })

  root.walkAtRules('keyframes', (rule) => {
    const name = rule.params.trim()

    if (!name.startsWith('xrextras-')) {
      failures.push(`${relativePath}: unnamespaced keyframes "${name}"`)
    }
  })

  root.walkAtRules('-webkit-keyframes', (rule) => {
    const name = rule.params.trim()

    if (!name.startsWith('xrextras-')) {
      failures.push(`${relativePath}: unnamespaced keyframes "${name}"`)
    }
  })
}

if (failures.length) {
  process.stderr.write('XRExtras CSS namespace check failed:\n')
  failures.forEach((failure) => process.stderr.write(`- ${failure}\n`))
  process.exit(1)
}

process.stdout.write('XRExtras CSS namespace check passed\n')
