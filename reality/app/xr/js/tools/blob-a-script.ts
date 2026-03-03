// @rule(js_cli)

import fs from 'fs'

const run = (filePaths: string[]) => {
  if (filePaths.length < 3) {
    console.error('Need 3 files in argv. Input targetFile output')
    console.error('Instead getting', filePaths)
    process.exit(1)
  }
  const outputFilePath = filePaths[filePaths.length - 1]
  const targetFilePattern = filePaths[filePaths.length - 2]
  console.log('Will write to output file', outputFilePath)
  let fileWritten = false
  for (let i = 0; i < filePaths.length - 2; i++) {
    const inputFilePath = filePaths[i]
    if (!inputFilePath.endsWith(targetFilePattern)) {
      continue
    }

    console.log('Processing input file', inputFilePath)
    const fileContent = fs.readFileSync(inputFilePath)
    const escapedFileContent = fileContent.toString()
      .replace(/`/g, '\\`')
      .replace(/\\n/g, '\\\\n')

    const contentToWrite = `
const scriptContent = \`
${escapedFileContent}
\`

const objectUrl = URL.createObjectURL(new Blob([scriptContent], {type: 'application/javascript'}))
export {
  objectUrl
}
    `
    fs.writeFileSync(outputFilePath, contentToWrite)
    fileWritten = true

    break
  }

  if (!fileWritten) {
    // write a shim file
    fs.writeFileSync(outputFilePath, 'const objectUrl = \'\'; export {objectUrl}')
  }
}

run(process.argv.slice(2))
