// @package(npm-html-app-packager)
// @attr(target = "node")
// @attr(webpack_mode = "development")
// @attr(esnext = 1)

import {parseScriptSrcs} from '@repo/reality/shared/studio/parse-script-srcs'

import {assert, describe, it} from '@repo/bzl/js/chai-js'

describe('Fetch Project Scripts Tests', () => {
  it('should successfully return data for relative scripts', () => {
    const htmlInput = `
    <html>
      <script src="script1.js"></script>
      <script src="script2.js"></script>
    </html>`
    const result = parseScriptSrcs(htmlInput)

    assert.deepStrictEqual(result, ['script1.js', 'script2.js'])
  })

  it('should successfully return app8 script data', () => {
    const htmlInput = `
      <html>
        <head>
          <!-- da94af44d0de213a100492b6054d49ebc55b0604 0bae22c55efedba8cfa72ec0619d3762 -->
          <meta name="8thwall:project" content="native-splats-lucas">
        </head>
        <body>
          <h1>Test</h1>
        </body>
      </html>`

    const result = parseScriptSrcs(htmlInput)

    const data = '/native-splats-lucas/' +
      'dist_da94af44d0de213a100492b6054d49ebc55b0604-0bae22c55efedba8cfa72ec0619d3762_bundle.js'

    assert.deepStrictEqual(result, [data])
  })

  it('should successfully return app8 script urls', () => {
    const htmlInput = `
      <html>
        <head>
          <!-- da94af44d0de213a100492b6054d49ebc55b0604 0bae22c55efedba8cfa72ec0619d3762 -->
          <meta name="8thwall:project" content="native-splats-lucas">
        </head>
        <body>
          <h1>Test</h1>
        </body>
      </html>`

    const result = parseScriptSrcs(htmlInput)

    const expectedUrl = '/native-splats-lucas/' +
      'dist_da94af44d0de213a100492b6054d49ebc55b0604-0bae22c55efedba8cfa72ec0619d3762_bundle.js'

    assert.deepStrictEqual(result, [expectedUrl])
  })

  it('should successfully parse data-xrweb-src urls', () => {
    const expectedUrl = 'https://apps.8thwall.com/xrweb?' +
              'appKey=3lv4TAsS0gqXOKsPlpXKssoFVJq1dsPWnSazOVUGDvPDVDqggrfZrFPmqdgLmoSMK7MpB4&s=1'

    const app8Url = 'https://cdn.8thwall.com/web/hosting/app8-llffcbpc.js'
    const htmlInput = `
      <html>
        <body>
          <script crossorigin="anonymous"
            src="${app8Url}"
            data-xrweb-src="${expectedUrl}"
          >
        </body>
      </html>`

    const result = parseScriptSrcs(htmlInput)

    assert.deepStrictEqual(result, [app8Url, expectedUrl])
  })

  it('should handle non-HTML responses', () => {
    const result = parseScriptSrcs('')

    assert.isArray(result)
    assert.lengthOf(result, 0)
  })
})
