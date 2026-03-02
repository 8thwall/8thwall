#include "f2c.h"

/* Table of constant values */

static real c_b4 = 1.f;

/* Subroutine */ int srotg_(real *sa, real *sb, real *c__, real *s)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal), r_sign(real *, real *);

    /* Local variables */
    real r__, z__, roe, scale;

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     construct givens plane rotation. */
/*     jack dongarra, linpack, 3/11/78. */


/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
    roe = *sb;
    if (dabs(*sa) > dabs(*sb)) {
	roe = *sa;
    }
    scale = dabs(*sa) + dabs(*sb);
    if (scale != 0.f) {
	goto L10;
    }
    *c__ = 1.f;
    *s = 0.f;
    r__ = 0.f;
    z__ = 0.f;
    goto L20;
L10:
/* Computing 2nd power */
    r__1 = *sa / scale;
/* Computing 2nd power */
    r__2 = *sb / scale;
    r__ = scale * sqrt(r__1 * r__1 + r__2 * r__2);
    r__ = r_sign(&c_b4, &roe) * r__;
    *c__ = *sa / r__;
    *s = *sb / r__;
    z__ = 1.f;
    if (dabs(*sa) > dabs(*sb)) {
	z__ = *s;
    }
    if (dabs(*sb) >= dabs(*sa) && *c__ != 0.f) {
	z__ = 1.f / *c__;
    }
L20:
    *sa = r__;
    *sb = z__;
    return 0;
} /* srotg_ */
