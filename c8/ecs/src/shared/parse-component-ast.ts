import {parse} from '@babel/parser'
import type {
  Statement, Expression, PatternLike, CallExpression, ObjectExpression, SourceLocation, Comment,
  Node,
} from '@babel/types'

import {ALL_ATTRIBUTE_TYPES, GROUP_TYPES} from './component-constants'
import type {
  Type, SchemaDefaults, SchemaDefaultType, SchemaPresentation, GroupPresentation, AttributeTypeName,
  GroupType, FieldPresentation, FieldPresentationMode, SectionPresentation,
  AssetKind, Presentation,
} from './schema'
import type {
  Location, ParsedComponents, StudioComponentData, StudioComponentError,
} from './studio-component'
import {ASSET_KINDS} from './asset-constants'

const REGISTER_COMPONENT_PARAMS = [
  'name', 'schema', 'schemaDefaults', 'stateMachine', 'data', 'add', 'tick', 'remove',
]

type Undefinable<T> = {[P in keyof T]: T[P] | undefined}

const nodeToLocation = (loc: SourceLocation): Location => ({
  startLine: loc.start.line,
  startColumn: loc.start.column,
  endLine: loc.end.line,
  endColumn: loc.end.column,
})

const createComponentError = (
  message: string, severity: 'warning' | 'error', node: Node | Comment | null
): StudioComponentError => ({
  message,
  severity,
  location: node?.loc ? nodeToLocation(node.loc) : undefined,
})

class ParseError extends Error {
  location?: Location

  constructor(message: string,
    node?: Node) {
    super(message)
    this.location = node?.loc ? nodeToLocation(node.loc) : undefined
  }
}

const getRegisterComponentNode = (node: CallExpression) => {
  // Handles ecs.registerComponent()
  if (node.type === 'CallExpression') {
    const {callee} = node
    if (callee.type === 'MemberExpression') {
      // TODO(christoph): Technically ecs could have been imported as a different name
      if (
        callee.object.type === 'Identifier' &&
        callee.object.name === 'ecs' &&
        callee.property.type === 'Identifier' &&
        callee.property.name === 'registerComponent'
      ) {
        return callee.property
      }
    }
    // Handles registerComponent()
    if (callee.type === 'Identifier' && callee.name === 'registerComponent') {
      return callee
    }
  }
  return null
}

const extractObjectExpression = (node: CallExpression): ObjectExpression => {
  if (node.arguments.length !== 1) {
    throw new ParseError('registerComponent takes exactly 1 argument', node)
  }
  if (node.arguments[0].type === 'ObjectExpression') {
    return node.arguments[0]
  } else {
    throw new ParseError('registerComponent takes an object argument', node)
  }
}

type RegisterComponentNode = {
  callee: Expression
  arguments: ObjectExpression
}

// NOTE(johnny): This function fetches all the registerComponent ecs calls from a node and
// returns the arguments of the call or [] otherwise.
const maybeResolveDeclaration = (node: Expression | Statement): RegisterComponentNode[] => {
  if (node.type === 'VariableDeclaration') {
    const componentArguments: RegisterComponentNode[] = []
    node.declarations.forEach((declaration) => {
      if (declaration.type === 'VariableDeclarator') {
        const init = declaration?.init
        if (init?.type === 'CallExpression') {
          const callee = getRegisterComponentNode(init)
          if (callee) {
            componentArguments.push({callee, arguments: extractObjectExpression(init)})
          }
        }
      }
    })
    return componentArguments
  } else if (node.type === 'ExpressionStatement') {
    const {expression} = node
    if (expression.type === 'CallExpression') {
      const callee = getRegisterComponentNode(expression)
      if (callee) {
        return [{callee, arguments: extractObjectExpression(expression)}]
      }
    }
  } else if (node.type === 'ExportNamedDeclaration') {
    // NOTE(johnny): Handle export const someVar = ecs.registerComponent(...)
    if (node.declaration && node.declaration.type === 'VariableDeclaration') {
      return maybeResolveDeclaration(node.declaration)
    }
  }
  return []
}

