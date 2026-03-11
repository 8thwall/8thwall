// Get the process arguments as a js dictionary. Named arguments are keys to the dictionary.
// Unnamed arguments are available with their order preserved in '_ordered'.
//
// Example:
//
// auth8 --help --dataRealm=dev
//
// Returns:
// {
//  _node: '#!/usr/local/bin/node',
//  _script: '/usr/local/bin/auth8,
//  _ordered: [],
//  dataRealm: 'dev',
//  help: true,
// }

type CliArgs = Omit<Record<string, string|boolean>, '_ordered'> & {
  _node: string
  _script: string
  _ordered: string[]
}

const getArgs = () => {
  const args = process.argv
  // The first two arguments are the node instance and the script that's being run.
  const dict = {
    _node: args[0],
    _script: args[1],
    _ordered: [],
  } as unknown as CliArgs

  args.slice(2).forEach((arg) => {  // Consume remaining arguments starting at 2.
    if (!arg.startsWith('--')) {
      dict._ordered.push(arg)  // Argument doesn't have a name, add to ordered list.
      return
    }
    const eqsep = arg.substring(2).split('=')  // get arg without '--' and split on =
    if (eqsep.length === 1) {
      dict[eqsep[0]] = true  // There is no '= sign for a value; assign true.
      return
    }
    const val = eqsep.slice(1).join('=')  // Add = back to remaining components if needed.
    dict[eqsep[0]] = val === 'false' || val === '0' ? false : val
  })
  return dict
}

interface FlagSpec {
  optionalFlags?: string[]
  requiredFlags?: string[]
  optionsForFlag?: Record<string, (string | boolean)[]>
  minOrdered?: number
  maxOrdered?: number
  help?: string
}

const checkArgErrs = (spec: FlagSpec, args: CliArgs) => {
  const {optionalFlags, requiredFlags, optionsForFlag, minOrdered, maxOrdered, help} = spec
  const errs: string[] = []

  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  const {_node, _script, _ordered, ...providedArgs} = args

  if (optionalFlags || requiredFlags) {
    const okKeys = new Set([...optionalFlags || [], ...requiredFlags || []])

    if (help) {
      okKeys.add('help')
    }

    Object.keys(providedArgs).forEach((k) => {
      if (!okKeys.has(k)) {
        errs.push(`Unexpected command line option: ${k}`)
      }
    })
  }

  if (maxOrdered != null && _ordered.length > maxOrdered) {
    errs.push(`Too many command line options: ${_ordered.join(', ')}`)
  }

  if (minOrdered != null && _ordered.length < minOrdered) {
    errs.push(`Not enough command line options: ${_ordered.join(', ')}`)
  }

  if (optionsForFlag) {
    Object.entries(optionsForFlag).forEach(([flag, valid]) => {
      const v = providedArgs[flag]
      if (v !== undefined && !valid.includes(v)) {
        errs.push(`Unexpected ${flag}: ${v}`)
      }
    })
  }

  if (requiredFlags) {
    const missing = requiredFlags.filter(v => providedArgs[v] === undefined)
    if (missing.length) {
      errs.push(`Missing required arguments: ${missing.join(', ')}`)
    }
  }

  return errs
}

const checkArgs = (spec: FlagSpec): CliArgs => {
  const {help} = spec
  const args = getArgs()

  if (args.help && help) {
    console.log(help)  // eslint-disable-line no-console
    process.exit(0)
  }

  const errs = checkArgErrs(spec, args)
  if (errs.length || (help && args.help)) {
    errs.forEach(err => console.error(err))  // eslint-disable-line no-console
    if (help) {
      console.log(help)  // eslint-disable-line no-console
    }
    process.exit(1)
  }

  return args
}

export {
  getArgs,
  checkArgErrs,
  checkArgs,
}
