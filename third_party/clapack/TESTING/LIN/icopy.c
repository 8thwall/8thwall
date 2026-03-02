#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int icopy_(integer *n, integer *sx, integer *incx, integer *
	sy, integer *incy)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, m, ix, iy, mp1;


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ICOPY copies an integer vector x to an integer vector y. */
/*  Uses unrolled loops for increments equal to 1. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The length of the vectors SX and SY. */

/*  SX      (input) INTEGER array, dimension (1+(N-1)*abs(INCX)) */
/*          The vector X. */

/*  INCX    (input) INTEGER */
/*          The spacing between consecutive elements of SX. */

/*  SY      (output) INTEGER array, dimension (1+(N-1)*abs(INCY)) */
/*          The vector Y. */

/*  INCY    (input) INTEGER */
/*          The spacing between consecutive elements of SY. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --sy;
    --sx;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
    if (*incx == 1 && *incy == 1) {
	goto L20;
    }

/*     Code for unequal increments or equal increments not equal to 1 */

    ix = 1;
    iy = 1;
    if (*incx < 0) {
	ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
	iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	sy[iy] = sx[ix];
	ix += *incx;
	iy += *incy;
/* L10: */
    }
    return 0;

/*     Code for both increments equal to 1 */

/*     Clean-up loop */

L20:
    m = *n % 7;
    if (m == 0) {
	goto L40;
    }
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	sy[i__] = sx[i__];
/* L30: */
    }
    if (*n < 7) {
	return 0;
    }
L40:
    mp1 = m + 1;
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 7) {
	sy[i__] = sx[i__];
	sy[i__ + 1] = sx[i__ + 1];
	sy[i__ + 2] = sx[i__ + 2];
	sy[i__ + 3] = sx[i__ + 3];
	sy[i__ + 4] = sx[i__ + 4];
	sy[i__ + 5] = sx[i__ + 5];
	sy[i__ + 6] = sx[i__ + 6];
/* L50: */
    }
    return 0;

/*     End of ICOPY */

} /* icopy_ */
