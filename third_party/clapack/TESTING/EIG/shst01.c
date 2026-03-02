#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b7 = 1.f;
static real c_b8 = 0.f;
static real c_b11 = -1.f;

/* Subroutine */ int shst01_(integer *n, integer *ilo, integer *ihi, real *a, 
	integer *lda, real *h__, integer *ldh, real *q, integer *ldq, real *
	work, integer *lwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, q_dim1, q_offset;
    real r__1, r__2;

    /* Local variables */
    real eps, unfl, ovfl;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm;
    extern /* Subroutine */ int sort01_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *, real *);
    real wnorm;
    extern /* Subroutine */ int slabad_(real *, real *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *);
    integer ldwork;
    real smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SHST01 tests the reduction of a general matrix A to upper Hessenberg */
/*  form:  A = Q*H*Q'.  Two test ratios are computed; */

/*  RESULT(1) = norm( A - Q*H*Q' ) / ( norm(A) * N * EPS ) */
/*  RESULT(2) = norm( I - Q'*Q ) / ( N * EPS ) */

/*  The matrix Q is assumed to be given explicitly as it would be */
/*  following SGEHRD + SORGHR. */

/*  In this version, ILO and IHI are not used and are assumed to be 1 and */
/*  N, respectively. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          A is assumed to be upper triangular in rows and columns */
/*          1:ILO-1 and IHI+1:N, so Q differs from the identity only in */
/*          rows and columns ILO+1:IHI. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The original n by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  H       (input) REAL array, dimension (LDH,N) */
/*          The upper Hessenberg matrix H from the reduction A = Q*H*Q' */
/*          as computed by SGEHRD.  H is assumed to be zero below the */
/*          first subdiagonal. */

/*  LDH     (input) INTEGER */
/*          The leading dimension of the array H.  LDH >= max(1,N). */

/*  Q       (input) REAL array, dimension (LDQ,N) */
/*          The orthogonal matrix Q from the reduction A = Q*H*Q' as */
/*          computed by SGEHRD + SORGHR. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max(1,N). */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= 2*N*N. */

/*  RESULT  (output) REAL array, dimension (2) */
/*          RESULT(1) = norm( A - Q*H*Q' ) / ( norm(A) * N * EPS ) */
/*          RESULT(2) = norm( I - Q'*Q ) / ( N * EPS ) */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --work;
    --result;

    /* Function Body */
    if (*n <= 0) {
	result[1] = 0.f;
	result[2] = 0.f;
	return 0;
    }

    unfl = slamch_("Safe minimum");
    eps = slamch_("Precision");
    ovfl = 1.f / unfl;
    slabad_(&unfl, &ovfl);
    smlnum = unfl * *n / eps;

/*     Test 1:  Compute norm( A - Q*H*Q' ) / ( norm(A) * N * EPS ) */

/*     Copy A to WORK */

    ldwork = max(1,*n);
    slacpy_(" ", n, n, &a[a_offset], lda, &work[1], &ldwork);

/*     Compute Q*H */

    sgemm_("No transpose", "No transpose", n, n, n, &c_b7, &q[q_offset], ldq, 
	    &h__[h_offset], ldh, &c_b8, &work[ldwork * *n + 1], &ldwork);

/*     Compute A - Q*H*Q' */

    sgemm_("No transpose", "Transpose", n, n, n, &c_b11, &work[ldwork * *n + 
	    1], &ldwork, &q[q_offset], ldq, &c_b7, &work[1], &ldwork);

/* Computing MAX */
    r__1 = slange_("1", n, n, &a[a_offset], lda, &work[ldwork * *n + 1]);
    anorm = dmax(r__1,unfl);
    wnorm = slange_("1", n, n, &work[1], &ldwork, &work[ldwork * *n + 1]);

/*     Note that RESULT(1) cannot overflow and is bounded by 1/(N*EPS) */

/* Computing MAX */
    r__1 = smlnum, r__2 = anorm * eps;
    result[1] = dmin(wnorm,anorm) / dmax(r__1,r__2) / *n;

/*     Test 2:  Compute norm( I - Q'*Q ) / ( N * EPS ) */

    sort01_("Columns", n, n, &q[q_offset], ldq, &work[1], lwork, &result[2]);

    return 0;

/*     End of SHST01 */

} /* shst01_ */
