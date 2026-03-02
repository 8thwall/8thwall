#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static integer c__1 = 1;

/* Subroutine */ int zppt03_(char *uplo, integer *n, doublecomplex *a, 
	doublecomplex *ainv, doublecomplex *work, integer *ldwork, doublereal 
	*rwork, doublereal *rcond, doublereal *resid)
{
    /* System generated locals */
    integer work_dim1, work_offset, i__1, i__2, i__3;
    doublecomplex z__1;

    /* Builtin functions */
    void d_cnjg(doublecomplex *, doublecomplex *);

    /* Local variables */
    integer i__, j, jj;
    doublereal eps;
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern /* Subroutine */ int zcopy_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zhpmv_(char *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, integer *);
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *);
    doublereal ainvnm;
    extern doublereal zlanhp_(char *, char *, integer *, doublecomplex *, 
	    doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZPPT03 computes the residual for a Hermitian packed matrix times its */
/*  inverse: */
/*     norm( I - A*AINV ) / ( N * norm(A) * norm(AINV) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          Hermitian matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (N*(N+1)/2) */
/*          The original Hermitian matrix A, stored as a packed */
/*          triangular matrix. */

/*  AINV    (input) COMPLEX*16 array, dimension (N*(N+1)/2) */
/*          The (Hermitian) inverse of the matrix A, stored as a packed */
/*          triangular matrix. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LDWORK,N) */

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
    anorm = zlanhp_("1", uplo, n, &a[1], &rwork[1]);
    ainvnm = zlanhp_("1", uplo, n, &ainv[1], &rwork[1]);
    if (anorm <= 0. || ainvnm <= 0.) {
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
	    zcopy_(&j, &ainv[jj], &c__1, &work[(j + 1) * work_dim1 + 1], &
		    c__1);
	    i__2 = j - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = j + (i__ + 1) * work_dim1;
		d_cnjg(&z__1, &ainv[jj + i__ - 1]);
		work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L10: */
	    }
	    jj += j;
/* L20: */
	}
	jj = (*n - 1) * *n / 2 + 1;
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n + (i__ + 1) * work_dim1;
	    d_cnjg(&z__1, &ainv[jj + i__ - 1]);
	    work[i__2].r = z__1.r, work[i__2].i = z__1.i;
/* L30: */
	}

/*        Multiply by A */

	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    z__1.r = -1., z__1.i = -0.;
	    zhpmv_("Upper", n, &z__1, &a[1], &work[(j + 1) * work_dim1 + 1], &
		    c__1, &c_b1, &work[j * work_dim1 + 1], &c__1);
/* L40: */
	}
	z__1.r = -1., z__1.i = -0.;
	zhpmv_("Upper", n, &z__1, &a[1], &ainv[jj], &c__1, &c_b1, &work[*n * 
		work_dim1 + 1], &c__1);

/*     UPLO = 'L': */
/*     Copy the trailing N-1 x N-1 submatrix of AINV to WORK(1:N,1:N-1) */
/*     and multiply by A, moving each column to the right. */

    } else {

/*        Copy AINV */

	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__ * work_dim1 + 1;
	    d_cnjg(&z__1, &ainv[i__ + 1]);
	    work[i__2].r = z__1.r, work[i__2].i = z__1.i;
/* L50: */
	}
	jj = *n + 1;
	i__1 = *n;
	for (j = 2; j <= i__1; ++j) {
	    i__2 = *n - j + 1;
	    zcopy_(&i__2, &ainv[jj], &c__1, &work[j + (j - 1) * work_dim1], &
		    c__1);
	    i__2 = *n - j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = j + (j + i__ - 1) * work_dim1;
		d_cnjg(&z__1, &ainv[jj + i__]);
		work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L60: */
	    }
	    jj = jj + *n - j + 1;
/* L70: */
	}

/*        Multiply by A */

	for (j = *n; j >= 2; --j) {
	    z__1.r = -1., z__1.i = -0.;
	    zhpmv_("Lower", n, &z__1, &a[1], &work[(j - 1) * work_dim1 + 1], &
		    c__1, &c_b1, &work[j * work_dim1 + 1], &c__1);
/* L80: */
	}
	z__1.r = -1., z__1.i = -0.;
	zhpmv_("Lower", n, &z__1, &a[1], &ainv[1], &c__1, &c_b1, &work[
		work_dim1 + 1], &c__1);

    }

/*     Add the identity matrix to WORK . */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__ + i__ * work_dim1;
	i__3 = i__ + i__ * work_dim1;
	z__1.r = work[i__3].r + 1., z__1.i = work[i__3].i + 0.;
	work[i__2].r = z__1.r, work[i__2].i = z__1.i;
/* L90: */
    }

/*     Compute norm(I - A*AINV) / (N * norm(A) * norm(AINV) * EPS) */

    *resid = zlange_("1", n, n, &work[work_offset], ldwork, &rwork[1]);

    *resid = *resid * *rcond / eps / (doublereal) (*n);

    return 0;

/*     End of ZPPT03 */

} /* zppt03_ */
