#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b7 = -1.f;
static real c_b8 = 0.f;

/* Subroutine */ int sget03_(integer *n, real *a, integer *lda, real *ainv, 
	integer *ldainv, real *work, integer *ldwork, real *rwork, real *
	rcond, real *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, ainv_dim1, ainv_offset, work_dim1, work_offset, 
	    i__1;

    /* Local variables */
    integer i__;
    real eps;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
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

/*  SGET03 computes the residual for a general matrix times its inverse: */
/*     norm( I - AINV*A ) / ( N * norm(A) * norm(AINV) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The original N x N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  AINV    (input) REAL array, dimension (LDAINV,N) */
/*          The inverse of the matrix A. */

/*  LDAINV  (input) INTEGER */
/*          The leading dimension of the array AINV.  LDAINV >= max(1,N). */

/*  WORK    (workspace) REAL array, dimension (LDWORK,N) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK.  LDWORK >= max(1,N). */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RCOND   (output) REAL */
/*          The reciprocal of the condition number of A, computed as */
/*          ( 1/norm(A) ) / norm(AINV). */

/*  RESID   (output) REAL */
/*          norm(I - AINV*A) / ( N * norm(A) * norm(AINV) * EPS ) */

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
	*rcond = 1.f;
	*resid = 0.f;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0 or AINVNM = 0. */

    eps = slamch_("Epsilon");
    anorm = slange_("1", n, n, &a[a_offset], lda, &rwork[1]);
    ainvnm = slange_("1", n, n, &ainv[ainv_offset], ldainv, &rwork[1]);
    if (anorm <= 0.f || ainvnm <= 0.f) {
	*rcond = 0.f;
	*resid = 1.f / eps;
	return 0;
    }
    *rcond = 1.f / anorm / ainvnm;

/*     Compute I - A * AINV */

    sgemm_("No transpose", "No transpose", n, n, n, &c_b7, &ainv[ainv_offset], 
	     ldainv, &a[a_offset], lda, &c_b8, &work[work_offset], ldwork);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	work[i__ + i__ * work_dim1] += 1.f;
/* L10: */
    }

/*     Compute norm(I - AINV*A) / (N * norm(A) * norm(AINV) * EPS) */

    *resid = slange_("1", n, n, &work[work_offset], ldwork, &rwork[1]);

    *resid = *resid * *rcond / eps / (real) (*n);

    return 0;

/*     End of SGET03 */

} /* sget03_ */
