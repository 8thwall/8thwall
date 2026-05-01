// Sizes here are based on how Chrome truncates printing for these objects.
const MAX_SET_OR_MAP_SIZE = 5
const MAX_ARRAY_SIZE = 100

type PrintOptions = {
  alone?: boolean  // if the value is being printed alone
  top?: boolean    // if the value is being printed at the top level
  hide?: boolean   // if the value being printed is hiding its properties
}

// Get a string representation of a value
const getPrintable = (value: unknown, options: PrintOptions = {}): string => {
  if (value === null) {
    return 'null'
  }

  if (value === undefined) {
    return 'undefined'
  }

  // value is not recognized as a string unless checked directly
  if (typeof value === 'string') {
    return options.top ? value : `"${value}"`
  }

  const type = typeof value

  if (type === 'number' || type === 'boolean') {
    return value.toString()
  }

  if (type === 'bigint') {
    return `${value.toString()}n`
  }

  if (type === 'function') {
    return `[Function: ${value.toString()}]`
  }

  if (value instanceof Array) {
    if (options.hide) {
      return `${value.constructor.name}(${value.length})`
    }
    const truncated = value.slice(0, MAX_ARRAY_SIZE)
    const printableStart = value.length > 1 ? `(${value.length}) [` : '['
    const printable = `${truncated.map(e => getPrintable(e, {hide: true})).join(', ')}`
    const printableEnd = value.length > MAX_ARRAY_SIZE ? ', ...]' : ']'
    return printableStart + printable + printableEnd
  }

  if (value instanceof Error) {
    return value.toString()
  }

  if (value instanceof Set || value instanceof Map) {
    if (options.hide) {
      return `${value.constructor.name}(${value.size})`
    }
    const truncated = [...value].slice(0, MAX_SET_OR_MAP_SIZE)
    const printable = truncated.map(e => (
      value instanceof Set
        ? getPrintable(e, {hide: true})
        : `${getPrintable(e[0], {hide: true})} => ${getPrintable(e[1], {hide: true})}`)).join(', ')
    const printableEnd = value.size > MAX_SET_OR_MAP_SIZE ? ', ...}' : '}'
    return `${value.constructor.name}(${value.size}) {${printable}${printableEnd}`
  }

  if (value instanceof Object || type === 'object') {
    if (options.hide) {
      return value.constructor === Object ? '{...}' : value.constructor.name
    }

    const keys = Object.keys(value)
    if (!keys.length) {
      // Emtpy Javascript object.
      if (value.constructor === Object) {
        return '{}'
      }
      // Object type we are not currently handling.
      return String(value)
    }

    const keyStrings = keys.map(key => (
      `${key}: ${getPrintable(value[key], {hide: true})}`
    ))

    return `{${keyStrings.join(', ')}}`
  }

  return '[Unknown]'
}

const getPrintableArgs = (args: unknown[]) => {
  if (args.length === 0) {
    return ''
  } else if (args.length === 1) {
    return getPrintable(args[0], {alone: true, top: true})
  }
  return args.map(v => getPrintable(v, {top: true})).join(' ')
}

export {
  getPrintableArgs,
}
