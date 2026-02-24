// @visibility(//visibility:public)

const CPP_TYPE_TO_TS = {
  'void': 'void',
  'bool': 'boolean',
  'AppLogRecordHeader::AccessMethod': 'number',
  'double': 'number',
  'uint32_t': 'number',
  'uint8_t': 'number',
  'float': 'number',
  'int': 'number',
  'GLuint': 'number',
  'size_t': 'number',
  'ecs_entity_t': 'bigint',
}

const messages: string[] = []

const cppTypeToTs = (rawType: string) => {
  const type = rawType.trim()
  if (type.endsWith('*')) {
    return 'number'  // Pointers are indices into module.HEAPU8
  }
  return CPP_TYPE_TO_TS[type] || (messages.push(`Unknown type: ${type}`), 'never')
}

// Maps object name to a map of field name to type
type WindowObjects = Record<string, Record<string, string>>

type Declaration = {
  returnType: string
  functionName: string
  parameters: string[]
}

const findDeclarations = (contents: string, declarations: Declaration[]) => {
  const PUBLIC_REGEX = /\bC8_PUBLIC\n([^\n]*?)\b(c8EmAsm_[^(]+)\(([^(]*)\) \{/gm

  let match = PUBLIC_REGEX.exec(contents)

  while (match) {
    const [, returnType, functionName, parameters] = match

    const parameterArray = parameters.length ? parameters.split(',').map(e => e.trim()) : []

    declarations.push({
      returnType,
      functionName,
      parameters: parameterArray,
    })

    match = PUBLIC_REGEX.exec(contents)
  }
}

const findWindowObjects = (contents: string, windowObjects: WindowObjects) => {
  const ASSIGNMENT_REGEX = /window\.(_c8\S*)\.(\S+)\s*=\s*(\S+)/g

  let match = ASSIGNMENT_REGEX.exec(contents)

  while (match) {
    const [, objectName, fieldName, rhs] = match

    let fieldType = 'number'
    if (rhs.startsWith('UTF8ToString')) {
      fieldType = 'string'
    } else if (rhs.startsWith('GL.textures')) {
      fieldType = 'EmscriptenTexture'
    }
    const existingType = windowObjects[objectName]?.[fieldName]
    if (existingType && existingType !== fieldType) {
      throw new Error(`Mismatched types for ${objectName}.${fieldName}`)
    }

    windowObjects[objectName] = windowObjects[objectName] || {}
    windowObjects[objectName][fieldName] = fieldType

    match = ASSIGNMENT_REGEX.exec(contents)
  }
}

const parameterToTs = (rawParameter: string) => {
  let parameter = rawParameter.trim()
  let isOptional = false
  if (parameter.includes('=')) {
    isOptional = true
    parameter = parameter.split('=')[0].trim()
  }
  const [, type, name] = parameter.match(/^(.*)\b(\w+)$/)
  return `${name}${isOptional ? '?' : ''}: ${cppTypeToTs(type)}`
}

export {
  messages,
  WindowObjects,
  Declaration,
  cppTypeToTs,
  findDeclarations,
  findWindowObjects,
  parameterToTs,
}
