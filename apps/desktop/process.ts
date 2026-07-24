import type ChildProcess from 'child_process'
import {execa, parseCommandString} from 'execa'

type ProcessOptions = {
  command: string[]
  env?: Record<string, string>
  cwd: string
}

const IS_WINDOWS = process.platform === 'win32'

type Process = {
  kill: () => Promise<void>
  exitSuccess: () => Promise<void>
  isRunning: () => boolean
  nodeChildProcess: ChildProcess.ChildProcess
}

const isProcessRunning = (
  process: ChildProcess.ChildProcess | undefined
): process is ChildProcess.ChildProcess => (
  !!process && !process.killed && process.exitCode === null
)

const killProcess = async (process: ChildProcess.ChildProcess | undefined): Promise<void> => {
  if (!isProcessRunning(process)) {
    return
  }

  if (IS_WINDOWS) {
    await execa('taskkill', ['/pid', String(process.pid), '/T', '/F'])
    return
  }

  await new Promise<void>((resolve) => {
    process.on('exit', () => {
      resolve()
    })

    process.kill('SIGTERM')

    // Fallback: force kill after 5 seconds if SIGTERM doesn't work
    setTimeout(() => {
      if (process && !process.killed) {
        process.kill('SIGKILL')
      }
    }, 5000)
  })
}

const startProcess = (options: ProcessOptions): Process => {
  const child = execa(
    options.command[0],
    options.command.slice(1),
    {
      cwd: options.cwd,
      env: options.env,
    }
  )
  return {
    isRunning: () => isProcessRunning(child.nodeChildProcess),
    kill: () => killProcess(child.nodeChildProcess),
    nodeChildProcess: child.nodeChildProcess,
    exitSuccess: async () => {
      await child
    },
  }
}

export {
  parseCommandString,
  startProcess,
}

export type {
  ProcessOptions,
  Process,
}
