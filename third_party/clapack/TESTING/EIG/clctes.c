#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b3 = 1.f;

logical clctes_(complex *z__, complex *d__)
{
    /* System generated locals */
    real r__1, r__2, r__3, r__4;
    logical ret_val;

    /* Builtin functions */
    double r_imag(complex *), r_sign(real *, real *);

    /* Local variables */
    real zmax;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLCTES returns .TRUE. if the eigenvalue Z/D is to be selected */
/*  (specifically, in this subroutine, if the real part of the */
/*  eigenvalue is negative), and otherwise it returns .FALSE.. */

/*  It is used by the test routine CDRGES to test whether the driver */
/*  routine CGGES succesfully sorts eigenvalues. */

/*  Arguments */
/*  ========= */

/*  Z       (input) COMPLEX */
/*          The numerator part of a complex eigenvalue Z/D. */

/*  D       (input) COMPLEX */
/*          The denominator part of a complex eigenvalue Z/D. */

/*  ===================================================================== */

/*     .. Parameters .. */

/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    if (d__->r == 0.f && d__->i == 0.f) {
	ret_val = z__->r < 0.f;
    } else {
	if (z__->r == 0.f || d__->r == 0.f) {
	    r__1 = r_imag(z__);
	    r__2 = r_imag(d__);
	    ret_val = r_sign(&c_b3, &r__1) != r_sign(&c_b3, &r__2);
	} else if (r_imag(z__) == 0.f || r_imag(d__) == 0.f) {
	    r__1 = z__->r;
	    r__2 = d__->r;
	    ret_val = r_sign(&c_b3, &r__1) != r_sign(&c_b3, &r__2);
	} else {
/* Computing MAX */
	    r__3 = (r__1 = z__->r, dabs(r__1)), r__4 = (r__2 = r_imag(z__), 
		    dabs(r__2));
	    zmax = dmax(r__3,r__4);
	    ret_val = z__->r / zmax * d__->r + r_imag(z__) / zmax * r_imag(
		    d__) < 0.f;
	}
    }

    return ret_val;

/*     End of CLCTES */

} /* clctes_ */
