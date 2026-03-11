import {exec as execNode} from 'child_process'
import path from 'path'
import {promises as fs} from 'fs'

const filterFolderPrefix = (line: string) => line.startsWith('src/') &&
!line.startsWith('src/shared/ecs/') &&
!line.startsWith('src/shared/studio/') &&
!line.startsWith('src/shared/nae/') &&
!line.startsWith('src/third_party/') &&
!line.startsWith('src/client/ads/gen/')

const cwd = process.cwd()

const getCommandOutput = (command: string) => new Promise<string>((resolve) => {
  execNode(
    command,
    {
      cwd,
      env: {
        ...process.env,
        NODE_OPTIONS: '--max-old-space-size=8092',
      },
    },
    (_, stdout) => {
      resolve(stdout.toString().trim())
    }
  )
})

const listChangedTypescript = async (fromCommit: string) => {
  const changedFilesCommand = [
    'git diff',
    '-w',                 // Ignore whitespace changes
    '--name-status',      // Show filenames instead of lines
    '--diff-filter=MAR',  // Limit to modified, added, renamed, or deleted files
    '-z',                 // Use a NUL character delimited output
    fromCommit,
  ].join(' ')

  const outputParts = (await getCommandOutput(changedFilesCommand)).split('\x00').filter(Boolean)

  const changedFiles: string[] = []

  let index = 0
  while (index < outputParts.length) {
    // Rename lines have two filenames, the rest have one.
    switch (outputParts[index][0]) {
      case 'M':
      case 'A':
        changedFiles.push(outputParts[index + 1])
        index += 2
        break
      case 'R':
        changedFiles.push(outputParts[index + 2])
        index += 3
        break
      default:
        throw new Error(`Unexpected element: ${outputParts[index]}`)
    }
  }

  return changedFiles.map(e => path.relative('reality/cloud/xrhome', e))
    .filter(filterFolderPrefix)
    .filter(e => e.endsWith('.ts') || e.endsWith('.tsx'))
}

const run = async (baseCommitId: string) => {
  const errorText = await getCommandOutput('npx tsc --project tsconfig.json --noEmit')

  const unfilteredLines = errorText.split('\n')
  // We expect to always see this error, so not seeeing it is an indication of TypeScript exiting
  // early due to parse errors or OOM.
  if (!unfilteredLines.some(line => line.startsWith('test/run-queue-test'))) {
    // eslint-disable-next-line no-console
    console.error(errorText)
    throw new Error('TypeScript exited early, check for parse errors or OOM')
  }

  const lines = unfilteredLines.filter(filterFolderPrefix)
  if (lines.length) {
    // eslint-disable-next-line no-console
    console.log('Errors found:', lines.length)

    lines.forEach((line) => {
      // eslint-disable-next-line no-console
      console.log(line)
    })

    process.exit(1)
  }

  const changedFiles = await listChangedTypescript(baseCommitId)

  const legacyErrorFiles = await Promise.all(changedFiles.map(async (file) => {
    const content = await fs.readFile(file, 'utf8')
    return content.includes('LEGACY_TS_ERRORS') ? file : null
  })).then(results => results.filter(Boolean))

  if (legacyErrorFiles.length) {
    // eslint-disable-next-line no-console
    console.log('LEGACY_TS_ERRORS found in files, they should be fixed before making changes:')
    legacyErrorFiles.forEach((file) => {
      // eslint-disable-next-line no-console
      console.log('  ', file)
    })
    process.exit(1)
  }

  // eslint-disable-next-line no-console
  console.log('No errors found.')
}

run(process.argv[2])
