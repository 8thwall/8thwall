// @rule(js_cli)
import {promises as fs, realpathSync} from 'fs'
import path from 'path'

const SKIPPED_PACKAGES = new Set([
  '@docusaurus/types',
  '@docusaurus/module-type-aliases',
  'aws-lambda-types',
  'undici-types',
  'tslib',
  // TODO(christoph): This has a typescript error internally, maybe needs an upgrade/patch-package
  'capnp-ts',
])

// These packages don't support the "import * as" format
const DEFAULT_IMPORT_PACKAGES = new Set([
  'chai-bytes',
  'color-rgba',
])

const PACKAGE_ROOT = process.argv[2]
const MODULES_ROOT = path.dirname(
  realpathSync(path.join('external', process.argv[3], 'fingerprint.sha1'))
)
const OUTPUT_FILE = process.argv[4]

type Package = {
  dependencies?: Record<string, string>
  devDependencies?: Record<string, string>

  // https://www.typescriptlang.org/docs/handbook/declaration-files/publishing.html
  types?: string
  typings?: string
}

const standardizeImportName = (name: string) => (
  name.replace(/-/g, '_').replace(/[^a-zA-Z0-9_]/g, '$$')
)

const extractPackageJson = async (root: string): Promise<Package> => (
  JSON.parse(await fs.readFile(path.join(root, 'package.json'), 'utf8'))
)

const containsTypes = async (root: string): Promise<boolean> => {
  const childrenFiles = await fs.readdir(root)
  if (childrenFiles.some(e => e.endsWith('.ts'))) {
    return true
  }
  const subPackage = await extractPackageJson(root)
  if (subPackage.types || subPackage.typings) {
    return true
  }

  return false
}

const listTypedPackages = async (): Promise<string[]> => {
  const rootPackage = await extractPackageJson(PACKAGE_ROOT)
  const dependencies = new Set([...Object.keys({
    ...rootPackage.dependencies,
    ...rootPackage.devDependencies,
  })])

  const typedPackages: string[] = []

  await Promise.all([...dependencies]
    .filter(e => !e.startsWith('@types/') && !SKIPPED_PACKAGES.has(e))
    .map(async (dependency) => {
      const isTyped = (
        dependencies.has(`@types/${dependency}`) ||
        await containsTypes(path.join(MODULES_ROOT, 'node_modules', dependency))
      )

      if (isTyped) {
        typedPackages.push(dependency)
      }
    }))

  typedPackages.sort()

  return typedPackages
}

const run = async () => {
  const typedPackages = await listTypedPackages()

  const lines: string[] = []

  typedPackages.forEach((e) => {
    if (DEFAULT_IMPORT_PACKAGES.has(e)) {
      lines.push(`import type ${standardizeImportName(e)} from '${e}'`)
    } else {
      lines.push(`import type * as ${standardizeImportName(e)} from '${e}'`)
    }
  })

  lines.push(`
const unique: unique symbol = Symbol('unique')
`)

  typedPackages.forEach((e) => {
    const name = standardizeImportName(e)
    lines.push(`// @ts-expect-error
const val_${name}: typeof ${name} = unique
`)
  })

  const output = lines.join('\n')

  if (OUTPUT_FILE) {
    await fs.writeFile(OUTPUT_FILE, output)
  } else {
    // eslint-disable-next-line no-console
    console.log(output)
  }
}

run()