const fixSchemaType = (type: string, node: Expression | PatternLike): Type => {
  switch (type) {
    case 'eid':
    case 'f32':
    case 'f64':
    case 'i32':
    case 'ui8':
    case 'ui32':
    case 'string':
    case 'boolean':
      return type
    default:
      throw new ParseError(`unknown type: ${type}`, node)
  }
}
const extractSchemaType = (outerNode: Expression | PatternLike) => {
  let node: Node = outerNode
  if (node.type === 'ArrayExpression') {
    if (!node.elements[0]) {
      throw new ParseError('expected at least one one element in array', node)
    }
    // eslint-disable-next-line prefer-destructuring
    node = node.elements[0]
  }
  if (node.type === 'StringLiteral') {
    return node.value
  }
  if (node.type === 'MemberExpression') {
    if (node.object.type !== 'Identifier' || node.object.name !== 'ecs') {
      throw new ParseError('expected to use type from ecs', node)
    }
    if (node.property.type !== 'Identifier') {
      throw new ParseError('expected Identifier', node)
    }
    return node.property.name
  }
  if (node.type === 'Identifier') {
    return node.name
  }
  throw new ParseError('expected StringLiteral, MemberExpression or Identifier', node)
}

const extractSchemaDefaultValueType = (node: Expression | PatternLike): SchemaDefaultType => {
  if (node.type === 'UnaryExpression' && node.operator === '-') {
    const innerValue = extractSchemaDefaultValueType(node.argument)
    if (typeof innerValue !== 'number') {
      throw new ParseError(
        `Only negative numbers are valid. Got a negative: ${node.type}`, node
      )
    }
    return -innerValue
  }

  switch (node.type) {
    case 'NumericLiteral':
    case 'BooleanLiteral':
    case 'StringLiteral':
      return node.value
    default:
      throw new ParseError(
        `expected NumericLiteral, StringLiteral, or BooleanLiteral. Got: ${node.type}`, node
      )
  }
}

const extractInlineDefault = (outerNode: Expression | PatternLike) => {
  if (outerNode.type !== 'ArrayExpression') {
    return undefined
  }
  if (outerNode.elements.length < 2) {
    return undefined
  }

  const node = outerNode.elements[1]

  if (!node) {
    return undefined
  }

  if (node.type === 'SpreadElement') {
    throw new ParseError('expected a default value, not a spread element', node)
  }

  return extractSchemaDefaultValueType(node)
}

type CollectionContext<CollectionPresentation extends Presentation> = {
  name: string
  presentation: CollectionPresentation
  comment: Comment
  numFields: number
}

type ParsingContext = {
  schema: Record<string, Type>
  res: SchemaPresentation
  warnings: StudioComponentError[]
  currentGroup: CollectionContext<GroupPresentation> | null
  currentSection: CollectionContext<SectionPresentation> | null
}

const warnModeAlreadySet = (
  identifier: string,
  comment: Comment,
  directiveType: string,
  newMode: FieldPresentationMode,
  ctx: ParsingContext
) => {
  if (newMode === ctx.res.fields[identifier].mode) {
    ctx.warnings.push(createComponentError(
      `expected only one ${directiveType} for ${identifier}`, 'warning', comment
    ))
  } else {
    ctx.warnings.push(createComponentError(`cannot set ${identifier} to ${directiveType
    } (already set to ${ctx.res.fields[identifier].mode})`, 'warning', comment))
  }
}

const startCollection = (
  name: string, collectionType: 'group'|'section', current: CollectionContext<Presentation>|null,
  collections: Record<string, Presentation>, comment: Comment, ctx: ParsingContext
): CollectionContext<Presentation> | null => {
  // Check that a collection isn't already in progress
  if (current) {
    ctx.warnings.push(createComponentError(`Cannot start @${collectionType} ${name
    }. ${collectionType} ${current.name} already in progress`, 'warning', comment))
    return null
  }

  if (collections[name]) {
    ctx.warnings.push(createComponentError(
      `Duplicate @${collectionType} of name ${name}`, 'warning', comment
    ))
    return null
  }

  if (ctx.schema[name]) {
    ctx.warnings.push(createComponentError(
      `Cannot start @${collectionType} '${name}'. Field of name '${name}' already exists`,
      'warning', comment
    ))
    return null
  }

  const presentation = {}
  collections[name] = presentation

  return {name, presentation, comment, numFields: 0}
}

