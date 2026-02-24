const fs = require('fs')
const path = require('path')

const [output, ...files] = process.argv.slice(2)

let finalConfig = ''
for (let fileName of files) {
  const fileContent = fs.readFileSync(fileName, 'utf8')
  const varName = path.basename(fileName).replace(/[-.]/g, "_")
  // camelCase varName
  const camelCaseVarName = varName.replace(/_([a-z])/g, (m, p1) => p1.toUpperCase())
  finalConfig += `export const ${camelCaseVarName} = ${JSON.stringify(fileContent)}\n`
}

fs.writeFileSync(output, finalConfig)
