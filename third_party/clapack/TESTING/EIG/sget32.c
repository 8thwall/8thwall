#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;

/* Subroutine */ int sget32_(real *rmax, integer *lmax, integer *ninfo, 
	integer *knt)
{
    /* Initialized data */

    static integer itval[32]	/* was [2][2][8] */ = { 8,4,2,1,4,8,1,2,2,1,8,
	    4,1,2,4,8,9,4,2,1,4,9,1,2,2,1,9,4,1,2,4,9 };

    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    real b[4]	/* was [2][2] */, x[4]	/* was [2][2] */;
    integer n1, n2, ib;
    real tl[4]	/* was [2][2] */, tr[4]	/* was [2][2] */;
    integer ib1, ib2, ib3;
    real den, val[3], eps;
    integer itl;
    real res, sgn;
    integer itr;
    real tmp;
    integer info, isgn;
    real tnrm, xnrm, scale, xnorm;
    extern /* Subroutine */ int slasy2_(logical *, logical *, integer *, 
	    integer *, integer *, real *, integer *, real *, integer *, real *
, integer *, real *, real *, integer *, real *, integer *), 
	    slabad_(real *, real *);
    extern doublereal slamch_(char *);
    real bignum;
    integer itranl, itlscl;
    logical ltranl;
    integer itranr, itrscl;
    logical ltranr;
    real smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGET32 tests SLASY2, a routine for solving */

/*          op(TL)*X + ISGN*X*op(TR) = SCALE*B */

/*  where TL is N1 by N1, TR is N2 by N2, and N1,N2 =1 or 2 only. */
/*  X and B are N1 by N2, op() is an optional transpose, an */
/*  ISGN = 1 or -1. SCALE is chosen less than or equal to 1 to */
/*  avoid overflow in X. */

/*  The test condition is that the scaled residual */

/*  norm( op(TL)*X + ISGN*X*op(TR) = SCALE*B ) */
/*       / ( max( ulp*norm(TL), ulp*norm(TR)) * norm(X), SMLNUM ) */

/*  should be on the order of 1. Here, ulp is the machine precision. */
/*  Also, it is verified that SCALE is less than or equal to 1, and */
/*  that XNORM = infinity-norm(X). */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) REAL */
/*          Value of the largest test ratio. */

/*  LMAX    (output) INTEGER */
/*          Example number where largest test ratio achieved. */

/*  NINFO   (output) INTEGER */
/*          Number of examples returned with INFO.NE.0. */

/*  KNT     (output) INTEGER */
/*          Total number of examples tested. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Data statements .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Get machine parameters */

    eps = slamch_("P");
    smlnum = slamch_("S") / eps;
    bignum = 1.f / smlnum;
    slabad_(&smlnum, &bignum);

/*     Set up test case parameters */

    val[0] = sqrt(smlnum);
    val[1] = 1.f;
    val[2] = sqrt(bignum);

    *knt = 0;
    *ninfo = 0;
    *lmax = 0;
    *rmax = 0.f;

