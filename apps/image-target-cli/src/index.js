#!/usr/bin/env node

import {createCliInterface} from './cli.js'
import {selectProcessorOptions} from './interactive.js'

async function main() {
  const rl = createCliInterface()
  try {
    await selectProcessorOptions(rl)
  } catch (err) {
    console.log(err)
    console.error('Error:', err.message)
    process.exit(1)
  } finally {
    rl.close()
  }
}

main()
