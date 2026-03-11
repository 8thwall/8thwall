// @attr(visibility = ["//visibility:public"])
// @attr(esnext = 1)

import * as htmlparser2 from 'htmlparser2'

import {parseBuildInfoComment} from '@repo/reality/shared/studio/build-info-comment'

type DistUrlData = {
  commitId: string
  userBuildSettingsHash: string
  appName: string
}

const parseScriptSrcs = (htmlText: string) => {
  const scriptUrls: string[] = []

  const distUrlData: DistUrlData = {
    commitId: '',
    userBuildSettingsHash: '',
    appName: '',
  }

  const parser = new htmlparser2.Parser({
    onopentag(name, attribs) {
      if (name === 'script') {
        if (attribs.src) {
          scriptUrls.push(attribs.src)
        }

        if (attribs['data-xrweb-src']) {
          scriptUrls.push(attribs['data-xrweb-src'])
        }
      }

      if (name === 'meta') {
        if (attribs.name === '8thwall:project') {
          distUrlData.appName = attribs.content
        }
      }
    },
    oncomment(comment) {
      // Try parse build info from the metadata comment
      const buildInfo = parseBuildInfoComment(comment)
      if (!buildInfo) {
        return
      }

      const {commitId, userBuildSettingsHash} = buildInfo
      distUrlData.commitId = commitId
      distUrlData.userBuildSettingsHash = userBuildSettingsHash
    },
  })

  parser.write(htmlText)
  parser.end()

  // If we all of the data needed to build the dist URL, add it to the script URLs
  if (distUrlData.commitId && distUrlData.userBuildSettingsHash && distUrlData.appName) {
    const distUrl = `/${distUrlData.appName}/` +
      `dist_${distUrlData.commitId}-${distUrlData.userBuildSettingsHash}_bundle.js`
    scriptUrls.push(distUrl)
  }

  return scriptUrls
}

export {parseScriptSrcs}
