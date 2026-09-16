const {spawnSync} = require('child_process')

module.exports = async (configuration) => {
  if (process.env.RELEASE !== 'true' || process.env.DEPLOY_STAGE !== 'prod') {
    // eslint-disable-next-line no-console
    console.log('Skipping code signing for non-release build.')
    return
  }

  spawnSync('smctl', [
    'sign',
    '--keypair-alias', process.env.DIGICERT_KEYPAIR_ALIAS,
    '--certificate', process.env.DIGICERT_CERTIFICATE_PATH,
    '--config-file', process.env.PKCS11_CONFIG,
    '--input', configuration.path,
    '--tool', 'jsign',
  ], {
    stdio: 'inherit',
  })
}
