// @rule(js_cli)
// @package(npm-eslint)
// attr(externals = "eslint")
// @attr(externalize_npm = 1)

/* eslint-disable no-console */
import {EslintMessage, getErrorsBetweenCommit} from './eslint-diff'

const MAX_MESSAGE_LENGTH = 70

const toFixedLength = (s: string, maxLength: number) => {
  if (s.length <= maxLength) {
    return s.padEnd(maxLength)
  } else {
    return `${s.substr(0, maxLength - 3)}...`
  }
}

const getErrorLine = (filename: string, error: EslintMessage) => {
  const line = [
    filename,
    ':',
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
    const forkPoint = process.argv[2]
    if (!forkPoint) {
      throw new Error('Expected argv[2]')
    }
    const report = await getErrorsBetweenCommit(forkPoint, '')
    if (!report.errorReports.length) {
      console.log('No new errors to report')
      return
    }

    report.errorReports.forEach((r) => {
      r.newErrors.forEach(e => console.log(getErrorLine(r.filename, e)))
      console.log('')
    })

    process.exit(1)
  } catch (err) {
    console.error(err)
    process.exit(1)
  }
}

run()
