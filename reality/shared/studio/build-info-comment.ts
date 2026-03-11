// @attr(visibility = ["//visibility:public"])

const generateBuildInfoComment = (
  commitId: string,
  userBuildSettingsHash: string
): string => `<!-- ${commitId} ${userBuildSettingsHash} -->`

const parseBuildInfoComment = (htmlText: string): {
  commitId: string
  userBuildSettingsHash: string
} | null => {
  const regex = /^([a-f0-9]{40})\s([a-f0-9]{32})$/
  const match = htmlText.trim().match(regex)

  if (!match) {
    return null
  }

  const [, commitId, userBuildSettingsHash] = match
  return {commitId, userBuildSettingsHash}
}

export {generateBuildInfoComment, parseBuildInfoComment}
