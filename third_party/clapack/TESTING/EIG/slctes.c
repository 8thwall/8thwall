#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b2 = 1.f;

logical slctes_(real *zr, real *zi, real *d__)
{
    /* System generated locals */
    logical ret_val;

    /* Builtin functions */
    double r_sign(real *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLCTES returns .TRUE. if the eigenvalue (ZR/D) + sqrt(-1)*(ZI/D) */
/*  is to be selected (specifically, in this subroutine, if the real */
/*  part of the eigenvalue is negative), and otherwise it returns */
/*  .FALSE.. */

/*  It is used by the test routine SDRGES to test whether the driver */
/*  routine SGGES succesfully sorts eigenvalues. */

/*  Arguments */
/*  ========= */

/*  ZR      (input) REAL */
/*          The numerator of the real part of a complex eigenvalue */
/*          (ZR/D) + i*(ZI/D). */

/*  ZI      (input) REAL */
/*          The numerator of the imaginary part of a complex eigenvalue */
/*          (ZR/D) + i*(ZI). */

/*  D       (input) REAL */
/*          The denominator part of a complex eigenvalue */
/*          (ZR/D) + i*(ZI/D). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    if (*d__ == 0.f) {
	ret_val = *zr < 0.f;
    } else {
	ret_val = r_sign(&c_b2, zr) != r_sign(&c_b2, d__);
    }

    return ret_val;

/*     End of SLCTES */

} /* slctes_ */
