// https://github.com/bazelbuild/buildtools/blob/main/build/rewrite.go#L737
const getPriority = (name: string): number => {
  if (name.startsWith(':')) {
    return 1
  }
  if (name.startsWith('//')) {
    return 2
  }
  if (name.startsWith('@')) {
    return 3
  }
  return 0
}

const normalize = (name: string): string => name
  .replace(/^"|"$/g, '')  // Strip any quotes if present

// https://github.com/bazelbuild/buildtools/blob/main/build/rewrite.go#L763-L782
const compareStringListElements = (leftQuoted: string, rightQuoted: string): number => {
  const left = normalize(leftQuoted)
  const right = normalize(rightQuoted)

  const leftPriority = getPriority(left)
  const rightPriority = getPriority(right)

  if (leftPriority !== rightPriority) {
    return leftPriority - rightPriority
  }

  const leftParts = left.split(/:|\./)
  const rightParts = right.split(/:|\./)

  for (let i = 0; i < Math.min(leftParts.length, rightParts.length); i++) {
    if (leftParts[i] !== rightParts[i]) {
      return leftParts[i] < rightParts[i] ? -1 : 1
    }
  }

  return leftParts.length - rightParts.length
}

const sortStringList = (list: string[]): string[] => [...list].sort(compareStringListElements)

export {
  compareStringListElements,
  sortStringList,
}
