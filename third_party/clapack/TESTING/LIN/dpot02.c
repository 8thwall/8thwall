#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b5 = -1.;
static doublereal c_b6 = 1.;
static integer c__1 = 1;

/* Subroutine */ int dpot02_(char *uplo, integer *n, integer *nrhs, 
	doublereal *a, integer *lda, doublereal *x, integer *ldx, doublereal *
	b, integer *ldb, doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    integer j;
    doublereal eps;
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal anorm, bnorm;
    extern /* Subroutine */ int dsymm_(char *, char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    doublereal xnorm;
    extern doublereal dlamch_(char *), dlansy_(char *, char *, 
	    integer *, doublereal *, integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DPOT02 computes the residual for the solution of a symmetric system */
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

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The original symmetric matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N) */

/*  X       (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The computed solution vectors for the system of linear */
/*          equations. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.   LDX >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the right hand side vectors for the system of */
/*          linear equations. */
/*          On exit, B is overwritten with the difference B - A*X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESID   (output) DOUBLE PRECISION */
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
	*resid = 0.;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = dlamch_("Epsilon");
    anorm = dlansy_("1", uplo, n, &a[a_offset], lda, &rwork[1]);
    if (anorm <= 0.) {
	*resid = 1. / eps;
	return 0;
    }

/*     Compute  B - A*X */

    dsymm_("Left", uplo, n, nrhs, &c_b5, &a[a_offset], lda, &x[x_offset], ldx, 
	     &c_b6, &b[b_offset], ldb);

/*     Compute the maximum over the number of right hand sides of */
/*        norm( B - A*X ) / ( norm(A) * norm(X) * EPS ) . */

    *resid = 0.;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	bnorm = dasum_(n, &b[j * b_dim1 + 1], &c__1);
	xnorm = dasum_(n, &x[j * x_dim1 + 1], &c__1);
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

/*     End of DPOT02 */

} /* dpot02_ */
