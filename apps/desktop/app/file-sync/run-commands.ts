import path from 'path'
import fs from 'fs'
import log from 'electron-log'

import {MILLISECONDS_PER_MINUTE} from '@repo/reality/cloud/xrhome/src/shared/time-utils'

import {NODE_MODULES_PATH} from '../resources'
import {forwardProcessOutput} from '../system-log/listeners'
import {parseCommandString, Process, startProcess} from '../../process'

const NPM_CLI_PATH = path.join(NODE_MODULES_PATH, 'npm/bin/npm-cli.js')
const EXEC_PATH: string = process.platform === 'darwin'
// @ts-expect-error It exists according to
// https://github.com/electron/electron/blob/v39.5.1/lib/node/init.ts#L35
  ? process.helperExecPath
  : process.execPath.replace(/\\+$/, '')

const COMMON_ENV = {
  NO_COLOR: '1',
}

type ScriptRunOptions = {
  cwd: string
  name: 'build' | 'serve'
  env?: Record<string, string>
  arguments?: ('--port' | `${number}`)[]
}

const runScript = (options: ScriptRunOptions): Process => {
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

  const additionalArgs = (options.arguments || [])

  // If command starts with 'node ', fork the electron process so that we can run scripts
  // even if node isn't installed.
  if (scriptCommand.startsWith('node ')) {
    const command = [
      EXEC_PATH,
      ...parseCommandString(scriptCommand.slice('node'.length)),
      ...additionalArgs,
    ]
    log.info(`Running command: ${name} - ${command.join(' ')}`)
    return startProcess({
      command,
      cwd,
      env: {...env, ELECTRON_RUN_AS_NODE: '1'},
    })
  }

  // Otherwise invoke the command through npm. Any dependencies will need to be installed
  // on the system.
  return startProcess({
    command: [EXEC_PATH, NPM_CLI_PATH, 'run', name, '--', ...additionalArgs],
    cwd,
    env: {...env, ELECTRON_RUN_AS_NODE: '1'},
  })
}

const runInstallCommand = async (
  appKey: string, savePath: string, extraArguments?: string[]
): Promise<void> => {
  log.info('Running install command', savePath, extraArguments)
  const child = startProcess({
    command: [EXEC_PATH, NPM_CLI_PATH, 'install', ...(extraArguments || [])],
    cwd: savePath,
    env: {...COMMON_ENV, ELECTRON_RUN_AS_NODE: '1'},
  })
  log.info('Install started')
  forwardProcessOutput(appKey, child.nodeChildProcess)

  let timeout: ReturnType<typeof setTimeout> | undefined
  let killed = false
  const restartTimeout = () => {
    clearTimeout(timeout)
    if (killed) {
      return
    }
    timeout = setTimeout(() => {
      log.info('NPM install timeout reached, killing process')
      killed = true
      child.kill()
    }, 2 * MILLISECONDS_PER_MINUTE)
  }
  restartTimeout()
  child.nodeChildProcess.stdout?.on('data', () => restartTimeout())
  child.nodeChildProcess.stderr?.on('data', () => restartTimeout())

  try {
    await child.exitSuccess()
    log.info('Install complete!')
  } catch (err) {
    log.error('Install failed:', err)
    throw err
  } finally {
    clearTimeout(timeout)
  }
}

const runBuildCommand = async (
  appKey: string,
  savePath: string
): Promise<void> => {
  const child = runScript({
    cwd: savePath,
    name: 'build',
    env: COMMON_ENV,
  })

  forwardProcessOutput(appKey, child.nodeChildProcess)

  await child.exitSuccess()
}

const runServeCommand = (savePath: string, port: number): Process => runScript({
  cwd: savePath,
  name: 'serve',
  env: {...COMMON_ENV, PORT: port.toString()},
  arguments: ['--port', `${port}`],
})

export {
  runServeCommand,
  runInstallCommand,
  runBuildCommand,
}
