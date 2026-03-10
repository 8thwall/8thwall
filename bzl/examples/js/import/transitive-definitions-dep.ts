const dataImportedFromDep: string = 'hi'

interface TypeImportedFromDep {
  exampleField: string
}

declare global {
  interface MySharedInterface {
    sharedProperty2: string
  }
}

export {
  dataImportedFromDep,
}

export type {
  TypeImportedFromDep,
}
