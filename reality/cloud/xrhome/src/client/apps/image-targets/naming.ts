/* eslint-disable camelcase */
import type {enum_ImageTargets_type} from '../../common/types/db'
import {DISALLOWED_NAME_CHARACTERS} from '../../../shared/image-target-constants'

const makeFileName = (currentName: string, otherImageNames: string[]): string => {
  let suggestedNameBase = currentName
  const possibleExtension = suggestedNameBase.toLowerCase().slice(-4)
  if (possibleExtension === '.jpg' || possibleExtension === '.png') {
    suggestedNameBase = suggestedNameBase.slice(0, -4)
  } else if (suggestedNameBase.toLowerCase().slice(-5) === '.jpeg') {
    suggestedNameBase = suggestedNameBase.slice(0, -5)
  }

  // Filter out disallowed characters
  suggestedNameBase = suggestedNameBase.split('')
    .filter(e => !DISALLOWED_NAME_CHARACTERS.includes(e))
    .join('') || 'ImageTarget'

  let number = 1
  let suggestedName = suggestedNameBase
  while (otherImageNames.includes(suggestedName)) {
    suggestedName = `${suggestedNameBase}-${number++}`
  }
  return suggestedName
}

const TYPE_TO_STRING: Record<enum_ImageTargets_type, string> = {
  'PLANAR': 'flat',
  'CYLINDER': 'cylindrical',
  'CONICAL': 'conical',
  'SCAN': 'scan',
  'UNSPECIFIED': '',
}
const toTypeName = (type: enum_ImageTargets_type): string => TYPE_TO_STRING[type] || ''

export {
  toTypeName,
  makeFileName,
}
