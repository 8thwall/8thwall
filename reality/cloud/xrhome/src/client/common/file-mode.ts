export const MODE_DIR_777 = 16895   // Octal  40777: directory with permissions 777
export const MODE_FILE_666 = 33206  // Octal 100666: file with permissions 666

export const isFileMode = (mode: number) => {
  const oct = mode.toString(8)
  return oct.length == 6 && oct.startsWith('100')
}

export const isDirMode = (mode: number) => {
  const oct = mode.toString(8)
  return oct.length == 5 && oct.startsWith('40')
}
