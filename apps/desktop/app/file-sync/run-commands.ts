import {ChildProcess, exec, fork} from 'child_process'
import path from 'path'
import fs from 'fs'
import log from 'electron-log'

import {NODE_MODULES_PATH} from '../resources'

const LOCAL_SSL_PROXY_CLI = 'local-ssl-proxy/build/main.js'
const NPM_CLI_PATH = path.join(NODE_MODULES_PATH, 'npm/bin/npm-cli.js')
const EXEC_PATH = process.platform === 'darwin'
// @ts-expect-error It exists according to
// https://github.com/electron/electron/blob/v39.5.1/lib/node/init.ts#L35
  ? process.helperExecPath
  : process.execPath.replace(/\\+$/, '')

type ScriptRunOptions = {
  cwd: string
  name: 'build' | 'serve'
  env?: Record<string, string>
  arguments?: ('--port' | `${number}`)[]
}

const runScript = (options: ScriptRunOptions): ChildProcess => {
  const {cwd, name, env} = options

  const packageJsonPath = path.resolve(cwd, 'package.json')
  let scriptCommand: string | undefined
  try {
    if (fs.existsSync(packageJsonPath)) {
      const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf-8'))
      scriptCommand = packageJson.scripts?.[name]
    }
  } catch (error) {
    log.error('Failed to read package.json:', error)
    throw new Error('Failed to read package.json')
  }

  if (!scriptCommand) {
    throw new Error(`npm run ${name} command not found in package.json`)
  }

  // TODO(christoph): Escape these args, currently only allows port and port number
  const additionalArgs = (options.arguments || []).join(' ')

  // If command starts with 'node ', fork the electron process so that we can run scripts
  // even if node isn't installed.
  if (scriptCommand.startsWith('node ')) {
    const command = `"${EXEC_PATH}"${scriptCommand.slice('node'.length)} ${additionalArgs}`
    log.info(`Running command: ${name} - ${command}`)
    return exec(
      command,
      {cwd, env: {...env, ELECTRON_RUN_AS_NODE: '1'}}
    )
  }

  // Otherwise invoke the command through the shell, assuming npm is installed on the PATH.
  return exec(
    `npm run ${name} -- ${additionalArgs}`,
    {cwd, env, shell: process.env.SHELL}
  )
}

const runInstallCommand = async (
  savePath: string
): Promise<void> => new Promise((resolve, reject) => {
  const child = fork(
    NPM_CLI_PATH,
    ['install'],
    {cwd: savePath, stdio: 'pipe', env: {ELECTRON_RUN_AS_NODE: '1'}, detached: true}
  )

  const timeout = setTimeout(() => {
    log.error('npm install timed out, killing process')
    child.kill('SIGTERM')
    reject(new Error('npm install timed out'))
  }, 60000)

  let stderr = ''

  child.stderr?.on('data', (d) => {
    const out = d.toString()
    stderr += out
  })

  child.on('exit', (code, signal) => {
    clearTimeout(timeout)
    if (code === 0) {
      resolve()
    } else {
      const msg = `npm install failed (exit ${code}, signal ${signal})\nstderr:\n${stderr}`
      log.error(msg)
      reject(new Error(msg))
    }
  })

  child.on('error', (err) => {
    clearTimeout(timeout)
    log.error('npm install process error:', err)
    if (stderr) {
      log.error('npm install stderr:', stderr)
    }
    reject(err)
  })
})

const runBuildCommand = (
  savePath: string
): Promise<void> => new Promise((resolve, reject) => {
  const child = runScript({
    cwd: savePath,
    name: 'build',
  })

  child.on('exit', (code, signal) => {
    if (signal === 'SIGTERM' || signal === 'SIGKILL') {
      return
    }
    if (code === 0) {
      resolve()
    } else {
      const msg = 'webpack build failed'
      log.error(msg)
      reject(new Error(msg))
    }
  })

  child.on('error', (err) => {
    log.error('webpack build process error:', err)
    reject(err)
  })
})

// Run a https proxy that proxy all traffic to https://localhost:{proxySrcPort} to
// http://localhost:{proxyDestPort}
// This uses https://www.npmjs.com/package/http-proxy under the hood.
// TODO(dat): Switch to calling http-proxy directly instead of spawning a child process?
//            Perhaps the best option is to provide reverse proxy, ala ngrok, so our users can
//            connect remotely from anywhere.
const runProxyCommand = (
  projectPath: string,
  proxySrcPort: number = 9001,
  proxyDestPort: number = 9002
): ChildProcess => {
  const localSslProxyCliPath = path.resolve(projectPath, 'node_modules', LOCAL_SSL_PROXY_CLI)

  const child = fork(
    localSslProxyCliPath,
    ['--source', proxySrcPort.toString(), '--target', proxyDestPort.toString()],
    {stdio: 'pipe', env: {ELECTRON_RUN_AS_NODE: '1'}, detached: false}
  )

  let stderr = ''

  child.stderr?.on('data', (d) => {
    const out = d.toString()
    stderr += out
  })

  child.on('exit', (code, signal) => {
    if (signal === 'SIGTERM' || signal === 'SIGKILL') {
      return
    }
    if (code !== 0) {
      const msg = `ssl proxy failed (exit ${code}, signal ${signal})\nstderr:\n${stderr}`
      log.error(msg)
    }
  })

  child.on('error', (err) => {
    log.error('ssl proxy process error:', err)
    if (stderr) {
      log.error('ssl proxy stderr:', stderr)
    }
    throw err
  })

  return child
}

const runServeCommand = (savePath: string, port: number): ChildProcess => {
  const child = runScript({
    cwd: savePath,
    name: 'serve',
    env: {PORT: port.toString()},
    arguments: ['--port', `${port}`],
  })

  let stderr = ''

  child.stderr?.on('data', (d) => {
    const out = d.toString()
    stderr += out
  })

  child.on('exit', (code, signal) => {
    if (signal === 'SIGTERM' || signal === 'SIGKILL') {
      return
    }
    if (code !== 0) {
      const msg = `webpack failed (exit ${code}, signal ${signal})\nstderr:\n${stderr}`
      log.error(msg)
    }
  })

  child.on('error', (err) => {
    log.error('webpack process error:', err)
    if (stderr) {
      log.error('webpack stderr:', stderr)
    }
    throw err
  })

  return child
}

export {
  runServeCommand,
  runProxyCommand,
  runInstallCommand,
  runBuildCommand,
}
