/* eslint-disable max-len */

module.exports = {
  rules: {
    'no-restricted-imports': ['error',
      {
        'paths': [{
          'name': 'three',
          'message': 'Please use "./three" or "./three-types" instead to avoid runtime deps on three',
        }],
      },
    ],
  },
}
