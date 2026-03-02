#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b19 = 1.f;

/* Subroutine */ int sget33_(real *rmax, integer *lmax, integer *ninfo, 
	integer *knt)
{
    /* System generated locals */
    real r__1, r__2, r__3;

    /* Builtin functions */
    double r_sign(real *, real *);

    /* Local variables */
    real q[4]	/* was [2][2] */, t[4]	/* was [2][2] */;
    integer i1, i2, i3, i4, j1, j2, j3;
    real t1[4]	/* was [2][2] */, t2[4]	/* was [2][2] */, cs, sn, vm[3];
    integer im1, im2, im3, im4;
    real wi1, wi2, wr1, wr2, val[4], eps, res, sum, tnrm;
    extern /* Subroutine */ int slanv2_(real *, real *, real *, real *, real *
, real *, real *, real *, real *, real *), slabad_(real *, real *)
	    ;
    extern doublereal slamch_(char *);
    real bignum, smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGET33 tests SLANV2, a routine for putting 2 by 2 blocks into */
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

/*  RMAX    (output) REAL */
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

    eps = slamch_("P");
    smlnum = slamch_("S") / eps;
    bignum = 1.f / smlnum;
    slabad_(&smlnum, &bignum);

/*     Set up test case parameters */

    val[0] = 1.f;
    val[1] = eps * 2.f + 1.f;
    val[2] = 2.f;
    val[3] = 2.f - eps * 4.f;
    vm[0] = smlnum;
    vm[1] = 1.f;
    vm[2] = bignum;

    *knt = 0;
    *ninfo = 0;
    *lmax = 0;
    *rmax = 0.f;

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
				    r__1 = dabs(t[0]), r__2 = dabs(t[2]), 
					    r__1 = max(r__1,r__2), r__2 = 
					    dabs(t[1]), r__1 = max(r__1,r__2),
					     r__2 = dabs(t[3]);
				    tnrm = dmax(r__1,r__2);
				    t1[0] = t[0];
				    t1[2] = t[2];
				    t1[1] = t[1];
				    t1[3] = t[3];
				    q[0] = 1.f;
				    q[2] = 0.f;
				    q[1] = 0.f;
				    q[3] = 1.f;

				    slanv2_(t, &t[2], &t[1], &t[3], &wr1, &
					    wi1, &wr2, &wi2, &cs, &sn);
				    for (j1 = 1; j1 <= 2; ++j1) {
					res = q[j1 - 1] * cs + q[j1 + 1] * sn;
					q[j1 + 1] = -q[j1 - 1] * sn + q[j1 + 
						1] * cs;
					q[j1 - 1] = res;
/* L10: */
				    }

				    res = 0.f;
/* Computing 2nd power */
				    r__2 = q[0];
/* Computing 2nd power */
				    r__3 = q[2];
				    res += (r__1 = r__2 * r__2 + r__3 * r__3 
					    - 1.f, dabs(r__1)) / eps;
/* Computing 2nd power */
				    r__2 = q[3];
/* Computing 2nd power */
				    r__3 = q[1];
				    res += (r__1 = r__2 * r__2 + r__3 * r__3 
					    - 1.f, dabs(r__1)) / eps;
				    res += (r__1 = q[0] * q[1] + q[2] * q[3], 
					    dabs(r__1)) / eps;
				    for (j1 = 1; j1 <= 2; ++j1) {
					for (j2 = 1; j2 <= 2; ++j2) {
					    t2[j1 + (j2 << 1) - 3] = 0.f;
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
					    res += dabs(sum) / eps / tnrm;
/* L60: */
					}
/* L70: */
				    }
				    if (t[1] != 0.f && (t[0] != t[3] || 
					    r_sign(&c_b19, &t[2]) * r_sign(&
					    c_b19, &t[1]) > 0.f)) {
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

/*     End of SGET33 */

} /* sget33_ */
