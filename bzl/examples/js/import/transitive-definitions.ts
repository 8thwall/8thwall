// @rule(js_binary)
// @attr(export_library = 1)
// @attr(full_dts = 1)
// @dep(//bzl/examples/js/import:transitive-definitions-dep)

const myData: string = 'hi'

declare global {
  interface MySharedInterface {
    sharedProperty1: string
  }
}

const myData2: globalThis.MySharedInterface | undefined = undefined

export * from './transitive-definitions-dep'
export {
  myData2,
  myData,
}
