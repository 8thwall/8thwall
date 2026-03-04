// @rule(js_cli)
// @attr[](data = "//c8/ecs/src/shared/features:features-files")
import fs from 'fs'

/* NOTE(christoph): This script generates typescript code that looks like:

```
export * from '@nia/c8/ecs/src/shared/features/jolt'
```

With a line for each file in the features directory (except edition.ts) */

const exportLines: string[] = []
const RUNFILES_DIR = process.env.RUNFILES_DIR!
fs.readdirSync(`${RUNFILES_DIR}/_main/c8/ecs/src/shared/features`).forEach((entry) => {
  exportLines.push(`export * from '@nia/c8/ecs/src/shared/features/${entry.replace(/\.ts$/g, '')}'`)
})

// eslint-disable-next-line no-console
console.log(exportLines.join('\n'))
