#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int dptt01_(integer *n, doublereal *d__, doublereal *e, 
	doublereal *df, doublereal *ef, doublereal *work, doublereal *resid)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2, d__3, d__4, d__5;

    /* Local variables */
    integer i__;
    doublereal de, eps, anorm;
    extern doublereal dlamch_(char *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DPTT01 reconstructs a tridiagonal matrix A from its L*D*L' */
/*  factorization and computes the residual */
/*     norm(L*D*L' - A) / ( n * norm(A) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGTER */
/*          The order of the matrix A. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix A. */

/*  E       (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of the tridiagonal matrix A. */

/*  DF      (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the factor L from the L*D*L' */
/*          factorization of A. */

/*  EF      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of the factor L from the */
/*          L*D*L' factorization of A. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */

/*  RESID   (output) DOUBLE PRECISION */
/*          norm(L*D*L' - A) / (n * norm(A) * EPS) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    --work;
    --ef;
    --df;
    --e;
    --d__;

    /* Function Body */
    if (*n <= 0) {
	*resid = 0.;
	return 0;
    }

    eps = dlamch_("Epsilon");

/*     Construct the difference L*D*L' - A. */

    work[1] = df[1] - d__[1];
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	de = df[i__] * ef[i__];
	work[*n + i__] = de - e[i__];
	work[i__ + 1] = de * ef[i__] + df[i__ + 1] - d__[i__ + 1];
/* L10: */
    }

/*     Compute the 1-norms of the tridiagonal matrices A and WORK. */

    if (*n == 1) {
	anorm = d__[1];
	*resid = abs(work[1]);
    } else {
/* Computing MAX */
	d__2 = d__[1] + abs(e[1]), d__3 = d__[*n] + (d__1 = e[*n - 1], abs(
		d__1));
	anorm = max(d__2,d__3);
/* Computing MAX */
	d__4 = abs(work[1]) + (d__1 = work[*n + 1], abs(d__1)), d__5 = (d__2 =
		 work[*n], abs(d__2)) + (d__3 = work[(*n << 1) - 1], abs(d__3)
		);
	*resid = max(d__4,d__5);
	i__1 = *n - 1;
	for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__3 = anorm, d__4 = d__[i__] + (d__1 = e[i__], abs(d__1)) + (
		    d__2 = e[i__ - 1], abs(d__2));
	    anorm = max(d__3,d__4);
/* Computing MAX */
	    d__4 = *resid, d__5 = (d__1 = work[i__], abs(d__1)) + (d__2 = 
		    work[*n + i__ - 1], abs(d__2)) + (d__3 = work[*n + i__], 
		    abs(d__3));
	    *resid = max(d__4,d__5);
/* L20: */
	}
    }

/*     Compute norm(L*D*L' - A) / (n * norm(A) * EPS) */

    if (anorm <= 0.) {
	if (*resid != 0.) {
	    *resid = 1. / eps;
	}
    } else {
	*resid = *resid / (doublereal) (*n) / anorm / eps;
    }

    return 0;

/*     End of DPTT01 */

} /* dptt01_ */
