#include "f2c.h"
#include "blaswrap.h"

logical sisnan_(real *sin__)
{
    /* System generated locals */
    logical ret_val;

    /* Local variables */
    extern logical slaisnan_(real *, real *);


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SISNAN returns .TRUE. if its argument is NaN, and .FALSE. */
/*  otherwise.  To be replaced by the Fortran 2003 intrinsic in the */
/*  future. */

/*  Arguments */
/*  ========= */

/*  SIN      (input) REAL */
/*          Input to test for NaN. */

/*  ===================================================================== */

/*  .. External Functions .. */
/*  .. */
/*  .. Executable Statements .. */
    ret_val = slaisnan_(sin__, sin__);
    return ret_val;
} /* sisnan_ */
