#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b3 = 1.;

logical zlctes_(doublecomplex *z__, doublecomplex *d__)
{
    /* System generated locals */
    doublereal d__1, d__2, d__3, d__4;
    logical ret_val;

    /* Builtin functions */
    double d_imag(doublecomplex *), d_sign(doublereal *, doublereal *);

    /* Local variables */
    doublereal zmax;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLCTES returns .TRUE. if the eigenvalue Z/D is to be selected */
/*  (specifically, in this subroutine, if the real part of the */
/*  eigenvalue is negative), and otherwise it returns .FALSE.. */

/*  It is used by the test routine ZDRGES to test whether the driver */
/*  routine ZGGES succesfully sorts eigenvalues. */

/*  Arguments */
/*  ========= */

/*  Z       (input) COMPLEX*16 */
/*          The numerator part of a complex eigenvalue Z/D. */

/*  D       (input) COMPLEX*16 */
/*          The denominator part of a complex eigenvalue Z/D. */

/*  ===================================================================== */

/*     .. Parameters .. */

/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    if (d__->r == 0. && d__->i == 0.) {
	ret_val = z__->r < 0.;
    } else {
	if (z__->r == 0. || d__->r == 0.) {
	    d__1 = d_imag(z__);
	    d__2 = d_imag(d__);
	    ret_val = d_sign(&c_b3, &d__1) != d_sign(&c_b3, &d__2);
	} else if (d_imag(z__) == 0. || d_imag(d__) == 0.) {
	    d__1 = z__->r;
	    d__2 = d__->r;
	    ret_val = d_sign(&c_b3, &d__1) != d_sign(&c_b3, &d__2);
	} else {
/* Computing MAX */
	    d__3 = (d__1 = z__->r, abs(d__1)), d__4 = (d__2 = d_imag(z__), 
		    abs(d__2));
	    zmax = max(d__3,d__4);
	    ret_val = z__->r / zmax * d__->r + d_imag(z__) / zmax * d_imag(
		    d__) < 0.;
	}
    }

    return ret_val;

/*     End of ZLCTES */

} /* zlctes_ */
