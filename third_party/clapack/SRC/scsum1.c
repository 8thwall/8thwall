#include "f2c.h"
#include "blaswrap.h"

doublereal scsum1_(integer *n, complex *cx, integer *incx)
{
    /* System generated locals */
    integer i__1, i__2;
    real ret_val;

    /* Builtin functions */
    double c_abs(complex *);

    /* Local variables */
    integer i__, nincx;
    real stemp;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SCSUM1 takes the sum of the absolute values of a complex */
/*  vector and returns a single precision result. */

/*  Based on SCASUM from the Level 1 BLAS. */
/*  The change is to use the 'genuine' absolute value. */

/*  Contributed by Nick Higham for use with CLACON. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of elements in the vector CX. */

/*  CX      (input) COMPLEX array, dimension (N) */
/*          The vector whose elements will be summed. */

/*  INCX    (input) INTEGER */
/*          The spacing between successive values of CX.  INCX > 0. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --cx;

    /* Function Body */
    ret_val = 0.f;
    stemp = 0.f;
    if (*n <= 0) {
	return ret_val;
    }
    if (*incx == 1) {
	goto L20;
    }

/*     CODE FOR INCREMENT NOT EQUAL TO 1 */

    nincx = *n * *incx;
    i__1 = nincx;
    i__2 = *incx;
    for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {

/*        NEXT LINE MODIFIED. */

	stemp += c_abs(&cx[i__]);
/* L10: */
    }
    ret_val = stemp;
    return ret_val;

/*     CODE FOR INCREMENT EQUAL TO 1 */

L20:
    i__2 = *n;
    for (i__ = 1; i__ <= i__2; ++i__) {

/*        NEXT LINE MODIFIED. */

	stemp += c_abs(&cx[i__]);
/* L30: */
    }
    ret_val = stemp;
    return ret_val;

/*     End of SCSUM1 */

} /* scsum1_ */
