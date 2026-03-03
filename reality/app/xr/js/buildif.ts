// Copyright (c) 2023 Niantic, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

// @rule(js_binary)
// @attr(commonjs = 1)
// @attr(export_library = 1)

const isLocal = process.env.C8VERSION !== 'true'

const When = {
  // never run this code. When set to this, determine if you should just remove the code.
  Never: false,
  // only available on locally built engine
  isLocal,
  // always run this code. Used for launch, remove this flag after a while
  Always: true,
}

const BuildIf = Object.freeze({
  EARS_20230911: When.Always,
  LOCAL: When.isLocal,
})

const replacements = Object.entries(BuildIf).map(([label, when]) => ({
  search: `BuildIf.${label}`,
  replace: `${!!when}`,
  flags: 'g',
}))

type BuildIfReplacements = typeof BuildIf

export {
  replacements,
  BuildIfReplacements,
}
