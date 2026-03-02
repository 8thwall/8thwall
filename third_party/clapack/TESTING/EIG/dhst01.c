#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b7 = 1.;
static doublereal c_b8 = 0.;
static doublereal c_b11 = -1.;

/* Subroutine */ int dhst01_(integer *n, integer *ilo, integer *ihi, 
	doublereal *a, integer *lda, doublereal *h__, integer *ldh, 
	doublereal *q, integer *ldq, doublereal *work, integer *lwork, 
	doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, q_dim1, q_offset;
    doublereal d__1, d__2;

    /* Local variables */
    doublereal eps, unfl, ovfl;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *),
	     dort01_(char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *);
    doublereal anorm, wnorm;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer ldwork;
    doublereal smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DHST01 tests the reduction of a general matrix A to upper Hessenberg */
/*  form:  A = Q*H*Q'.  Two test ratios are computed; */

/*  RESULT(1) = norm( A - Q*H*Q' ) / ( norm(A) * N * EPS ) */
/*  RESULT(2) = norm( I - Q'*Q ) / ( N * EPS ) */

/*  The matrix Q is assumed to be given explicitly as it would be */
/*  following DGEHRD + DORGHR. */

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

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The original n by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  H       (input) DOUBLE PRECISION array, dimension (LDH,N) */
/*          The upper Hessenberg matrix H from the reduction A = Q*H*Q' */
/*          as computed by DGEHRD.  H is assumed to be zero below the */
/*          first subdiagonal. */

/*  LDH     (input) INTEGER */
/*          The leading dimension of the array H.  LDH >= max(1,N). */

/*  Q       (input) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          The orthogonal matrix Q from the reduction A = Q*H*Q' as */
/*          computed by DGEHRD + DORGHR. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= 2*N*N. */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
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
	result[1] = 0.;
	result[2] = 0.;
	return 0;
    }

    unfl = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    ovfl = 1. / unfl;
    dlabad_(&unfl, &ovfl);
    smlnum = unfl * *n / eps;

/*     Test 1:  Compute norm( A - Q*H*Q' ) / ( norm(A) * N * EPS ) */

/*     Copy A to WORK */

    ldwork = max(1,*n);
    dlacpy_(" ", n, n, &a[a_offset], lda, &work[1], &ldwork);

/*     Compute Q*H */

    dgemm_("No transpose", "No transpose", n, n, n, &c_b7, &q[q_offset], ldq, 
	    &h__[h_offset], ldh, &c_b8, &work[ldwork * *n + 1], &ldwork);

/*     Compute A - Q*H*Q' */

    dgemm_("No transpose", "Transpose", n, n, n, &c_b11, &work[ldwork * *n + 
	    1], &ldwork, &q[q_offset], ldq, &c_b7, &work[1], &ldwork);

/* Computing MAX */
    d__1 = dlange_("1", n, n, &a[a_offset], lda, &work[ldwork * *n + 1]);
    anorm = max(d__1,unfl);
    wnorm = dlange_("1", n, n, &work[1], &ldwork, &work[ldwork * *n + 1]);

/*     Note that RESULT(1) cannot overflow and is bounded by 1/(N*EPS) */

/* Computing MAX */
    d__1 = smlnum, d__2 = anorm * eps;
    result[1] = min(wnorm,anorm) / max(d__1,d__2) / *n;

/*     Test 2:  Compute norm( I - Q'*Q ) / ( N * EPS ) */

    dort01_("Columns", n, n, &q[q_offset], ldq, &work[1], lwork, &result[2]);

    return 0;

/*     End of DHST01 */

} /* dhst01_ */
