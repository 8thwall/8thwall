#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int slaptm_(integer *n, integer *nrhs, real *alpha, real *
	d__, real *e, real *x, integer *ldx, real *beta, real *b, integer *
	ldb)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1, i__2;

    /* Local variables */
    integer i__, j;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLAPTM multiplies an N by NRHS matrix X by a symmetric tridiagonal */
/*  matrix A and stores the result in a matrix B.  The operation has the */
/*  form */

/*     B := alpha * A * X + beta * B */

/*  where alpha may be either 1. or -1. and beta may be 0., 1., or -1. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices X and B. */

/*  ALPHA   (input) REAL */
/*          The scalar alpha.  ALPHA must be 1. or -1.; otherwise, */
/*          it is assumed to be 0. */

/*  D       (input) REAL array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix A. */

/*  E       (input) REAL array, dimension (N-1) */
/*          The (n-1) subdiagonal or superdiagonal elements of A. */

/*  X       (input) REAL array, dimension (LDX,NRHS) */
/*          The N by NRHS matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(N,1). */

/*  BETA    (input) REAL */
/*          The scalar beta.  BETA must be 0., 1., or -1.; otherwise, */
/*          it is assumed to be 1. */

/*  B       (input/output) REAL array, dimension (LDB,NRHS) */
/*          On entry, the N by NRHS matrix B. */
/*          On exit, B is overwritten by the matrix expression */
/*          B := alpha * A * X + beta * B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(N,1). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --d__;
    --e;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    if (*n == 0) {
	return 0;
    }

/*     Multiply B by BETA if BETA.NE.1. */

    if (*beta == 0.f) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = 0.f;
/* L10: */
	    }
/* L20: */
	}
    } else if (*beta == -1.f) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = -b[i__ + j * b_dim1];
/* L30: */
	    }
/* L40: */
	}
    }

    if (*alpha == 1.f) {

/*        Compute B := B + A*X */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    if (*n == 1) {
		b[j * b_dim1 + 1] += d__[1] * x[j * x_dim1 + 1];
	    } else {
		b[j * b_dim1 + 1] = b[j * b_dim1 + 1] + d__[1] * x[j * x_dim1 
			+ 1] + e[1] * x[j * x_dim1 + 2];
		b[*n + j * b_dim1] = b[*n + j * b_dim1] + e[*n - 1] * x[*n - 
			1 + j * x_dim1] + d__[*n] * x[*n + j * x_dim1];
		i__2 = *n - 1;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    b[i__ + j * b_dim1] = b[i__ + j * b_dim1] + e[i__ - 1] * 
			    x[i__ - 1 + j * x_dim1] + d__[i__] * x[i__ + j * 
			    x_dim1] + e[i__] * x[i__ + 1 + j * x_dim1];
/* L50: */
		}
	    }
/* L60: */
	}
    } else if (*alpha == -1.f) {

/*        Compute B := B - A*X */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    if (*n == 1) {
		b[j * b_dim1 + 1] -= d__[1] * x[j * x_dim1 + 1];
	    } else {
		b[j * b_dim1 + 1] = b[j * b_dim1 + 1] - d__[1] * x[j * x_dim1 
			+ 1] - e[1] * x[j * x_dim1 + 2];
		b[*n + j * b_dim1] = b[*n + j * b_dim1] - e[*n - 1] * x[*n - 
			1 + j * x_dim1] - d__[*n] * x[*n + j * x_dim1];
		i__2 = *n - 1;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    b[i__ + j * b_dim1] = b[i__ + j * b_dim1] - e[i__ - 1] * 
			    x[i__ - 1 + j * x_dim1] - d__[i__] * x[i__ + j * 
			    x_dim1] - e[i__] * x[i__ + 1 + j * x_dim1];
/* L70: */
		}
	    }
/* L80: */
	}
    }
    return 0;

/*     End of SLAPTM */

} /* slaptm_ */
