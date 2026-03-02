#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b11 = -1.;
static doublereal c_b12 = 0.;

/* Subroutine */ int dpot03_(char *uplo, integer *n, doublereal *a, integer *
	lda, doublereal *ainv, integer *ldainv, doublereal *work, integer *
	ldwork, doublereal *rwork, doublereal *rcond, doublereal *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, ainv_dim1, ainv_offset, work_dim1, work_offset, 
	    i__1, i__2;

    /* Local variables */
    integer i__, j;
    doublereal eps;
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern /* Subroutine */ int dsymm_(char *, char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    doublereal ainvnm;
    extern doublereal dlansy_(char *, char *, integer *, doublereal *, 
	    integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DPOT03 computes the residual for a symmetric matrix times its */
/*  inverse: */
/*     norm( I - A*AINV ) / ( N * norm(A) * norm(AINV) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The original symmetric matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N) */

/*  AINV    (input/output) DOUBLE PRECISION array, dimension (LDAINV,N) */
/*          On entry, the inverse of the matrix A, stored as a symmetric */
/*          matrix in the same format as A. */
/*          In this version, AINV is expanded into a full matrix and */
/*          multiplied by A, so the opposing triangle of AINV will be */
/*          changed; i.e., if the upper triangular part of AINV is */
/*          stored, the lower triangular part will be used as work space. */

/*  LDAINV  (input) INTEGER */
/*          The leading dimension of the array AINV.  LDAINV >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LDWORK,N) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK.  LDWORK >= max(1,N). */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of A, computed as */
/*          ( 1/norm(A) ) / norm(AINV). */

/*  RESID   (output) DOUBLE PRECISION */
/*          norm(I - A*AINV) / ( N * norm(A) * norm(AINV) * EPS ) */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    ainv_dim1 = *ldainv;
    ainv_offset = 1 + ainv_dim1;
    ainv -= ainv_offset;
    work_dim1 = *ldwork;
    work_offset = 1 + work_dim1;
    work -= work_offset;
    --rwork;

    /* Function Body */
    if (*n <= 0) {
	*rcond = 1.;
	*resid = 0.;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0 or AINVNM = 0. */

    eps = dlamch_("Epsilon");
    anorm = dlansy_("1", uplo, n, &a[a_offset], lda, &rwork[1]);
    ainvnm = dlansy_("1", uplo, n, &ainv[ainv_offset], ldainv, &rwork[1]);
    if (anorm <= 0. || ainvnm <= 0.) {
	*rcond = 0.;
	*resid = 1. / eps;
	return 0;
    }
    *rcond = 1. / anorm / ainvnm;

/*     Expand AINV into a full matrix and call DSYMM to multiply */
/*     AINV on the left by A. */

    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		ainv[j + i__ * ainv_dim1] = ainv[i__ + j * ainv_dim1];
/* L10: */
	    }
/* L20: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
		ainv[j + i__ * ainv_dim1] = ainv[i__ + j * ainv_dim1];
/* L30: */
	    }
/* L40: */
	}
    }
    dsymm_("Left", uplo, n, n, &c_b11, &a[a_offset], lda, &ainv[ainv_offset], 
	    ldainv, &c_b12, &work[work_offset], ldwork);

/*     Add the identity matrix to WORK . */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	work[i__ + i__ * work_dim1] += 1.;
/* L50: */
    }

/*     Compute norm(I - A*AINV) / (N * norm(A) * norm(AINV) * EPS) */

    *resid = dlange_("1", n, n, &work[work_offset], ldwork, &rwork[1]);

    *resid = *resid * *rcond / eps / (doublereal) (*n);

    return 0;

/*     End of DPOT03 */

} /* dpot03_ */
