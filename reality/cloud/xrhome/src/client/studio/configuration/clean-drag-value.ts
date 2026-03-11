const cleanDragValue = (value: number, isPercentage: boolean, fixed: number) => {
  const fixedValue = Number(value.toFixed(fixed ?? 3))
  return isPercentage ? `${fixedValue}%` : fixedValue.toString()
}

export {cleanDragValue}
