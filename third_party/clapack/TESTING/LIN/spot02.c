#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b5 = -1.f;
static real c_b6 = 1.f;
static integer c__1 = 1;

/* Subroutine */ int spot02_(char *uplo, integer *n, integer *nrhs, real *a, 
	integer *lda, real *x, integer *ldx, real *b, integer *ldb, real *
	rwork, real *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, i__1;
    real r__1, r__2;

    /* Local variables */
    integer j;
    real eps, anorm, bnorm;
    extern doublereal sasum_(integer *, real *, integer *);
    real xnorm;
    extern /* Subroutine */ int ssymm_(char *, char *, integer *, integer *, 
	    real *, real *, integer *, real *, integer *, real *, real *, 
	    integer *);
    extern doublereal slamch_(char *), slansy_(char *, char *, 
	    integer *, real *, integer *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SPOT02 computes the residual for the solution of a symmetric system */
/*  of linear equations  A*x = b: */

/*     RESID = norm(B - A*X) / ( norm(A) * norm(X) * EPS ), */

/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of B, the matrix of right hand sides. */
/*          NRHS >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The original symmetric matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N) */

/*  X       (input) REAL array, dimension (LDX,NRHS) */
/*          The computed solution vectors for the system of linear */
/*          equations. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.   LDX >= max(1,N). */

/*  B       (input/output) REAL array, dimension (LDB,NRHS) */
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

/*     Quick exit if N = 0 or NRHS = 0. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
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
    anorm = slansy_("1", uplo, n, &a[a_offset], lda, &rwork[1]);
    if (anorm <= 0.f) {
	*resid = 1.f / eps;
	return 0;
    }

/*     Compute  B - A*X */

    ssymm_("Left", uplo, n, nrhs, &c_b5, &a[a_offset], lda, &x[x_offset], ldx, 
	     &c_b6, &b[b_offset], ldb);

/*     Compute the maximum over the number of right hand sides of */
/*        norm( B - A*X ) / ( norm(A) * norm(X) * EPS ) . */

    *resid = 0.f;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	bnorm = sasum_(n, &b[j * b_dim1 + 1], &c__1);
	xnorm = sasum_(n, &x[j * x_dim1 + 1], &c__1);
	if (xnorm <= 0.f) {
	    *resid = 1.f / eps;
	} else {
/* Computing MAX */
	    r__1 = *resid, r__2 = bnorm / anorm / xnorm / eps;
	    *resid = dmax(r__1,r__2);
	}
/* L10: */
    }

    return 0;

/*     End of SPOT02 */

} /* spot02_ */
