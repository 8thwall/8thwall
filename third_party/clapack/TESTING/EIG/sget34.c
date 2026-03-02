#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__16 = 16;
static integer c__0 = 0;
static integer c__1 = 1;
static integer c__4 = 4;
static integer c__5 = 5;
static logical c_true = TRUE_;
static integer c__2 = 2;
static integer c__32 = 32;
static integer c__3 = 3;
static real c_b64 = 1.f;

/* Subroutine */ int sget34_(real *rmax, integer *lmax, integer *ninfo, 
	integer *knt)
{
    /* System generated locals */
    real r__1, r__2, r__3;

    /* Builtin functions */
    double sqrt(doublereal), r_sign(real *, real *);

    /* Local variables */
    integer i__, j;
    real q[16]	/* was [4][4] */, t[16]	/* was [4][4] */, t1[16]	/* 
	    was [4][4] */;
    integer ia, ib, ic;
    real vm[2];
    integer ia11, ia12, ia21, ia22, ic11, ic12, ic21, ic22, iam, icm;
    real val[9], eps, res;
    integer info;
    real tnrm, work[32];
    extern /* Subroutine */ int shst01_(integer *, integer *, integer *, real 
	    *, integer *, real *, integer *, real *, integer *, real *, 
	    integer *, real *), scopy_(integer *, real *, integer *, real *, 
	    integer *), slabad_(real *, real *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int slaexc_(logical *, integer *, real *, integer 
	    *, real *, integer *, integer *, integer *, integer *, real *, 
	    integer *);
    real bignum, smlnum, result[2];


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGET34 tests SLAEXC, a routine for swapping adjacent blocks (either */
/*  1 by 1 or 2 by 2) on the diagonal of a matrix in real Schur form. */
/*  Thus, SLAEXC computes an orthogonal matrix Q such that */

/*      Q' * [ A B ] * Q  = [ C1 B1 ] */
/*           [ 0 C ]        [ 0  A1 ] */

/*  where C1 is similar to C and A1 is similar to A.  Both A and C are */
/*  assumed to be in standard form (equal diagonal entries and */
/*  offdiagonal with differing signs) and A1 and C1 are returned with the */
/*  same properties. */

/*  The test code verifies these last last assertions, as well as that */
/*  the residual in the above equation is small. */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) REAL */
/*          Value of the largest test ratio. */

/*  LMAX    (output) INTEGER */
/*          Example number where largest test ratio achieved. */

/*  NINFO   (output) INTEGER array, dimension (2) */
/*          NINFO(J) is the number of examples where INFO=J occurred. */

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

    /* Parameter adjustments */
    --ninfo;

    /* Function Body */
    eps = slamch_("P");
    smlnum = slamch_("S") / eps;
    bignum = 1.f / smlnum;
    slabad_(&smlnum, &bignum);

/*     Set up test case parameters */

    val[0] = 0.f;
    val[1] = sqrt(smlnum);
    val[2] = 1.f;
    val[3] = 2.f;
    val[4] = sqrt(bignum);
    val[5] = -sqrt(smlnum);
    val[6] = -1.f;
    val[7] = -2.f;
    val[8] = -sqrt(bignum);
    vm[0] = 1.f;
    vm[1] = eps * 2.f + 1.f;
    scopy_(&c__16, &val[3], &c__0, t, &c__1);

    ninfo[1] = 0;
    ninfo[2] = 0;
    *knt = 0;
    *lmax = 0;
    *rmax = 0.f;

/*     Begin test loop */

    for (ia = 1; ia <= 9; ++ia) {
	for (iam = 1; iam <= 2; ++iam) {
	    for (ib = 1; ib <= 9; ++ib) {
		for (ic = 1; ic <= 9; ++ic) {
		    t[0] = val[ia - 1] * vm[iam - 1];
		    t[5] = val[ic - 1];
		    t[4] = val[ib - 1];
		    t[1] = 0.f;
/* Computing MAX */
		    r__1 = dabs(t[0]), r__2 = dabs(t[5]), r__1 = max(r__1,
			    r__2), r__2 = dabs(t[4]);
		    tnrm = dmax(r__1,r__2);
		    scopy_(&c__16, t, &c__1, t1, &c__1);
		    scopy_(&c__16, val, &c__0, q, &c__1);
		    scopy_(&c__4, &val[2], &c__0, q, &c__5);
		    slaexc_(&c_true, &c__2, t, &c__4, q, &c__4, &c__1, &c__1, 
			    &c__1, work, &info);
		    if (info != 0) {
			++ninfo[info];
		    }
		    shst01_(&c__2, &c__1, &c__2, t1, &c__4, t, &c__4, q, &
			    c__4, work, &c__32, result);
		    res = result[0] + result[1];
		    if (info != 0) {
			res += 1.f / eps;
		    }
		    if (t[0] != t1[5]) {
			res += 1.f / eps;
		    }
		    if (t[5] != t1[0]) {
			res += 1.f / eps;
		    }
		    if (t[1] != 0.f) {
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
/* L40: */
    }

    for (ia = 1; ia <= 5; ++ia) {
	for (iam = 1; iam <= 2; ++iam) {
	    for (ib = 1; ib <= 5; ++ib) {
		for (ic11 = 1; ic11 <= 5; ++ic11) {
		    for (ic12 = 2; ic12 <= 5; ++ic12) {
			for (ic21 = 2; ic21 <= 4; ++ic21) {
			    for (ic22 = -1; ic22 <= 1; ic22 += 2) {
				t[0] = val[ia - 1] * vm[iam - 1];
				t[4] = val[ib - 1];
				t[8] = val[ib - 1] * -2.f;
				t[1] = 0.f;
				t[5] = val[ic11 - 1];
				t[9] = val[ic12 - 1];
				t[2] = 0.f;
				t[6] = -val[ic21 - 1];
				t[10] = val[ic11 - 1] * (real) ic22;
/* Computing MAX */
				r__1 = dabs(t[0]), r__2 = dabs(t[4]), r__1 = 
					max(r__1,r__2), r__2 = dabs(t[8]), 
					r__1 = max(r__1,r__2), r__2 = dabs(t[
					5]), r__1 = max(r__1,r__2), r__2 = 
					dabs(t[9]), r__1 = max(r__1,r__2), 
					r__2 = dabs(t[6]), r__1 = max(r__1,
					r__2), r__2 = dabs(t[10]);
				tnrm = dmax(r__1,r__2);
				scopy_(&c__16, t, &c__1, t1, &c__1);
				scopy_(&c__16, val, &c__0, q, &c__1);
				scopy_(&c__4, &val[2], &c__0, q, &c__5);
				slaexc_(&c_true, &c__3, t, &c__4, q, &c__4, &
					c__1, &c__1, &c__2, work, &info);
				if (info != 0) {
				    ++ninfo[info];
				}
				shst01_(&c__3, &c__1, &c__3, t1, &c__4, t, &
					c__4, q, &c__4, work, &c__32, result);
				res = result[0] + result[1];
				if (info == 0) {
				    if (t1[0] != t[10]) {
					res += 1.f / eps;
				    }
				    if (t[2] != 0.f) {
					res += 1.f / eps;
				    }
				    if (t[6] != 0.f) {
					res += 1.f / eps;
				    }
				    if (t[1] != 0.f && (t[0] != t[5] || 
					    r_sign(&c_b64, &t[4]) == r_sign(&
					    c_b64, &t[1]))) {
					res += 1.f / eps;
				    }
				}
				++(*knt);
				if (res > *rmax) {
				    *lmax = *knt;
				    *rmax = res;
				}
/* L50: */
			    }
/* L60: */
			}
/* L70: */
		    }
/* L80: */
		}
/* L90: */
	    }
/* L100: */
	}
/* L110: */
    }

    for (ia11 = 1; ia11 <= 5; ++ia11) {
	for (ia12 = 2; ia12 <= 5; ++ia12) {
	    for (ia21 = 2; ia21 <= 4; ++ia21) {
		for (ia22 = -1; ia22 <= 1; ia22 += 2) {
		    for (icm = 1; icm <= 2; ++icm) {
			for (ib = 1; ib <= 5; ++ib) {
			    for (ic = 1; ic <= 5; ++ic) {
				t[0] = val[ia11 - 1];
				t[4] = val[ia12 - 1];
				t[8] = val[ib - 1] * -2.f;
				t[1] = -val[ia21 - 1];
				t[5] = val[ia11 - 1] * (real) ia22;
				t[9] = val[ib - 1];
				t[2] = 0.f;
				t[6] = 0.f;
				t[10] = val[ic - 1] * vm[icm - 1];
/* Computing MAX */
				r__1 = dabs(t[0]), r__2 = dabs(t[4]), r__1 = 
					max(r__1,r__2), r__2 = dabs(t[8]), 
					r__1 = max(r__1,r__2), r__2 = dabs(t[
					5]), r__1 = max(r__1,r__2), r__2 = 
					dabs(t[9]), r__1 = max(r__1,r__2), 
					r__2 = dabs(t[6]), r__1 = max(r__1,
					r__2), r__2 = dabs(t[10]);
				tnrm = dmax(r__1,r__2);
				scopy_(&c__16, t, &c__1, t1, &c__1);
				scopy_(&c__16, val, &c__0, q, &c__1);
				scopy_(&c__4, &val[2], &c__0, q, &c__5);
				slaexc_(&c_true, &c__3, t, &c__4, q, &c__4, &
					c__1, &c__2, &c__1, work, &info);
				if (info != 0) {
				    ++ninfo[info];
				}
				shst01_(&c__3, &c__1, &c__3, t1, &c__4, t, &
					c__4, q, &c__4, work, &c__32, result);
				res = result[0] + result[1];
				if (info == 0) {
				    if (t1[10] != t[0]) {
					res += 1.f / eps;
				    }
				    if (t[1] != 0.f) {
					res += 1.f / eps;
				    }
				    if (t[2] != 0.f) {
					res += 1.f / eps;
				    }
				    if (t[6] != 0.f && (t[5] != t[10] || 
					    r_sign(&c_b64, &t[9]) == r_sign(&
					    c_b64, &t[6]))) {
					res += 1.f / eps;
				    }
				}
				++(*knt);
				if (res > *rmax) {
				    *lmax = *knt;
				    *rmax = res;
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

    for (ia11 = 1; ia11 <= 5; ++ia11) {
	for (ia12 = 2; ia12 <= 5; ++ia12) {
	    for (ia21 = 2; ia21 <= 4; ++ia21) {
		for (ia22 = -1; ia22 <= 1; ia22 += 2) {
		    for (ib = 1; ib <= 5; ++ib) {
			for (ic11 = 3; ic11 <= 4; ++ic11) {
			    for (ic12 = 3; ic12 <= 4; ++ic12) {
				for (ic21 = 3; ic21 <= 4; ++ic21) {
				    for (ic22 = -1; ic22 <= 1; ic22 += 2) {
					for (icm = 5; icm <= 7; ++icm) {
					    iam = 1;
					    t[0] = val[ia11 - 1] * vm[iam - 1]
						    ;
					    t[4] = val[ia12 - 1] * vm[iam - 1]
						    ;
					    t[8] = val[ib - 1] * -2.f;
					    t[12] = val[ib - 1] * .5f;
					    t[1] = -t[4] * val[ia21 - 1];
					    t[5] = val[ia11 - 1] * (real) 
						    ia22 * vm[iam - 1];
					    t[9] = val[ib - 1];
					    t[13] = val[ib - 1] * 3.f;
					    t[2] = 0.f;
					    t[6] = 0.f;
					    t[10] = val[ic11 - 1] * (r__1 = 
						    val[icm - 1], dabs(r__1));
					    t[14] = val[ic12 - 1] * (r__1 = 
						    val[icm - 1], dabs(r__1));
					    t[3] = 0.f;
					    t[7] = 0.f;
					    t[11] = -t[14] * val[ic21 - 1] * (
						    r__1 = val[icm - 1], dabs(
						    r__1));
					    t[15] = val[ic11 - 1] * (real) 
						    ic22 * (r__1 = val[icm - 
						    1], dabs(r__1));
					    tnrm = 0.f;
					    for (i__ = 1; i__ <= 4; ++i__) {
			  for (j = 1; j <= 4; ++j) {
/* Computing MAX */
			      r__2 = tnrm, r__3 = (r__1 = t[i__ + (j << 2) - 
				      5], dabs(r__1));
			      tnrm = dmax(r__2,r__3);
/* L190: */
			  }
/* L200: */
					    }
					    scopy_(&c__16, t, &c__1, t1, &
						    c__1);
					    scopy_(&c__16, val, &c__0, q, &
						    c__1);
					    scopy_(&c__4, &val[2], &c__0, q, &
						    c__5);
					    slaexc_(&c_true, &c__4, t, &c__4, 
						    q, &c__4, &c__1, &c__2, &
						    c__2, work, &info);
					    if (info != 0) {
			  ++ninfo[info];
					    }
					    shst01_(&c__4, &c__1, &c__4, t1, &
						    c__4, t, &c__4, q, &c__4, 
						    work, &c__32, result);
					    res = result[0] + result[1];
					    if (info == 0) {
			  if (t[2] != 0.f) {
			      res += 1.f / eps;
			  }
			  if (t[3] != 0.f) {
			      res += 1.f / eps;
			  }
			  if (t[6] != 0.f) {
			      res += 1.f / eps;
			  }
			  if (t[7] != 0.f) {
			      res += 1.f / eps;
			  }
			  if (t[1] != 0.f && (t[0] != t[5] || r_sign(&c_b64, &
				  t[4]) == r_sign(&c_b64, &t[1]))) {
			      res += 1.f / eps;
			  }
			  if (t[11] != 0.f && (t[10] != t[15] || r_sign(&
				  c_b64, &t[14]) == r_sign(&c_b64, &t[11]))) {
			      res += 1.f / eps;
			  }
					    }
					    ++(*knt);
					    if (res > *rmax) {
			  *lmax = *knt;
			  *rmax = res;
					    }
/* L210: */
					}
/* L220: */
				    }
/* L230: */
				}
/* L240: */
			    }
/* L250: */
			}
/* L260: */
		    }
/* L270: */
		}
/* L280: */
	    }
/* L290: */
	}
/* L300: */
    }

    return 0;

/*     End of SGET34 */

} /* sget34_ */
