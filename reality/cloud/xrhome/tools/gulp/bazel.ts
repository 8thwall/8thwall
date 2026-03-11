import {CODE8, runChildProcess} from './process'

const BAZEL_SILENT_OPTIONS = '--ui_event_filters=-info,-stdout,-stderr --noshow_progress'

let isBazelRunning = false
const wrapBazel = async (fn: () => Promise<void>) => {
  if (isBazelRunning) {
    throw new Error('Bazel is already running. Please ensure all bazel processes are in series()')
  }
  isBazelRunning = true
  try {
    await fn()
  } finally {
    isBazelRunning = false
  }
}

type Target = `//${string}`
const bazelBuild = async (targets: Target | Target[], extraFlags?: string[]) => (
  wrapBazel(() => runChildProcess(
    `bazel build ${BAZEL_SILENT_OPTIONS} \
${extraFlags ? extraFlags.join(' ') : ''} -- \
${typeof targets === 'string' ? targets : targets.join(' ')}`,
    {cwd: CODE8}
  ))
)

export {
  wrapBazel,
  bazelBuild,
}
