/* eslint-env es6 */

/* eslint-disable max-len */
// From:
//   https://github.com/airbnb/javascript/blob/master/packages/eslint-config-airbnb-base/rules/style.js
const DISALLOWED_SYNTAX = [
  {
    'selector': 'ForInStatement',
    'message': 'for..in loops iterate over the entire prototype chain, which is virtually never what you want. Use Object.{keys,values,entries}, and iterate over the resulting array.',
  },
  // NOTE(christoph): We're allowing this one because we're using generators and we can avoid
  // Anonymous functions if we use "for of" loops.
  // {
  //   "selector": "ForOfStatement",
  //   "message": "iterators/generators require regenerator-runtime, which is too heavyweight for this guide to allow them. Separately, loops should be avoided in favor of array iterations."
  // },
  {
    'selector': 'LabeledStatement',
    'message': 'Labels are a form of GOTO; using them makes code confusing and hard to maintain and understand.',
  },
  {
    'selector': 'WithStatement',
    'message': '`with` is disallowed in strict mode because it makes code impossible to predict and optimize.',
  },
]
/* eslint-enable max-len */

const spacingRules = {
  'brace-style': 'error',
  'comma-spacing': 'error',
  'eol-last': 'error',
  '@typescript-eslint/indent': ['error', 2, {
    SwitchCase: 1,
    // eslint-disable-next-line max-len
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
  'local-rules/inline-comment-spacing': 'error',
  'local-rules/typedef-separators': 'error',
  'local-rules/multiline-ternary': 'error',
  'max-len': ['error', {'code': 100}],
  'no-multi-spaces': ['error', {'ignoreEOLComments': true}],
  'object-curly-newline': ['error', {'multiline': true, 'consistent': true}],
  'object-curly-spacing': ['error', 'never'],
  'object-property-newline': ['error', {'allowAllPropertiesOnSameLine': true}],
  'one-var-declaration-per-line': ['error', 'initializations'],
  'operator-linebreak': ['error', 'after', {'overrides': {'?': 'before', ':': 'before'}}],
  'react/jsx-curly-newline': ['off'],
  'react/jsx-indent': ['error', 2, {indentLogicalExpressions: true, checkAttributes: true}],
  'react/jsx-one-expression-per-line': 'off',
  'react/jsx-wrap-multilines': ['error', {
    'declaration': 'parens-new-line',
    'assignment': 'parens-new-line',
    'return': 'parens-new-line',
    'arrow': 'parens-new-line',
    'condition': 'ignore',
    'logical': 'ignore',
    'prop': 'parens-new-line',
  }],
  'semi-style': ['error', 'first'],
  'space-before-blocks': 'error',
  '@typescript-eslint/type-annotation-spacing': 'error',
}

const syntaxRules = {
  'arrow-body-style': ['error', 'as-needed'],
  'arrow-parens': ['error', 'as-needed', {requireForBlockBody: true}],
  'comma-dangle': ['error', {
    'arrays': 'always-multiline',
    'objects': 'always-multiline',
    'imports': 'always-multiline',
    'exports': 'always-multiline',
    'functions': 'never',
  }],
  'implicit-arrow-linebreak': 'error',
  'jsx-quotes': ['error', 'prefer-single'],
  'no-else-return': ['off'],
  'no-restricted-syntax': ['error', ...DISALLOWED_SYNTAX],
  'no-underscore-dangle': 'off',  // For _c8
  'no-unused-expressions': 'off',
  '@typescript-eslint/no-unused-expressions': 'error',
  'react/jsx-no-useless-fragment': 'error',
  'react/jsx-props-no-spreading': 'off',
  'quotes': ['error', 'single'],
  'semi': ['error', 'never', {beforeStatementContinuationChars: 'always'}],
  'quote-props': ['error', 'consistent'],
  'local-rules/underscore-argument': 'error',
}

const semanticsRules = {
  'no-param-reassign': ['error', {props: false}],
  'no-plusplus': 'off',
  'no-use-before-define': 'off',
  'no-var': 'error',
  'no-shadow': 'off',
  '@typescript-eslint/no-shadow': 'error',
  'local-rules/acronym-capitalization': 'error',
  'one-var': 'off',
  'react-hooks/exhaustive-deps': 'off',
  'react/destructuring-assignment': 'off',
  'react/jsx-key': 'error',
  'react/prop-types': 'off',
  'react/require-default-props': 'off',
  '@typescript-eslint/no-use-before-define': 'error',
}

const projectRules = {
  'import/exports-last': 'error',
  'import/group-exports': 'error',
  'import/no-extraneous-dependencies': 'off',
  'import/no-duplicates': 'off',  // https://github.com/benmosher/eslint-plugin-import/issues/1403
  'local-rules/commonjs': 'error',
  'no-unused-vars': 'off',
  'local-rules/implicit-any': 'error',
  '@typescript-eslint/no-unused-vars': 'error',
  'react/jsx-filename-extension': ['error', {'extensions': ['.tsx', '.jsx']}],
  'import/extensions': ['error', 'ignorePackages', {
    'ts': 'never',
    'tsx': 'never',
    'js': 'never',
    'jsx': 'never',
  }],
  'import/prefer-default-export': 'off',
  'import/order': ['error', {
    'groups': [
      ['builtin', 'external'],
    ],
    'newlines-between': 'always-and-inside-groups',
  }],
  'local-rules/type-only-imports': 'error',
  'local-rules/untyped-array': 'error',
}

const domRules = {
  'jsx-a11y/label-has-for': ['off'],  // Deprecated
  'jsx-a11y/label-has-associated-control': ['error', {
    'controlComponents': [
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
    'assert': 'both',
    'depth': 25,
  }],
}

module.exports = {
  'env': {
    browser: true,
    es2020: true,
    node: true,
  },
  'globals': {
    'Build8': 'readonly',
    'BuildIf': 'readonly',
    'globalThis': 'writeable',  // TODO: env.es6->env.es2020, github.com/eslint/eslint/issues/12670
  },
  'extends': [
    'eslint:recommended',
    'plugin:react/recommended',
    'plugin:import/typescript',
    'airbnb',
    'airbnb/hooks',
  ],
  'parser': '@typescript-eslint/parser',
  'parserOptions': {
    'ecmaFeatures': {
      'jsx': true,
    },
    'ecmaVersion': 2018,
    'sourceType': 'module',
    'warnOnUnsupportedTypeScriptVersion': false,
  },
  'plugins': ['react', '@typescript-eslint', 'eslint-plugin-local-rules'],
  'rules': {
    ...spacingRules,
    ...syntaxRules,
    ...semanticsRules,
    ...projectRules,
    ...domRules,
  },
  'settings': {
    'import/resolver': {
      'node': {
        'extensions': ['.js', '.jsx', '.ts', '.tsx'],
      },
      'typescript': {},  // this loads <rootdir>/tsconfig.json to eslint
      'alias': {
        'map': [
          ['@repo', `${__dirname}/`],
        ],
        'extensions': ['.js', '.jsx', '.ts', '.tsx'],
      },
    },
    'react': {
      version: 'detect',
    },
  },
}
