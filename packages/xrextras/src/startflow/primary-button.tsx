import {h, ComponentChildren, FunctionComponent} from 'preact'

import styles from './primary-button.module.scss'

interface PrimaryButtonProps {
  children: ComponentChildren
  onClick: () => void
}

const PrimaryButton: FunctionComponent<PrimaryButtonProps> = ({children, onClick}) => (
  <button className={styles.button} type='button' onClick={onClick}>
    {children}
  </button>
)

export {PrimaryButton}
