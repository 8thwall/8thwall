#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int cget04_(integer *n, integer *nrhs, complex *x, integer *
	ldx, complex *xact, integer *ldxact, real *rcond, real *resid)
{
    /* System generated locals */
    integer x_dim1, x_offset, xact_dim1, xact_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2, r__3, r__4;
    complex q__1, q__2;

    /* Builtin functions */
    double r_imag(complex *);

    /* Local variables */
    integer i__, j, ix;
    real eps, xnorm;
    extern integer icamax_(integer *, complex *, integer *);
    real diffnm;
    extern doublereal slamch_(char *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CGET04 computes the difference between a computed solution and the */
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

/*  X       (input) COMPLEX array, dimension (LDX,NRHS) */
/*          The computed solution vectors.  Each vector is stored as a */
/*          column of the matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  XACT    (input) COMPLEX array, dimension (LDX,NRHS) */
/*          The exact solution vectors.  Each vector is stored as a */
/*          column of the matrix XACT. */

/*  LDXACT  (input) INTEGER */
/*          The leading dimension of the array XACT.  LDXACT >= max(1,N). */

/*  RCOND   (input) REAL */
/*          The reciprocal of the condition number of the coefficient */
/*          matrix in the system of equations. */

/*  RESID   (output) REAL */
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
	*resid = 0.f;
	return 0;
    }

/*     Exit with RESID = 1/EPS if RCOND is invalid. */

    eps = slamch_("Epsilon");
    if (*rcond < 0.f) {
	*resid = 1.f / eps;
	return 0;
    }

/*     Compute the maximum of */
/*        norm(X - XACT) / ( norm(XACT) * EPS ) */
/*     over all the vectors X and XACT . */

    *resid = 0.f;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	ix = icamax_(n, &xact[j * xact_dim1 + 1], &c__1);
	i__2 = ix + j * xact_dim1;
	xnorm = (r__1 = xact[i__2].r, dabs(r__1)) + (r__2 = r_imag(&xact[ix + 
		j * xact_dim1]), dabs(r__2));
	diffnm = 0.f;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * x_dim1;
	    i__4 = i__ + j * xact_dim1;
	    q__2.r = x[i__3].r - xact[i__4].r, q__2.i = x[i__3].i - xact[i__4]
		    .i;
	    q__1.r = q__2.r, q__1.i = q__2.i;
/* Computing MAX */
	    r__3 = diffnm, r__4 = (r__1 = q__1.r, dabs(r__1)) + (r__2 = 
		    r_imag(&q__1), dabs(r__2));
	    diffnm = dmax(r__3,r__4);
/* L10: */
	}
	if (xnorm <= 0.f) {
	    if (diffnm > 0.f) {
		*resid = 1.f / eps;
	    }
	} else {
/* Computing MAX */
	    r__1 = *resid, r__2 = diffnm / xnorm * *rcond;
	    *resid = dmax(r__1,r__2);
	}
/* L20: */
    }
    if (*resid * eps < 1.f) {
	*resid /= eps;
    }

    return 0;

/*     End of CGET04 */

} /* cget04_ */
