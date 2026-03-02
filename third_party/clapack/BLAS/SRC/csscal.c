#include "f2c.h"

/* Subroutine */ int csscal_(integer *n, real *sa, complex *cx, integer *incx)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1, r__2;
    complex q__1;

    /* Builtin functions */
    double r_imag(complex *);

    /* Local variables */
    integer i__, nincx;

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     scales a complex vector by a real constant. */
/*     jack dongarra, linpack, 3/11/78. */
/*     modified 3/93 to return if incx .le. 0. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */


/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
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
	r__1 = *sa * cx[i__4].r;
	r__2 = *sa * r_imag(&cx[i__]);
	q__1.r = r__1, q__1.i = r__2;
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
	r__1 = *sa * cx[i__3].r;
	r__2 = *sa * r_imag(&cx[i__]);
	q__1.r = r__1, q__1.i = r__2;
	cx[i__1].r = q__1.r, cx[i__1].i = q__1.i;
/* L30: */
    }
    return 0;
} /* csscal_ */
