// @rule(js_cli)
// @package(npm-webpack-build)
// @attr(esnext = True)
import path from 'path'
import {promises as fs} from 'fs'

import type {File as BabelFile} from '@babel/types'
import {parse, ParserPlugin} from '@babel/parser'

import {checkArgs} from '@repo/c8/cli/args'

import {NAME_PRIORITY} from './attribute-sort-order'
import {compareStringListElements} from './sort-string-list'

const getRuleName = (filePath: string) => path.basename(filePath).split('.')[0]

const getSortableName = (a: string) => a.match(/^([^ ]+)/)![1]

const sortAttributes = (_left: string, _right: string) => {
  const left = getSortableName(_left)
  const right = getSortableName(_right)

  const leftPriority = NAME_PRIORITY[left] ?? 0
  const rightPriority = NAME_PRIORITY[right] ?? 0

  if (leftPriority === 0 && rightPriority === 0) {
    return left.localeCompare(right)
  }

  return leftPriority - rightPriority
}

const dependencyPathToTarget = (dependencyPath: string, workspaceRoot: string) => {
  const relative = path.relative(workspaceRoot, dependencyPath)
  if (relative.startsWith('.')) {
    throw new Error(
      `Dependency out of workspace: ${JSON.stringify({dependencyPath, workspaceRoot, relative})}`
    )
  }
  const buildPath = path.dirname(relative)
  const ruleName = getRuleName(dependencyPath)

  // For "//example/target:target", we return "//example/target" instead
  if (path.basename(buildPath) === ruleName) {
    return `//${buildPath}`
  }
  return `//${buildPath}:${ruleName}`
}

// Input:  reality/engine/api/base/camera-intrinsics.capnp
// Output: //reality/engine/api/base:camera-intrinsics.capnp-ts
const capnpPathToTarget = (capnpPath: string) => (
  `//${path.dirname(capnpPath)}:${path.basename(capnpPath)}-ts`
)

type RunContext = {
  workspaceRoot: string
  pendingWrites: Map<string, string>
  noNewRules: boolean  // Don't create new rules, only update existing ones.
}

const exists = async (importPath: string) => {
  const possibleExtensions = ['.js', '.ts', '.tsx', '.d.ts']

  return (await Promise.all(possibleExtensions.map(async (ext) => {
    try {
      await fs.stat(`${importPath}${ext}`)
      return true
    } catch (err) {
      return false
    }
  }))).some(Boolean)
}

const getPlugins = (filePath: string): ParserPlugin[] => {
  if (filePath.endsWith('.tsx')) {
    return ['typescript', 'jsx']
  } else {
    return ['typescript']
  }
}

