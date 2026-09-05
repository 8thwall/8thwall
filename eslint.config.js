const js = require('@eslint/js')
const globals = require('globals')
const { readFileSync } = require('fs')
const tseslint = require('typescript-eslint')
const react = require('eslint-plugin-react')
const reactHooks = require('eslint-plugin-react-hooks')
const jsxA11y = require('eslint-plugin-jsx-a11y')
const importPlugin = require('eslint-plugin-import')
const localRulesPlugin = require('eslint-plugin-local-rules')
const stylistic = require('@stylistic/eslint-plugin')

const { getEslintMap } = require('./reality/cloud/xrhome/alias-config')
const airbnbRules = require('./rules/airbnb')
const { wrapLegacyContextRules } = require('./rules/legacy-context')

const ROOT = __dirname

// NOTE(christoph): .prettierignore is the single source of truth for ignored paths (shared by
// Prettier, lint.sh, and this config). It uses gitignore-style syntax, which flat-config
// ignores understand as well.
const readIgnorePatterns = () =>
  readFileSync(`${ROOT}/.prettierignore`, 'utf8')
    .split('\n')
    .map(line => line.trim())
    .filter(line => line && !line.startsWith('#'))

// File extensions that are linted, and a helper to build the file globs for a directory prefix.
// e.g. globs('c8/ecs/src') -> ['c8/ecs/src/**/*.js', 'c8/ecs/src/**/*.ts', 'c8/ecs/src/**/*.tsx']
const LINTED_EXTENSIONS = ['js', 'ts', 'tsx']
const TYPESCRIPT_EXTENSIONS = ['ts', 'tsx']
const globs = prefix => LINTED_EXTENSIONS.map(ext => `${prefix}/**/*.${ext}`)

// From:
//   https://github.com/airbnb/javascript/blob/master/packages/eslint-config-airbnb-base/rules/style.js
const DISALLOWED_SYNTAX = [
  {
    selector: 'ForInStatement',
    message:
      'for..in loops iterate over the entire prototype chain, which is virtually never what you want. Use Object.{keys,values,entries}, and iterate over the resulting array.',
  },
  // NOTE(christoph): We're allowing this one because we're using generators and we can avoid
  // Anonymous functions if we use "for of" loops.
  {
    selector: 'LabeledStatement',
    message:
      'Labels are a form of GOTO; using them makes code confusing and hard to maintain and understand.',
  },
  {
    selector: 'WithStatement',
    message:
      '`with` is disallowed in strict mode because it makes code impossible to predict and optimize.',
  },
]

const spacingRules = {
  'brace-style': 'error',
  'comma-spacing': 'error',
  'eol-last': 'error',
  '@typescript-eslint/indent': ['error', 2, {
    SwitchCase: 1,

    // Copied from https://github.com/airbnb/javascript/blob/master/packages/eslint-config-airbnb-base/rules/style.js#L146
    ignoredNodes: [
      'JSXElement',
      'JSXElement > *',
      'JSXAttribute',
      'JSXIdentifier',
      'JSXNamespacedName',
      'JSXMemberExpression',
      'JSXSpreadAttribute',
      'JSXExpressionContainer',
      'JSXOpeningElement',
      'JSXClosingElement',
      'JSXText',
      'JSXEmptyExpression',
      'JSXSpreadChild',
      'TSUnionType',
    ],
  }],
  'indent': 'off',
  'keyword-spacing': 'error',
  // 'local-rules/inline-comment-spacing': 'error',
  'local-rules/typedef-separators': 'error',
  // 'local-rules/multiline-ternary': 'error',
  'max-len': ['error', { code: 100 }],
  'no-multi-spaces': ['error', { ignoreEOLComments: true }],
  'object-curly-newline': ['error', { multiline: true, consistent: true }],
  'object-curly-spacing': ['error', 'never'],
  'object-property-newline': ['error', { allowAllPropertiesOnSameLine: true }],
  'one-var-declaration-per-line': ['error', 'initializations'],
  'operator-linebreak': ['error', 'after', { overrides: { '?': 'before', ':': 'before' } }],
  'react/jsx-curly-newline': ['off'],
  'react/jsx-indent': ['error', 2, { indentLogicalExpressions: true, checkAttributes: true }],
  'react/jsx-one-expression-per-line': 'off',
  'react/jsx-wrap-multilines': ['error', {
    declaration: 'parens-new-line',
    assignment: 'parens-new-line',
    return: 'parens-new-line',
    arrow: 'parens-new-line',
    condition: 'ignore',
    logical: 'ignore',
    prop: 'parens-new-line',
  }],
  'semi-style': ['error', 'first'],
  'space-before-blocks': 'error',
  '@typescript-eslint/type-annotation-spacing': 'error',
}

const semanticsRules = {
  'arrow-body-style': ['error', 'as-needed'],
  'no-restricted-syntax': ['error', ...DISALLOWED_SYNTAX],
  'no-param-reassign': ['error', { props: false }],
  'no-plusplus': 'off',
  'no-use-before-define': 'off',
  'no-var': 'error',
  'no-shadow': 'off',
  '@typescript-eslint/no-shadow': 'error',
  '@typescript-eslint/no-use-before-define': ['error', { functions: false, classes: false }],
  '@typescript-eslint/no-unused-expressions': 'error',
  'no-unused-expressions': 'off',
  'no-unused-vars': 'off',
  '@typescript-eslint/no-unused-vars': 'error',
  'no-underscore-dangle': 'off', // For _c8
  'one-var': 'off',
}

