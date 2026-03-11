// @attr[](visibility = "//reality/cloud:shared")
// @attr[](visibility = "//reality/app/nae:__subpackages__")

import {PublishCommand, SNSClient} from '@aws-sdk/client-sns'

const messaging = (sns: SNSClient) => {
  const sendBuildNotification = async (
    body: string,
    attributes: Record<string, unknown>
  ) => sns.send(new PublishCommand({
    Message: body,
    MessageAttributes: Object.keys(attributes)
      .filter(key => attributes[key] !== undefined)
      .reduce((acc, val) => {
        acc[val] = {
          DataType: 'String',
          StringValue: attributes[val],
        }
        return acc
      }, {}),
    TopicArn: process.env.BUILD_NOTIFICATION_TOPIC,
  }))

  return {
    sendBuildNotification,
  }
}

export {
  messaging,
}
