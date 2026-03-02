#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b19 = 1.;

/* Subroutine */ int dget33_(doublereal *rmax, integer *lmax, integer *ninfo, 
	integer *knt)
{
    /* System generated locals */
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *);

    /* Local variables */
    doublereal q[4]	/* was [2][2] */, t[4]	/* was [2][2] */;
    integer i1, i2, i3, i4, j1, j2, j3;
    doublereal t1[4]	/* was [2][2] */, t2[4]	/* was [2][2] */, cs, sn, vm[
	    3];
    integer im1, im2, im3, im4;
    doublereal wi1, wi2, wr1, wr2, val[4], eps, res, sum, tnrm;
    extern /* Subroutine */ int dlanv2_(doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *), dlabad_(
	    doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    doublereal bignum, smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGET33 tests DLANV2, a routine for putting 2 by 2 blocks into */
/*  standard form.  In other words, it computes a two by two rotation */
/*  [[C,S];[-S,C]] where in */

/*     [ C S ][T(1,1) T(1,2)][ C -S ] = [ T11 T12 ] */
/*     [-S C ][T(2,1) T(2,2)][ S  C ]   [ T21 T22 ] */

/*  either */
/*     1) T21=0 (real eigenvalues), or */
/*     2) T11=T22 and T21*T12<0 (complex conjugate eigenvalues). */
/*  We also  verify that the residual is small. */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) DOUBLE PRECISION */
/*          Value of the largest test ratio. */

/*  LMAX    (output) INTEGER */
/*          Example number where largest test ratio achieved. */

/*  NINFO   (output) INTEGER */
/*          Number of examples returned with INFO .NE. 0. */

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
/*     .. Executable Statements .. */

/*     Get machine parameters */

    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Set up test case parameters */

    val[0] = 1.;
    val[1] = eps * 2. + 1.;
    val[2] = 2.;
    val[3] = 2. - eps * 4.;
    vm[0] = smlnum;
    vm[1] = 1.;
    vm[2] = bignum;

    *knt = 0;
    *ninfo = 0;
    *lmax = 0;
    *rmax = 0.;

/*     Begin test loop */

    for (i1 = 1; i1 <= 4; ++i1) {
	for (i2 = 1; i2 <= 4; ++i2) {
	    for (i3 = 1; i3 <= 4; ++i3) {
		for (i4 = 1; i4 <= 4; ++i4) {
		    for (im1 = 1; im1 <= 3; ++im1) {
			for (im2 = 1; im2 <= 3; ++im2) {
			    for (im3 = 1; im3 <= 3; ++im3) {
				for (im4 = 1; im4 <= 3; ++im4) {
				    t[0] = val[i1 - 1] * vm[im1 - 1];
				    t[2] = val[i2 - 1] * vm[im2 - 1];
				    t[1] = -val[i3 - 1] * vm[im3 - 1];
				    t[3] = val[i4 - 1] * vm[im4 - 1];
/* Computing MAX */
				    d__1 = abs(t[0]), d__2 = abs(t[2]), d__1 =
					     max(d__1,d__2), d__2 = abs(t[1]),
					     d__1 = max(d__1,d__2), d__2 = 
					    abs(t[3]);
				    tnrm = max(d__1,d__2);
				    t1[0] = t[0];
				    t1[2] = t[2];
				    t1[1] = t[1];
				    t1[3] = t[3];
				    q[0] = 1.;
				    q[2] = 0.;
				    q[1] = 0.;
				    q[3] = 1.;

				    dlanv2_(t, &t[2], &t[1], &t[3], &wr1, &
					    wi1, &wr2, &wi2, &cs, &sn);
				    for (j1 = 1; j1 <= 2; ++j1) {
					res = q[j1 - 1] * cs + q[j1 + 1] * sn;
					q[j1 + 1] = -q[j1 - 1] * sn + q[j1 + 
						1] * cs;
					q[j1 - 1] = res;
/* L10: */
				    }

				    res = 0.;
/* Computing 2nd power */
				    d__2 = q[0];
/* Computing 2nd power */
				    d__3 = q[2];
				    res += (d__1 = d__2 * d__2 + d__3 * d__3 
					    - 1., abs(d__1)) / eps;
/* Computing 2nd power */
				    d__2 = q[3];
/* Computing 2nd power */
				    d__3 = q[1];
				    res += (d__1 = d__2 * d__2 + d__3 * d__3 
					    - 1., abs(d__1)) / eps;
				    res += (d__1 = q[0] * q[1] + q[2] * q[3], 
					    abs(d__1)) / eps;
				    for (j1 = 1; j1 <= 2; ++j1) {
					for (j2 = 1; j2 <= 2; ++j2) {
					    t2[j1 + (j2 << 1) - 3] = 0.;
					    for (j3 = 1; j3 <= 2; ++j3) {
			  t2[j1 + (j2 << 1) - 3] += t1[j1 + (j3 << 1) - 3] * 
				  q[j3 + (j2 << 1) - 3];
/* L20: */
					    }
/* L30: */
					}
/* L40: */
				    }
				    for (j1 = 1; j1 <= 2; ++j1) {
					for (j2 = 1; j2 <= 2; ++j2) {
					    sum = t[j1 + (j2 << 1) - 3];
					    for (j3 = 1; j3 <= 2; ++j3) {
			  sum -= q[j3 + (j1 << 1) - 3] * t2[j3 + (j2 << 1) - 
				  3];
/* L50: */
					    }
					    res += abs(sum) / eps / tnrm;
/* L60: */
					}
/* L70: */
				    }
				    if (t[1] != 0. && (t[0] != t[3] || d_sign(
					    &c_b19, &t[2]) * d_sign(&c_b19, &
					    t[1]) > 0.)) {
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

    return 0;

/*     End of DGET33 */

} /* dget33_ */
