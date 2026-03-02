#include "f2c.h"
#include "blaswrap.h"

doublereal ssxt1_(integer *ijob, real *d1, integer *n1, real *d2, integer *n2, 
	 real *abstol, real *ulp, real *unfl)
{
    /* System generated locals */
    integer i__1;
    real ret_val, r__1, r__2, r__3, r__4;

    /* Local variables */
    integer i__, j;
    real temp1, temp2;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SSXT1  computes the difference between a set of eigenvalues. */
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

/*  D1      (input) REAL array, dimension (N1) */
/*          The first array.  D1 should be in increasing order, i.e., */
/*          D1(j) <= D1(j+1). */

/*  N1      (input) INTEGER */
/*          The length of D1. */

/*  D2      (input) REAL array, dimension (N2) */
/*          The second array.  D2 should be in increasing order, i.e., */
/*          D2(j) <= D2(j+1). */

/*  N2      (input) INTEGER */
/*          The length of D2. */

/*  ABSTOL  (input) REAL */
/*          The absolute tolerance, used as a measure of the error. */

/*  ULP     (input) REAL */
/*          Machine precision. */

/*  UNFL    (input) REAL */
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
    temp1 = 0.f;

    j = 1;
    i__1 = *n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
L10:
	if (d2[j] < d1[i__] && j < *n2) {
	    ++j;
	    goto L10;
	}
	if (j == 1) {
	    temp2 = (r__1 = d2[j] - d1[i__], dabs(r__1));
	    if (*ijob == 2) {
/* Computing MAX */
		r__2 = *unfl, r__3 = *abstol + *ulp * (r__1 = d1[i__], dabs(
			r__1));
		temp2 /= dmax(r__2,r__3);
	    }
	} else {
/* Computing MIN */
	    r__3 = (r__1 = d2[j] - d1[i__], dabs(r__1)), r__4 = (r__2 = d1[
		    i__] - d2[j - 1], dabs(r__2));
	    temp2 = dmin(r__3,r__4);
	    if (*ijob == 2) {
/* Computing MAX */
		r__2 = *unfl, r__3 = *abstol + *ulp * (r__1 = d1[i__], dabs(
			r__1));
		temp2 /= dmax(r__2,r__3);
	    }
	}
	temp1 = dmax(temp1,temp2);
/* L20: */
    }

    ret_val = temp1;
    return ret_val;

/*     End of SSXT1 */

} /* ssxt1_ */
