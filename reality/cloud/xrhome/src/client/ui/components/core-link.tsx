import React from 'react'
import {Link} from 'react-router-dom'

interface ICoreLink extends Omit<React.AnchorHTMLAttributes<HTMLAnchorElement>, 'href'> {
  to?: string    // To uses <Link>,
  href?: string  // href uses an <a> tag.
  rel?: string
  target?: string
  a8?: string
  newTab?: boolean
}

const CoreLink: React.FC<ICoreLink> = ({
  to, href, className, children, newTab, rel, target, ...rest
}) => {
  if (to) {
    return <Link to={to} className={className} {...rest}>{children}</Link>
  } else {
    return (
      <a
        href={href}
        rel={rel || (newTab ? 'noopener noreferrer' : undefined)}
        target={target || (newTab ? '_blank' : undefined)}
        className={className}
        {...rest}
      >{children}
      </a>
    )
  }
}

export {
  CoreLink,
}

export type {
  ICoreLink,
}
