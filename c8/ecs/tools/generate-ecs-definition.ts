// @rule(js_cli)

import fs from 'fs'

const run = async () => {
  const definitionRaw = await fs.promises.readFile(process.argv[2], 'utf-8')

  const defFile = definitionRaw
    .replace(/.* UserData.*{[^}]*};?/g, '')  // hide entire UserData type
    .replace(/.* Location\$.*{[^}]*}>?;?/g, '')  // hide entire Location type
    .split('\n')
    .filter(e => !e.includes('_id'))  // hide world._id
    .filter(e => !e.includes(': UserData'))  // hide any props that are UserData
    .filter(e => !e.includes('location?: Location'))  // hide any props that are Location
    // hide Ui.video
    .filter(e => !e.includes('video: "string";') && !e.includes('video: Resource'))
    .filter(e => !e.match(/.* as Location,/g))  // hide export of Location
    .filter(e => !e.includes('registerCompoundShape'))  // hide compound shape functions
    .join('\n')

  // eslint-disable-next-line no-console
  console.log(defFile)
}

run()
