#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;

/* Subroutine */ int dget31_(doublereal *rmax, integer *lmax, integer *ninfo, 
	integer *knt)
{
    /* Initialized data */

    static logical ltrans[2] = { FALSE_,TRUE_ };

    /* System generated locals */
    doublereal d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8, d__9, d__10, 
	    d__11, d__12, d__13;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    doublereal a[4]	/* was [2][2] */, b[4]	/* was [2][2] */, x[4]	/* 
	    was [2][2] */, d1, d2, ca;
    integer ia, ib, na;
    doublereal wi;
    integer nw;
    doublereal wr;
    integer id1, id2, ica;
    doublereal den, vab[3], vca[5], vdd[4], eps;
    integer iwi;
    doublereal res, tmp;
    integer iwr;
    doublereal vwi[4], vwr[4];
    integer info;
    doublereal unfl, smin, scale;
    integer ismin;
    doublereal vsmin[4], xnorm;
    extern /* Subroutine */ int dlaln2_(logical *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     doublereal *, doublereal *, integer *, doublereal *, doublereal *
, doublereal *, integer *, doublereal *, doublereal *, integer *),
	     dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    doublereal bignum;
    integer itrans;
    doublereal smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGET31 tests DLALN2, a routine for solving */

/*     (ca A - w D)X = sB */

/*  where A is an NA by NA matrix (NA=1 or 2 only), w is a real (NW=1) or */
/*  complex (NW=2) constant, ca is a real constant, D is an NA by NA real */
/*  diagonal matrix, and B is an NA by NW matrix (when NW=2 the second */
/*  column of B contains the imaginary part of the solution).  The code */
/*  returns X and s, where s is a scale factor, less than or equal to 1, */
/*  which is chosen to avoid overflow in X. */

/*  If any singular values of ca A-w D are less than another input */
/*  parameter SMIN, they are perturbed up to SMIN. */

/*  The test condition is that the scaled residual */

/*      norm( (ca A-w D)*X - s*B ) / */
/*            ( max( ulp*norm(ca A-w D), SMIN )*norm(X) ) */

/*  should be on the order of 1.  Here, ulp is the machine precision. */
/*  Also, it is verified that SCALE is less than or equal to 1, and that */
/*  XNORM = infinity-norm(X). */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) DOUBLE PRECISION */
/*          Value of the largest test ratio. */

/*  LMAX    (output) INTEGER */
/*          Example number where largest test ratio achieved. */

/*  NINFO   (output) INTEGER array, dimension (3) */
/*          NINFO(1) = number of examples with INFO less than 0 */
/*          NINFO(2) = number of examples with INFO greater than 0 */

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
    /* Parameter adjustments */
    --ninfo;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Get machine parameters */

    eps = dlamch_("P");
    unfl = dlamch_("U");
    smlnum = dlamch_("S") / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Set up test case parameters */

    vsmin[0] = smlnum;
    vsmin[1] = eps;
    vsmin[2] = .01;
    vsmin[3] = 1. / eps;
    vab[0] = sqrt(smlnum);
    vab[1] = 1.;
    vab[2] = sqrt(bignum);
    vwr[0] = 0.;
    vwr[1] = .5;
    vwr[2] = 2.;
    vwr[3] = 1.;
    vwi[0] = smlnum;
    vwi[1] = eps;
    vwi[2] = 1.;
    vwi[3] = 2.;
    vdd[0] = sqrt(smlnum);
    vdd[1] = 1.;
    vdd[2] = 2.;
    vdd[3] = sqrt(bignum);
    vca[0] = 0.;
    vca[1] = sqrt(smlnum);
    vca[2] = eps;
    vca[3] = .5;
    vca[4] = 1.;

    *knt = 0;
    ninfo[1] = 0;
    ninfo[2] = 0;
    *lmax = 0;
    *rmax = 0.;

