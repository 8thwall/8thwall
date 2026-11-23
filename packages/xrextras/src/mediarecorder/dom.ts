const HTML_NAMESPACE = 'http://www.w3.org/1999/xhtml'

// A-Frame types overload document.createElement() to return an Entity for every tag.
const createHtmlElement = <TagName extends keyof HTMLElementTagNameMap>(tagName: TagName) => (
  document.createElementNS(HTML_NAMESPACE, tagName) as HTMLElementTagNameMap[TagName]
)

export {createHtmlElement}
