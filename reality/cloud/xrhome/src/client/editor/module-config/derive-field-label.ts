// "myFieldName to "My Field Name", see derive-field-label-test.ts
const deriveFieldLabel = (fieldName: string) => {
  if (!fieldName) {
    return ''
  }
  const pascalCase = fieldName[0].toUpperCase() + fieldName.substring(1)
  return pascalCase.replace(/([^A-Z])([A-Z])/g, '$1 $2')
}

export {
  deriveFieldLabel,
}
