#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublecomplex c_b12 = {-1.,0.};

/* Subroutine */ int ztrt02_(char *uplo, char *trans, char *diag, integer *n, 
	integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *x, 
	integer *ldx, doublecomplex *b, integer *ldb, doublecomplex *work, 
	doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    integer j;
    doublereal eps;
    extern logical lsame_(char *, char *);
    doublereal anorm, bnorm, xnorm;
    extern /* Subroutine */ int zcopy_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zaxpy_(integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), ztrmv_(
	    char *, char *, char *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *);
    extern doublereal dlamch_(char *), dzasum_(integer *, 
	    doublecomplex *, integer *), zlantr_(char *, char *, char *, 
	    integer *, integer *, doublecomplex *, integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZTRT02 computes the residual for the computed solution to a */
/*  triangular system of linear equations  A*x = b,  A**T *x = b, */
/*  or A**H *x = b.  Here A is a triangular matrix, A**T is the transpose */
/*  of A, A**H is the conjugate transpose of A, and x and b are N by NRHS */
/*  matrices.  The test ratio is the maximum over the number of right */
/*  hand sides of */
/*     norm(b - op(A)*x) / ( norm(op(A)) * norm(x) * EPS ), */
/*  where op(A) denotes A, A**T, or A**H, and EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the operation applied to A. */
/*          = 'N':  A *x = b     (No transpose) */
/*          = 'T':  A**T *x = b  (Transpose) */
/*          = 'C':  A**H *x = b  (Conjugate transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices X and B.  NRHS >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The triangular matrix A.  If UPLO = 'U', the leading n by n */
/*          upper triangular part of the array A contains the upper */
/*          triangular matrix, and the strictly lower triangular part of */
/*          A is not referenced.  If UPLO = 'L', the leading n by n lower */
/*          triangular part of the array A contains the lower triangular */
/*          matrix, and the strictly upper triangular part of A is not */
/*          referenced.  If DIAG = 'U', the diagonal elements of A are */
/*          also not referenced and are assumed to be 1. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  X       (input) COMPLEX*16 array, dimension (LDX,NRHS) */
/*          The computed solution vectors for the system of linear */
/*          equations. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  B       (input) COMPLEX*16 array, dimension (LDB,NRHS) */
/*          The right hand side vectors for the system of linear */
/*          equations. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  WORK    (workspace) COMPLEX*16 array, dimension (N) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESID   (output) DOUBLE PRECISION */
/*          The maximum over the number of right hand sides of */
/*          norm(op(A)*x - b) / ( norm(op(A)) * norm(x) * EPS ). */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;
    --rwork;

    /* Function Body */
    if (*n <= 0 || *nrhs <= 0) {
	*resid = 0.;
	return 0;
    }

/*     Compute the 1-norm of A or A**H. */

    if (lsame_(trans, "N")) {
	anorm = zlantr_("1", uplo, diag, n, n, &a[a_offset], lda, &rwork[1]);
    } else {
	anorm = zlantr_("I", uplo, diag, n, n, &a[a_offset], lda, &rwork[1]);
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = dlamch_("Epsilon");
    if (anorm <= 0.) {
	*resid = 1. / eps;
	return 0;
    }

/*     Compute the maximum over the number of right hand sides of */
/*        norm(op(A)*x - b) / ( norm(op(A)) * norm(x) * EPS ) */

    *resid = 0.;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	zcopy_(n, &x[j * x_dim1 + 1], &c__1, &work[1], &c__1);
	ztrmv_(uplo, trans, diag, n, &a[a_offset], lda, &work[1], &c__1);
	zaxpy_(n, &c_b12, &b[j * b_dim1 + 1], &c__1, &work[1], &c__1);
	bnorm = dzasum_(n, &work[1], &c__1);
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

/*     End of ZTRT02 */

} /* ztrt02_ */
