import {readFileSync} from 'fs'

import type {Recording, SequenceMetadata} from '../src/client/editor/app-preview/app-preview-utils'

/* eslint-disable max-len */
// Usage:
// - Go to https://docs.google.com/spreadsheets/d/<REMOVED_BEFORE_OPEN_SOURCING>
// - Paste this into your terminal: pbpaste | npx ts-node tools/process-sequences.ts -- > ~/repo/prod8/cdn/web/app-preview/sequence_metadata.json
// - Copy the spreadsheet to your clipboard
// - Press enter in the terminal
/* eslint-enable max-len */

const data = readFileSync(0, 'utf8')

const [headerLine, ...lines] = data.split('\n').filter(line => line.trim().length > 0)

const headers = headerLine.split('\t')

const rows = lines.map(line => line.split('\t'))

const asString = (v: any) => {
  if (typeof v !== 'string') {
    throw new Error(`Expected string, got ${typeof v}`)
  }
  return v.trim()
}

const asUrl = (v: any) => {
  try {
    return new URL(asString(v)).toString()
  } catch (e) {
    throw new Error(`Expected URL, got ${v}`)
  }
}

// eslint-disable-next-line arrow-parens
const asOneOf = <T>(values: T[], v: any): T => {
  if (!values.includes(v)) {
    throw new Error(`Expected one of ${values.join(', ')}, got ${v}`)
  }
  return v
}

// eslint-disable-next-line arrow-parens
const optionalOr = <T>(v: any, fn: (v: any) => T) => {
  if (!v) {
    return undefined
  }

  return fn(v)
}

const asNumber = (v: any) => {
  const value = Number(v)
  if (Number.isNaN(value)) {
    throw new Error(`Expected number, got ${v}`)
  }
  return value
}

type ParsedRow = {
  sequenceName: string
  variationName: string
  tag: string
  section: string
  cameraUrl: string
  sequenceUrl: string
  videoWidth: number
  videoHeight: number
}

const parsedRows = rows.map((row) => {
  const obj: Record<string, string> = {}
  row.forEach((value, index) => {
    obj[headers[index]] = value
  })
  return obj
}).map((row): ParsedRow => ({
  sequenceName: asString(row['Sequence Name']),
  variationName: asString(row['Variation Name']),
  tag: asString(row.Tag),
  section: asOneOf(['WORLD', 'PEOPLE', 'IMAGE_TARGETS'], row.Section),
  cameraUrl: asUrl(row['Camera URL']),
  sequenceUrl: optionalOr(row['Sequence URL'], asUrl),
  videoWidth: optionalOr(row['Video Width'], asNumber),
  videoHeight: optionalOr(row['Video Height'], asNumber),
}))

const sequenceMetadata: SequenceMetadata = {
  // eslint-disable-next-line max-len
  defaultUserAgent: 'Mozilla/5.0 (Linux; Android 10; Pixel 3a) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.159 Mobile Safari/537.36',
  sequences: [],
}

parsedRows.forEach((row) => {
  let sequence = sequenceMetadata.sequences.find(s => s.name === row.sequenceName)
  if (sequence) {
    if (sequence.tag !== row.tag) {
      throw new Error(`Sequence ${row.sequenceName} has conflicting tag.`)
    }
    if (sequence.section !== row.section) {
      throw new Error(`Sequence ${row.sequenceName} has conflicting section.`)
    }
  } else {
    sequence = {
      name: row.sequenceName,
      tag: row.tag,
      section: row.section,
      variations: {},
    }
    sequenceMetadata.sequences.push(sequence)
  }

  const existingVariation = sequence.variations[row.variationName]

  const newRecording: Recording = {
    cameraUrl: row.cameraUrl,
    sequenceUrl: row.sequenceUrl,
  }

  if (row.videoHeight && row.videoWidth) {
    newRecording.orientation = row.videoHeight > row.videoWidth ? 'portrait' : 'landscape'
  }

  if (!existingVariation) {
    sequence.variations[row.variationName] = newRecording
  } else if (Array.isArray(existingVariation)) {
    existingVariation.push(newRecording)
  } else {
    sequence.variations[row.variationName] = [existingVariation, newRecording]
  }
})

// eslint-disable-next-line no-console
console.log(JSON.stringify(sequenceMetadata, null, 2))
