// @rule(js_binary)

/* eslint-disable no-console */

class MyClass {
  constructor() {
    // eslint-disable-next-line no-console
    console.log('MyClass')
  }
}

console.log(MyClass)

// NOTE(christoph): By referencing the class twice, uglifyjs will not inline
// it as an anonymous class.
console.log(MyClass)