/*     Begin test loop */

    for (id1 = 1; id1 <= 4; ++id1) {
	d1 = vdd[id1 - 1];
	for (id2 = 1; id2 <= 4; ++id2) {
	    d2 = vdd[id2 - 1];
	    for (ica = 1; ica <= 5; ++ica) {
		ca = vca[ica - 1];
		for (itrans = 0; itrans <= 1; ++itrans) {
		    for (ismin = 1; ismin <= 4; ++ismin) {
			smin = vsmin[ismin - 1];

			na = 1;
			nw = 1;
			for (ia = 1; ia <= 3; ++ia) {
			    a[0] = vab[ia - 1];
			    for (ib = 1; ib <= 3; ++ib) {
				b[0] = vab[ib - 1];
				for (iwr = 1; iwr <= 4; ++iwr) {
				    if (d1 == 1. && d2 == 1. && ca == 1.) {
					wr = vwr[iwr - 1] * a[0];
				    } else {
					wr = vwr[iwr - 1];
				    }
				    wi = 0.;
				    dlaln2_(&ltrans[itrans], &na, &nw, &smin, 
					    &ca, a, &c__2, &d1, &d2, b, &c__2, 
					     &wr, &wi, x, &c__2, &scale, &
					    xnorm, &info);
				    if (info < 0) {
					++ninfo[1];
				    }
				    if (info > 0) {
					++ninfo[2];
				    }
				    res = (d__1 = (ca * a[0] - wr * d1) * x[0]
					     - scale * b[0], abs(d__1));
				    if (info == 0) {
/* Computing MAX */
					d__2 = eps * (d__1 = (ca * a[0] - wr *
						 d1) * x[0], abs(d__1));
					den = max(d__2,smlnum);
				    } else {
/* Computing MAX */
					d__1 = smin * abs(x[0]);
					den = max(d__1,smlnum);
				    }
				    res /= den;
				    if (abs(x[0]) < unfl && abs(b[0]) <= 
					    smlnum * (d__1 = ca * a[0] - wr * 
					    d1, abs(d__1))) {
					res = 0.;
				    }
				    if (scale > 1.) {
					res += 1. / eps;
				    }
				    res += (d__1 = xnorm - abs(x[0]), abs(
					    d__1)) / max(smlnum,xnorm) / eps;
				    if (info != 0 && info != 1) {
					res += 1. / eps;
				    }
				    ++(*knt);
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

			na = 1;
			nw = 2;
			for (ia = 1; ia <= 3; ++ia) {
			    a[0] = vab[ia - 1];
			    for (ib = 1; ib <= 3; ++ib) {
				b[0] = vab[ib - 1];
				b[2] = vab[ib - 1] * -.5;
				for (iwr = 1; iwr <= 4; ++iwr) {
				    if (d1 == 1. && d2 == 1. && ca == 1.) {
					wr = vwr[iwr - 1] * a[0];
				    } else {
					wr = vwr[iwr - 1];
				    }
				    for (iwi = 1; iwi <= 4; ++iwi) {
					if (d1 == 1. && d2 == 1. && ca == 1.) 
						{
					    wi = vwi[iwi - 1] * a[0];
					} else {
					    wi = vwi[iwi - 1];
					}
					dlaln2_(&ltrans[itrans], &na, &nw, &
						smin, &ca, a, &c__2, &d1, &d2, 
						 b, &c__2, &wr, &wi, x, &c__2, 
						 &scale, &xnorm, &info);
					if (info < 0) {
					    ++ninfo[1];
					}
					if (info > 0) {
					    ++ninfo[2];
					}
					res = (d__1 = (ca * a[0] - wr * d1) * 
						x[0] + wi * d1 * x[2] - scale 
						* b[0], abs(d__1));
					res += (d__1 = -wi * d1 * x[0] + (ca *
						 a[0] - wr * d1) * x[2] - 
						scale * b[2], abs(d__1));
					if (info == 0) {
/* Computing MAX */
/* Computing MAX */
					    d__4 = (d__1 = ca * a[0] - wr * 
						    d1, abs(d__1)), d__5 = (
						    d__2 = d1 * wi, abs(d__2))
						    ;
					    d__3 = eps * (max(d__4,d__5) * (
						    abs(x[0]) + abs(x[2])));
					    den = max(d__3,smlnum);
					} else {
/* Computing MAX */
					    d__1 = smin * (abs(x[0]) + abs(x[
						    2]));
					    den = max(d__1,smlnum);
					}
					res /= den;
					if (abs(x[0]) < unfl && abs(x[2]) < 
						unfl && abs(b[0]) <= smlnum * 
						(d__1 = ca * a[0] - wr * d1, 
						abs(d__1))) {
					    res = 0.;
					}
					if (scale > 1.) {
					    res += 1. / eps;
					}
					res += (d__1 = xnorm - abs(x[0]) - 
						abs(x[2]), abs(d__1)) / max(
						smlnum,xnorm) / eps;
					if (info != 0 && info != 1) {
					    res += 1. / eps;
					}
					++(*knt);
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

			na = 2;
			nw = 1;
			for (ia = 1; ia <= 3; ++ia) {
			    a[0] = vab[ia - 1];
			    a[2] = vab[ia - 1] * -3.;
			    a[1] = vab[ia - 1] * -7.;
			    a[3] = vab[ia - 1] * 21.;
			    for (ib = 1; ib <= 3; ++ib) {
				b[0] = vab[ib - 1];
				b[1] = vab[ib - 1] * -2.;
				for (iwr = 1; iwr <= 4; ++iwr) {
				    if (d1 == 1. && d2 == 1. && ca == 1.) {
					wr = vwr[iwr - 1] * a[0];
				    } else {
					wr = vwr[iwr - 1];
				    }
				    wi = 0.;
				    dlaln2_(&ltrans[itrans], &na, &nw, &smin, 
					    &ca, a, &c__2, &d1, &d2, b, &c__2, 
					     &wr, &wi, x, &c__2, &scale, &
					    xnorm, &info);
				    if (info < 0) {
					++ninfo[1];
				    }
				    if (info > 0) {
					++ninfo[2];
				    }
				    if (itrans == 1) {
					tmp = a[2];
					a[2] = a[1];
					a[1] = tmp;
				    }
				    res = (d__1 = (ca * a[0] - wr * d1) * x[0]
					     + ca * a[2] * x[1] - scale * b[0]
					    , abs(d__1));
				    res += (d__1 = ca * a[1] * x[0] + (ca * a[
					    3] - wr * d2) * x[1] - scale * b[
					    1], abs(d__1));
				    if (info == 0) {
/* Computing MAX */
/* Computing MAX */
					d__6 = (d__1 = ca * a[0] - wr * d1, 
						abs(d__1)) + (d__2 = ca * a[2]
						, abs(d__2)), d__7 = (d__3 = 
						ca * a[1], abs(d__3)) + (d__4 
						= ca * a[3] - wr * d2, abs(
						d__4));
/* Computing MAX */
					d__8 = abs(x[0]), d__9 = abs(x[1]);
					d__5 = eps * (max(d__6,d__7) * max(
						d__8,d__9));
					den = max(d__5,smlnum);
				    } else {
/* Computing MAX */
/* Computing MAX */
/* Computing MAX */
					d__8 = (d__1 = ca * a[0] - wr * d1, 
						abs(d__1)) + (d__2 = ca * a[2]
						, abs(d__2)), d__9 = (d__3 = 
						ca * a[1], abs(d__3)) + (d__4 
						= ca * a[3] - wr * d2, abs(
						d__4));
					d__6 = smin / eps, d__7 = max(d__8,
						d__9);
/* Computing MAX */
					d__10 = abs(x[0]), d__11 = abs(x[1]);
					d__5 = eps * (max(d__6,d__7) * max(
						d__10,d__11));
					den = max(d__5,smlnum);
				    }
				    res /= den;
				    if (abs(x[0]) < unfl && abs(x[1]) < unfl 
					    && abs(b[0]) + abs(b[1]) <= 
					    smlnum * ((d__1 = ca * a[0] - wr *
					     d1, abs(d__1)) + (d__2 = ca * a[
					    2], abs(d__2)) + (d__3 = ca * a[1]
					    , abs(d__3)) + (d__4 = ca * a[3] 
					    - wr * d2, abs(d__4)))) {
					res = 0.;
				    }
				    if (scale > 1.) {
					res += 1. / eps;
				    }
/* Computing MAX */
				    d__2 = abs(x[0]), d__3 = abs(x[1]);
				    res += (d__1 = xnorm - max(d__2,d__3), 
					    abs(d__1)) / max(smlnum,xnorm) / 
					    eps;
				    if (info != 0 && info != 1) {
					res += 1. / eps;
				    }
				    ++(*knt);
				    if (res > *rmax) {
					*lmax = *knt;
					*rmax = res;
				    }
/* L80: */
				}
/* L90: */
			    }
/* L100: */
			}

			na = 2;
			nw = 2;
			for (ia = 1; ia <= 3; ++ia) {
			    a[0] = vab[ia - 1] * 2.;
			    a[2] = vab[ia - 1] * -3.;
			    a[1] = vab[ia - 1] * -7.;
			    a[3] = vab[ia - 1] * 21.;
			    for (ib = 1; ib <= 3; ++ib) {
				b[0] = vab[ib - 1];
				b[1] = vab[ib - 1] * -2.;
				b[2] = vab[ib - 1] * 4.;
				b[3] = vab[ib - 1] * -7.;
				for (iwr = 1; iwr <= 4; ++iwr) {
				    if (d1 == 1. && d2 == 1. && ca == 1.) {
					wr = vwr[iwr - 1] * a[0];
				    } else {
					wr = vwr[iwr - 1];
				    }
				    for (iwi = 1; iwi <= 4; ++iwi) {
					if (d1 == 1. && d2 == 1. && ca == 1.) 
						{
					    wi = vwi[iwi - 1] * a[0];
					} else {
					    wi = vwi[iwi - 1];
					}
					dlaln2_(&ltrans[itrans], &na, &nw, &
						smin, &ca, a, &c__2, &d1, &d2, 
						 b, &c__2, &wr, &wi, x, &c__2, 
						 &scale, &xnorm, &info);
					if (info < 0) {
					    ++ninfo[1];
					}
					if (info > 0) {
					    ++ninfo[2];
					}
					if (itrans == 1) {
					    tmp = a[2];
					    a[2] = a[1];
					    a[1] = tmp;
					}
					res = (d__1 = (ca * a[0] - wr * d1) * 
						x[0] + ca * a[2] * x[1] + wi *
						 d1 * x[2] - scale * b[0], 
						abs(d__1));
					res += (d__1 = (ca * a[0] - wr * d1) *
						 x[2] + ca * a[2] * x[3] - wi 
						* d1 * x[0] - scale * b[2], 
						abs(d__1));
					res += (d__1 = ca * a[1] * x[0] + (ca 
						* a[3] - wr * d2) * x[1] + wi 
						* d2 * x[3] - scale * b[1], 
						abs(d__1));
					res += (d__1 = ca * a[1] * x[2] + (ca 
						* a[3] - wr * d2) * x[3] - wi 
						* d2 * x[1] - scale * b[3], 
						abs(d__1));
					if (info == 0) {
/* Computing MAX */
/* Computing MAX */
					    d__8 = (d__1 = ca * a[0] - wr * 
						    d1, abs(d__1)) + (d__2 = 
						    ca * a[2], abs(d__2)) + (
						    d__3 = wi * d1, abs(d__3))
						    , d__9 = (d__4 = ca * a[1]
						    , abs(d__4)) + (d__5 = ca 
						    * a[3] - wr * d2, abs(
						    d__5)) + (d__6 = wi * d2, 
						    abs(d__6));
/* Computing MAX */
					    d__10 = abs(x[0]) + abs(x[1]), 
						    d__11 = abs(x[2]) + abs(x[
						    3]);
					    d__7 = eps * (max(d__8,d__9) * 
						    max(d__10,d__11));
					    den = max(d__7,smlnum);
					} else {
/* Computing MAX */
/* Computing MAX */
/* Computing MAX */
					    d__10 = (d__1 = ca * a[0] - wr * 
						    d1, abs(d__1)) + (d__2 = 
						    ca * a[2], abs(d__2)) + (
						    d__3 = wi * d1, abs(d__3))
						    , d__11 = (d__4 = ca * a[
						    1], abs(d__4)) + (d__5 = 
						    ca * a[3] - wr * d2, abs(
						    d__5)) + (d__6 = wi * d2, 
						    abs(d__6));
					    d__8 = smin / eps, d__9 = max(
						    d__10,d__11);
/* Computing MAX */
					    d__12 = abs(x[0]) + abs(x[1]), 
						    d__13 = abs(x[2]) + abs(x[
						    3]);
					    d__7 = eps * (max(d__8,d__9) * 
						    max(d__12,d__13));
					    den = max(d__7,smlnum);
					}
					res /= den;
					if (abs(x[0]) < unfl && abs(x[1]) < 
						unfl && abs(x[2]) < unfl && 
						abs(x[3]) < unfl && abs(b[0]) 
						+ abs(b[1]) <= smlnum * ((
						d__1 = ca * a[0] - wr * d1, 
						abs(d__1)) + (d__2 = ca * a[2]
						, abs(d__2)) + (d__3 = ca * a[
						1], abs(d__3)) + (d__4 = ca * 
						a[3] - wr * d2, abs(d__4)) + (
						d__5 = wi * d2, abs(d__5)) + (
						d__6 = wi * d1, abs(d__6)))) {
					    res = 0.;
					}
					if (scale > 1.) {
					    res += 1. / eps;
					}
/* Computing MAX */
					d__2 = abs(x[0]) + abs(x[2]), d__3 = 
						abs(x[1]) + abs(x[3]);
					res += (d__1 = xnorm - max(d__2,d__3),
						 abs(d__1)) / max(smlnum,
						xnorm) / eps;
					if (info != 0 && info != 1) {
					    res += 1. / eps;
					}
					++(*knt);
					if (res > *rmax) {
					    *lmax = *knt;
					    *rmax = res;
					}
/* L110: */
				    }
/* L120: */
				}
/* L130: */
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

    return 0;

/*     End of DGET31 */

} /* dget31_ */
