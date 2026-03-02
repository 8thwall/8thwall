#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b13 = -1.;
static doublereal c_b15 = 0.;

/* Subroutine */ int dppt03_(char *uplo, integer *n, doublereal *a, 
	doublereal *ainv, doublereal *work, integer *ldwork, doublereal *
	rwork, doublereal *rcond, doublereal *resid)
{
    /* System generated locals */
    integer work_dim1, work_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, jj;
    doublereal eps;
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *), dspmv_(char *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *), 
	    dlansp_(char *, char *, integer *, doublereal *, doublereal *);
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

/*  DPPT03 computes the residual for a symmetric packed matrix times its */
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

/*  A       (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The original symmetric matrix A, stored as a packed */
/*          triangular matrix. */

/*  AINV    (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The (symmetric) inverse of the matrix A, stored as a packed */
/*          triangular matrix. */

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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick exit if N = 0. */

    /* Parameter adjustments */
    --a;
    --ainv;
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
    anorm = dlansp_("1", uplo, n, &a[1], &rwork[1]);
    ainvnm = dlansp_("1", uplo, n, &ainv[1], &rwork[1]);
    if (anorm <= 0. || ainvnm == 0.) {
	*rcond = 0.;
	*resid = 1. / eps;
	return 0;
    }
    *rcond = 1. / anorm / ainvnm;

/*     UPLO = 'U': */
/*     Copy the leading N-1 x N-1 submatrix of AINV to WORK(1:N,2:N) and */
/*     expand it to a full matrix, then multiply by A one column at a */
/*     time, moving the result one column to the left. */

    if (lsame_(uplo, "U")) {

/*        Copy AINV */

	jj = 1;
	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    dcopy_(&j, &ainv[jj], &c__1, &work[(j + 1) * work_dim1 + 1], &
		    c__1);
	    i__2 = j - 1;
	    dcopy_(&i__2, &ainv[jj], &c__1, &work[j + (work_dim1 << 1)], 
		    ldwork);
	    jj += j;
/* L10: */
	}
	jj = (*n - 1) * *n / 2 + 1;
	i__1 = *n - 1;
	dcopy_(&i__1, &ainv[jj], &c__1, &work[*n + (work_dim1 << 1)], ldwork);

/*        Multiply by A */

	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    dspmv_("Upper", n, &c_b13, &a[1], &work[(j + 1) * work_dim1 + 1], 
		    &c__1, &c_b15, &work[j * work_dim1 + 1], &c__1)
		    ;
/* L20: */
	}
	dspmv_("Upper", n, &c_b13, &a[1], &ainv[jj], &c__1, &c_b15, &work[*n *
		 work_dim1 + 1], &c__1);

/*     UPLO = 'L': */
/*     Copy the trailing N-1 x N-1 submatrix of AINV to WORK(1:N,1:N-1) */
/*     and multiply by A, moving each column to the right. */

    } else {

/*        Copy AINV */

	i__1 = *n - 1;
	dcopy_(&i__1, &ainv[2], &c__1, &work[work_dim1 + 1], ldwork);
	jj = *n + 1;
	i__1 = *n;
	for (j = 2; j <= i__1; ++j) {
	    i__2 = *n - j + 1;
	    dcopy_(&i__2, &ainv[jj], &c__1, &work[j + (j - 1) * work_dim1], &
		    c__1);
	    i__2 = *n - j;
	    dcopy_(&i__2, &ainv[jj + 1], &c__1, &work[j + j * work_dim1], 
		    ldwork);
	    jj = jj + *n - j + 1;
/* L30: */
	}

/*        Multiply by A */

	for (j = *n; j >= 2; --j) {
	    dspmv_("Lower", n, &c_b13, &a[1], &work[(j - 1) * work_dim1 + 1], 
		    &c__1, &c_b15, &work[j * work_dim1 + 1], &c__1)
		    ;
/* L40: */
	}
	dspmv_("Lower", n, &c_b13, &a[1], &ainv[1], &c__1, &c_b15, &work[
		work_dim1 + 1], &c__1);

    }

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

/*     End of DPPT03 */

} /* dppt03_ */
