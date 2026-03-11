import React from 'react'

interface ILinkOut {
  url: string
  className?: string
  children?: React.ReactNode
  a8?: string
}

const LinkOut: React.FC<ILinkOut> = ({
  url, className = null, children = [], a8 = null, ...props
}) => (
  <a target='_blank' rel='noopener noreferrer' href={url} className={className} a8={a8} {...props}>
    {children}
  </a>
)

export default LinkOut
