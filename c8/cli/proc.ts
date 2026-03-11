/* eslint-disable no-console */

import {spawn, type SpawnOptions} from 'child_process'

// Execute a long-running command on the commandline, and stream stdout and stderr to the console
// while it's running. Unlike "exec", which captures and returns stdout after the program exits,
// streamExec streams output to console while the program is executing.
const streamExec = (
  cmd: string,
  logPrefix: string = '',
  options?: SpawnOptions
) => new Promise<void>((resolve, reject) => {
  const cmdArgs = cmd.split(' ')  // Not a generic solution.
  const isInherited = options?.stdio === 'inherit'
  const proc = spawn(cmdArgs[0], cmdArgs.slice(1), options!)

  if (!isInherited) {
    proc.stdout?.on('data', data => console.log(logPrefix, data.toString().trim()))
    proc.stderr?.on('data', data => console.error(logPrefix, data.toString().trim()))
    proc.on('exit', code => (code ? reject() : resolve()))
  } else {
    // If stdio is set to 'inherit', we don't need to handle stdout and stderr
    // because they will be inherited from the parent process.
    // However, we still need to handle SIGINT to allow graceful shutdown.
    // This is important for long-running processes that need to be interrupted.
    const handleSigint = () => {
      proc.kill('SIGINT')
    }
    process.on('SIGINT', handleSigint)

    proc.on('exit', (code) => {
      process.off('SIGINT', handleSigint)
      if (code) {
        reject()
      } else {
        resolve()
      }
    })
  }
})

const streamSpawn = <T extends boolean = false>(
  cmd: string,
  args: string[],
  capture?: T,
  options: SpawnOptions = {}
): Promise<
  T extends true ? { stdout: string; stderr: string } : void
  > => new Promise((resolve, reject) => {
  type ReturnType = T extends true ? { stdout: string; stderr: string } : void

  const proc = spawn(cmd, args, options)
  const isInherited = options?.stdio === 'inherit'

  let stdoutData = ''
  let stderrData = ''

  if (!isInherited) {
    proc.stdout?.on('data', (data: Buffer) => {
      const output = data.toString().trim()
      if (capture) {
        stdoutData += `${output}\n`
      } else {
        console.log(output)
      }
    })

    proc.stderr?.on('data', (data: Buffer) => {
      const output = data.toString().trim()
      if (capture) {
        stderrData += `${output}\n`
      } else {
        console.error(output)
      }
    })
  }

  const handleSigint = () => {
    proc.kill('SIGINT')
  }
  process.on('SIGINT', handleSigint)

  proc.on('exit', (code: number) => {
    process.off('SIGINT', handleSigint)
    if (code) {
      const error = new Error(`Command failed with exit code: ${code}`)
      if (!capture) {
        reject(error)
        return
      }

      const errorObject = {
        error,
        stdout: stdoutData.trim(),
        stderr: stderrData.trim(),
      }

      reject(errorObject)
    } else if (capture) {
      resolve({stdout: stdoutData.trim(), stderr: stderrData.trim()} as ReturnType)
    } else {
      resolve(undefined as ReturnType)
    }
  })
  })

export {
  streamExec,
  streamSpawn,
}
