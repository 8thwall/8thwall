/* eslint-disable no-console */
import path from 'path'
// NOTE(christoph) Gulp doesn't handle child process correctly, so use the runChildProcess instead
import childDoNotUse from 'child_process'
import type {StdioOptions} from 'child_process'
import {parseArgsStringToArgv} from 'string-argv'

const CODE8 = path.resolve(
  __dirname,
  childDoNotUse.execSync('bazel info workspace', {
    stdio: ['ignore', 'pipe', 'ignore'],
  }).toString().trim()
)

type ChildProcessOptions = {cwd?: string, stdio?: StdioOptions}

const JUST_STDERR: StdioOptions = ['ignore', 'ignore', 'inherit']

const runChildProcess = (fullCommand: string, options: ChildProcessOptions = {}) => {
  if (!options.stdio) {
    options.stdio = JUST_STDERR
  }

  const firstSpaceIndex = fullCommand.indexOf(' ')

  let command: string
  let args: string[] | undefined
  if (firstSpaceIndex === -1) {
    command = fullCommand
    args = []
  } else {
    command = fullCommand.substr(0, firstSpaceIndex)
    args = parseArgsStringToArgv(fullCommand.substr(firstSpaceIndex + 1))
  }

  return new Promise<void>((resolve, reject) => {
    const childProcess = childDoNotUse.spawn(command, args, options)
    childProcess.on('exit', (code) => {
      if (code) {
        reject(new Error(`Failed to run: ${fullCommand}`))
      } else {
        resolve()
      }
    })
  })
}

export {
  CODE8,
  runChildProcess,
}
