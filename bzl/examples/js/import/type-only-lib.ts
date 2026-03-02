// @rule(js_binary)
// @attr(export_library = 1)

const DATA = 'unique-data-that-should-not-be-bundled'

type ExampleType = 'v1' | 'v2'

export {
  DATA,
  ExampleType,
}
