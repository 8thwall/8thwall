#include "f2c.h"
#include "blaswrap.h"

doublereal dsxt1_(integer *ijob, doublereal *d1, integer *n1, doublereal *d2, 
	integer *n2, doublereal *abstol, doublereal *ulp, doublereal *unfl)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1, d__2, d__3, d__4;

    /* Local variables */
    integer i__, j;
    doublereal temp1, temp2;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSXT1  computes the difference between a set of eigenvalues. */
/*  The result is returned as the function value. */

/*  IJOB = 1:   Computes   max { min | D1(i)-D2(j) | } */
/*                          i     j */

/*  IJOB = 2:   Computes   max { min | D1(i)-D2(j) | / */
/*                          i     j */
/*                               ( ABSTOL + |D1(i)|*ULP ) } */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the type of tests to be performed.  (See above.) */

/*  D1      (input) DOUBLE PRECISION array, dimension (N1) */
/*          The first array.  D1 should be in increasing order, i.e., */
/*          D1(j) <= D1(j+1). */

/*  N1      (input) INTEGER */
/*          The length of D1. */

/*  D2      (input) DOUBLE PRECISION array, dimension (N2) */
/*          The second array.  D2 should be in increasing order, i.e., */
/*          D2(j) <= D2(j+1). */

/*  N2      (input) INTEGER */
/*          The length of D2. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute tolerance, used as a measure of the error. */

/*  ULP     (input) DOUBLE PRECISION */
/*          Machine precision. */

/*  UNFL    (input) DOUBLE PRECISION */
/*          The smallest positive number whose reciprocal does not */
/*          overflow. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --d2;
    --d1;

    /* Function Body */
    temp1 = 0.;

    j = 1;
    i__1 = *n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
L10:
	if (d2[j] < d1[i__] && j < *n2) {
	    ++j;
	    goto L10;
	}
	if (j == 1) {
	    temp2 = (d__1 = d2[j] - d1[i__], abs(d__1));
	    if (*ijob == 2) {
/* Computing MAX */
		d__2 = *unfl, d__3 = *abstol + *ulp * (d__1 = d1[i__], abs(
			d__1));
		temp2 /= max(d__2,d__3);
	    }
	} else {
/* Computing MIN */
	    d__3 = (d__1 = d2[j] - d1[i__], abs(d__1)), d__4 = (d__2 = d1[i__]
		     - d2[j - 1], abs(d__2));
	    temp2 = min(d__3,d__4);
	    if (*ijob == 2) {
/* Computing MAX */
		d__2 = *unfl, d__3 = *abstol + *ulp * (d__1 = d1[i__], abs(
			d__1));
		temp2 /= max(d__2,d__3);
	    }
	}
	temp1 = max(temp1,temp2);
/* L20: */
    }

    ret_val = temp1;
    return ret_val;

/*     End of DSXT1 */

} /* dsxt1_ */
