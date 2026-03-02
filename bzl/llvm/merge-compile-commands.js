/* eslint-disable no-console */

import {promises as fs} from 'fs'

const main = async () => {
  const directory = process.argv[2]
  const paths = process.argv.slice(3)

  const compileCommands = {}

  const cmds = paths.map(async (path) => {
    const ccData = await fs.readFile(path, 'utf8')
    const result = JSON.parse(ccData)
    result.forEach((cmd) => {
      compileCommands[cmd.file] = cmd
      compileCommands[cmd.file].directory = directory
    })
  })
  await Promise.all(cmds)
  const outputData = JSON.stringify(Object.values(compileCommands), null, 2)
  console.log(`Wrote merged compile commands to ${process.cwd()}/compile_commands.json`)
  await fs.writeFile('compile_commands.json', outputData, 'utf8')
}

main()
