const makeJsonResponse = (data: any, statusCode: number = 200) => (
  new Response(JSON.stringify(data), {
    status: statusCode,
    headers: {
      'Content-Type': 'application/json',
    },
  })
)

export {
  makeJsonResponse,
}
