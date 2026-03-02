#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int dtpt01_(char *uplo, char *diag, integer *n, doublereal *
	ap, doublereal *ainvp, doublereal *rcond, doublereal *work, 
	doublereal *resid)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer j, jc;
    doublereal eps;
    extern logical lsame_(char *, char *);
    doublereal anorm;
    logical unitd;
    extern /* Subroutine */ int dtpmv_(char *, char *, char *, integer *, 
	    doublereal *, doublereal *, integer *);
    extern doublereal dlamch_(char *), dlantp_(char *, char *, char *, 
	     integer *, doublereal *, doublereal *);
    doublereal ainvnm;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTPT01 computes the residual for a triangular matrix A times its */
/*  inverse when A is stored in packed format: */
/*     RESID = norm(A*AINV - I) / ( N * norm(A) * norm(AINV) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The original upper or lower triangular matrix A, packed */
/*          columnwise in a linear array.  The j-th column of A is stored */
/*          in the array AP as follows: */
/*          if UPLO = 'U', AP((j-1)*j/2 + i) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', */
/*             AP((j-1)*(n-j) + j*(j+1)/2 + i-j) = A(i,j) for j<=i<=n. */

/*  AINVP   (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the (triangular) inverse of the matrix A, packed */
/*          columnwise in a linear array as in AP. */
/*          On exit, the contents of AINVP are destroyed. */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal condition number of A, computed as */
/*          1/(norm(A) * norm(AINV)). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESID   (output) DOUBLE PRECISION */
/*          norm(A*AINV - I) / ( N * norm(A) * norm(AINV) * EPS ) */

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

/*     Quick exit if N = 0. */

    /* Parameter adjustments */
    --work;
    --ainvp;
    --ap;

    /* Function Body */
    if (*n <= 0) {
	*rcond = 1.;
	*resid = 0.;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0 or AINVNM = 0. */

    eps = dlamch_("Epsilon");
    anorm = dlantp_("1", uplo, diag, n, &ap[1], &work[1]);
    ainvnm = dlantp_("1", uplo, diag, n, &ainvp[1], &work[1]);
    if (anorm <= 0. || ainvnm <= 0.) {
	*rcond = 0.;
	*resid = 1. / eps;
	return 0;
    }
    *rcond = 1. / anorm / ainvnm;

/*     Compute A * AINV, overwriting AINV. */

    unitd = lsame_(diag, "U");
    if (lsame_(uplo, "U")) {
	jc = 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (unitd) {
		ainvp[jc + j - 1] = 1.;
	    }

/*           Form the j-th column of A*AINV */

	    dtpmv_("Upper", "No transpose", diag, &j, &ap[1], &ainvp[jc], &
		    c__1);

/*           Subtract 1 from the diagonal */

	    ainvp[jc + j - 1] += -1.;
	    jc += j;
/* L10: */
	}
    } else {
	jc = 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (unitd) {
		ainvp[jc] = 1.;
	    }

/*           Form the j-th column of A*AINV */

	    i__2 = *n - j + 1;
	    dtpmv_("Lower", "No transpose", diag, &i__2, &ap[jc], &ainvp[jc], 
		    &c__1);

/*           Subtract 1 from the diagonal */

	    ainvp[jc] += -1.;
	    jc = jc + *n - j + 1;
/* L20: */
	}
    }

/*     Compute norm(A*AINV - I) / (N * norm(A) * norm(AINV) * EPS) */

    *resid = dlantp_("1", uplo, "Non-unit", n, &ainvp[1], &work[1]);

    *resid = *resid * *rcond / (doublereal) (*n) / eps;

    return 0;

/*     End of DTPT01 */

} /* dtpt01_ */
