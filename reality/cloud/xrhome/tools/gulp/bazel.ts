import {CODE8, runChildProcess} from './process'

const BAZEL_SILENT_OPTIONS = '--ui_event_filters=-info,-stdout,-stderr --noshow_progress'
const BAZEL_STDOUT_ONLY_OPTIONS = '--ui_event_filters=-info,-stderr --noshow_progress'

let isBazelRunning = false
const wrapBazel = async <T>(fn: () => Promise<T>): Promise<T> => {
  if (isBazelRunning) {
    throw new Error('Bazel is already running. Please ensure all bazel processes are in series()')
  }
  isBazelRunning = true
  try {
    return await fn()
  } finally {
    isBazelRunning = false
  }
}

type Target = `//${string}`

const formatTargets = (targets: Target | Target[]) => (typeof targets === 'string'
  ? targets
  : targets.join(' '))

const formatFlags = (extraFlags?: string[]) => (extraFlags ? extraFlags.join(' ') : '')

const bazelBuild = async (targets: Target | Target[], extraFlags?: string[]) => (
  wrapBazel(() => runChildProcess(
    `bazel build ${BAZEL_SILENT_OPTIONS} ${formatFlags(extraFlags)} -- ${formatTargets(targets)}`,
    {cwd: CODE8}
  ))
)

const bazelOutputFiles = async (
  targets: Target | Target[], extraFlags?: string[]
): Promise<string[]> => wrapBazel(async () => {
  const out = await runChildProcess(
    `bazel cquery --output=files ${BAZEL_STDOUT_ONLY_OPTIONS} ${formatFlags(extraFlags)} ` +
      `-- ${formatTargets(targets)}`,
    {cwd: CODE8, stdio: ['ignore', 'pipe', 'inherit']}
  )
  return out.split('\n').filter(Boolean)
})

export {
  wrapBazel,
  bazelBuild,
  bazelOutputFiles,
}
