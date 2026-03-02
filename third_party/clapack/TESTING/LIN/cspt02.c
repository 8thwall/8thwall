#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {1.f,0.f};
static integer c__1 = 1;

/* Subroutine */ int cspt02_(char *uplo, integer *n, integer *nrhs, complex *
	a, complex *x, integer *ldx, complex *b, integer *ldb, real *rwork, 
	real *resid)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1;
    real r__1, r__2;
    complex q__1;

    /* Local variables */
    integer j;
    real eps, anorm, bnorm;
    extern /* Subroutine */ int cspmv_(char *, integer *, complex *, complex *
, complex *, integer *, complex *, complex *, integer *);
    real xnorm;
    extern doublereal slamch_(char *), clansp_(char *, char *, 
	    integer *, complex *, real *), scasum_(integer *, 
	    complex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CSPT02 computes the residual in the solution of a complex symmetric */
/*  system of linear equations  A*x = b  when packed storage is used for */
/*  the coefficient matrix.  The ratio computed is */

/*     RESID = norm( B - A*X ) / ( norm(A) * norm(X) * EPS). */

/*  where EPS is the machine precision. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          complex symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of B, the matrix of right hand sides. */
/*          NRHS >= 0. */

/*  A       (input) COMPLEX array, dimension (N*(N+1)/2) */
/*          The original complex symmetric matrix A, stored as a packed */
/*          triangular matrix. */

/*  X       (input) COMPLEX array, dimension (LDX,NRHS) */
/*          The computed solution vectors for the system of linear */
/*          equations. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.   LDX >= max(1,N). */

/*  B       (input/output) COMPLEX array, dimension (LDB,NRHS) */
/*          On entry, the right hand side vectors for the system of */
/*          linear equations. */
/*          On exit, B is overwritten with the difference B - A*X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RESID   (output) REAL */
/*          The maximum over the number of right hand sides of */
/*          norm(B - A*X) / ( norm(A) * norm(X) * EPS ). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick exit if N = 0 or NRHS = 0 */

    /* Parameter adjustments */
    --a;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --rwork;

    /* Function Body */
    if (*n <= 0 || *nrhs <= 0) {
	*resid = 0.f;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = slamch_("Epsilon");
    anorm = clansp_("1", uplo, n, &a[1], &rwork[1]);
    if (anorm <= 0.f) {
	*resid = 1.f / eps;
	return 0;
    }

/*     Compute  B - A*X  for the matrix of right hand sides B. */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	q__1.r = -1.f, q__1.i = -0.f;
	cspmv_(uplo, n, &q__1, &a[1], &x[j * x_dim1 + 1], &c__1, &c_b1, &b[j *
		 b_dim1 + 1], &c__1);
/* L10: */
    }

/*     Compute the maximum over the number of right hand sides of */
/*        norm( B - A*X ) / ( norm(A) * norm(X) * EPS ) . */

    *resid = 0.f;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	bnorm = scasum_(n, &b[j * b_dim1 + 1], &c__1);
	xnorm = scasum_(n, &x[j * x_dim1 + 1], &c__1);
	if (xnorm <= 0.f) {
	    *resid = 1.f / eps;
	} else {
/* Computing MAX */
	    r__1 = *resid, r__2 = bnorm / anorm / xnorm / eps;
	    *resid = dmax(r__1,r__2);
	}
/* L20: */
    }

    return 0;

/*     End of CSPT02 */

} /* cspt02_ */
