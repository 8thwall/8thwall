const {getEslintMap} = require('./alias-config')

module.exports = {
  rules: {
    'local-rules/express-deprecated-send': 'error',
    'local-rules/ui-component-styling': 'error',
    'local-rules/reality-shared-imports': 'error',
    'local-rules/prefer-await': 'warn',
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
    'no-restricted-imports': ['error',
      {
        'paths': [{
          'name': 'react-redux',
          'importNames': ['useSelector'],
          'message': 'Please use xrhome/src/client/hooks.ts useSelector instead',
        }, {
          'name': 'react-redux',
          'importNames': ['connect'],
          'message': 'Please use xrhome/src/client/common/connect.ts instead',
        }, {
          'name': 'real-semantic-ui-react',
          'message': 'Please use `semantic-ui-react` instead',
        }, {
          name: 'supertest',
          message: 'Please use mock-request.ts instead',
        }, {
          'name': 'firebase/auth',
          'importNames': ['getAuth'],
          'message': 'Please use xrhome/src/client/lightship/common/firebase.ts instead',
        }],
      },
    ],
    'react/no-unknown-property': ['error', {ignore: ['a8']}],
    'react/state-in-constructor': 'off',
  },
  settings: {
    'import/resolver': {
      node: {
        extensions: ['.js', '.jsx', '.ts', '.tsx'],
      },
      alias: {
        map: getEslintMap(),
        extensions: ['.js', '.jsx', '.ts', '.tsx'],
      },
      typescript: {},
    },
  },
}
