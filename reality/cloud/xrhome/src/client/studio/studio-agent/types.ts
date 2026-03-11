type MessageRequest = {
  requestId: string
  action: string
  parameters: Record<string, any>
  sender: string
}

type MessageResponse = {
  requestId: string
  action: string
  isError?: boolean
  error?: unknown
  response?: any
  type?: 'publish' | 'publishAll'| 'subscribe' | 'unsubscribe'
  channel?: string
}

export type {
  MessageRequest,
  MessageResponse,
}
