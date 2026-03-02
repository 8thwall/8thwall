const isPartOf = (object, potentialParent) => {
  if (!object || !potentialParent) {
    return false
  }

  if (object === potentialParent) {
    return true
  }

  return isPartOf(object.parent, potentialParent)
}

export {
  isPartOf,
}