const startGroup = (directiveContent: string, comment: Comment, ctx: ParsingContext) => {
  // Get the group name
  const [name, groupType] = directiveContent.split(':', 2).map(s => s.trim())

  const newGroup = startCollection(name, 'group', ctx.currentGroup, ctx.res.groups, comment, ctx)
  if (!newGroup) {
    return
  }

  ctx.currentGroup = newGroup

  // Get the group type, if supplied
  if (groupType) {
    if (!GROUP_TYPES.includes(groupType as GroupType)) {
      ctx.warnings.push(createComponentError(
        `expected one of ${GROUP_TYPES.join(', ')} for @group type`, 'warning', comment
      ))
    } else {
      ctx.currentGroup.presentation.type = groupType as GroupType
    }
  }
}

const endGroup = (comment: Comment, ctx: ParsingContext) => {
  if (!ctx.currentGroup) {
    ctx.warnings.push(createComponentError(
      'Cannot end @group. No @group in progress', 'warning', comment
    ))
    return
  }

  const numExpectedFields = ({
    'vector3': 3,
    // TODO add RGBA (4 fields) when supported
    'color': 3,  // RGB expected in order
    'default': undefined,
  })[ctx.currentGroup.presentation.type || 'default']

  if (numExpectedFields && ctx.currentGroup.numFields !== numExpectedFields) {
    ctx.warnings.push(createComponentError(
      `expected ${numExpectedFields} fields for group '${ctx.currentGroup.name}' of type ${
        ctx.currentGroup.presentation.type} (received ${ctx.currentGroup.numFields})`, 'warning',
      ctx.currentGroup.comment
    ))
  }

  ctx.currentGroup = null
}

const extractDirectiveData = (directive: string) => {
  const spaceIndex = directive.indexOf(' ')
  const directiveType = spaceIndex === -1 ? directive : directive.slice(0, spaceIndex)
  const directiveContent = spaceIndex === -1 ? '' : directive.slice(spaceIndex + 1)
  return [directiveType, directiveContent]
}

const setCollectionLabel = (
  collection: CollectionContext<Presentation> | null, collectionType: string, label: string,
  comment: Comment, ctx: ParsingContext
) => {
  if (!collection) {
    ctx.warnings.push(createComponentError(
      `Cannot set label for ${collectionType} as no ${collectionType} in progress`,
      'warning', comment
    ))
  } else if (collection.presentation.label) {
    ctx.warnings.push(createComponentError(
      `expected only one label for ${collectionType} ${collection.name}`, 'warning', comment
    ))
  } else {
    collection.presentation.label = label
  }
}

const setCollectionCondition = (
  collection: CollectionContext<Presentation> | null, collectionType: string, condition: string,
  comment: Comment, ctx: ParsingContext
) => {
  if (!collection) {
    ctx.warnings.push(createComponentError(
      `Cannot set condition for ${collectionType} as no ${collectionType} in progress`,
      'warning', comment
    ))
  } else if (collection.presentation.condition) {
    ctx.warnings.push(createComponentError(
      `expected only one condition for ${collectionType} ${collection.name}`, 'warning', comment
    ))
  } else {
    collection.presentation.condition = condition
  }
}

const extractGroupPresentationElement = (
  groupContent: string, comment: Comment, ctx: ParsingContext
) => {
  const [type, content] = extractDirectiveData(groupContent)
  switch (type) {
    default:
      ctx.warnings.push(createComponentError(
        `Unknown group presentation element '${type}'`, 'warning', comment
      ))
      break
    case 'start': {
      startGroup(content, comment, ctx)
      break
    }
    case 'label': {
      setCollectionLabel(ctx.currentGroup, 'group', content, comment, ctx)
      break
    }
    case 'condition': {
      setCollectionCondition(ctx.currentGroup, 'group', content, comment, ctx)
      break
    }
    case 'end': {
      endGroup(comment, ctx)
      break
    }
  }
}

const startSection = (directiveContent: string, comment: Comment, ctx: ParsingContext) => {
  // Get the section name
  const [name, property] = directiveContent.split(':', 2).map(s => s.trim())

  if (ctx.currentGroup) {
    ctx.warnings.push(
      createComponentError(
        `Cannot create section ${name} inside group ${ctx.currentGroup.name}`,
        'warning', comment
      )
    )
    return
  }

  const newSection = startCollection(
    name, 'section', ctx.currentSection, ctx.res.sections, comment, ctx
  )
  if (!newSection) {
    return
  }

  ctx.currentSection = newSection

  // Get the group type, if supplied
  if (property) {
    if (property === 'defaultClosed') {
      ctx.currentSection.presentation.defaultClosed = true
    } else {
      ctx.warnings.push(createComponentError(
        `Unknown section property ${property}`, 'warning', comment
      ))
    }
  }
}

