interface EventDetails {
  detail: any
}

interface Framework {
  dispatchEvent(eventName: string, eventDetails?: EventDetails)
}

interface SkyFoundDetails {
  name: string
  percentage: number
}

export type {
  EventDetails,
  Framework,
  SkyFoundDetails,
}
