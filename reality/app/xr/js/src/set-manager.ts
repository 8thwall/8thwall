// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
//
// Manages a set composed of multiple named lists
//
// Usage:
//   const mySet = new SetManager(['initial'])
//   console.log(mySet.get()) // Set(['initial'])
//   mySet.add('list1', ['a', 'b', 'c'])
//   mySet.add('list2', () => (['a', 'b', 'z'])
//   console.log(mySet.get()) // Set(['initial', 'a', 'b', 'c', 'z'])
//   mySet.remove('list1')
//   console.log(mySet.get()) // Set(['initial', 'a', 'b', z'])

// SetManager constructor takes an array argument for items which are always in the set.
function SetManager(initialItems_) {
  const namedListMap_ = {}  // Maps names to list of items
  let rawSet_  // Set of all items in all lists. Lazily updated.

  this.add = (name, valueProvider) => {
    if (!valueProvider) {
      return
    }

    const isArray = Array.isArray(valueProvider)
    const isFunction = typeof (valueProvider) === 'function'

    if (!isArray && !isFunction) {
      return
    }

    if (isArray && valueProvider.length === 0) {
      return
    }

    // Slice to duplicate array
    namedListMap_[name] = isArray ? valueProvider.slice(0) : valueProvider

    // Mark rawSet_ dirty
    rawSet_ = null
  }

  this.remove = (name) => {
    if (!namedListMap_[name]) {
      return
    }

    delete namedListMap_[name]

    // Mark rawSet_ dirty
    rawSet_ = null
  }

  this.get = () => {
    let dynamic = false
    if (!rawSet_) {
      rawSet_ = new Set(initialItems_)

      Object.values(namedListMap_).forEach((valueProvider) => {
        // Determine if the valueProvider is an array or a function that returns an array.
        let valueArray = valueProvider
        if (typeof (valueProvider) === 'function') {
          valueArray = valueProvider()
          dynamic = true
        }
        // Add values from the underlying array.
        if (Array.isArray(valueArray)) {
          valueArray.forEach(item => rawSet_.add(item))
        }
      })
    }

    // If some providers are dynamic, we can't cache the value.
    const setValue = rawSet_
    if (dynamic) {
      rawSet_ = null
    }
    return setValue
  }

  // Get a copy of the set so it can't be used to modified the original
  this.getCopy = () => new Set(this.get())
}

export {
  SetManager,
}
