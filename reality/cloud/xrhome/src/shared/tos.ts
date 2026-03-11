type TermsVersion = '20180928' | '20191022' | '20220524' | '20220921' | '20230418' | '20230529' |
  '20231114'

type TermsAgreements = {
  [T in TermsVersion]?: number
}

const TOS_CURRENT_VERSION: TermsVersion = '20231114'

export {
  TOS_CURRENT_VERSION,
}

export type {
  TermsVersion,
  TermsAgreements,
}
