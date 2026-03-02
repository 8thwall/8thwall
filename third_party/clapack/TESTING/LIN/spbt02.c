#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b5 = -1.f;
static integer c__1 = 1;
static real c_b7 = 1.f;

/* Subroutine */ int spbt02_(char *uplo, integer *n, integer *kd, integer *
	nrhs, real *a, integer *lda, real *x, integer *ldx, real *b, integer *
	ldb, real *rwork, real *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, i__1;
    real r__1, r__2;

    /* Local variables */
    integer j;
    real eps, anorm, bnorm;
    extern doublereal sasum_(integer *, real *, integer *);
    extern /* Subroutine */ int ssbmv_(char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *, integer *);
    real xnorm;
    extern doublereal slamch_(char *), slansb_(char *, char *, 
	    integer *, integer *, real *, integer *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SPBT02 computes the residual for a solution of a symmetric banded */
/*  system of equations  A*x = b: */
/*     RESID = norm( B - A*X ) / ( norm(A) * norm(X) * EPS) */
/*  where EPS is the machine precision. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of super-diagonals of the matrix A if UPLO = 'U', */
/*          or the number of sub-diagonals if UPLO = 'L'.  KD >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The original symmetric band matrix A.  If UPLO = 'U', the */
/*          upper triangular part of A is stored as a band matrix; if */
/*          UPLO = 'L', the lower triangular part of A is stored.  The */
/*          columns of the appropriate triangle are stored in the columns */
/*          of A and the diagonals of the triangle are stored in the rows */
/*          of A.  See SPBTRF for further details. */

/*  LDA     (input) INTEGER. */
/*          The leading dimension of the array A.  LDA >= max(1,KD+1). */

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
    anorm = slansb_("1", uplo, n, kd, &a[a_offset], lda, &rwork[1]);
    if (anorm <= 0.f) {
	*resid = 1.f / eps;
	return 0;
    }

/*     Compute  B - A*X */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	ssbmv_(uplo, n, kd, &c_b5, &a[a_offset], lda, &x[j * x_dim1 + 1], &
		c__1, &c_b7, &b[j * b_dim1 + 1], &c__1);
/* L10: */
    }

/*     Compute the maximum over the number of right hand sides of */
/*          norm( B - A*X ) / ( norm(A) * norm(X) * EPS ) */

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
/* L20: */
    }

    return 0;

/*     End of SPBT02 */

} /* spbt02_ */
