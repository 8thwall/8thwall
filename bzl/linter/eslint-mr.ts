/* eslint-disable no-console */
import {EslintMessage, getErrorsBetweenCommit} from './eslint-diff'
import {clearAutomatedMrCommentsFromCi, postMrCommentFromCi} from '../../ci-support/gitlab-service'

const MAX_MESSAGE_LENGTH = 70

const COMMENT_MATCH = '🔍'

const toFixedLength = (s: string, maxLength: number) => {
  if (s.length <= maxLength) {
    return s.padEnd(maxLength)
  } else {
    return `${s.substr(0, maxLength - 3)}...`
  }
}

const getErrorLine = (error: EslintMessage) => {
  const line = [
    error.line.toString().padStart(5),
    ':',
    error.column.toString().padEnd(4),
    ' ',
    toFixedLength(error.message, MAX_MESSAGE_LENGTH),
    ' ',
    (error.ruleId || ''),
    error.fix ? ' (Fixable)' : '',
  ].join('')

  return line
}

const run = async () => {
  try {
    const forkPoint = process.env.CI_MERGE_REQUEST_DIFF_BASE_SHA
    if (!forkPoint) {
      throw new Error('Expected CI_MERGE_REQUEST_DIFF_BASE_SHA to be defined')
    }
    const report = await getErrorsBetweenCommit(forkPoint, '')
    if (!report.errorReports.length) {
      await clearAutomatedMrCommentsFromCi(COMMENT_MATCH)
      console.log('No new errors to report')
      return
    }

    const summaryLines: string[] = []

    const outputLine = (line: string) => {
      if (summaryLines.length < 25) {
        summaryLines.push(line)
      }
      console.log(line)
    }

    report.errorReports.forEach((r) => {
      outputLine(r.filename)
      r.newErrors.forEach(e => outputLine(getErrorLine(e)))
      outputLine('')
    })

    const fullMessage = `
<details><summary>
${report.newTotalErrors} New ESLint ${report.newTotalErrors === 1 ? 'error' : 'errors'}
</summary>

\`\`\`

${summaryLines.join('\n')}

\`\`\`

${COMMENT_MATCH} [Full output](${process.env.CI_JOB_URL})

</details>

`

    await postMrCommentFromCi(fullMessage)

    process.exit(1)
  } catch (err) {
    // eslint-disable-next-line no-console
    console.error(err)
    process.exit(1)
  }
}

run()