/*     Begin test loop */

    for (itranl = 0; itranl <= 1; ++itranl) {
	for (itranr = 0; itranr <= 1; ++itranr) {
	    for (isgn = -1; isgn <= 1; isgn += 2) {
		sgn = (real) isgn;
		ltranl = itranl == 1;
		ltranr = itranr == 1;

		n1 = 1;
		n2 = 1;
		for (itl = 1; itl <= 3; ++itl) {
		    for (itr = 1; itr <= 3; ++itr) {
			for (ib = 1; ib <= 3; ++ib) {
			    tl[0] = val[itl - 1];
			    tr[0] = val[itr - 1];
			    b[0] = val[ib - 1];
			    ++(*knt);
			    slasy2_(&ltranl, &ltranr, &isgn, &n1, &n2, tl, &
				    c__2, tr, &c__2, b, &c__2, &scale, x, &
				    c__2, &xnorm, &info);
			    if (info != 0) {
				++(*ninfo);
			    }
			    res = (r__1 = (tl[0] + sgn * tr[0]) * x[0] - 
				    scale * b[0], dabs(r__1));
			    if (info == 0) {
/* Computing MAX */
				r__1 = eps * ((dabs(tr[0]) + dabs(tl[0])) * 
					dabs(x[0]));
				den = dmax(r__1,smlnum);
			    } else {
/* Computing MAX */
				r__1 = dabs(x[0]);
				den = smlnum * dmax(r__1,1.f);
			    }
			    res /= den;
			    if (scale > 1.f) {
				res += 1.f / eps;
			    }
			    res += (r__1 = xnorm - dabs(x[0]), dabs(r__1)) / 
				    dmax(smlnum,xnorm) / eps;
			    if (info != 0 && info != 1) {
				res += 1.f / eps;
			    }
			    if (res > *rmax) {
				*lmax = *knt;
				*rmax = res;
			    }
/* L10: */
			}
/* L20: */
		    }
/* L30: */
		}

		n1 = 2;
		n2 = 1;
		for (itl = 1; itl <= 8; ++itl) {
		    for (itlscl = 1; itlscl <= 3; ++itlscl) {
			for (itr = 1; itr <= 3; ++itr) {
			    for (ib1 = 1; ib1 <= 3; ++ib1) {
				for (ib2 = 1; ib2 <= 3; ++ib2) {
				    b[0] = val[ib1 - 1];
				    b[1] = val[ib2 - 1] * -4.f;
				    tl[0] = itval[((itl << 1) + 1 << 1) - 6] *
					     val[itlscl - 1];
				    tl[1] = itval[((itl << 1) + 1 << 1) - 5] *
					     val[itlscl - 1];
				    tl[2] = itval[((itl << 1) + 2 << 1) - 6] *
					     val[itlscl - 1];
				    tl[3] = itval[((itl << 1) + 2 << 1) - 5] *
					     val[itlscl - 1];
				    tr[0] = val[itr - 1];
				    ++(*knt);
				    slasy2_(&ltranl, &ltranr, &isgn, &n1, &n2, 
					     tl, &c__2, tr, &c__2, b, &c__2, &
					    scale, x, &c__2, &xnorm, &info);
				    if (info != 0) {
					++(*ninfo);
				    }
				    if (ltranl) {
					tmp = tl[2];
					tl[2] = tl[1];
					tl[1] = tmp;
				    }
				    res = (r__1 = (tl[0] + sgn * tr[0]) * x[0]
					     + tl[2] * x[1] - scale * b[0], 
					    dabs(r__1));
				    res += (r__1 = (tl[3] + sgn * tr[0]) * x[
					    1] + tl[1] * x[0] - scale * b[1], 
					    dabs(r__1));
				    tnrm = dabs(tr[0]) + dabs(tl[0]) + dabs(
					    tl[2]) + dabs(tl[1]) + dabs(tl[3])
					    ;
/* Computing MAX */
				    r__1 = dabs(x[0]), r__2 = dabs(x[1]);
				    xnrm = dmax(r__1,r__2);
/* Computing MAX */
				    r__1 = smlnum, r__2 = smlnum * xnrm, r__1 
					    = max(r__1,r__2), r__2 = tnrm * 
					    eps * xnrm;
				    den = dmax(r__1,r__2);
				    res /= den;
				    if (scale > 1.f) {
					res += 1.f / eps;
				    }
				    res += (r__1 = xnorm - xnrm, dabs(r__1)) /
					     dmax(smlnum,xnorm) / eps;
				    if (res > *rmax) {
					*lmax = *knt;
					*rmax = res;
				    }
/* L40: */
				}
/* L50: */
			    }
/* L60: */
			}
/* L70: */
		    }
/* L80: */
		}

		n1 = 1;
		n2 = 2;
		for (itr = 1; itr <= 8; ++itr) {
		    for (itrscl = 1; itrscl <= 3; ++itrscl) {
			for (itl = 1; itl <= 3; ++itl) {
			    for (ib1 = 1; ib1 <= 3; ++ib1) {
				for (ib2 = 1; ib2 <= 3; ++ib2) {
				    b[0] = val[ib1 - 1];
				    b[2] = val[ib2 - 1] * -2.f;
				    tr[0] = itval[((itr << 1) + 1 << 1) - 6] *
					     val[itrscl - 1];
				    tr[1] = itval[((itr << 1) + 1 << 1) - 5] *
					     val[itrscl - 1];
				    tr[2] = itval[((itr << 1) + 2 << 1) - 6] *
					     val[itrscl - 1];
				    tr[3] = itval[((itr << 1) + 2 << 1) - 5] *
					     val[itrscl - 1];
				    tl[0] = val[itl - 1];
				    ++(*knt);
				    slasy2_(&ltranl, &ltranr, &isgn, &n1, &n2, 
					     tl, &c__2, tr, &c__2, b, &c__2, &
					    scale, x, &c__2, &xnorm, &info);
				    if (info != 0) {
					++(*ninfo);
				    }
				    if (ltranr) {
					tmp = tr[2];
					tr[2] = tr[1];
					tr[1] = tmp;
				    }
				    tnrm = dabs(tl[0]) + dabs(tr[0]) + dabs(
					    tr[2]) + dabs(tr[3]) + dabs(tr[1])
					    ;
				    xnrm = dabs(x[0]) + dabs(x[2]);
				    res = (r__1 = (tl[0] + sgn * tr[0]) * x[0]
					     + sgn * tr[1] * x[2] - scale * b[
					    0], dabs(r__1));
				    res += (r__1 = (tl[0] + sgn * tr[3]) * x[
					    2] + sgn * tr[2] * x[0] - scale * 
					    b[2], dabs(r__1));
/* Computing MAX */
				    r__1 = smlnum, r__2 = smlnum * xnrm, r__1 
					    = max(r__1,r__2), r__2 = tnrm * 
					    eps * xnrm;
				    den = dmax(r__1,r__2);
				    res /= den;
				    if (scale > 1.f) {
					res += 1.f / eps;
				    }
				    res += (r__1 = xnorm - xnrm, dabs(r__1)) /
					     dmax(smlnum,xnorm) / eps;
				    if (res > *rmax) {
					*lmax = *knt;
					*rmax = res;
				    }
/* L90: */
				}
/* L100: */
			    }
/* L110: */
			}
/* L120: */
		    }
/* L130: */
		}

		n1 = 2;
		n2 = 2;
		for (itr = 1; itr <= 8; ++itr) {
		    for (itrscl = 1; itrscl <= 3; ++itrscl) {
			for (itl = 1; itl <= 8; ++itl) {
			    for (itlscl = 1; itlscl <= 3; ++itlscl) {
				for (ib1 = 1; ib1 <= 3; ++ib1) {
				    for (ib2 = 1; ib2 <= 3; ++ib2) {
					for (ib3 = 1; ib3 <= 3; ++ib3) {
					    b[0] = val[ib1 - 1];
					    b[1] = val[ib2 - 1] * -4.f;
					    b[2] = val[ib3 - 1] * -2.f;
/* Computing MIN */
					    r__1 = val[ib1 - 1], r__2 = val[
						    ib2 - 1], r__1 = min(r__1,
						    r__2), r__2 = val[ib3 - 1]
						    ;
					    b[3] = dmin(r__1,r__2) * 8.f;
					    tr[0] = itval[((itr << 1) + 1 << 
						    1) - 6] * val[itrscl - 1];
					    tr[1] = itval[((itr << 1) + 1 << 
						    1) - 5] * val[itrscl - 1];
					    tr[2] = itval[((itr << 1) + 2 << 
						    1) - 6] * val[itrscl - 1];
					    tr[3] = itval[((itr << 1) + 2 << 
						    1) - 5] * val[itrscl - 1];
					    tl[0] = itval[((itl << 1) + 1 << 
						    1) - 6] * val[itlscl - 1];
					    tl[1] = itval[((itl << 1) + 1 << 
						    1) - 5] * val[itlscl - 1];
					    tl[2] = itval[((itl << 1) + 2 << 
						    1) - 6] * val[itlscl - 1];
					    tl[3] = itval[((itl << 1) + 2 << 
						    1) - 5] * val[itlscl - 1];
					    ++(*knt);
					    slasy2_(&ltranl, &ltranr, &isgn, &
						    n1, &n2, tl, &c__2, tr, &
						    c__2, b, &c__2, &scale, x, 
						     &c__2, &xnorm, &info);
					    if (info != 0) {
			  ++(*ninfo);
					    }
					    if (ltranr) {
			  tmp = tr[2];
			  tr[2] = tr[1];
			  tr[1] = tmp;
					    }
					    if (ltranl) {
			  tmp = tl[2];
			  tl[2] = tl[1];
			  tl[1] = tmp;
					    }
					    tnrm = dabs(tr[0]) + dabs(tr[1]) 
						    + dabs(tr[2]) + dabs(tr[3]
						    ) + dabs(tl[0]) + dabs(tl[
						    1]) + dabs(tl[2]) + dabs(
						    tl[3]);
/* Computing MAX */
					    r__1 = dabs(x[0]) + dabs(x[2]), 
						    r__2 = dabs(x[1]) + dabs(
						    x[3]);
					    xnrm = dmax(r__1,r__2);
					    res = (r__1 = (tl[0] + sgn * tr[0]
						    ) * x[0] + sgn * tr[1] * 
						    x[2] + tl[2] * x[1] - 
						    scale * b[0], dabs(r__1));
					    res += (r__1 = tl[0] * x[2] + sgn 
						    * tr[2] * x[0] + sgn * tr[
						    3] * x[2] + tl[2] * x[3] 
						    - scale * b[2], dabs(r__1)
						    );
					    res += (r__1 = tl[1] * x[0] + sgn 
						    * tr[0] * x[1] + sgn * tr[
						    1] * x[3] + tl[3] * x[1] 
						    - scale * b[1], dabs(r__1)
						    );
					    res += (r__1 = (tl[3] + sgn * tr[
						    3]) * x[3] + sgn * tr[2] *
						     x[1] + tl[1] * x[2] - 
						    scale * b[3], dabs(r__1));
/* Computing MAX */
					    r__1 = smlnum, r__2 = smlnum * 
						    xnrm, r__1 = max(r__1,
						    r__2), r__2 = tnrm * eps *
						     xnrm;
					    den = dmax(r__1,r__2);
					    res /= den;
					    if (scale > 1.f) {
			  res += 1.f / eps;
					    }
					    res += (r__1 = xnorm - xnrm, dabs(
						    r__1)) / dmax(smlnum,
						    xnorm) / eps;
					    if (res > *rmax) {
			  *lmax = *knt;
			  *rmax = res;
					    }
/* L140: */
					}
/* L150: */
				    }
/* L160: */
				}
/* L170: */
			    }
/* L180: */
			}
/* L190: */
		    }
/* L200: */
		}
/* L210: */
	    }
/* L220: */
	}
/* L230: */
    }

    return 0;

/*     End of SGET32 */

} /* sget32_ */