const reactRules = {
  'react/destructuring-assignment': 'off',
  'react/jsx-key': 'error',
  'react/prop-types': 'off',
  'react/require-default-props': 'off',
  'react/function-component-definition': [
    'error',
    {
      namedComponents: 'arrow-function',
      unnamedComponents: 'arrow-function',
    },
  ],
  'react-hooks/exhaustive-deps': 'off',
  'react/jsx-no-useless-fragment': 'error',
  'react/jsx-props-no-spreading': 'off',
  'react/jsx-filename-extension': ['error', { extensions: ['.tsx'] }],
}

const importRules = {
  'import/exports-last': 'error',
  'import/group-exports': 'error',
  'import/no-extraneous-dependencies': 'off',
  'import/no-duplicates': 'off',
  'import/prefer-default-export': 'off',
  // NOTE(christoph): Bazel-style specifiers (bzl/js/chai-js, @8thwall/xrextras/...css) and
  // test-only deps don't resolve statically. Matches the old MR tool's DISABLED_RULES.
  'import/no-unresolved': 'off',
  'import/extensions': [
    'error',
    'ignorePackages',
    {
      ts: 'never',
      tsx: 'never',
      js: 'never',
    },
  ],
  'import/order': [
    'error',
    {
      'groups': [['builtin', 'external']],
      'newlines-between': 'always-and-inside-groups',
    },
  ],
}

const domRules = {
  'jsx-a11y/label-has-for': 'off', // Deprecated
  'jsx-a11y/label-has-associated-control': [
    'error',
    {
      controlComponents: [
        'PrimaryRadioButton',
        'Checkbox',
        'Input',
        'Form.Field',
        'StandardTextInput',
        'StandardToggleInput',
        'CoreDropdown',
        'NumberInput',
        'NumberOrPercentInput',
        'RangeSliderInput',
      ],
      assert: 'both',
      depth: 25,
    },
  ],
}

const localRules = {
  'local-rules/acronym-capitalization': 'error',
  'local-rules/commonjs': 'error',
  'local-rules/implicit-any': 'error',
  'local-rules/type-only-imports': 'error',
  'local-rules/typedef-separators': 'error',
  'local-rules/untyped-array': 'error',
  'local-rules/underscore-argument': 'error',
  'local-rules/prefer-await': 'warn',
}

module.exports = [
  {
    ignores: readIgnorePatterns(),
  },
  stylistic.configs.recommended,
  {
    files: LINTED_EXTENSIONS.map(ext => `**/*.${ext}`),

    languageOptions: {
      ecmaVersion: 'latest',
      sourceType: 'module',
      parser: tseslint.parser,
      parserOptions: {
        ecmaFeatures: { jsx: true },
      },
      globals: {
        ...globals.browser,
        ...globals.node,
        Build8: 'readonly',
        BuildIf: 'readonly',
        XR8: 'readonly',
      },
    },

    plugins: {
      '@stylistic': stylistic,
      'react': wrapLegacyContextRules(react),
      'react-hooks': wrapLegacyContextRules(reactHooks),
      'jsx-a11y': wrapLegacyContextRules(jsxA11y),
      'import': wrapLegacyContextRules(importPlugin),
      '@typescript-eslint': tseslint.plugin,
      'local-rules': localRulesPlugin,
    },

    settings: {
      'react': { version: 'detect' },
      'import/resolver': {
        node: { extensions: ['.js', '.ts', '.tsx'] },
        typescript: {}, // this loads <rootdir>/tsconfig.json to eslint
        alias: {
          map: [['@repo', ROOT]],
          extensions: ['.js', '.ts', '.tsx'],
        },
      },
    },

    rules: {
      ...js.configs.recommended.rules,
      // ...airbnbRules,
      ...spacingRules,
      ...semanticsRules,
      // ...reactRules,
      // ...importRules,
      // ...localRules,
      // ...domRules,
    },
  },
  stylistic.configs['disable-legacy'],

  {
    files: TYPESCRIPT_EXTENSIONS.map(ext => `**/*.${ext}`),
    rules: {
      // NOTE(christoph): The TypeScript compiler already reports undefined identifiers, so core
      // no-undef (which can't see ambient declarations) only produces false positives in TS files.
      'no-undef': 'off',
      // Type-only named imports from untyped JS packages can't be verified statically; the
      // TypeScript compiler checks them instead.
      'import/named': 'off',
    },
  },

  // Ambient declaration files mirror upstream API names (e.g. React's HTMLAttributes) and
  // structure (multiple classes per module), which house rules can't allow renaming/rearranging.
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
        node: { extensions: ['.js', '.ts', '.tsx'] },
        alias: {
          map: getEslintMap(),
          extensions: ['.js', '.ts', '.tsx'],
        },
        typescript: {},
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
