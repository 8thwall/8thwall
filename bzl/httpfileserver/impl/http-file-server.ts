/* eslint-disable no-console */

// Express server for serving static content in a specified directory with https.
import * as compression from 'compression'
import * as cors from 'cors'
import * as express from 'express'
import * as fs from 'fs'
import * as https from 'https'
import * as http from 'http'
import * as os from 'os'
import * as path from 'path'
import * as qrcode from 'qrcode-terminal'

import {getCert} from 'bzl/httpfileserver/impl/cert'

const MAX_FILES = 25  // Max files to print for listing static content.

type CliArgs = Omit<Record<string, string|boolean>, '_ordered'> & {
  _node: string
  _script: string
  _ordered: string[]
}

const getArgs = () => {
  const args = process.argv
  const dict = {  // The first two arguments are the node instance and the script that's being run.
    _node: args[0],
    _script: args[1],
    _ordered: [],
  } as CliArgs

  args.slice(2).forEach((arg) => {  // Consume remaining arguments starting at 2.
    if (!arg.startsWith('--')) {
      dict._ordered.push(arg)  // Argument doesn't have a name, add to ordered list.
      return
    }
    const eqsep = arg.substring(2).split('=')  // get arg without '--' and split on =
    if (eqsep.length === 1) {
      dict[eqsep[0]] = true  // There is no '= sign for a value; assign true.
      return
    }
    const val = eqsep.slice(1).join('=')  // Add = back to remaining components if needed.
    dict[eqsep[0]] = val === 'false' || val === '0' ? false : val
  })
  return dict
}


// Try to figure out the most likely IP address a developer should use to hit this server.
const guessIp = () => {
  const ips = Object.entries(os.networkInterfaces())
    .map(([name, interfaces]) => interfaces.map(val => ({...val, name})))
    .flat()
    .sort((a, b) => {
      // Prefer IPv4 addresses
      if (a.family !== b.family) {
        return a.family === 'IPv4' ? -1 : 1
      }
      // Prefer non-internal addresses
      if (a.internal !== b.internal) {
        return a.internal ? 1 : -1
      }
      // Prefer interfaces like 'en0' to ones like 'eth0' or 'utun3'
      if (a.name.startsWith('en') !== b.name.startsWith('en')) {
        return a.name.startsWith('en') ? -1 : 1
      }
      // Otherwise, prefer alphabetically lower interface names.
      return a.name.localeCompare(b.name)
    })
  return ips.length ? ips[0].address : 'localhost'
}

// Return recursive files under a directory.
const walk = dir => fs.readdirSync(dir, {withFileTypes: true}).reduce((results, file) => {
  const name = path.join(dir, file.name)
  if (file.isDirectory()) {
    results.push(...walk(name))
  } else {
    results.push(name)
  }
  return results
}, [])

const args = getArgs()

if (args._ordered.length !== 1) {
  throw new Error(
    `Usage: node server.js [--https=true] /dir/to/serve (got ${process.argv.join(' ')})`)
}

let dir = args._ordered[0]
if (dir[0] !== '/') {
  dir = path.resolve(process.cwd(), dir)
}
console.log(`Serving directory ${dir} with https - accepting CORS requests without restrictions`)

const app = express()
app.use(cors())
app.use(compression())
app.use(express.static(dir))
app.get('/', (request, response) => {
  if (args.root_redirect) {
    response.redirect(args.root_redirect)
  } else {
    response.send(200)
  }
})
app.get('*', (request, response) => { response.sendFile(`${path.join(dir, 'index.html')}`) })

// Only use http if https is explicitly set to 'false'.
const useHttp = args.https === false
const port = args.port || 8888

// Find files that will be served by by this static content server.
const root = `${useHttp ? 'http' : 'https'}://${guessIp()}:${port}`
const files = walk(dir)
  .map(str => `${root}${str.substring(dir.length)}`)  // Remove path prefix and change to URL.
  .slice(0, MAX_FILES + 1)  // Limit the number of results
if (files.length > MAX_FILES) {
  files[MAX_FILES] = '...'  // but show that the number was limited if it was.
}

// Tell the user what content will be served and where.
console.log(
  `Serving static files on ${root}${args.root_redirect ? ' -> ' + args.root_redirect : ''}`)
if (args.root_redirect) {
  qrcode.generate(root, {small: true}, code => console.log(`\n\n${code}\n\n`))
}
console.log('=========================')
console.log('Files:')
files.forEach(f => console.log(f))

// Start the server.
if (useHttp) {
  http.createServer(app).listen(port)
} else {
  https.createServer(getCert({info: console.log}), app).listen(port)
}
