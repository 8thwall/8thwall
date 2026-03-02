#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int sstect_(integer *n, real *a, real *b, real *shift, 
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

/*     SSTECT counts the number NUM of eigenvalues of a tridiagonal */
/*     matrix T which are less than or equal to SHIFT. T has */
/*     diagonal entries A(1), ... , A(N), and offdiagonal entries */
/*     B(1), ..., B(N-1). */
/*     See W. Kahan "Accurate Eigenvalues of a Symmetric Tridiagonal */
/*     Matrix", Report CS41, Computer Science Dept., Stanford */
/*     University, July 21, 1966 */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The dimension of the tridiagonal matrix T. */

/*  A       (input) REAL array, dimension (N) */
/*          The diagonal entries of the tridiagonal matrix T. */

/*  B       (input) REAL array, dimension (N-1) */
/*          The offdiagonal entries of the tridiagonal matrix T. */

/*  SHIFT   (input) REAL */
/*          The shift, used as described under Purpose. */

/*  NUM     (output) INTEGER */
/*          The number of eigenvalues of T less than or equal */
/*          to SHIFT. */

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
    --b;
    --a;

    /* Function Body */
    unfl = slamch_("Safe minimum");
    ovfl = slamch_("Overflow");

/*     Find largest entry */

    mx = dabs(a[1]);
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	r__3 = mx, r__4 = (r__1 = a[i__ + 1], dabs(r__1)), r__3 = max(r__3,
		r__4), r__4 = (r__2 = b[i__], dabs(r__2));
	mx = dmax(r__3,r__4);
/* L10: */
    }

/*     Handle easy cases, including zero matrix */

    if (*shift >= mx * 3.f) {
	*num = *n;
	return 0;
    }
    if (*shift < mx * -3.f) {
	*num = 0;
	return 0;
    }

/*     Compute scale factors as in Kahan's report */
/*     At this point, MX .NE. 0 so we can divide by it */

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

    *num = 0;
    sshift = *shift * m1 * m2;
    u = a[1] * m1 * m2 - sshift;
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
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	tmp = b[i__ - 1] * m1 * m2;
	u = a[i__] * m1 * m2 - tmp * (tmp / u) - sshift;
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

/*     End of SSTECT */

} /* sstect_ */
