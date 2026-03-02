#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};

/* Subroutine */ int csyt03_(char *uplo, integer *n, complex *a, integer *lda, 
	 complex *ainv, integer *ldainv, complex *work, integer *ldwork, real 
	*rwork, real *rcond, real *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, ainv_dim1, ainv_offset, work_dim1, work_offset, 
	    i__1, i__2, i__3, i__4;
    complex q__1;

    /* Local variables */
    integer i__, j;
    real eps;
    extern logical lsame_(char *, char *);
    real anorm;
    extern /* Subroutine */ int csymm_(char *, char *, integer *, integer *, 
	    complex *, complex *, integer *, complex *, integer *, complex *, 
	    complex *, integer *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    real ainvnm;
    extern doublereal clansy_(char *, char *, integer *, complex *, integer *, 
	     real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CSYT03 computes the residual for a complex symmetric matrix times */
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

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The original complex symmetric matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N) */

/*  AINV    (input/output) COMPLEX array, dimension (LDAINV,N) */
/*          On entry, the inverse of the matrix A, stored as a symmetric */
/*          matrix in the same format as A. */
/*          In this version, AINV is expanded into a full matrix and */
/*          multiplied by A, so the opposing triangle of AINV will be */
/*          changed; i.e., if the upper triangular part of AINV is */
/*          stored, the lower triangular part will be used as work space. */

/*  LDAINV  (input) INTEGER */
/*          The leading dimension of the array AINV.  LDAINV >= max(1,N). */

/*  WORK    (workspace) COMPLEX array, dimension (LDWORK,N) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK.  LDWORK >= max(1,N). */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RCOND   (output) REAL */
/*          The reciprocal of the condition number of A, computed as */
/*          RCOND = 1/ (norm(A) * norm(AINV)). */

/*  RESID   (output) REAL */
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
	*rcond = 1.f;
	*resid = 0.f;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0 or AINVNM = 0. */

    eps = slamch_("Epsilon");
    anorm = clansy_("1", uplo, n, &a[a_offset], lda, &rwork[1]);
    ainvnm = clansy_("1", uplo, n, &ainv[ainv_offset], ldainv, &rwork[1]);
    if (anorm <= 0.f || ainvnm <= 0.f) {
	*rcond = 0.f;
	*resid = 1.f / eps;
	return 0;
    }
    *rcond = 1.f / anorm / ainvnm;

/*     Expand AINV into a full matrix and call CSYMM to multiply */
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
    q__1.r = -1.f, q__1.i = -0.f;
    csymm_("Left", uplo, n, n, &q__1, &a[a_offset], lda, &ainv[ainv_offset], 
	    ldainv, &c_b1, &work[work_offset], ldwork);

/*     Add the identity matrix to WORK . */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__ + i__ * work_dim1;
	i__3 = i__ + i__ * work_dim1;
	q__1.r = work[i__3].r + 1.f, q__1.i = work[i__3].i + 0.f;
	work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L50: */
    }

/*     Compute norm(I - A*AINV) / (N * norm(A) * norm(AINV) * EPS) */

    *resid = clange_("1", n, n, &work[work_offset], ldwork, &rwork[1]);

    *resid = *resid * *rcond / eps / (real) (*n);

    return 0;

/*     End of CSYT03 */

} /* csyt03_ */
