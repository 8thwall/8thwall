#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b4 = -1.;
static doublereal c_b5 = 1.;
static integer c__1 = 1;

/* Subroutine */ int zptt02_(char *uplo, integer *n, integer *nrhs, 
	doublereal *d__, doublecomplex *e, doublecomplex *x, integer *ldx, 
	doublecomplex *b, integer *ldb, doublereal *resid)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    integer j;
    doublereal eps, anorm, bnorm, xnorm;
    extern doublereal dlamch_(char *), zlanht_(char *, integer *, 
	    doublereal *, doublecomplex *), dzasum_(integer *, 
	    doublecomplex *, integer *);
    extern /* Subroutine */ int zlaptm_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublecomplex *, 
	    integer *, doublereal *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZPTT02 computes the residual for the solution to a symmetric */
/*  tridiagonal system of equations: */
/*     RESID = norm(B - A*X) / (norm(A) * norm(X) * EPS), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the superdiagonal or the subdiagonal of the */
/*          tridiagonal matrix A is stored. */
/*          = 'U':  E is the superdiagonal of A */
/*          = 'L':  E is the subdiagonal of A */

/*  N       (input) INTEGTER */
/*          The order of the matrix A. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix A. */

/*  E       (input) COMPLEX*16 array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of the tridiagonal matrix A. */

/*  X       (input) COMPLEX*16 array, dimension (LDX,NRHS) */
/*          The n by nrhs matrix of solution vectors X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  B       (input/output) COMPLEX*16 array, dimension (LDB,NRHS) */
/*          On entry, the n by nrhs matrix of right hand side vectors B. */
/*          On exit, B is overwritten with the difference B - A*X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  RESID   (output) DOUBLE PRECISION */
/*          norm(B - A*X) / (norm(A) * norm(X) * EPS) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

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
    if (*n <= 0) {
	*resid = 0.;
	return 0;
    }

/*     Compute the 1-norm of the tridiagonal matrix A. */

    anorm = zlanht_("1", n, &d__[1], &e[1]);

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = dlamch_("Epsilon");
    if (anorm <= 0.) {
	*resid = 1. / eps;
	return 0;
    }

/*     Compute B - A*X. */

    zlaptm_(uplo, n, nrhs, &c_b4, &d__[1], &e[1], &x[x_offset], ldx, &c_b5, &
	    b[b_offset], ldb);

/*     Compute the maximum over the number of right hand sides of */
/*        norm(B - A*X) / ( norm(A) * norm(X) * EPS ). */

    *resid = 0.;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	bnorm = dzasum_(n, &b[j * b_dim1 + 1], &c__1);
	xnorm = dzasum_(n, &x[j * x_dim1 + 1], &c__1);
	if (xnorm <= 0.) {
	    *resid = 1. / eps;
	} else {
/* Computing MAX */
	    d__1 = *resid, d__2 = bnorm / anorm / xnorm / eps;
	    *resid = max(d__1,d__2);
	}
/* L10: */
    }

    return 0;

/*     End of ZPTT02 */

} /* zptt02_ */
