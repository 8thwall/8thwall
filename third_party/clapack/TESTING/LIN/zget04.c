#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int zget04_(integer *n, integer *nrhs, doublecomplex *x, 
	integer *ldx, doublecomplex *xact, integer *ldxact, doublereal *rcond, 
	 doublereal *resid)
{
    /* System generated locals */
    integer x_dim1, x_offset, xact_dim1, xact_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3, d__4;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    double d_imag(doublecomplex *);

    /* Local variables */
    integer i__, j, ix;
    doublereal eps, xnorm;
    extern doublereal dlamch_(char *);
    doublereal diffnm;
    extern integer izamax_(integer *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZGET04 computes the difference between a computed solution and the */
/*  true solution to a system of linear equations. */

/*  RESID =  ( norm(X-XACT) * RCOND ) / ( norm(XACT) * EPS ), */
/*  where RCOND is the reciprocal of the condition number and EPS is the */
/*  machine epsilon. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices X and XACT.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of the matrices X and XACT.  NRHS >= 0. */

/*  X       (input) COMPLEX*16 array, dimension (LDX,NRHS) */
/*          The computed solution vectors.  Each vector is stored as a */
/*          column of the matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  XACT    (input) COMPLEX*16 array, dimension (LDX,NRHS) */
/*          The exact solution vectors.  Each vector is stored as a */
/*          column of the matrix XACT. */

/*  LDXACT  (input) INTEGER */
/*          The leading dimension of the array XACT.  LDXACT >= max(1,N). */

/*  RCOND   (input) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the coefficient */
/*          matrix in the system of equations. */

/*  RESID   (output) DOUBLE PRECISION */
/*          The maximum over the NRHS solution vectors of */
/*          ( norm(X-XACT) * RCOND ) / ( norm(XACT) * EPS ) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick exit if N = 0 or NRHS = 0. */

    /* Parameter adjustments */
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    xact_dim1 = *ldxact;
    xact_offset = 1 + xact_dim1;
    xact -= xact_offset;

    /* Function Body */
    if (*n <= 0 || *nrhs <= 0) {
	*resid = 0.;
	return 0;
    }

/*     Exit with RESID = 1/EPS if RCOND is invalid. */

    eps = dlamch_("Epsilon");
    if (*rcond < 0.) {
	*resid = 1. / eps;
	return 0;
    }

/*     Compute the maximum of */
/*        norm(X - XACT) / ( norm(XACT) * EPS ) */
/*     over all the vectors X and XACT . */

    *resid = 0.;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	ix = izamax_(n, &xact[j * xact_dim1 + 1], &c__1);
	i__2 = ix + j * xact_dim1;
	xnorm = (d__1 = xact[i__2].r, abs(d__1)) + (d__2 = d_imag(&xact[ix + 
		j * xact_dim1]), abs(d__2));
	diffnm = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * x_dim1;
	    i__4 = i__ + j * xact_dim1;
	    z__2.r = x[i__3].r - xact[i__4].r, z__2.i = x[i__3].i - xact[i__4]
		    .i;
	    z__1.r = z__2.r, z__1.i = z__2.i;
/* Computing MAX */
	    d__3 = diffnm, d__4 = (d__1 = z__1.r, abs(d__1)) + (d__2 = d_imag(
		    &z__1), abs(d__2));
	    diffnm = max(d__3,d__4);
/* L10: */
	}
	if (xnorm <= 0.) {
	    if (diffnm > 0.) {
		*resid = 1. / eps;
	    }
	} else {
/* Computing MAX */
	    d__1 = *resid, d__2 = diffnm / xnorm * *rcond;
	    *resid = max(d__1,d__2);
	}
/* L20: */
    }
    if (*resid * eps < 1.) {
	*resid /= eps;
    }

    return 0;

/*     End of ZGET04 */

} /* zget04_ */
