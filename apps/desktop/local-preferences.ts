import {getDb} from './application-state'

const getPreference = (key: string): string | undefined => {
  const row = getDb().prepare<[string], {value: string}>(
    'SELECT value FROM preferences WHERE key = ?'
  ).get(key)
  return row?.value
}

const setPreference = (key: string, value: string) => {
  getDb().prepare('INSERT OR REPLACE INTO preferences (key, value) VALUES (?, ?)').run(key, value)
}

const clearPreference = (key: string) => {
  getDb().prepare('DELETE FROM preferences WHERE key = ?').run(key)
}

export {
  getPreference,
  setPreference,
  clearPreference,
}
