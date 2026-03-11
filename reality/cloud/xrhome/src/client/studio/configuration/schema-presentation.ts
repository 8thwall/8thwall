import type {DeepReadonly} from 'ts-essentials'
import type {
  FieldPresentation, GroupPresentation, SectionPresentation, PropertyType, ReadData, Schema, Type,
  Presentation,
} from '@ecs/shared/schema'
import type {StudioComponentMetadata} from '@ecs/shared/studio-component'

import {evaluateCondition} from '../evaluate-condition'
import {TYPE_TO_FIELD_DEFAULT} from './schema-presentation-constants'

type SingleDisplayData = {
  mode: 'single', type: Type, value: PropertyType, presentation: FieldPresentation
}

type GroupDisplayData = {
  // Note: subField assumes that the array order of fields matches the expected display order for
  // for the type of group. e.g. for a Vector3 group, the fields should be in the order X, Y, Z.
  // (should this ever need to change, we can add a field order to the group presentation)
  mode: 'group', subFields: Record<string, SingleDisplayData>, presentation: GroupPresentation
}

type SectionDisplayData = {
  mode: 'section'
  subFields: Record<string, SingleDisplayData|GroupDisplayData>
  presentation: SectionPresentation
}

// Schemas for the properties of a component are a linear list, but when displaying them as fields
// we need a tree structure that represents the hierarchy of the fields (with groups, sections
// etc). This type represents a node in that tree with all the associated display data.
type FieldDisplayData = SingleDisplayData | GroupDisplayData | SectionDisplayData

type FieldDisplayDataTree = Record<string, FieldDisplayData>

const shouldDisplayField = <T extends Schema>(
  // eslint-disable-next-line arrow-parens
  metadata: DeepReadonly<StudioComponentMetadata<T>>, values: ReadData<T>, key: string
): boolean => {
  const presentation = metadata.schemaPresentation?.fields[key]
  if (!presentation) {
    return true
  }

  // If the presentation has a group, check if the group condition is met.
  if (presentation.group) {
    const group = metadata.schemaPresentation.groups[presentation.group]
    if (!evaluateCondition(metadata, values, group.condition)) {
      return false
    }
  }

  // If the presentation has a section, check if the section condition is met.
  if (presentation.section) {
    const section = metadata.schemaPresentation.sections[presentation.section]
    if (!evaluateCondition(metadata, values, section.condition)) {
      return false
    }
  }

  return evaluateCondition(metadata, values, presentation.condition)
}

const filterConditionalFields = <T extends Schema>(
  // eslint-disable-next-line arrow-parens
  metadata: DeepReadonly<StudioComponentMetadata<T>>, values: ReadData<T>
): Partial<T> => {
  const displayList = {}
  Object.entries(metadata.schema).forEach(([key, value]) => {
    if (shouldDisplayField(metadata, values, key)) {
      displayList[key] = value
    }
  })

  return displayList
}

const getCollectionData = (
  mode: 'section'|'group',
  currentFieldPresentation: FieldPresentation,
  parentData: Record<string, FieldDisplayData>,
  collectionPresentations: Record<string, Presentation>
): GroupDisplayData|SectionDisplayData => {
  const name = currentFieldPresentation?.[mode]
  if (!name) {
    return undefined
  }

  const existingNode = parentData[name]
  if (existingNode) {
    if (existingNode.mode !== mode) {
      throw new Error(`Expected ${mode} mode`)
    }
    return existingNode
  }

  const newNode: SectionDisplayData | GroupDisplayData = {
    mode, presentation: collectionPresentations[name], subFields: {},
  }
  parentData[name] = newNode
  return newNode
}

const buildDisplayDataTree = <T extends Schema>(
  // eslint-disable-next-line arrow-parens
  metadata: DeepReadonly<StudioComponentMetadata<T>>, values: ReadData<T>
): FieldDisplayDataTree => {
  const res = filterConditionalFields(metadata, values)

  const tree: FieldDisplayDataTree = {}
  const {schemaPresentation} = metadata

  Object.entries(res).forEach(([key, value]) => {
    // schemaPresentation will be undefined if presentation data has not been parsed for this schema
    const presentation = schemaPresentation?.fields[key]
    const data: FieldDisplayData = {
      mode: 'single',
      type: value,
      presentation,
      value: values[key] ?? metadata.schemaDefaults?.[key] ?? TYPE_TO_FIELD_DEFAULT[value],
    }

    const section = getCollectionData('section', presentation, tree, schemaPresentation?.sections)
    const parentData = section?.subFields ?? tree
    const group = getCollectionData('group', presentation, parentData, schemaPresentation?.groups)
    const fields = group?.subFields ?? parentData
    fields[key] = data
  })

  return tree
}

export {
  FieldDisplayData,
  buildDisplayDataTree,
}
