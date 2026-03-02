#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__10 = 10;
static integer c__1 = 1;
static doublereal c_b14 = -1.;

doublereal dqpt01_(integer *m, integer *n, integer *k, doublereal *a, 
	doublereal *af, integer *lda, doublereal *tau, integer *jpvt, 
	doublereal *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, i__1, i__2;
    doublereal ret_val;

    /* Local variables */
    integer i__, j, info;
    doublereal norma;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *), daxpy_(integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal rwork[1];
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int xerbla_(char *, integer *), dormqr_(
	    char *, char *, integer *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DQPT01 tests the QR-factorization with pivoting of a matrix A.  The */
/*  array AF contains the (possibly partial) QR-factorization of A, where */
/*  the upper triangle of AF(1:k,1:k) is a partial triangular factor, */
/*  the entries below the diagonal in the first k columns are the */
/*  Householder vectors, and the rest of AF contains a partially updated */
/*  matrix. */

/*  This function returns ||A*P - Q*R||/(||norm(A)||*eps*M) */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrices A and AF. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and AF. */

/*  K       (input) INTEGER */
/*          The number of columns of AF that have been reduced */
/*          to upper triangular form. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA, N) */
/*          The original matrix A. */

/*  AF      (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The (possibly partial) output of DGEQPF.  The upper triangle */
/*          of AF(1:k,1:k) is a partial triangular factor, the entries */
/*          below the diagonal in the first k columns are the Householder */
/*          vectors, and the rest of AF contains a partially updated */
/*          matrix. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A and AF. */

/*  TAU     (input) DOUBLE PRECISION array, dimension (K) */
/*          Details of the Householder transformations as returned by */
/*          DGEQPF. */

/*  JPVT    (input) INTEGER array, dimension (N) */
/*          Pivot information as returned by DGEQPF. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= M*N+N. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    af_dim1 = *lda;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --jpvt;
    --work;

    /* Function Body */
    ret_val = 0.;

/*     Test if there is enough workspace */

    if (*lwork < *m * *n + *n) {
	xerbla_("DQPT01", &c__10);
	return ret_val;
    }

/*     Quick return if possible */

    if (*m <= 0 || *n <= 0) {
	return ret_val;
    }

    norma = dlange_("One-norm", m, n, &a[a_offset], lda, rwork);

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = min(j,*m);
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[(j - 1) * *m + i__] = af[i__ + j * af_dim1];
/* L10: */
	}
	i__2 = *m;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    work[(j - 1) * *m + i__] = 0.;
/* L20: */
	}
/* L30: */
    }
    i__1 = *n;
    for (j = *k + 1; j <= i__1; ++j) {
	dcopy_(m, &af[j * af_dim1 + 1], &c__1, &work[(j - 1) * *m + 1], &c__1)
		;
/* L40: */
    }

    i__1 = *lwork - *m * *n;
    dormqr_("Left", "No transpose", m, n, k, &af[af_offset], lda, &tau[1], &
	    work[1], m, &work[*m * *n + 1], &i__1, &info);

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {

/*        Compare i-th column of QR and jpvt(i)-th column of A */

	daxpy_(m, &c_b14, &a[jpvt[j] * a_dim1 + 1], &c__1, &work[(j - 1) * *m 
		+ 1], &c__1);
/* L50: */
    }

    ret_val = dlange_("One-norm", m, n, &work[1], m, rwork) / ((
	    doublereal) max(*m,*n) * dlamch_("Epsilon"));
    if (norma != 0.) {
	ret_val /= norma;
    }

    return ret_val;

/*     End of DQPT01 */

} /* dqpt01_ */
