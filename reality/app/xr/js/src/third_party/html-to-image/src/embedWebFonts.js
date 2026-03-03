import * as util_1 from './util'
import * as embedResources_1 from './embedResources'
import {fetch} from './fetch'
import {logError} from './log'
import {cacheGet, cacheSet} from './cache'

/**
 * This function makes a get request for the url and returns the result as text called {cssText}.
 */
function fetchCSS(url, options) {
  const cache = cacheGet('css', url, options)
  if (cache != null) {
    return cache
  }
  const deferred = fetch(url, options).then(res => ({
    url,
    cssText: res.text(),
  }))
  cacheSet('css', url, deferred, options)
  return deferred
}

function parseCSS(source) {
  if (source == null) {
    return []
  }
  const result = []
  const commentsRegex = /(\/\*[\s\S]*?\*\/)/gi
  // strip out comments
  let cssText = source.replace(commentsRegex, '')
  const keyframesRegex = new RegExp('((@.*?keyframes [\\s\\S]*?){([\\s\\S]*?}\\s*?)})', 'gi')
  // eslint-disable-next-line no-constant-condition
  while (true) {
    const matches = keyframesRegex.exec(cssText)
    if (matches === null) {
      break
    }
    result.push(matches[0])
  }
  cssText = cssText.replace(keyframesRegex, '')
  const importRegex = /@import[\s\S]*?url\([^)]*\)[\s\S]*?;/gi
  // to match css & media queries together
  const combinedCSSRegex = '((\\s*?(?:\\/\\*[\\s\\S]*?\\*\\/)?\\s*?@media[\\s\\S]' +
        '*?){([\\s\\S]*?)}\\s*?})|(([\\s\\S]*?){([\\s\\S]*?)})'
  // unified regex
  const unifiedRegex = new RegExp(combinedCSSRegex, 'gi')
  // eslint-disable-next-line no-constant-condition
  while (true) {
    let matches = importRegex.exec(cssText)
    if (matches === null) {
      matches = unifiedRegex.exec(cssText)
      if (matches === null) {
        break
      } else {
        importRegex.lastIndex = unifiedRegex.lastIndex
      }
    } else {
      unifiedRegex.lastIndex = importRegex.lastIndex
    }
    result.push(matches[0])
  }
  return result
}

/**
 * This function manually loads style rules that we don't have access to in the browser.
 * Stylesheets loaded cross origin cannot be inspected directly so fetch them manually and parse
 * them ourselves. We have a CSSStyleSheet that we load the rules into so that we can then extract
 * the font rules from. We keep a state around in the cache in order to not put the sheet into the
 * window.document.styleSheets list. We additionally optimize by loading rules only once per url.
 *
 * It then returns all the css rules in the document as well as the font rules we fetched manually.
 */
function getFontFaceRules(document, options) {
  let state = cacheGet('getFontFaceRules', document, options)
  if (!state) {
    state = {
      helperSheet: new CSSStyleSheet(),
      numImportRules: 0,  // Keep track of index to insert import rules at into the helperSheet.
      seenSheetUrls: new Set(),
      seenRuleIncludesUrls: new Set(),
    }
    cacheSet('getFontFaceRules', document, state, options)
  }

  // helperSheet is the non-document style sheet that we load manually fetched rules into that
  // we could not read directly (cross origin).
  const {helperSheet, seenSheetUrls, seenRuleIncludesUrls} = state

  const sheetsToIterate = util_1.toArray(document.styleSheets)
  sheetsToIterate.push(helperSheet)

  const ret = []
  const deferreds = []
  // First loop inlines imports
  sheetsToIterate.forEach((sheet) => {
    if ('cssRules' in sheet) {
      try {
        util_1.toArray(sheet.cssRules).forEach((item) => {
          if (item.type === CSSRule.IMPORT_RULE) {
            const url = item.href
            if (seenSheetUrls.has(url)) {
              return
            }
            seenSheetUrls.add(url)
            const deferred = fetchCSS(url, options)
              .then(metadata => metadata.cssText)
              .then(cssText => parseCSS(cssText).forEach((rule) => {
                // Temp rule to normalize rule text for de-duping.
                const tempSheet = new CSSStyleSheet()
                tempSheet.insertRule(rule)
                const ruleText = tempSheet.cssRules[0].cssText

                try {
                  if (!Array.from(helperSheet.cssRules).some(r => ruleText === r.cssText)) {
                    // @import rules must exist before style rules otherwise HierarchyRequestError.
                    helperSheet.insertRule(rule, rule.startsWith('@import')
                      ? (state.numImportRules++)
                      : helperSheet.cssRules.length)
                  }
                } catch (error) {
                  logError(options, 'Error inserting rule from remote css', {
                    rule,
                    error,
                  })
                }
              }))
              .catch((e) => {
                logError(options, 'Error loading remote css', e.toString())
              })
            deferreds.push(deferred)
          }
        })
      } catch (e) {
        // Assumes that sheet.cssRules could not be accessed so manually fetch the sheet.
        if (sheet.href != null) {
          if (seenRuleIncludesUrls.has(sheet.href)) {
            return
          }
          deferreds.push(fetchCSS(sheet.href, options)
            .then(metadata => metadata.cssText)
            .then(cssText => parseCSS(cssText).forEach((rule) => {
              helperSheet.insertRule(rule, helperSheet.cssRules.length)
              seenRuleIncludesUrls.add(sheet.href)
            }))
            .catch((err) => {
              logError(options, 'Error loading remote stylesheet', err.toString())
            }))
        }
        logError(options, 'Error inlining remote css file', e.toString())
      }
    }
  })
  return Promise.all(deferreds).then(() => {
    // Second loop parses rules
    sheetsToIterate.forEach((sheet) => {
      if ('cssRules' in sheet) {
        try {
          util_1.toArray(sheet.cssRules).forEach((rule) => {
            if (rule.type === CSSRule.FONT_FACE_RULE &&
              embedResources_1.shouldEmbed(rule.style.getPropertyValue('src'))
            ) {
              ret.push(rule)
            }
          })
        } catch (e) {
          logError(options, `Error while reading CSS rules from ${sheet.href}`, e.toString())
        }
      }
    })
    return ret
  })
}

