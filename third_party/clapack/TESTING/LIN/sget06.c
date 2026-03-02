#include "f2c.h"
#include "blaswrap.h"

doublereal sget06_(real *rcond, real *rcondc)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    real rat, eps;
    extern doublereal slamch_(char *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGET06 computes a test ratio to compare two values for RCOND. */

/*  Arguments */
/*  ========== */

/*  RCOND   (input) REAL */
/*          The estimate of the reciprocal of the condition number of A, */
/*          as computed by SGECON. */

/*  RCONDC  (input) REAL */
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

    eps = slamch_("Epsilon");
    if (*rcond > 0.f) {
	if (*rcondc > 0.f) {
	    rat = dmax(*rcond,*rcondc) / dmin(*rcond,*rcondc) - (1.f - eps);
	} else {
	    rat = *rcond / eps;
	}
    } else {
	if (*rcondc > 0.f) {
	    rat = *rcondc / eps;
	} else {
	    rat = 0.f;
	}
    }
    ret_val = rat;
    return ret_val;

/*     End of SGET06 */

} /* sget06_ */
