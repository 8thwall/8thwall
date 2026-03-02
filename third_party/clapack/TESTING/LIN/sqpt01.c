#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__10 = 10;
static integer c__1 = 1;
static real c_b14 = -1.f;

doublereal sqpt01_(integer *m, integer *n, integer *k, real *a, real *af, 
	integer *lda, real *tau, integer *jpvt, real *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, i__1, i__2;
    real ret_val;

    /* Local variables */
    integer i__, j, info;
    real norma;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    real rwork[1];
    extern /* Subroutine */ int saxpy_(integer *, real *, real *, integer *, 
	    real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int xerbla_(char *, integer *), sormqr_(
	    char *, char *, integer *, integer *, integer *, real *, integer *
, real *, real *, integer *, real *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SQPT01 tests the QR-factorization with pivoting of a matrix A.  The */
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

/*  A       (input) REAL array, dimension (LDA, N) */
/*          The original matrix A. */

/*  AF      (input) REAL array, dimension (LDA,N) */
/*          The (possibly partial) output of SGEQPF.  The upper triangle */
/*          of AF(1:k,1:k) is a partial triangular factor, the entries */
/*          below the diagonal in the first k columns are the Householder */
/*          vectors, and the rest of AF contains a partially updated */
/*          matrix. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A and AF. */

/*  TAU     (input) REAL array, dimension (K) */
/*          Details of the Householder transformations as returned by */
/*          SGEQPF. */

/*  JPVT    (input) INTEGER array, dimension (N) */
/*          Pivot information as returned by SGEQPF. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

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
    ret_val = 0.f;

/*     Test if there is enough workspace */

    if (*lwork < *m * *n + *n) {
	xerbla_("SQPT01", &c__10);
	return ret_val;
    }

/*     Quick return if possible */

    if (*m <= 0 || *n <= 0) {
	return ret_val;
    }

    norma = slange_("One-norm", m, n, &a[a_offset], lda, rwork);

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = min(j,*m);
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[(j - 1) * *m + i__] = af[i__ + j * af_dim1];
/* L10: */
	}
	i__2 = *m;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    work[(j - 1) * *m + i__] = 0.f;
/* L20: */
	}
/* L30: */
    }
    i__1 = *n;
    for (j = *k + 1; j <= i__1; ++j) {
	scopy_(m, &af[j * af_dim1 + 1], &c__1, &work[(j - 1) * *m + 1], &c__1)
		;
/* L40: */
    }

    i__1 = *lwork - *m * *n;
    sormqr_("Left", "No transpose", m, n, k, &af[af_offset], lda, &tau[1], &
	    work[1], m, &work[*m * *n + 1], &i__1, &info);

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {

/*        Compare i-th column of QR and jpvt(i)-th column of A */

	saxpy_(m, &c_b14, &a[jpvt[j] * a_dim1 + 1], &c__1, &work[(j - 1) * *m 
		+ 1], &c__1);
/* L50: */
    }

    ret_val = slange_("One-norm", m, n, &work[1], m, rwork) / ((
	    real) max(*m,*n) * slamch_("Epsilon"));
    if (norma != 0.f) {
	ret_val /= norma;
    }

    return ret_val;

/*     End of SQPT01 */

} /* sqpt01_ */
