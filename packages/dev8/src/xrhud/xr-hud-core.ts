const oom = (val: number): string => {
  let nv = val
  if (nv < 1000) {
    return nv.toString()
  }
  nv /= 1000
  if (nv < 1000) {
    return `${nv.toFixed(2)}k`  // thousand
  }
  nv /= 1000
  if (nv < 1000) {
    return `${nv.toFixed(2)}m`  // million
  }
  nv /= 1000
  if (nv < 1000) {
    return `${nv.toFixed(2)}b`  // billion
  }
  nv /= 1000
  return `${nv.toFixed(2)}t`    // trillion
}

const dataSize = (val: number): string => {
  let nv = val
  if (nv < 1024) {
    return nv.toString()
  }
  nv /= 1024
  if (nv < 1024) {
    return `${nv.toFixed(2)}kb`  // kilobytes
  }
  nv /= 1024
  if (nv < 1024) {
    return `${nv.toFixed(2)}mb`  // megabytes
  }
  nv /= 1024
  if (nv < 1024) {
    return `${nv.toFixed(2)}gb`  // gigabytes
  }
  nv /= 1024
  if (nv < 1024) {
    return `${nv.toFixed(2)}tb`  // terabytes
  }
  nv /= 1024
  return `${nv.toFixed(2)}pb`    // petabytes
}

const removeDom = (node: HTMLElement) => {
  if (node.parentElement) {
    node.parentElement.removeChild(node)
  }
}

export {
  dataSize,
  oom,
  removeDom,
}
