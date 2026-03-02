#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int ssvdct_(integer *n, real *s, real *e, real *shift, 
	integer *num)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    integer i__;
    real u, m1, m2, mx, tmp, tom, sun, sov, unfl, ovfl, ssun;
    extern doublereal slamch_(char *);
    real sshift;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SSVDCT counts the number NUM of eigenvalues of a 2*N by 2*N */
/*  tridiagonal matrix T which are less than or equal to SHIFT.  T is */
/*  formed by putting zeros on the diagonal and making the off-diagonals */
/*  equal to S(1), E(1), S(2), E(2), ... , E(N-1), S(N).  If SHIFT is */
/*  positive, NUM is equal to N plus the number of singular values of a */
/*  bidiagonal matrix B less than or equal to SHIFT.  Here B has diagonal */
/*  entries S(1), ..., S(N) and superdiagonal entries E(1), ... E(N-1). */
/*  If SHIFT is negative, NUM is equal to the number of singular values */
/*  of B greater than or equal to -SHIFT. */

/*  See W. Kahan "Accurate Eigenvalues of a Symmetric Tridiagonal */
/*  Matrix", Report CS41, Computer Science Dept., Stanford University, */
/*  July 21, 1966 */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The dimension of the bidiagonal matrix B. */

/*  S       (input) REAL array, dimension (N) */
/*          The diagonal entries of the bidiagonal matrix B. */

/*  E       (input) REAL array of dimension (N-1) */
/*          The superdiagonal entries of the bidiagonal matrix B. */

/*  SHIFT   (input) REAL */
/*          The shift, used as described under Purpose. */

/*  NUM     (output) INTEGER */
/*          The number of eigenvalues of T less than or equal to SHIFT. */

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

/*     Get machine constants */

    /* Parameter adjustments */
    --e;
    --s;

    /* Function Body */
    unfl = slamch_("Safe minimum") * 2;
    ovfl = 1.f / unfl;

/*     Find largest entry */

    mx = dabs(s[1]);
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	r__3 = mx, r__4 = (r__1 = s[i__ + 1], dabs(r__1)), r__3 = max(r__3,
		r__4), r__4 = (r__2 = e[i__], dabs(r__2));
	mx = dmax(r__3,r__4);
/* L10: */
    }

    if (mx == 0.f) {
	if (*shift < 0.f) {
	    *num = 0;
	} else {
	    *num = *n << 1;
	}
	return 0;
    }

/*     Compute scale factors as in Kahan's report */

    sun = sqrt(unfl);
    ssun = sqrt(sun);
    sov = sqrt(ovfl);
    tom = ssun * sov;
    if (mx <= 1.f) {
	m1 = 1.f / mx;
	m2 = tom;
    } else {
	m1 = 1.f;
	m2 = tom / mx;
    }

/*     Begin counting */

    u = 1.f;
    *num = 0;
    sshift = *shift * m1 * m2;
    u = -sshift;
    if (u <= sun) {
	if (u <= 0.f) {
	    ++(*num);
	    if (u > -sun) {
		u = -sun;
	    }
	} else {
	    u = sun;
	}
    }
    tmp = s[1] * m1 * m2;
    u = -tmp * (tmp / u) - sshift;
    if (u <= sun) {
	if (u <= 0.f) {
	    ++(*num);
	    if (u > -sun) {
		u = -sun;
	    }
	} else {
	    u = sun;
	}
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	tmp = e[i__] * m1 * m2;
	u = -tmp * (tmp / u) - sshift;
	if (u <= sun) {
	    if (u <= 0.f) {
		++(*num);
		if (u > -sun) {
		    u = -sun;
		}
	    } else {
		u = sun;
	    }
	}
	tmp = s[i__ + 1] * m1 * m2;
	u = -tmp * (tmp / u) - sshift;
	if (u <= sun) {
	    if (u <= 0.f) {
		++(*num);
		if (u > -sun) {
		    u = -sun;
		}
	    } else {
		u = sun;
	    }
	}
/* L20: */
    }
    return 0;

/*     End of SSVDCT */

} /* ssvdct_ */