const setSectionDefaultClosed = (comment: Comment, ctx: ParsingContext) => {
  if (!ctx.currentSection) {
    ctx.warnings.push(createComponentError(
      'Cannot set defaultClosed for section as no section in progress',
      'warning', comment
    ))
  } else {
    ctx.currentSection.presentation.defaultClosed = true
  }
}

const endSection = (comment: Comment, ctx: ParsingContext) => {
  if (!ctx.currentSection) {
    ctx.warnings.push(createComponentError(
      'Cannot end @section. No @section in progress', 'warning', comment
    ))
  } else if (ctx.currentGroup) {
    ctx.warnings.push(createComponentError(
      `Cannot end @section ${ctx.currentSection.name} inside group ${
        ctx.currentGroup.name}`, 'warning', comment
    ))
  } else {
    ctx.currentSection = null
  }
}

const extraSectionPresentationElement = (
  sectionContent: string, comment: Comment, ctx: ParsingContext
) => {
  const [type, content] = extractDirectiveData(sectionContent)
  switch (type) {
    default:
      ctx.warnings.push(createComponentError(
        `Unknown section presentation element '${type}'`, 'warning', comment
      ))
      break
    case 'start': {
      startSection(content, comment, ctx)
      break
    }
    case 'defaultClosed':
      setSectionDefaultClosed(comment, ctx)
      break
    case 'label': {
      setCollectionLabel(ctx.currentSection, 'section', content, comment, ctx)
      break
    }
    case 'condition': {
      setCollectionCondition(ctx.currentSection, 'section', content, comment, ctx)
      break
    }
    case 'end': {
      endSection(comment, ctx)
      break
    }
  }
}

const makeOrGetFieldPresentation = (identifier: string, ctx: ParsingContext): FieldPresentation => {
  if (!ctx.res.fields[identifier]) {
    ctx.res.fields[identifier] = {}
  }

  return ctx.res.fields[identifier]
}

