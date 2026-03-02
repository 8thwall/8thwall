#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};

/* Subroutine */ int zsyt03_(char *uplo, integer *n, doublecomplex *a, 
	integer *lda, doublecomplex *ainv, integer *ldainv, doublecomplex *
	work, integer *ldwork, doublereal *rwork, doublereal *rcond, 
	doublereal *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, ainv_dim1, ainv_offset, work_dim1, work_offset, 
	    i__1, i__2, i__3, i__4;
    doublecomplex z__1;

    /* Local variables */
    integer i__, j;
    doublereal eps;
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern /* Subroutine */ int zsymm_(char *, char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *);
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *);
    doublereal ainvnm;
    extern doublereal zlansy_(char *, char *, integer *, doublecomplex *, 
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

/*  ZSYT03 computes the residual for a complex symmetric matrix times */
/*  its inverse: */
/*     norm( I - A*AINV ) / ( N * norm(A) * norm(AINV) * EPS ) */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          complex symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The original complex symmetric matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N) */

/*  AINV    (input/output) COMPLEX*16 array, dimension (LDAINV,N) */
/*          On entry, the inverse of the matrix A, stored as a symmetric */
/*          matrix in the same format as A. */
/*          In this version, AINV is expanded into a full matrix and */
/*          multiplied by A, so the opposing triangle of AINV will be */
/*          changed; i.e., if the upper triangular part of AINV is */
/*          stored, the lower triangular part will be used as work space. */

/*  LDAINV  (input) INTEGER */
/*          The leading dimension of the array AINV.  LDAINV >= max(1,N). */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LDWORK,N) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK.  LDWORK >= max(1,N). */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of A, computed as */
/*          RCOND = 1/ (norm(A) * norm(AINV)). */

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

/*     Quick exit if N = 0 */

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
    anorm = zlansy_("1", uplo, n, &a[a_offset], lda, &rwork[1]);
    ainvnm = zlansy_("1", uplo, n, &ainv[ainv_offset], ldainv, &rwork[1]);
    if (anorm <= 0. || ainvnm <= 0.) {
	*rcond = 0.;
	*resid = 1. / eps;
	return 0;
    }
    *rcond = 1. / anorm / ainvnm;

/*     Expand AINV into a full matrix and call ZSYMM to multiply */
/*     AINV on the left by A (store the result in WORK). */

    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = j + i__ * ainv_dim1;
		i__4 = i__ + j * ainv_dim1;
		ainv[i__3].r = ainv[i__4].r, ainv[i__3].i = ainv[i__4].i;
/* L10: */
	    }
/* L20: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
		i__3 = j + i__ * ainv_dim1;
		i__4 = i__ + j * ainv_dim1;
		ainv[i__3].r = ainv[i__4].r, ainv[i__3].i = ainv[i__4].i;
/* L30: */
	    }
/* L40: */
	}
    }
    z__1.r = -1., z__1.i = -0.;
    zsymm_("Left", uplo, n, n, &z__1, &a[a_offset], lda, &ainv[ainv_offset], 
	    ldainv, &c_b1, &work[work_offset], ldwork);

/*     Add the identity matrix to WORK . */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__ + i__ * work_dim1;
	i__3 = i__ + i__ * work_dim1;
	z__1.r = work[i__3].r + 1., z__1.i = work[i__3].i + 0.;
	work[i__2].r = z__1.r, work[i__2].i = z__1.i;
/* L50: */
    }

/*     Compute norm(I - A*AINV) / (N * norm(A) * norm(AINV) * EPS) */

    *resid = zlange_("1", n, n, &work[work_offset], ldwork, &rwork[1]);

    *resid = *resid * *rcond / eps / (doublereal) (*n);

    return 0;

/*     End of ZSYT03 */

} /* zsyt03_ */