const parseSourceFile = async (filePath: string, workspaceRoot: string) => {
  const folderPath = path.dirname(filePath)
  const contents = await fs.readFile(filePath, 'utf-8')

  let ast: BabelFile
  try {
    ast = parse(contents, {
      sourceType: 'module',
      sourceFilename: path.basename(filePath),
      plugins: getPlugins(filePath),
    })
  } catch (err) {
    throw new Error(`Error parsing ${filePath}: ${err.message}`)
  }

  let ruleOverride = ''
  let ruleName = getRuleName(filePath)
  const skippedLines = new Set<number>()
  const attrs: string[] = []
  const deps = new Set<string>()
  const listAttrs: Record<string, string[]> = {}

  const pushListAttr = (attribute: string, value: string) => {
    listAttrs[attribute] = listAttrs[attribute] ?? []
    listAttrs[attribute].push(value)
  }

  let shouldExit = false
  let sublibraryOf: string | undefined
  ast.comments?.forEach((comment) => {
    const sublibraryMatch = comment.value.match(/@sublibrary\((.*)\)/)
    if (sublibraryMatch) {
      [, sublibraryOf] = sublibraryMatch
      return
    }

    if (comment.value.trim().startsWith('@inliner-skip-next')) {
      skippedLines.add(comment.loc.start.line + 1)
      return
    }

    if (comment.value.trim().startsWith('@inliner-off')) {
      shouldExit = true
      return
    }

    const depMatch = comment.value.match(/@dep\((.*)\)/)
    if (depMatch) {
      deps.add(depMatch[1])
      return
    }

    const attrMatch = comment.value.match(/@attr\((.*)\)/)
    if (attrMatch) {
      attrs.push(attrMatch[1])
      return
    }

    const listAttrMatch = comment.value.match(/@attr\[\]\((.*) = (.*)\)/)
    if (listAttrMatch) {
      const [, attribute, value] = listAttrMatch
      pushListAttr(attribute, value)
      return
    }

    const nameMatch = comment.value.match(/@name\((.*)\)/)
    if (nameMatch) {
      [, ruleName] = nameMatch
      return
    }

    const ruleMatch = comment.value.match(/@rule\((.*)\)/)
    if (ruleMatch) {
      [, ruleOverride] = ruleMatch
      return
    }

    const packageMatch = comment.value.match(/@package\((.*)\)/)
    if (packageMatch) {
      const [, rule] = packageMatch
      attrs.push(`npm_rule = "@${rule}//:${rule}"`)
      return
    }

    const visibilityMatch = comment.value.match(/@visibility\((.*)\)/)
    if (visibilityMatch) {
      attrs.push(`visibility = [\n        "${visibilityMatch[1]}",\n    ]`)
    }
  })

  const processDependency = async (importPath: string) => {
    if (importPath.startsWith('.')) {
      const relativeResolved = path.resolve(folderPath, importPath)
      if (await exists(relativeResolved)) {
        deps.add(dependencyPathToTarget(relativeResolved, workspaceRoot))
      }
    } else {
      const withoutNia = importPath.replace(/^@repo\//, '')
      const workspaceResolved = path.resolve(workspaceRoot, withoutNia)
      if (await exists(workspaceResolved)) {
        deps.add(dependencyPathToTarget(workspaceResolved, workspaceRoot))
      } else if (withoutNia.endsWith('.capnp')) {
        deps.add(capnpPathToTarget(withoutNia))
      }
    }
  }

  await Promise.all(ast.program.body.map(async (statement) => {
    // TODO(christoph): Handle `require('')`
    if (skippedLines.has(statement.loc?.start.line ?? -1)) {
      return
    }

    if (statement.type === 'ImportDeclaration') {
      await processDependency(statement.source.value)
    } else if (statement.type === 'ExportNamedDeclaration' && statement.source) {
      await processDependency(statement.source.value)
    } else if (statement.type === 'ExportAllDeclaration') {
      await processDependency(statement.source.value)
    }
  }))

  if (sublibraryOf && !shouldExit) {
    return {
      rule: 'js_library',
      ruleName,
      shouldExit: false,
      attrs: [
        'srcs = []',
        `deps = [\n        "${sublibraryOf}",\n    ]`,
      ],
    }
  }

  const rule = ruleOverride || (ruleName.endsWith('-test') ? 'js_test' : 'js_library')

  if (deps.size) {
    attrs.push(`deps = [
${[...deps].sort(compareStringListElements).map(e => `        "${e}",`).join('\n')}
    ]`)
  }

  pushListAttr(rule === 'js_library' ? 'srcs' : 'main', `"${path.basename(filePath)}"`)

  Object.entries(listAttrs).forEach(([attribute, values]) => {
    attrs.push(`${attribute} = [
${values.sort(compareStringListElements).map(e => `        ${e},`).join('\n')}
    ]`)
  })

  if (rule === 'js_test' && !attrs.some(a => a.startsWith('size = '))) {
    attrs.push('size = "small"')
  }

  return {
    rule,
    ruleName,
    shouldExit,
    attrs: attrs.sort(sortAttributes),
  }
}

const loadBuildFileWithCleanup = async (buildPath: string, ctx: RunContext) => {
  const existingContent = ctx.pendingWrites.get(buildPath)

  if (existingContent !== undefined) {
    return existingContent
  }

  let currentContent: string
  try {
    currentContent = await fs.readFile(buildPath, 'utf8')
  } catch (err) {
    if (err.code === 'ENOENT') {
      // eslint-disable-next-line no-console
      console.warn(`Creating new BUILD file: ${buildPath}`)
      const initialContent = 'load("//bzl/js:js.bzl", "js_library")\n'
      ctx.pendingWrites.set(buildPath, initialContent)
      return initialContent
    }
    throw err
  }

  const newContentParts: string[] = []

  const jsRuleMatch = /\n(?:js_test|js_cli|js_binary|js_library)\((.*?)\n\)/sg

  const jsRules = [...currentContent.matchAll(jsRuleMatch)]

  const existingFiles = new Set(await fs.readdir(path.dirname(buildPath)))

  let lastIndexEnd = 0
  jsRules.forEach((match) => {
    if (typeof match.index !== 'number') {
      throw new Error('Missing match index in matchAll')
    }

    const fileSetMatches = [...match[0].matchAll(/\n {4}(?:srcs|main) = \[([^\]]*)\]/sg)]

    const containsMissingFile = fileSetMatches.some(fileSetMatch => (
      fileSetMatch[1].split(',').some((potentialSrc) => {
        const src = potentialSrc.replace(/"/g, '').trim()

        return (
          src &&
    !src.includes('/') &&  // Only look at sibling files
          !src.includes(':') &&  // Ignore other targets if present
            !existingFiles.has(src)
        )
      })
    ))

    if (containsMissingFile) {
      newContentParts.push(currentContent.substring(lastIndexEnd, match.index))
      newContentParts.push('')
      lastIndexEnd = match.index + match[0].length
    }
  })

  newContentParts.push(currentContent.substring(lastIndexEnd))

  const newContent = newContentParts.join('')

  ctx.pendingWrites.set(buildPath, newContent)

  return newContent
}

const applyInliner = async (filePath: string, ctx: RunContext) => {
  const buildPath = path.resolve(filePath, '../BUILD')

  const {ruleName, rule, shouldExit, attrs} = await parseSourceFile(filePath, ctx.workspaceRoot)

  const prevBuildFileContents = await loadBuildFileWithCleanup(buildPath, ctx)

  if (shouldExit) {
    return
  }

  const regexString = `(${rule})\\(\n +name = "${ruleName}",(?:.*?\\n\\))`
  const ruleRegex = new RegExp(regexString, 's')

  const ruleMatch = prevBuildFileContents.match(ruleRegex)

  const newRule = `\
${rule}(
    name = "${ruleName}",
${attrs.map(a => `    ${a},`).join('\n')}
)`

  let after: string

  if (ruleMatch && typeof ruleMatch.index === 'number') {
    const existingRuleStart = ruleMatch.index
    const existingRuleEnd = ruleMatch.index + ruleMatch[0].length

    after = `\
${prevBuildFileContents.substring(0, existingRuleStart)}\
${newRule}\
${prevBuildFileContents.substring(existingRuleEnd)}`
  } else if (ctx.noNewRules) {
    // We don't see the rule in the BUILD file, and we're not supposed to create new rules.
    return
  } else {
    after = `${prevBuildFileContents}
${newRule}
`
  }

  ctx.pendingWrites.set(buildPath, after)
}

const expandFilePath = async (filePath: string): Promise<string | string[]> => {
  if (path.basename(filePath) === 'BUILD') {
    const folder = path.dirname(filePath)
    const children = await fs.readdir(folder)
    return children
      .filter(c => c.endsWith('.ts') && !c.endsWith('.d.ts'))
      .map(c => path.join(folder, c))
  }
  return filePath
}

const runInliner = async (inputPaths: string[], noNewRules: boolean, check: boolean) => {
  const context: RunContext = {
    pendingWrites: new Map(),
    workspaceRoot: process.cwd(),
    noNewRules,
  }

  const filePaths = new Set((await Promise.all(inputPaths.map(expandFilePath))).flat())

  for (const filePath of filePaths) {
    // eslint-disable-next-line no-await-in-loop
    await applyInliner(filePath, context)
  }

  if (check) {
    let failed = false
    await Promise.all([...context.pendingWrites.entries()].map(async ([buildFile, content]) => {
      const currentContent = await fs.readFile(buildFile, 'utf-8')
      if (currentContent !== content) {
        const buildPath = path.relative(context.workspaceRoot, buildFile)
        // eslint-disable-next-line no-console
        console.error(`Out of date: ${buildPath}`)
        failed = true
      }
    }))
    if (failed) {
      process.exit(1)
    }
  } else {
    await Promise.all(
      [...context.pendingWrites.entries()]
        .map(([filePath, content]) => fs.writeFile(filePath, content))
    )
  }
}

const args = checkArgs({
  minOrdered: 1,
  optionalFlags: ['no-new', 'check'],
  optionsForFlag: {
    'no-new': [true, '1', 'true'],
    'check': [true, '1', 'true'],
  },
})

runInliner(args._ordered, !!args['no-new'], !!args.check)
