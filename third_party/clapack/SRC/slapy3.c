#include "f2c.h"
#include "blaswrap.h"

doublereal slapy3_(real *x, real *y, real *z__)
{
    /* System generated locals */
    real ret_val, r__1, r__2, r__3;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    real w, xabs, yabs, zabs;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLAPY3 returns sqrt(x**2+y**2+z**2), taking care not to cause */
/*  unnecessary overflow. */

/*  Arguments */
/*  ========= */

/*  X       (input) REAL */
/*  Y       (input) REAL */
/*  Z       (input) REAL */
/*          X, Y and Z specify the values x, y and z. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    xabs = dabs(*x);
    yabs = dabs(*y);
    zabs = dabs(*z__);
/* Computing MAX */
    r__1 = max(xabs,yabs);
    w = dmax(r__1,zabs);
    if (w == 0.f) {
/*     W can be zero for max(0,nan,0) */
/*     adding all three entries together will make sure */
/*     NaN will not disappear. */
	ret_val = xabs + yabs + zabs;
    } else {
/* Computing 2nd power */
	r__1 = xabs / w;
/* Computing 2nd power */
	r__2 = yabs / w;
/* Computing 2nd power */
	r__3 = zabs / w;
	ret_val = w * sqrt(r__1 * r__1 + r__2 * r__2 + r__3 * r__3);
    }
    return ret_val;

/*     End of SLAPY3 */

} /* slapy3_ */
