#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static real c_b10 = -1.f;

/* Subroutine */ int stpt02_(char *uplo, char *trans, char *diag, integer *n, 
	integer *nrhs, real *ap, real *x, integer *ldx, real *b, integer *ldb, 
	 real *work, real *resid)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1;
    real r__1, r__2;

    /* Local variables */
    integer j;
    real eps;
    extern logical lsame_(char *, char *);
    real anorm, bnorm;
    extern doublereal sasum_(integer *, real *, integer *);
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    real xnorm;
    extern /* Subroutine */ int saxpy_(integer *, real *, real *, integer *, 
	    real *, integer *), stpmv_(char *, char *, char *, integer *, 
	    real *, real *, integer *);
    extern doublereal slamch_(char *), slantp_(char *, char *, char *, 
	     integer *, real *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  STPT02 computes the residual for the computed solution to a */
/*  triangular system of linear equations  A*x = b  or  A'*x = b  when */
/*  the triangular matrix A is stored in packed format.  Here A' is the */
/*  transpose of A and x and b are N by NRHS matrices.  The test ratio is */
/*  the maximum over the number of right hand sides of */
/*     norm(b - op(A)*x) / ( norm(op(A)) * norm(x) * EPS ), */
/*  where op(A) denotes A or A' and EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the operation applied to A. */
/*          = 'N':  A *x = b  (No transpose) */
/*          = 'T':  A'*x = b  (Transpose) */
/*          = 'C':  A'*x = b  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices X and B.  NRHS >= 0. */

/*  AP      (input) REAL array, dimension (N*(N+1)/2) */
/*          The upper or lower triangular matrix A, packed columnwise in */
/*          a linear array.  The j-th column of A is stored in the array */
/*          AP as follows: */
/*          if UPLO = 'U', AP((j-1)*j/2 + i) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', */
/*             AP((j-1)*(n-j) + j*(j+1)/2 + i-j) = A(i,j) for j<=i<=n. */

/*  X       (input) REAL array, dimension (LDX,NRHS) */
/*          The computed solution vectors for the system of linear */
/*          equations. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  B       (input) REAL array, dimension (LDB,NRHS) */
/*          The right hand side vectors for the system of linear */
/*          equations. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  WORK    (workspace) REAL array, dimension (N) */

/*  RESID   (output) REAL */
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
    --ap;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;

    /* Function Body */
    if (*n <= 0 || *nrhs <= 0) {
	*resid = 0.f;
	return 0;
    }

/*     Compute the 1-norm of A or A'. */

    if (lsame_(trans, "N")) {
	anorm = slantp_("1", uplo, diag, n, &ap[1], &work[1]);
    } else {
	anorm = slantp_("I", uplo, diag, n, &ap[1], &work[1]);
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = slamch_("Epsilon");
    if (anorm <= 0.f) {
	*resid = 1.f / eps;
	return 0;
    }

/*     Compute the maximum over the number of right hand sides of */
/*        norm(op(A)*x - b) / ( norm(op(A)) * norm(x) * EPS ). */

    *resid = 0.f;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	scopy_(n, &x[j * x_dim1 + 1], &c__1, &work[1], &c__1);
	stpmv_(uplo, trans, diag, n, &ap[1], &work[1], &c__1);
	saxpy_(n, &c_b10, &b[j * b_dim1 + 1], &c__1, &work[1], &c__1);
	bnorm = sasum_(n, &work[1], &c__1);
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

/*     End of STPT02 */

} /* stpt02_ */