// Extract the given presentation element (if present as a comment) and add it to the result
const extractPresentationElement = (identifier: string, comment: Comment, ctx: ParsingContext) => {
  const [directiveType, directiveContent] = extractDirectiveData(comment.value.trim())

  switch (directiveType) {
    case '@label': {
      const presentation = makeOrGetFieldPresentation(identifier, ctx)
      if (presentation.label) {
        ctx.warnings.push(createComponentError(
          `expected only one @label for ${identifier}`, 'warning', comment
        ))
      } else {
        presentation.label = directiveContent
      }
      break
    }
    case '@enum': {
      const presentation = makeOrGetFieldPresentation(identifier, ctx)
      if (presentation.mode) {
        warnModeAlreadySet(identifier, comment, directiveType, 'enum', ctx)
      } else if (directiveContent.includes('"') || directiveContent.includes('\'')) {
        ctx.warnings.push(
          createComponentError(
            'enum values should not contain quotes (TBD on parsing)', 'warning', comment
          )
        )
      } else {
        presentation.mode = 'enum'
        presentation.enumValues = directiveContent.split(',').map(v => v.trim())
      }
      break
    }
    case '@color': {
      const presentation = makeOrGetFieldPresentation(identifier, ctx)
      if (ctx.schema[identifier] !== 'string') {
        ctx.warnings.push(createComponentError(
          `@color is only valid for string fields, cannot apply to ${ctx.schema[identifier]}`,
          'warning', comment
        ))
      } else if (presentation.mode) {
        warnModeAlreadySet(identifier, comment, directiveType, 'color', ctx)
      } else if (directiveContent) {
        ctx.warnings.push(createComponentError(
          'There should be no content after @color', 'warning', comment
        ))
      } else {
        presentation.mode = 'color'
      }
      break
    }
    case '@required': {
      if (ctx.schema[identifier] === 'eid') {
        const presentation = makeOrGetFieldPresentation(identifier, ctx)
        presentation.required = true
      } else {
        ctx.warnings.push(
          createComponentError(
            `@required is only valid for eid fields, cannot apply to ${ctx.schema[identifier]}`,
            'warning', comment
          )
        )
      }
      break
    }
    case '@condition': {
      const presentation = makeOrGetFieldPresentation(identifier, ctx)
      if (presentation.condition) {
        ctx.warnings.push(
          createComponentError(
            `expected only one @condition for ${identifier}`, 'warning', comment
          )
        )
      } else if (ctx.currentGroup) {
        ctx.warnings.push(
          createComponentError(
            `Cannot set condition for field ${
              identifier} as it is part of group ${ctx.currentGroup.name}`,
            'warning', comment
          )
        )
      } else {
        presentation.condition = directiveContent
      }
      break
    }
    case '@attribute': {
      const presentation = makeOrGetFieldPresentation(identifier, ctx)
      if (presentation.mode) {
        warnModeAlreadySet(identifier, comment, directiveType, 'attribute', ctx)
      } else if (!ALL_ATTRIBUTE_TYPES.includes(directiveContent as AttributeTypeName)) {
        ctx.warnings.push(createComponentError(
          `expected one of ${ALL_ATTRIBUTE_TYPES.join(', ')} for @attribute ${identifier}`,
          'warning', comment
        ))
      } else {
        presentation.mode = 'attribute'
        presentation.attribute = directiveContent as AttributeTypeName
      }
      break
    }
    case '@property-of': {
      const presentation = makeOrGetFieldPresentation(identifier, ctx)
      if (presentation.mode) {
        warnModeAlreadySet(identifier, comment, directiveType, 'property', ctx)
      } else {
        presentation.mode = 'property'
        presentation.propertyOf = directiveContent
      }
      break
    }
    case '@group':
      extractGroupPresentationElement(directiveContent, comment, ctx)
      break
    case '@section':
      extraSectionPresentationElement(directiveContent, comment, ctx)
      break
    case '@asset': {
      const presentation = makeOrGetFieldPresentation(identifier, ctx)
      if (presentation.mode) {
        warnModeAlreadySet(identifier, comment, directiveType, 'asset', ctx)
      } else if (directiveContent && !ASSET_KINDS.includes(directiveContent as AssetKind)) {
        ctx.warnings.push(createComponentError(
          `Unknown asset kind ${directiveContent} for field ${identifier}`, 'warning', comment
        ))
      } else {
        presentation.mode = 'asset'
        presentation.assetKind =
          directiveContent ? directiveContent as AssetKind : undefined
      }
      break
    }
    case '@min': {
      const presentation = makeOrGetFieldPresentation(identifier, ctx)
      if (!(/^-?\d+(e-?\d+)?(\.\d+)?$/.test(directiveContent))) {
        ctx.warnings.push(createComponentError(
          `expected a number type for property '${identifier}'`, 'warning', comment
        ))
      } else if (presentation.min !== undefined) {
        ctx.warnings.push(createComponentError(
          `expected only one @min for ${identifier}`, 'warning', comment
        ))
      } else {
        presentation.min = Number(directiveContent)
      }
      break
    }
    case '@max': {
      const presentation = makeOrGetFieldPresentation(identifier, ctx)
      if (!(/^-?\d+(e-?\d+)?(\.\d+)?$/.test(directiveContent))) {
        ctx.warnings.push(createComponentError(
          `expected a number type for property '${identifier}'`, 'warning', comment
        ))
      } else if (presentation.max !== undefined) {
        ctx.warnings.push(createComponentError(
          `expected only one @max for ${identifier}`, 'warning', comment
        ))
      } else {
        presentation.max = Number(directiveContent)
      }
      break
    }
    default:
  }
}

const extractTrailingPresentationElement = (comment: Comment, ctx: ParsingContext) => {
  const [directiveType, directiveContent] = extractDirectiveData(comment.value.trim())
  switch (directiveType) {
    case '@group':
      extractGroupPresentationElement(directiveContent, comment, ctx)
      break
    case '@section':
      extraSectionPresentationElement(directiveContent, comment, ctx)
      break
    default:
  }
}

