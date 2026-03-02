#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int sstech_(integer *n, real *a, real *b, real *eig, real *
	tol, real *work, integer *info)
{
    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2, r__3;

    /* Local variables */
    integer i__, j;
    real mx, eps, emin;
    integer isub, bpnt, numl, numu, tpnt, count;
    real lower, upper, tuppr;
    extern doublereal slamch_(char *);
    real unflep;
    extern /* Subroutine */ int sstect_(integer *, real *, real *, real *, 
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

/*     Let T be the tridiagonal matrix with diagonal entries A(1) ,..., */
/*     A(N) and offdiagonal entries B(1) ,..., B(N-1)).  SSTECH checks to */
/*     see if EIG(1) ,..., EIG(N) are indeed accurate eigenvalues of T. */
/*     It does this by expanding each EIG(I) into an interval */
/*     [SVD(I) - EPS, SVD(I) + EPS], merging overlapping intervals if */
/*     any, and using Sturm sequences to count and verify whether each */
/*     resulting interval has the correct number of eigenvalues (using */
/*     SSTECT).  Here EPS = TOL*MACHEPS*MAXEIG, where MACHEPS is the */
/*     machine precision and MAXEIG is the absolute value of the largest */
/*     eigenvalue. If each interval contains the correct number of */
/*     eigenvalues, INFO = 0 is returned, otherwise INFO is the index of */
/*     the first eigenvalue in the first bad interval. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The dimension of the tridiagonal matrix T. */

/*  A       (input) REAL array, dimension (N) */
/*          The diagonal entries of the tridiagonal matrix T. */

/*  B       (input) REAL array, dimension (N-1) */
/*          The offdiagonal entries of the tridiagonal matrix T. */

/*  EIG     (input) REAL array, dimension (N) */
/*          The purported eigenvalues to be checked. */

/*  TOL     (input) REAL */
/*          Error tolerance for checking, a multiple of the */
/*          machine precision. */

/*  WORK    (workspace) REAL array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          0  if the eigenvalues are all correct (to within */
/*             1 +- TOL*MACHEPS*MAXEIG) */
/*          >0 if the interval containing the INFO-th eigenvalue */
/*             contains the incorrect number of eigenvalues. */

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

/*     Check input parameters */

    /* Parameter adjustments */
    --work;
    --eig;
    --b;
    --a;

    /* Function Body */
    *info = 0;
    if (*n == 0) {
	return 0;
    }
    if (*n < 0) {
	*info = -1;
	return 0;
    }
    if (*tol < 0.f) {
	*info = -5;
	return 0;
    }

/*     Get machine constants */

    eps = slamch_("Epsilon") * slamch_("Base");
    unflep = slamch_("Safe minimum") / eps;
    eps = *tol * eps;

/*     Compute maximum absolute eigenvalue, error tolerance */

    mx = dabs(eig[1]);
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MAX */
	r__2 = mx, r__3 = (r__1 = eig[i__], dabs(r__1));
	mx = dmax(r__2,r__3);
/* L10: */
    }
/* Computing MAX */
    r__1 = eps * mx;
    eps = dmax(r__1,unflep);

/*     Sort eigenvalues from EIG into WORK */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	work[i__] = eig[i__];
/* L20: */
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	isub = 1;
	emin = work[1];
	i__2 = *n + 1 - i__;
	for (j = 2; j <= i__2; ++j) {
	    if (work[j] < emin) {
		isub = j;
		emin = work[j];
	    }
/* L30: */
	}
	if (isub != *n + 1 - i__) {
	    work[isub] = work[*n + 1 - i__];
	    work[*n + 1 - i__] = emin;
	}
/* L40: */
    }

/*     TPNT points to singular value at right endpoint of interval */
/*     BPNT points to singular value at left  endpoint of interval */

    tpnt = 1;
    bpnt = 1;

/*     Begin loop over all intervals */

L50:
    upper = work[tpnt] + eps;
    lower = work[bpnt] - eps;

/*     Begin loop merging overlapping intervals */

L60:
    if (bpnt == *n) {
	goto L70;
    }
    tuppr = work[bpnt + 1] + eps;
    if (tuppr < lower) {
	goto L70;
    }

/*     Merge */

    ++bpnt;
    lower = work[bpnt] - eps;
    goto L60;
L70:

/*     Count singular values in interval [ LOWER, UPPER ] */

    sstect_(n, &a[1], &b[1], &lower, &numl);
    sstect_(n, &a[1], &b[1], &upper, &numu);
    count = numu - numl;
    if (count != bpnt - tpnt + 1) {

/*        Wrong number of singular values in interval */

	*info = tpnt;
	goto L80;
    }
    tpnt = bpnt + 1;
    bpnt = tpnt;
    if (tpnt <= *n) {
	goto L50;
    }
L80:
    return 0;

/*     End of SSTECH */

} /* sstech_ */
