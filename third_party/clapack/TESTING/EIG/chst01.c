#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b7 = {1.f,0.f};
static complex c_b8 = {0.f,0.f};
static complex c_b11 = {-1.f,0.f};

/* Subroutine */ int chst01_(integer *n, integer *ilo, integer *ihi, complex *
	a, integer *lda, complex *h__, integer *ldh, complex *q, integer *ldq, 
	 complex *work, integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, q_dim1, q_offset;
    real r__1, r__2;

    /* Local variables */
    real eps, unfl, ovfl;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *), cunt01_(char *, 
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    real *, real *);
    real anorm, wnorm;
    extern /* Subroutine */ int slabad_(real *, real *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *);
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

/*  CHST01 tests the reduction of a general matrix A to upper Hessenberg */
/*  form:  A = Q*H*Q'.  Two test ratios are computed; */

/*  RESULT(1) = norm( A - Q*H*Q' ) / ( norm(A) * N * EPS ) */
/*  RESULT(2) = norm( I - Q'*Q ) / ( N * EPS ) */

/*  The matrix Q is assumed to be given explicitly as it would be */
/*  following CGEHRD + CUNGHR. */

/*  In this version, ILO and IHI are not used, but they could be used */
/*  to save some work if this is desired. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          A is assumed to be upper triangular in rows and columns */
/*          1:ILO-1 and IHI+1:N, so Q differs from the identity only in */
/*          rows and columns ILO+1:IHI. */

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The original n by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  H       (input) COMPLEX array, dimension (LDH,N) */
/*          The upper Hessenberg matrix H from the reduction A = Q*H*Q' */
/*          as computed by CGEHRD.  H is assumed to be zero below the */
/*          first subdiagonal. */

/*  LDH     (input) INTEGER */
/*          The leading dimension of the array H.  LDH >= max(1,N). */

/*  Q       (input) COMPLEX array, dimension (LDQ,N) */
/*          The orthogonal matrix Q from the reduction A = Q*H*Q' as */
/*          computed by CGEHRD + CUNGHR. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max(1,N). */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= 2*N*N. */

/*  RWORK   (workspace) REAL array, dimension (N) */

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
    --rwork;
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
    clacpy_(" ", n, n, &a[a_offset], lda, &work[1], &ldwork);

/*     Compute Q*H */

    cgemm_("No transpose", "No transpose", n, n, n, &c_b7, &q[q_offset], ldq, 
	    &h__[h_offset], ldh, &c_b8, &work[ldwork * *n + 1], &ldwork);

/*     Compute A - Q*H*Q' */

    cgemm_("No transpose", "Conjugate transpose", n, n, n, &c_b11, &work[
	    ldwork * *n + 1], &ldwork, &q[q_offset], ldq, &c_b7, &work[1], &
	    ldwork);

/* Computing MAX */
    r__1 = clange_("1", n, n, &a[a_offset], lda, &rwork[1]);
    anorm = dmax(r__1,unfl);
    wnorm = clange_("1", n, n, &work[1], &ldwork, &rwork[1]);

/*     Note that RESULT(1) cannot overflow and is bounded by 1/(N*EPS) */

/* Computing MAX */
    r__1 = smlnum, r__2 = anorm * eps;
    result[1] = dmin(wnorm,anorm) / dmax(r__1,r__2) / *n;

/*     Test 2:  Compute norm( I - Q'*Q ) / ( N * EPS ) */

    cunt01_("Columns", n, n, &q[q_offset], ldq, &work[1], lwork, &rwork[1], &
	    result[2]);

    return 0;

/*     End of CHST01 */

} /* chst01_ */
