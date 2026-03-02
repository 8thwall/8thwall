#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;

/* Subroutine */ int sget31_(real *rmax, integer *lmax, integer *ninfo, 
	integer *knt)
{
    /* Initialized data */

    static logical ltrans[2] = { FALSE_,TRUE_ };

    /* System generated locals */
    real r__1, r__2, r__3, r__4, r__5, r__6, r__7, r__8, r__9, r__10, r__11, 
	    r__12, r__13;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    real a[4]	/* was [2][2] */, b[4]	/* was [2][2] */, x[4]	/* was [2][2] 
	    */, d1, d2, ca;
    integer ia, ib, na;
    real wi;
    integer nw;
    real wr;
    integer id1, id2, ica;
    real den, vab[3], vca[5], vdd[4], eps;
    integer iwi;
    real res, tmp;
    integer iwr;
    real vwi[4], vwr[4];
    integer info;
    real unfl, smin, scale;
    integer ismin;
    real vsmin[4], xnorm;
    extern /* Subroutine */ int slaln2_(logical *, integer *, integer *, real 
	    *, real *, real *, integer *, real *, real *, real *, integer *, 
	    real *, real *, real *, integer *, real *, real *, integer *), 
	    slabad_(real *, real *);
    extern doublereal slamch_(char *);
    real bignum;
    integer itrans;
    real smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGET31 tests SLALN2, a routine for solving */

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

/*  RMAX    (output) REAL */
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

    eps = slamch_("P");
    unfl = slamch_("U");
    smlnum = slamch_("S") / eps;
    bignum = 1.f / smlnum;
    slabad_(&smlnum, &bignum);

/*     Set up test case parameters */

    vsmin[0] = smlnum;
    vsmin[1] = eps;
    vsmin[2] = .01f;
    vsmin[3] = 1.f / eps;
    vab[0] = sqrt(smlnum);
    vab[1] = 1.f;
    vab[2] = sqrt(bignum);
    vwr[0] = 0.f;
    vwr[1] = .5f;
    vwr[2] = 2.f;
    vwr[3] = 1.f;
    vwi[0] = smlnum;
    vwi[1] = eps;
    vwi[2] = 1.f;
    vwi[3] = 2.f;
    vdd[0] = sqrt(smlnum);
    vdd[1] = 1.f;
    vdd[2] = 2.f;
    vdd[3] = sqrt(bignum);
    vca[0] = 0.f;
    vca[1] = sqrt(smlnum);
    vca[2] = eps;
    vca[3] = .5f;
    vca[4] = 1.f;

    *knt = 0;
    ninfo[1] = 0;
    ninfo[2] = 0;
    *lmax = 0;
    *rmax = 0.f;

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
				    if (d1 == 1.f && d2 == 1.f && ca == 1.f) {
					wr = vwr[iwr - 1] * a[0];
				    } else {
					wr = vwr[iwr - 1];
				    }
				    wi = 0.f;
				    slaln2_(&ltrans[itrans], &na, &nw, &smin, 
					    &ca, a, &c__2, &d1, &d2, b, &c__2, 
					     &wr, &wi, x, &c__2, &scale, &
					    xnorm, &info);
				    if (info < 0) {
					++ninfo[1];
				    }
				    if (info > 0) {
					++ninfo[2];
				    }
				    res = (r__1 = (ca * a[0] - wr * d1) * x[0]
					     - scale * b[0], dabs(r__1));
				    if (info == 0) {
/* Computing MAX */
					r__2 = eps * (r__1 = (ca * a[0] - wr *
						 d1) * x[0], dabs(r__1));
					den = dmax(r__2,smlnum);
				    } else {
/* Computing MAX */
					r__1 = smin * dabs(x[0]);
					den = dmax(r__1,smlnum);
				    }
				    res /= den;
				    if (dabs(x[0]) < unfl && dabs(b[0]) <= 
					    smlnum * (r__1 = ca * a[0] - wr * 
					    d1, dabs(r__1))) {
					res = 0.f;
				    }
				    if (scale > 1.f) {
					res += 1.f / eps;
				    }
				    res += (r__1 = xnorm - dabs(x[0]), dabs(
					    r__1)) / dmax(smlnum,xnorm) / eps;
				    if (info != 0 && info != 1) {
					res += 1.f / eps;
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
				b[2] = vab[ib - 1] * -.5f;
				for (iwr = 1; iwr <= 4; ++iwr) {
				    if (d1 == 1.f && d2 == 1.f && ca == 1.f) {
					wr = vwr[iwr - 1] * a[0];
				    } else {
					wr = vwr[iwr - 1];
				    }
				    for (iwi = 1; iwi <= 4; ++iwi) {
					if (d1 == 1.f && d2 == 1.f && ca == 
						1.f) {
					    wi = vwi[iwi - 1] * a[0];
					} else {
					    wi = vwi[iwi - 1];
					}
					slaln2_(&ltrans[itrans], &na, &nw, &
						smin, &ca, a, &c__2, &d1, &d2, 
						 b, &c__2, &wr, &wi, x, &c__2, 
						 &scale, &xnorm, &info);
					if (info < 0) {
					    ++ninfo[1];
					}
					if (info > 0) {
					    ++ninfo[2];
					}
					res = (r__1 = (ca * a[0] - wr * d1) * 
						x[0] + wi * d1 * x[2] - scale 
						* b[0], dabs(r__1));
					res += (r__1 = -wi * d1 * x[0] + (ca *
						 a[0] - wr * d1) * x[2] - 
						scale * b[2], dabs(r__1));
					if (info == 0) {
/* Computing MAX */
/* Computing MAX */
					    r__4 = (r__1 = ca * a[0] - wr * 
						    d1, dabs(r__1)), r__5 = (
						    r__2 = d1 * wi, dabs(r__2)
						    );
					    r__3 = eps * (dmax(r__4,r__5) * (
						    dabs(x[0]) + dabs(x[2])));
					    den = dmax(r__3,smlnum);
					} else {
/* Computing MAX */
					    r__1 = smin * (dabs(x[0]) + dabs(
						    x[2]));
					    den = dmax(r__1,smlnum);
					}
					res /= den;
					if (dabs(x[0]) < unfl && dabs(x[2]) < 
						unfl && dabs(b[0]) <= smlnum *
						 (r__1 = ca * a[0] - wr * d1, 
						dabs(r__1))) {
					    res = 0.f;
					}
					if (scale > 1.f) {
					    res += 1.f / eps;
					}
					res += (r__1 = xnorm - dabs(x[0]) - 
						dabs(x[2]), dabs(r__1)) / 
						dmax(smlnum,xnorm) / eps;
					if (info != 0 && info != 1) {
					    res += 1.f / eps;
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
			    a[2] = vab[ia - 1] * -3.f;
			    a[1] = vab[ia - 1] * -7.f;
			    a[3] = vab[ia - 1] * 21.f;
			    for (ib = 1; ib <= 3; ++ib) {
				b[0] = vab[ib - 1];
				b[1] = vab[ib - 1] * -2.f;
				for (iwr = 1; iwr <= 4; ++iwr) {
				    if (d1 == 1.f && d2 == 1.f && ca == 1.f) {
					wr = vwr[iwr - 1] * a[0];
				    } else {
					wr = vwr[iwr - 1];
				    }
				    wi = 0.f;
				    slaln2_(&ltrans[itrans], &na, &nw, &smin, 
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
				    res = (r__1 = (ca * a[0] - wr * d1) * x[0]
					     + ca * a[2] * x[1] - scale * b[0]
					    , dabs(r__1));
				    res += (r__1 = ca * a[1] * x[0] + (ca * a[
					    3] - wr * d2) * x[1] - scale * b[
					    1], dabs(r__1));
				    if (info == 0) {
/* Computing MAX */
/* Computing MAX */
					r__6 = (r__1 = ca * a[0] - wr * d1, 
						dabs(r__1)) + (r__2 = ca * a[
						2], dabs(r__2)), r__7 = (r__3 
						= ca * a[1], dabs(r__3)) + (
						r__4 = ca * a[3] - wr * d2, 
						dabs(r__4));
/* Computing MAX */
					r__8 = dabs(x[0]), r__9 = dabs(x[1]);
					r__5 = eps * (dmax(r__6,r__7) * dmax(
						r__8,r__9));
					den = dmax(r__5,smlnum);
				    } else {
/* Computing MAX */
/* Computing MAX */
/* Computing MAX */
					r__8 = (r__1 = ca * a[0] - wr * d1, 
						dabs(r__1)) + (r__2 = ca * a[
						2], dabs(r__2)), r__9 = (r__3 
						= ca * a[1], dabs(r__3)) + (
						r__4 = ca * a[3] - wr * d2, 
						dabs(r__4));
					r__6 = smin / eps, r__7 = dmax(r__8,
						r__9);
/* Computing MAX */
					r__10 = dabs(x[0]), r__11 = dabs(x[1])
						;
					r__5 = eps * (dmax(r__6,r__7) * dmax(
						r__10,r__11));
					den = dmax(r__5,smlnum);
				    }
				    res /= den;
				    if (dabs(x[0]) < unfl && dabs(x[1]) < 
					    unfl && dabs(b[0]) + dabs(b[1]) <=
					     smlnum * ((r__1 = ca * a[0] - wr 
					    * d1, dabs(r__1)) + (r__2 = ca * 
					    a[2], dabs(r__2)) + (r__3 = ca * 
					    a[1], dabs(r__3)) + (r__4 = ca * 
					    a[3] - wr * d2, dabs(r__4)))) {
					res = 0.f;
				    }
				    if (scale > 1.f) {
					res += 1.f / eps;
				    }
/* Computing MAX */
				    r__2 = dabs(x[0]), r__3 = dabs(x[1]);
				    res += (r__1 = xnorm - dmax(r__2,r__3), 
					    dabs(r__1)) / dmax(smlnum,xnorm) /
					     eps;
				    if (info != 0 && info != 1) {
					res += 1.f / eps;
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
			    a[0] = vab[ia - 1] * 2.f;
			    a[2] = vab[ia - 1] * -3.f;
			    a[1] = vab[ia - 1] * -7.f;
			    a[3] = vab[ia - 1] * 21.f;
			    for (ib = 1; ib <= 3; ++ib) {
				b[0] = vab[ib - 1];
				b[1] = vab[ib - 1] * -2.f;
				b[2] = vab[ib - 1] * 4.f;
				b[3] = vab[ib - 1] * -7.f;
				for (iwr = 1; iwr <= 4; ++iwr) {
				    if (d1 == 1.f && d2 == 1.f && ca == 1.f) {
					wr = vwr[iwr - 1] * a[0];
				    } else {
					wr = vwr[iwr - 1];
				    }
				    for (iwi = 1; iwi <= 4; ++iwi) {
					if (d1 == 1.f && d2 == 1.f && ca == 
						1.f) {
					    wi = vwi[iwi - 1] * a[0];
					} else {
					    wi = vwi[iwi - 1];
					}
					slaln2_(&ltrans[itrans], &na, &nw, &
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
					res = (r__1 = (ca * a[0] - wr * d1) * 
						x[0] + ca * a[2] * x[1] + wi *
						 d1 * x[2] - scale * b[0], 
						dabs(r__1));
					res += (r__1 = (ca * a[0] - wr * d1) *
						 x[2] + ca * a[2] * x[3] - wi 
						* d1 * x[0] - scale * b[2], 
						dabs(r__1));
					res += (r__1 = ca * a[1] * x[0] + (ca 
						* a[3] - wr * d2) * x[1] + wi 
						* d2 * x[3] - scale * b[1], 
						dabs(r__1));
					res += (r__1 = ca * a[1] * x[2] + (ca 
						* a[3] - wr * d2) * x[3] - wi 
						* d2 * x[1] - scale * b[3], 
						dabs(r__1));
					if (info == 0) {
/* Computing MAX */
/* Computing MAX */
					    r__8 = (r__1 = ca * a[0] - wr * 
						    d1, dabs(r__1)) + (r__2 = 
						    ca * a[2], dabs(r__2)) + (
						    r__3 = wi * d1, dabs(r__3)
						    ), r__9 = (r__4 = ca * a[
						    1], dabs(r__4)) + (r__5 = 
						    ca * a[3] - wr * d2, dabs(
						    r__5)) + (r__6 = wi * d2, 
						    dabs(r__6));
/* Computing MAX */
					    r__10 = dabs(x[0]) + dabs(x[1]), 
						    r__11 = dabs(x[2]) + dabs(
						    x[3]);
					    r__7 = eps * (dmax(r__8,r__9) * 
						    dmax(r__10,r__11));
					    den = dmax(r__7,smlnum);
					} else {
/* Computing MAX */
/* Computing MAX */
/* Computing MAX */
					    r__10 = (r__1 = ca * a[0] - wr * 
						    d1, dabs(r__1)) + (r__2 = 
						    ca * a[2], dabs(r__2)) + (
						    r__3 = wi * d1, dabs(r__3)
						    ), r__11 = (r__4 = ca * a[
						    1], dabs(r__4)) + (r__5 = 
						    ca * a[3] - wr * d2, dabs(
						    r__5)) + (r__6 = wi * d2, 
						    dabs(r__6));
					    r__8 = smin / eps, r__9 = dmax(
						    r__10,r__11);
/* Computing MAX */
					    r__12 = dabs(x[0]) + dabs(x[1]), 
						    r__13 = dabs(x[2]) + dabs(
						    x[3]);
					    r__7 = eps * (dmax(r__8,r__9) * 
						    dmax(r__12,r__13));
					    den = dmax(r__7,smlnum);
					}
					res /= den;
					if (dabs(x[0]) < unfl && dabs(x[1]) < 
						unfl && dabs(x[2]) < unfl && 
						dabs(x[3]) < unfl && dabs(b[0]
						) + dabs(b[1]) <= smlnum * ((
						r__1 = ca * a[0] - wr * d1, 
						dabs(r__1)) + (r__2 = ca * a[
						2], dabs(r__2)) + (r__3 = ca *
						 a[1], dabs(r__3)) + (r__4 = 
						ca * a[3] - wr * d2, dabs(
						r__4)) + (r__5 = wi * d2, 
						dabs(r__5)) + (r__6 = wi * d1,
						 dabs(r__6)))) {
					    res = 0.f;
					}
					if (scale > 1.f) {
					    res += 1.f / eps;
					}
/* Computing MAX */
					r__2 = dabs(x[0]) + dabs(x[2]), r__3 =
						 dabs(x[1]) + dabs(x[3]);
					res += (r__1 = xnorm - dmax(r__2,r__3)
						, dabs(r__1)) / dmax(smlnum,
						xnorm) / eps;
					if (info != 0 && info != 1) {
					    res += 1.f / eps;
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

/*     End of SGET31 */

} /* sget31_ */
