type ReleaseNotesData = {
  latestPopupId: string
  notes: ReleaseNoteData[]
}

type ReleaseNoteData = {
  id: string
  title: string
  year: number
  month: number
  day: number
  contents: string
  runtimeVersion?: string
}

export type {
  ReleaseNotesData,
  ReleaseNoteData,
}
