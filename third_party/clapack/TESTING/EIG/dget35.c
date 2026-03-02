#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__6 = 6;
static doublereal c_b35 = 1.;

/* Subroutine */ int dget35_(doublereal *rmax, integer *lmax, integer *ninfo, 
	integer *knt)
{
    /* Initialized data */

    static integer idim[8] = { 1,2,3,4,3,3,6,4 };
    static integer ival[288]	/* was [6][6][8] */ = { 1,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,0,0,0,0,-2,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
	    0,0,5,1,2,0,0,0,-8,-2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    3,4,0,0,0,0,-5,3,0,0,0,0,1,2,1,4,0,0,-3,-9,-1,1,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,1,0,0,0,0,0,2,3,0,0,0,0,5,6,7,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,3,-4,0,0,0,2,5,2,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,1,2,0,0,0,0,-2,0,0,0,0,0,5,6,3,4,0,0,-1,
	    -9,-5,2,0,0,8,8,8,8,5,6,9,9,9,9,-7,5,1,0,0,0,0,0,1,5,2,0,0,0,2,
	    -21,5,0,0,0,1,2,3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    /* System generated locals */
    integer i__1, i__2, i__3;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    double sqrt(doublereal), sin(doublereal);

    /* Local variables */
    doublereal a[36]	/* was [6][6] */, b[36]	/* was [6][6] */, c__[36]	
	    /* was [6][6] */;
    integer i__, j, m, n;
    doublereal cc[36]	/* was [6][6] */, vm1[3], vm2[3];
    integer ima, imb;
    doublereal dum[1], eps, res, res1;
    integer info;
    doublereal cnrm;
    integer isgn;
    doublereal rmul, tnrm, xnrm, scale;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    char trana[1], tranb[1];
    integer imlda1, imlda2, imldb1;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    integer imloff, itrana, itranb;
    doublereal bignum, smlnum;
    extern /* Subroutine */ int dtrsyl_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGET35 tests DTRSYL, a routine for solving the Sylvester matrix */
/*  equation */

/*     op(A)*X + ISGN*X*op(B) = scale*C, */

/*  A and B are assumed to be in Schur canonical form, op() represents an */
/*  optional transpose, and ISGN can be -1 or +1.  Scale is an output */
/*  less than or equal to 1, chosen to avoid overflow in X. */

/*  The test code verifies that the following residual is order 1: */

/*     norm(op(A)*X + ISGN*X*op(B) - scale*C) / */
/*         (EPS*max(norm(A),norm(B))*norm(X)) */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) DOUBLE PRECISION */
/*          Value of the largest test ratio. */

/*  LMAX    (output) INTEGER */
/*          Example number where largest test ratio achieved. */

/*  NINFO   (output) INTEGER */
/*          Number of examples where INFO is nonzero. */

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

    eps = dlamch_("P");
    smlnum = dlamch_("S") * 4. / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Set up test case parameters */

    vm1[0] = sqrt(smlnum);
    vm1[1] = 1.;
    vm1[2] = sqrt(bignum);
    vm2[0] = 1.;
    vm2[1] = eps * 2. + 1.;
    vm2[2] = 2.;

    *knt = 0;
    *ninfo = 0;
    *lmax = 0;
    *rmax = 0.;

/*     Begin test loop */

    for (itrana = 1; itrana <= 2; ++itrana) {
	for (itranb = 1; itranb <= 2; ++itranb) {
	    for (isgn = -1; isgn <= 1; isgn += 2) {
		for (ima = 1; ima <= 8; ++ima) {
		    for (imlda1 = 1; imlda1 <= 3; ++imlda1) {
			for (imlda2 = 1; imlda2 <= 3; ++imlda2) {
			    for (imloff = 1; imloff <= 2; ++imloff) {
				for (imb = 1; imb <= 8; ++imb) {
				    for (imldb1 = 1; imldb1 <= 3; ++imldb1) {
					if (itrana == 1) {
					    *(unsigned char *)trana = 'N';
					}
					if (itrana == 2) {
					    *(unsigned char *)trana = 'T';
					}
					if (itranb == 1) {
					    *(unsigned char *)tranb = 'N';
					}
					if (itranb == 2) {
					    *(unsigned char *)tranb = 'T';
					}
					m = idim[ima - 1];
					n = idim[imb - 1];
					tnrm = 0.;
					i__1 = m;
					for (i__ = 1; i__ <= i__1; ++i__) {
					    i__2 = m;
					    for (j = 1; j <= i__2; ++j) {
			  a[i__ + j * 6 - 7] = (doublereal) ival[i__ + (j + 
				  ima * 6) * 6 - 43];
			  if ((i__3 = i__ - j, abs(i__3)) <= 1) {
			      a[i__ + j * 6 - 7] *= vm1[imlda1 - 1];
			      a[i__ + j * 6 - 7] *= vm2[imlda2 - 1];
			  } else {
			      a[i__ + j * 6 - 7] *= vm1[imloff - 1];
			  }
/* Computing MAX */
			  d__2 = tnrm, d__3 = (d__1 = a[i__ + j * 6 - 7], abs(
				  d__1));
			  tnrm = max(d__2,d__3);
/* L10: */
					    }
/* L20: */
					}
					i__1 = n;
					for (i__ = 1; i__ <= i__1; ++i__) {
					    i__2 = n;
					    for (j = 1; j <= i__2; ++j) {
			  b[i__ + j * 6 - 7] = (doublereal) ival[i__ + (j + 
				  imb * 6) * 6 - 43];
			  if ((i__3 = i__ - j, abs(i__3)) <= 1) {
			      b[i__ + j * 6 - 7] *= vm1[imldb1 - 1];
			  } else {
			      b[i__ + j * 6 - 7] *= vm1[imloff - 1];
			  }
/* Computing MAX */
			  d__2 = tnrm, d__3 = (d__1 = b[i__ + j * 6 - 7], abs(
				  d__1));
			  tnrm = max(d__2,d__3);
/* L30: */
					    }
/* L40: */
					}
					cnrm = 0.;
					i__1 = m;
					for (i__ = 1; i__ <= i__1; ++i__) {
					    i__2 = n;
					    for (j = 1; j <= i__2; ++j) {
			  c__[i__ + j * 6 - 7] = sin((doublereal) (i__ * j));
/* Computing MAX */
			  d__1 = cnrm, d__2 = c__[i__ + j * 6 - 7];
			  cnrm = max(d__1,d__2);
			  cc[i__ + j * 6 - 7] = c__[i__ + j * 6 - 7];
/* L50: */
					    }
/* L60: */
					}
					++(*knt);
					dtrsyl_(trana, tranb, &isgn, &m, &n, 
						a, &c__6, b, &c__6, c__, &
						c__6, &scale, &info);
					if (info != 0) {
					    ++(*ninfo);
					}
					xnrm = dlange_("M", &m, &n, c__, &
						c__6, dum);
					rmul = 1.;
					if (xnrm > 1. && tnrm > 1.) {
					    if (xnrm > bignum / tnrm) {
			  rmul = 1. / max(xnrm,tnrm);
					    }
					}
					d__1 = -scale * rmul;
					dgemm_(trana, "N", &m, &n, &m, &rmul, 
						a, &c__6, c__, &c__6, &d__1, 
						cc, &c__6);
					d__1 = (doublereal) isgn * rmul;
					dgemm_("N", tranb, &m, &n, &n, &d__1, 
						c__, &c__6, b, &c__6, &c_b35, 
						cc, &c__6);
					res1 = dlange_("M", &m, &n, cc, &c__6, 
						 dum);
/* Computing MAX */
					d__1 = smlnum, d__2 = smlnum * xnrm, 
						d__1 = max(d__1,d__2), d__2 = 
						rmul * tnrm * eps * xnrm;
					res = res1 / max(d__1,d__2);
					if (res > *rmax) {
					    *lmax = *knt;
					    *rmax = res;
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
/* L120: */
		}
/* L130: */
	    }
/* L140: */
	}
/* L150: */
    }

    return 0;

/*     End of DGET35 */

} /* dget35_ */
