#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static integer c__1 = 1;

/* Subroutine */ int cppt03_(char *uplo, integer *n, complex *a, complex *
	ainv, complex *work, integer *ldwork, real *rwork, real *rcond, real *
	resid)
{
    /* System generated locals */
    integer work_dim1, work_offset, i__1, i__2, i__3;
    complex q__1;

    /* Builtin functions */
    void r_cnjg(complex *, complex *);

    /* Local variables */
    integer i__, j, jj;
    real eps;
    extern logical lsame_(char *, char *);
    real anorm;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *), chpmv_(char *, integer *, complex *, 
	    complex *, complex *, integer *, complex *, complex *, integer *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), clanhp_(char *, char *, integer *, 
	    complex *, real *), slamch_(char *);
    real ainvnm;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CPPT03 computes the residual for a Hermitian packed matrix times its */
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

/*  A       (input) COMPLEX array, dimension (N*(N+1)/2) */
/*          The original Hermitian matrix A, stored as a packed */
/*          triangular matrix. */

/*  AINV    (input) COMPLEX array, dimension (N*(N+1)/2) */
/*          The (Hermitian) inverse of the matrix A, stored as a packed */
/*          triangular matrix. */

/*  WORK    (workspace) COMPLEX array, dimension (LDWORK,N) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK.  LDWORK >= max(1,N). */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RCOND   (output) REAL */
/*          The reciprocal of the condition number of A, computed as */
/*          ( 1/norm(A) ) / norm(AINV). */

/*  RESID   (output) REAL */
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
	*rcond = 1.f;
	*resid = 0.f;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0 or AINVNM = 0. */

    eps = slamch_("Epsilon");
    anorm = clanhp_("1", uplo, n, &a[1], &rwork[1]);
    ainvnm = clanhp_("1", uplo, n, &ainv[1], &rwork[1]);
    if (anorm <= 0.f || ainvnm <= 0.f) {
	*rcond = 0.f;
	*resid = 1.f / eps;
	return 0;
    }
    *rcond = 1.f / anorm / ainvnm;

/*     UPLO = 'U': */
/*     Copy the leading N-1 x N-1 submatrix of AINV to WORK(1:N,2:N) and */
/*     expand it to a full matrix, then multiply by A one column at a */
/*     time, moving the result one column to the left. */

    if (lsame_(uplo, "U")) {

/*        Copy AINV */

	jj = 1;
	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    ccopy_(&j, &ainv[jj], &c__1, &work[(j + 1) * work_dim1 + 1], &
		    c__1);
	    i__2 = j - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = j + (i__ + 1) * work_dim1;
		r_cnjg(&q__1, &ainv[jj + i__ - 1]);
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L10: */
	    }
	    jj += j;
/* L20: */
	}
	jj = (*n - 1) * *n / 2 + 1;
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n + (i__ + 1) * work_dim1;
	    r_cnjg(&q__1, &ainv[jj + i__ - 1]);
	    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L30: */
	}

/*        Multiply by A */

	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    q__1.r = -1.f, q__1.i = -0.f;
	    chpmv_("Upper", n, &q__1, &a[1], &work[(j + 1) * work_dim1 + 1], &
		    c__1, &c_b1, &work[j * work_dim1 + 1], &c__1);
/* L40: */
	}
	q__1.r = -1.f, q__1.i = -0.f;
	chpmv_("Upper", n, &q__1, &a[1], &ainv[jj], &c__1, &c_b1, &work[*n * 
		work_dim1 + 1], &c__1);

/*     UPLO = 'L': */
/*     Copy the trailing N-1 x N-1 submatrix of AINV to WORK(1:N,1:N-1) */
/*     and multiply by A, moving each column to the right. */

    } else {

/*        Copy AINV */

	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__ * work_dim1 + 1;
	    r_cnjg(&q__1, &ainv[i__ + 1]);
	    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L50: */
	}
	jj = *n + 1;
	i__1 = *n;
	for (j = 2; j <= i__1; ++j) {
	    i__2 = *n - j + 1;
	    ccopy_(&i__2, &ainv[jj], &c__1, &work[j + (j - 1) * work_dim1], &
		    c__1);
	    i__2 = *n - j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = j + (j + i__ - 1) * work_dim1;
		r_cnjg(&q__1, &ainv[jj + i__]);
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L60: */
	    }
	    jj = jj + *n - j + 1;
/* L70: */
	}

/*        Multiply by A */

	for (j = *n; j >= 2; --j) {
	    q__1.r = -1.f, q__1.i = -0.f;
	    chpmv_("Lower", n, &q__1, &a[1], &work[(j - 1) * work_dim1 + 1], &
		    c__1, &c_b1, &work[j * work_dim1 + 1], &c__1);
/* L80: */
	}
	q__1.r = -1.f, q__1.i = -0.f;
	chpmv_("Lower", n, &q__1, &a[1], &ainv[1], &c__1, &c_b1, &work[
		work_dim1 + 1], &c__1);

    }

/*     Add the identity matrix to WORK . */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__ + i__ * work_dim1;
	i__3 = i__ + i__ * work_dim1;
	q__1.r = work[i__3].r + 1.f, q__1.i = work[i__3].i + 0.f;
	work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L90: */
    }

/*     Compute norm(I - A*AINV) / (N * norm(A) * norm(AINV) * EPS) */

    *resid = clange_("1", n, n, &work[work_offset], ldwork, &rwork[1]);

    *resid = *resid * *rcond / eps / (real) (*n);

    return 0;

/*     End of CPPT03 */

} /* cppt03_ */