/**
 * Given a node, return css font face rules that were loaded over the network.
 */
function parseWebFontRules(node, options) {
  return new Promise((resolve, reject) => {
    if (node.ownerDocument == null) {
      reject(new Error('Provided element is not within a Document'))
    }
    resolve(getFontFaceRules(node.ownerDocument, options))
  })
}

/**
 * Given a node, figure out all the fonts that are in use in it and its descendants.
 */
const getFontsInUse = (rootNode) => {
  const fontFamilies = new Set()
  const fontWeightsForFontFamilies = {}

  // It appears that the browser gives us quotes around names that include whitespace and not around
  // those that do not. The same appears to be true for getting the font names from the css rules.
  const fontFamiliesForNode = (n) => {
    n.style.fontFamily.split(',').map(f => f.trim()).forEach((f) => {
      fontFamilies.add(f)

      if (!fontWeightsForFontFamilies[f]) {
        fontWeightsForFontFamilies[f] = new Set()
      }

      fontWeightsForFontFamilies[f].add(n.style.fontWeight)
    })
    Array.from(n.children).forEach(fontFamiliesForNode)
  }
  fontFamiliesForNode(rootNode)

  return {
    fontFamilies,
    fontWeightsForFontFamilies,
  }
}

/**
 * Takes a map of sets: fontFamily => Set[fontWeight]
 */
const shouldDeDupe = (includedFonts, rule) => {
  const {fontFamily, fontWeight} = rule.style
  if (!includedFonts[fontFamily]) {
    includedFonts[fontFamily] = new Set()
  }
  if (includedFonts[fontFamily].has(fontWeight)) {
    return true
  }
  includedFonts[fontFamily].add(fontWeight)
  return false
}

/**
 * Given a node, figure out which fonts are in use and return them as text @font-face rules
 * to be used in a <style> tag e.g.
 * @font-face { font-family: "Signika Negative"; src: url("data:font/woff2;base64,d09GMgAAAAh2g...")
 */
const getWebFontCSS = (node, options) => parseWebFontRules(node, options).then((rules) => {
  const fontsInUse = getFontsInUse(node)
  const rulesToInclude = []

  // We de-dup here based on just the font family names and weights.
  const includedFonts = {}

  // If we remove unused font weights, we want to keep track of the smallest one to add it in later
  // so that we have a font declaration for the used font.
  // Map to object: fontFamily => {minWeight, rule}
  const fallbackRuleForFamily = {}

  rules.forEach((rule) => {
    const {fontFamily, fontWeight} = rule.style

    // Disabled rules don't do anything.
    if (rule.disabled) {
      return
    }

    // Unused fonts.
    if (!fontsInUse.fontFamilies.has(fontFamily)) {
      return
    }

    // Fonts whose weights are not in use (have to make a second pass to ensure we have at least 1).
    if (!fontsInUse.fontWeightsForFontFamilies[fontFamily].has(fontWeight)) {
      const existingMin = fallbackRuleForFamily[fontFamily] || {}
      fallbackRuleForFamily[fontFamily] = {
        rule,
        minWeight: Math.min(Number(fontWeight.split(' ')[0]), existingMin.minWeight || 1100),
      }
      return
    }

    // If we already have a rule with the same font-family and font-weight, skip.
    // This builds up the includedFonts object to keep track of which fonts are included.
    if (shouldDeDupe(includedFonts, rule)) {
      return
    }

    // TODO(pawel) Filter out unused font-weights as well. Needs additional considerations for
    // inexact matches.

    rulesToInclude.push(rule)
  })

  // Make a second pass through the fontsInUse to make sure we have at least one font declaration.
  fontsInUse.fontFamilies.forEach((f) => {
    if (!includedFonts[f]) {
      // Add the smallest instance of the font.
      if (!fallbackRuleForFamily[f]) {
        // NOTE(pawel) These should only be web fonts that aren't imported.
        return
      }
      rulesToInclude.push(fallbackRuleForFamily[f].rule)
    }
  })

  return rulesToInclude
}).then(rules => Promise.all(rules.map((rule) => {
  const baseUrl = rule.parentStyleSheet
    ? rule.parentStyleSheet.href
    : null
  try {
    const cache = cacheGet('embed-font-faces', rule, options)
    if (cache) {
      return cache
    }
    const res = embedResources_1.embedResources(rule.cssText, baseUrl, options)
    cacheSet('embed-font-faces', rule, res, options)
    return res
  } catch (err) {
    return ''
  }
})))
  .then(embedded => ([['', ...embedded, ''].join('\n')]))

/**
 * Figure out which fonts are in use and embedded them as a <style> node in the given node.
 */
function embedWebFonts(clonedNode, options) {
  return (options.fontEmbedCSS != null
    ? Promise.resolve(options.fontEmbedCSS)
    : getWebFontCSS(clonedNode, options)).then((cssText) => {
    const styleNode = document.createElement('style')
    const sytleContent = document.createTextNode(cssText)
    styleNode.appendChild(sytleContent)
    if (clonedNode.firstChild) {
      clonedNode.insertBefore(styleNode, clonedNode.firstChild)
    } else {
      clonedNode.appendChild(styleNode)
    }
    return clonedNode
  })
}

export {
  embedWebFonts,
  getWebFontCSS,
}
