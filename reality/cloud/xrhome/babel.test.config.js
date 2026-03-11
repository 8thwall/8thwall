module.exports = {
  'plugins': [
    [
      'css-modules-transform',
      {
        'extensions': ['.css', '.scss'],
      },
    ],
    '@babel/plugin-proposal-export-default-from',
    '@babel/plugin-proposal-class-properties',
    '@babel/plugin-proposal-export-namespace-from',
    '@babel/plugin-transform-modules-commonjs',
    '@babel/plugin-transform-runtime',
    '@babel/plugin-transform-object-assign',
    '@babel/plugin-syntax-dynamic-import',
    '@babel/plugin-proposal-object-rest-spread',
  ],
  'presets': [
    [
      '@babel/preset-env',
      {
        'targets': {
          'node': true,
        },
        'modules': 'commonjs',
      },
    ],
    '@babel/preset-react',
    '@babel/preset-typescript',
  ],
}
