/*  eslint import/extensions:  ["error", "ignorePackages"] */

import path from 'path'

import pluginLocal from './rules/index.js'
import {getEslintMap} from './reality/cloud/xrhome/alias-config.js'

// NOTE(christoph): When running through //bzl/linter:eslint, we want to consume the
// .../npm-eslint/node_modules path provided on NODE_PATH. Fallback to normal resolution otherwise.
const resolve = (moduleName, file) => import(process.env.NODE_PATH
  ? import.meta.resolve(path.resolve(process.env.NODE_PATH, moduleName, file))
  : moduleName)

const {default: tsParser} = await resolve('@typescript-eslint/parser', 'dist/index.js')
const {default: pluginStylistic} = await resolve('@stylistic/eslint-plugin', 'dist/index.js')
const {default: pluginJsxA11y} = await resolve('eslint-plugin-jsx-a11y', 'lib/index.js')
const {default: pluginJs} = await resolve('@eslint/js', 'src/index.js')
const {default: globals} = await resolve('globals', 'index.js')
const {default: pluginTsEslint} = await resolve('typescript-eslint', 'dist/index.js')
const {default: pluginReact} = await resolve('eslint-plugin-react', 'index.js')
const {default: pluginReactHooks} = await resolve('eslint-plugin-react-hooks', 'index.js')
const {default: pluginImport} = await resolve('eslint-plugin-import', 'lib/index.js')
const {fixupConfigRules} = await resolve('@eslint/compat', 'dist/cjs/index.cjs')
const {includeIgnoreFile} = await resolve('eslint/config', '../lib/config-api.js')

const ROOT = process.cwd()

const EXTENSIONS = ['.js', '.ts', '.tsx', '.mjs']

const globs = prefix => EXTENSIONS.map(ext => `${prefix}/**/*${ext}`)

export default [
  ...includeIgnoreFile([
    path.join(ROOT, '.gitignore'),
    path.join(ROOT, '.lintignore'),
  ], {gitignoreResolution: true}),
  pluginJs.configs.recommended,
  ...pluginTsEslint.configs.recommended,
  ...fixupConfigRules(pluginImport.flatConfigs.recommended),
  ...fixupConfigRules(pluginReact.configs.flat.recommended),
  pluginReactHooks.configs.flat.recommended,
  pluginJsxA11y.flatConfigs.recommended,
  pluginStylistic.configs.recommended,
  pluginLocal.configs.default,
  pluginLocal.configs.overrides,
  {
    linterOptions: {
      reportUnusedDisableDirectives: false,  // todo: re-enable this
    },
    settings: {
      'react': {
        version: 'detect',
      },
      'import/resolver': {
        node: {extensions: EXTENSIONS},
        typescript: {},  // this loads <rootdir>/tsconfig.json to eslint
        alias: {
          map: [['@repo', ROOT]],
          extensions: EXTENSIONS,
        },
      },
    },
  },
  {
    files: ['**/*.ts'],
    languageOptions: {
      parser: tsParser,
    },
  },
  {
    files: ['**/*.tsx'],
    languageOptions: {
      parser: tsParser,
      parserOptions: {
        jsx: true,
      },
    },
  },
  {
    files: ['**/*.js', '**/*.mjs', '**/*.tsx', '**/*.ts'],
    languageOptions: {
      globals: {
        ...globals.browser,
        ...globals.node,
        Build8: 'readonly',
        BuildIf: 'readonly',
        XR8: 'readonly',
      },
    },
  },
  {
    files: ['**/*.d.ts'],
    rules: {
      'local-rules/acronym-capitalization': 'off',
      'max-classes-per-file': 'off',
    },
  },
  {
    files: globs('apps/image-target-cli'),
    rules: {
      'import/extensions': ['error', 'ignorePackages'],
      'no-console': 'off',
      'no-await-in-loop': 'off',
    },
  },
  {
    files: globs('packages/xrextras/src'),
    rules: {
      'global-require': 'off',
    },
  },
  {
    files: globs('reality/app/xr/js'),
    rules: {
      'no-restricted-properties': [
        'error',
        {
          property: 'stringToUTF8',
          message: 'Use writeStringToEmscriptenHeap instead.',
        },
      ],
      'react-hooks/rules-of-hooks': 'off',
    },
  },
  {
    files: ['c8/ecs/src/8mesh/**', 'c8/ecs/src/runtime/**'],
    rules: {
      'no-restricted-imports': [
        'error',
        {
          paths: [
            {
              name: 'three',
              message:
                'Please use "./three" or "./three-types" instead to avoid runtime deps on three',
            },
          ],
        },
      ],
    },
  },
  {
    files: globs('reality/cloud/xrhome'),
    settings: {
      'import/resolver': {
        alias: {
          map: getEslintMap(),
        },
      },
    },
    rules: {
      'local-rules/express-deprecated-send': 'error',
      'local-rules/ui-component-styling': 'error',
      'local-rules/reality-shared-imports': 'error',
      'import/order': [
        'error',
        {
          'groups': [['builtin', 'external']],
          'pathGroups': [
            {
              pattern: 'semantic-ui-react',
              group: 'builtin',
            },
          ],
          'newlines-between': 'always-and-inside-groups',
        },
      ],
      'no-restricted-imports': [
        'error',
        {
          paths: [
            {
              name: 'react-redux',
              importNames: ['useSelector'],
              message: 'Please use xrhome/src/client/hooks.ts useSelector instead',
            },
            {
              name: 'react-redux',
              importNames: ['connect'],
              message: 'Please use xrhome/src/client/common/connect.ts instead',
            },
            {
              name: 'real-semantic-ui-react',
              message: 'Please use `semantic-ui-react` instead',
            },
            {
              name: 'supertest',
              message: 'Please use mock-request.ts instead',
            },
            {
              name: 'firebase/auth',
              importNames: ['getAuth'],
              message: 'Please use xrhome/src/client/lightship/common/firebase.ts instead',
            },
          ],
        },
      ],
      // NOTE(christoph): xrhome uses react-three-fiber, whose JSX props (position, rotation,
      // args, attach, ...) are unknown to eslint-plugin-react.
      'react/no-unknown-property': 'off',
      'react/state-in-constructor': 'off',
    },
  },
  {
    files: globs('reality/cloud/xrhome/src/client'),
    rules: {
      'local-rules/hardcoded-copy': 'warn',
      'local-rules/i18n-nesting': 'error',
    },
  },
  {
    files: ['reality/cloud/xrhome/src/client/studio/example/**'],
    rules: {
      'local-rules/hardcoded-copy': 'off',
    },
  },
  {
    files: ['reality/cloud/xrhome/src/client/ui/stories/**'],
    rules: {
      'local-rules/hardcoded-copy': 'off',
      'import/group-exports': 'off',
      'import/exports-last': 'off',
    },
  },
  {
    files: ['reality/cloud/xrhome/test/**'],
    languageOptions: {
      globals: {
        ...globals.mocha,
      },
    },
  },
]
