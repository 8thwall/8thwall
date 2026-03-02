#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int ssvdch_(integer *n, real *s, real *e, real *svd, real *
	tol, integer *info)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    real eps;
    integer bpnt;
    real unfl, ovfl;
    integer numl, numu, tpnt, count;
    real lower, upper, tuppr;
    extern doublereal slamch_(char *);
    real unflep;
    extern /* Subroutine */ int ssvdct_(integer *, real *, real *, real *, 
	    integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SSVDCH checks to see if SVD(1) ,..., SVD(N) are accurate singular */
/*  values of the bidiagonal matrix B with diagonal entries */
/*  S(1) ,..., S(N) and superdiagonal entries E(1) ,..., E(N-1)). */
/*  It does this by expanding each SVD(I) into an interval */
/*  [SVD(I) * (1-EPS) , SVD(I) * (1+EPS)], merging overlapping intervals */
/*  if any, and using Sturm sequences to count and verify whether each */
/*  resulting interval has the correct number of singular values (using */
/*  SSVDCT). Here EPS=TOL*MAX(N/10,1)*MACHEP, where MACHEP is the */
/*  machine precision. The routine assumes the singular values are sorted */
/*  with SVD(1) the largest and SVD(N) smallest.  If each interval */
/*  contains the correct number of singular values, INFO = 0 is returned, */
/*  otherwise INFO is the index of the first singular value in the first */
/*  bad interval. */

/*  Arguments */
/*  ========== */

/*  N       (input) INTEGER */
/*          The dimension of the bidiagonal matrix B. */

/*  S       (input) REAL array, dimension (N) */
/*          The diagonal entries of the bidiagonal matrix B. */

/*  E       (input) REAL array, dimension (N-1) */
/*          The superdiagonal entries of the bidiagonal matrix B. */

/*  SVD     (input) REAL array, dimension (N) */
/*          The computed singular values to be checked. */

/*  TOL     (input) REAL */
/*          Error tolerance for checking, a multiplier of the */
/*          machine precision. */

/*  INFO    (output) INTEGER */
/*          =0 if the singular values are all correct (to within */
/*             1 +- TOL*MACHEPS) */
/*          >0 if the interval containing the INFO-th singular value */
/*             contains the incorrect number of singular values. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Get machine constants */

    /* Parameter adjustments */
    --svd;
    --e;
    --s;

    /* Function Body */
    *info = 0;
    if (*n <= 0) {
	return 0;
    }
    unfl = slamch_("Safe minimum");
    ovfl = slamch_("Overflow");
    eps = slamch_("Epsilon") * slamch_("Base");

/*     UNFLEP is chosen so that when an eigenvalue is multiplied by the */
/*     scale factor sqrt(OVFL)*sqrt(sqrt(UNFL))/MX in SSVDCT, it exceeds */
/*     sqrt(UNFL), which is the lower limit for SSVDCT. */

    unflep = sqrt(sqrt(unfl)) / sqrt(ovfl) * svd[1] + unfl / eps;

/*     The value of EPS works best when TOL .GE. 10. */

/* Computing MAX */
    i__1 = *n / 10;
    eps = *tol * max(i__1,1) * eps;

/*     TPNT points to singular value at right endpoint of interval */
/*     BPNT points to singular value at left  endpoint of interval */

    tpnt = 1;
    bpnt = 1;

/*     Begin loop over all intervals */

L10:
    upper = (eps + 1.f) * svd[tpnt] + unflep;
    lower = (1.f - eps) * svd[bpnt] - unflep;
    if (lower <= unflep) {
	lower = -upper;
    }

/*     Begin loop merging overlapping intervals */

L20:
    if (bpnt == *n) {
	goto L30;
    }
    tuppr = (eps + 1.f) * svd[bpnt + 1] + unflep;
    if (tuppr < lower) {
	goto L30;
    }

/*     Merge */

    ++bpnt;
    lower = (1.f - eps) * svd[bpnt] - unflep;
    if (lower <= unflep) {
	lower = -upper;
    }
    goto L20;
L30:

/*     Count singular values in interval [ LOWER, UPPER ] */

    ssvdct_(n, &s[1], &e[1], &lower, &numl);
    ssvdct_(n, &s[1], &e[1], &upper, &numu);
    count = numu - numl;
    if (lower < 0.f) {
	count /= 2;
    }
    if (count != bpnt - tpnt + 1) {

/*        Wrong number of singular values in interval */

	*info = tpnt;
	goto L40;
    }
    tpnt = bpnt + 1;
    bpnt = tpnt;
    if (tpnt <= *n) {
	goto L10;
    }
L40:
    return 0;

/*     End of SSVDCH */

} /* ssvdch_ */
