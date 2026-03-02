#include "f2c.h"

/* Subroutine */ int cscal_(integer *n, complex *ca, complex *cx, integer *
	incx)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    complex q__1;

    /* Local variables */
    integer i__, nincx;

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     scales a vector by a constant. */
/*     jack dongarra, linpack,  3/11/78. */
/*     modified 3/93 to return if incx .le. 0. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */


/*     .. Local Scalars .. */
/*     .. */
    /* Parameter adjustments */
    --cx;

    /* Function Body */
    if (*n <= 0 || *incx <= 0) {
	return 0;
    }
    if (*incx == 1) {
	goto L20;
    }

/*        code for increment not equal to 1 */

    nincx = *n * *incx;
    i__1 = nincx;
    i__2 = *incx;
    for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
	i__3 = i__;
	i__4 = i__;
	q__1.r = ca->r * cx[i__4].r - ca->i * cx[i__4].i, q__1.i = ca->r * cx[
		i__4].i + ca->i * cx[i__4].r;
	cx[i__3].r = q__1.r, cx[i__3].i = q__1.i;
/* L10: */
    }
    return 0;

/*        code for increment equal to 1 */

L20:
    i__2 = *n;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = i__;
	i__3 = i__;
	q__1.r = ca->r * cx[i__3].r - ca->i * cx[i__3].i, q__1.i = ca->r * cx[
		i__3].i + ca->i * cx[i__3].r;
	cx[i__1].r = q__1.r, cx[i__1].i = q__1.i;
/* L30: */
    }
    return 0;
} /* cscal_ */