const confidenceCheckPresentation = (schema: Record<string, Type>, ctx: ParsingContext) => {
  Object.entries(ctx.res.fields).forEach(([fieldName, fieldPresentation]) => {
    if (fieldPresentation.propertyOf && !schema[fieldPresentation.propertyOf]) {
      ctx.warnings.push(createComponentError(
        `@property-of ${fieldPresentation.propertyOf} for ${fieldName
        } should be a valid field`, 'warning', null
      ))
    }
  })
}

const addToCurrentSection = (identifier: string, ctx: ParsingContext) => {
  if (!ctx.currentSection) {
    return
  }

  makeOrGetFieldPresentation(identifier, ctx).section = ctx.currentSection.name
  ++ctx.currentSection.numFields
}

const addToCurrentGroup = (identifier: string, ctx: ParsingContext, fieldType: Type) => {
  if (!ctx.currentGroup) {
    return
  }

  // Make sure that a field presentation exists for the identifier before adding it to the group
  // (it won't exist if there are no other presentation elements for the field)
  makeOrGetFieldPresentation(identifier, ctx).group = ctx.currentGroup.name
  ++ctx.currentGroup.numFields

  // Confidence check that fields are of the correct type for the group
  switch (ctx.currentGroup.presentation?.type) {
    case 'vector3':
    case 'color':
      switch (fieldType) {
        case 'f32':
        case 'i32':
        case 'ui8':
        case 'ui32':
          break
        default:
          ctx.warnings.push(createComponentError(
            `expected a number type for property '${identifier}' in ${
              ctx.currentGroup.presentation.type
            } group '${ctx.currentGroup.name}' (received ${fieldType})`, 'warning', null
          ))
      }
      break
    default:
  }
}

const parseSchema = (node: Expression | PatternLike) => {
  const schema: Record<string, Type> = {}
  const presentation: SchemaPresentation = {
    fields: {},
    groups: {},
    sections: {},
  }
  const warnings: StudioComponentError[] = []
  const ctx: ParsingContext = {
    schema,
    res: presentation,
    warnings,
    currentGroup: null,
    currentSection: null,
  }

  const inlineDefaults: Record<string, SchemaDefaultType> = {}

  if (node.type !== 'ObjectExpression') {
    throw new ParseError('expected ObjectExpression for schema', node)
  }

  // First pass to extract all the schema types
  node.properties.forEach((prop) => {
    if (prop.type !== 'ObjectProperty') {
      throw new ParseError('expected ObjectProperty', prop)
    }
    if (prop.key.type !== 'Identifier') {
      throw new ParseError('expected Identifier', prop)
    }
    if (schema[prop.key.name]) {
      throw new ParseError(`Duplicate field '${prop.key.name}'`, prop)
    }

    schema[prop.key.name] = fixSchemaType(extractSchemaType(prop.value), prop.value)
    const inlineDefault = extractInlineDefault(prop.value)
    if (inlineDefault !== undefined) {
      inlineDefaults[prop.key.name] = inlineDefault
    }
  })

  // Second pass to extract all the presentation elements
  node.properties.forEach((prop) => {
    if (prop.type !== 'ObjectProperty') {
      throw new ParseError('expected ObjectProperty', prop)
    }
    if (prop.key.type !== 'Identifier') {
      throw new ParseError('expected Identifier', prop)
    }

    const identifier = prop.key.name
    if (prop.leadingComments) {
      // Parse the schema looking for comments that contain presentation elements
      prop.leadingComments.forEach((comment) => {
        extractPresentationElement(identifier, comment, ctx)
      })
    }

    addToCurrentGroup(identifier, ctx, schema[identifier])
    addToCurrentSection(identifier, ctx)

    // Trailing comments should be parsed for presentation elements after everything else
    // is done to ensure that the presentation elements are applied to the correct fields
    if (prop.trailingComments) {
      prop.trailingComments.forEach((comment) => {
        extractTrailingPresentationElement(comment, ctx)
      })
    }
  })

  // There should be no current group at the end of the schema
  if (ctx.currentGroup) {
    warnings.push(createComponentError(
      `Expected '@group end' for group ${ctx.currentGroup.name}`,
      'warning', ctx.currentGroup.comment
    ))
    ctx.currentGroup = null
  }

  // There should be no current section at the end of the schema
  if (ctx.currentSection) {
    warnings.push(createComponentError(
      `Expected '@section end' for section ${ctx.currentSection.name}`,
      'warning', ctx.currentSection.comment
    ))
    ctx.currentSection = null
  }

  confidenceCheckPresentation(schema, ctx)

  return {
    schema,
    inlineDefaults: Object.keys(inlineDefaults).length ? inlineDefaults : undefined,
    presentation,
    warnings,
  }
}

