import * as fs from 'fs'
import * as path from 'path'
import * as selfsigned from 'selfsigned'
import * as os from 'os'

export function getCert(log, key = null, cert = null, name = 'https-static') {
  const options = {
    requestCert: false,
    key,
    cert,
    spdy: {protocols: ['h2', 'http/1.1']},
  }
  let fakeCert
  if (!options.key || !options.cert) {
    // Use a self-signed certificate if no certificate was configured.
    // Cycle certs every 24 hours
    const certPath = [os.homedir, '.c8', `${name}.pem`].join(path.sep)
    let certExists = fs.existsSync(certPath)

    const days = 365
    if (certExists) {
      const certStat = fs.statSync(certPath)
      const certTtl = 1000 * 60 * 60 * 24
      const now = new Date()

      const certAge = (now.valueOf() - certStat.ctime.valueOf()) / certTtl
      // cert is more than ${days} days old, kill it with fire
      if (certAge > days) {
        log.info(`SSL Certificate is more than ${days} days old. Regenerating...`)
        fs.unlinkSync(certPath)
        certExists = false
      }
    }

    if (!certExists) {
      log.info(`Generating SSL Certificate ${certPath}`)
      const attrs = [{name: 'commonName', value: 'localhost'}]
      const pems = selfsigned.generate(attrs, {
        algorithm: 'sha256',
        days,
        keySize: 2048,
        extensions: [{
          name: 'basicConstraints',
          cA: true,
        }, {
          name: 'keyUsage',
          keyCertSign: true,
          digitalSignature: true,
          nonRepudiation: true,
          keyEncipherment: true,
          dataEncipherment: true,
        }, {
          name: 'extKeyUsage',
          serverAuth: true,
          clientAuth: true,
        }, {
          name: 'subjectAltName',
          altNames: [
            {type: 2, value: 'localhost'},
            {type: 2, value: 'localhost.localdomain'},
            {type: 2, value: 'lvh.me'},
            {type: 2, value: '*.lvh.me'},
            {type: 2, value: '[::1]'},
            {type: 7, ip: '127.0.0.1'},
            {type: 7, ip: 'fe80::1'},
          ],
        }],
      })
      const certDir = path.dirname(certPath)
      if (!fs.existsSync(certDir)) {
        fs.mkdirSync(certDir)
      }
      fs.writeFileSync(certPath, pems.private + pems.cert, {encoding: 'utf-8'})
    }
    fakeCert = fs.readFileSync(certPath)
  }

  options.key = options.key || fakeCert
  options.cert = options.cert || fakeCert

  return options
}
