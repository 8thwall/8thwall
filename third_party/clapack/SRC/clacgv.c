#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int clacgv_(integer *n, complex *x, integer *incx)
{
    /* System generated locals */
    integer i__1, i__2;
    complex q__1;

    /* Builtin functions */
    void r_cnjg(complex *, complex *);

    /* Local variables */
    integer i__, ioff;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLACGV conjugates a complex vector of length N. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The length of the vector X.  N >= 0. */

/*  X       (input/output) COMPLEX array, dimension */
/*                         (1+(N-1)*abs(INCX)) */
/*          On entry, the vector of length N to be conjugated. */
/*          On exit, X is overwritten with conjg(X). */

/*  INCX    (input) INTEGER */
/*          The spacing between successive elements of X. */

/* ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*incx == 1) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    r_cnjg(&q__1, &x[i__]);
	    x[i__2].r = q__1.r, x[i__2].i = q__1.i;
/* L10: */
	}
    } else {
	ioff = 1;
	if (*incx < 0) {
	    ioff = 1 - (*n - 1) * *incx;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = ioff;
	    r_cnjg(&q__1, &x[ioff]);
	    x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    ioff += *incx;
/* L20: */
	}
    }
    return 0;

/*     End of CLACGV */

} /* clacgv_ */