const parseSchemaDefaults = (node: Expression | PatternLike): SchemaDefaults => {
  const res: Record<string, SchemaDefaultType> = {}
  if (node.type !== 'ObjectExpression') {
    throw new ParseError('expected ObjectExpression', node)
  }
  node.properties.forEach((prop) => {
    if (prop.type !== 'ObjectProperty') {
      throw new ParseError('expected ObjectProperty', prop)
    }
    if (prop.key.type !== 'Identifier') {
      throw new ParseError('expected Identifier', prop)
    }
    res[prop.key.name] = extractSchemaDefaultValueType(prop.value)
  })
  return res
}

const parseComponentInfo = (
  registerComponentNode: RegisterComponentNode
): {data: StudioComponentData, errors: StudioComponentError[]} => {
  const data: Undefinable<StudioComponentData> = {
    name: undefined,
    schema: undefined,
    schemaDefaults: undefined,
    location: undefined,
  }
  const componentErrors: StudioComponentError[] = []
  let inlineDefaults: Record<string, SchemaDefaultType> | undefined

  registerComponentNode.arguments.properties.forEach((property) => {
    if (property.type !== 'ObjectProperty') {
      throw new ParseError('expected only ObjectProperties', property)
    } else if (property.key.type !== 'Identifier') {
      throw new ParseError('expected only Identifier keys', property)
    }
    switch (property.key.name) {
      case 'name': {
        if (data.name) {
          throw new ParseError('expected only one name property', property)
        }
        if (property.value.type !== 'StringLiteral') {
          throw new ParseError('expected string for name value', property.value)
        }
        data.name = property.value.value
        break
      }
      case 'schema': {
        if (data.schema) {
          throw new ParseError('expected only one schema property')
        }

        const parsed = parseSchema(property.value)
        inlineDefaults = parsed.inlineDefaults
        data.schema = parsed.schema
        data.schemaPresentation = parsed.presentation
        componentErrors.push(...parsed.warnings)
        break
      }
      case 'schemaDefaults': {
        if (data.schemaDefaults) {
          throw new ParseError('expected only one schemaDefaults property')
        }
        data.schemaDefaults = parseSchemaDefaults(property.value)
        break
      }
      default: {
        if (!REGISTER_COMPONENT_PARAMS.includes(property.key.name)) {
          throw new ParseError('unexpected property', property.key)
        }
        break
      }
    }
  })

  if (!data.name) {
    throw new ParseError('expected name property', registerComponentNode.callee)
  }

  if (!data.schema) {
    data.schema = {}
  }

  if (registerComponentNode.callee.loc) {
    data.location = nodeToLocation(registerComponentNode.callee.loc)
  }

  if (inlineDefaults) {
    if (data.schemaDefaults) {
      componentErrors.push(createComponentError(
        'Cannot use schemaDefaults and defaults within schema at the same time',
        'error', null
      ))
    } else {
      data.schemaDefaults = inlineDefaults
    }
  }

  return {data: data as StudioComponentData, errors: componentErrors}
}

const parseComponentAst = (fileContent: string): ParsedComponents => {
  try {
    const parsedFile = parse(fileContent, {
      sourceType: 'module',
      plugins: [
        'typescript',
        'classProperties',
      ],
    })
    const errorsInFile: StudioComponentError[] = []
    const metadata = parsedFile.program.body.map((node) => {
      const registerComponentNodes = maybeResolveDeclaration(node).flat().filter(Boolean)
      return registerComponentNodes.map(
        (registerComponentNode) => {
          const {data, errors} = parseComponentInfo(registerComponentNode)
          errorsInFile.push(...errors)
          return data
        }
      )
    }).flat()
    return {componentData: metadata, errors: errorsInFile}
  } catch (e) {
    if (e instanceof ParseError) {
      return {
        componentData: [],
        errors: [{message: e.message, severity: 'error', location: e.location}],
      }
    }
    return {componentData: [], errors: []}
  }
}

export type {
  ParseError,
}

export {
  parseComponentAst,
}
