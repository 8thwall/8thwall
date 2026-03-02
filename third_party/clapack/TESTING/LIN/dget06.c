#include "f2c.h"
#include "blaswrap.h"

doublereal dget06_(doublereal *rcond, doublereal *rcondc)
{
    /* System generated locals */
    doublereal ret_val;

    /* Local variables */
    doublereal rat, eps;
    extern doublereal dlamch_(char *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGET06 computes a test ratio to compare two values for RCOND. */

/*  Arguments */
/*  ========== */

/*  RCOND   (input) DOUBLE PRECISION */
/*          The estimate of the reciprocal of the condition number of A, */
/*          as computed by DGECON. */

/*  RCONDC  (input) DOUBLE PRECISION */
/*          The reciprocal of the condition number of A, computed as */
/*          ( 1/norm(A) ) / norm(inv(A)). */

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

    eps = dlamch_("Epsilon");
    if (*rcond > 0.) {
	if (*rcondc > 0.) {
	    rat = max(*rcond,*rcondc) / min(*rcond,*rcondc) - (1. - eps);
	} else {
	    rat = *rcond / eps;
	}
    } else {
	if (*rcondc > 0.) {
	    rat = *rcondc / eps;
	} else {
	    rat = 0.;
	}
    }
    ret_val = rat;
    return ret_val;

/*     End of DGET06 */

} /* dget06_ */
