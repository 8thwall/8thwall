#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__10 = 10;
static integer c__1 = 1;
static complex c_b16 = {-1.f,0.f};

doublereal cqpt01_(integer *m, integer *n, integer *k, complex *a, complex *
	af, integer *lda, complex *tau, integer *jpvt, complex *work, integer 
	*lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, i__1, i__2, i__3, i__4;
    real ret_val;

    /* Local variables */
    integer i__, j, info;
    real norma;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *), caxpy_(integer *, complex *, complex *, 
	    integer *, complex *, integer *);
    real rwork[1];
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    extern /* Subroutine */ int xerbla_(char *, integer *), cunmqr_(
	    char *, char *, integer *, integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CQPT01 tests the QR-factorization with pivoting of a matrix A.  The */
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

/*  A       (input) COMPLEX array, dimension (LDA, N) */
/*          The original matrix A. */

/*  AF      (input) COMPLEX array, dimension (LDA,N) */
/*          The (possibly partial) output of CGEQPF.  The upper triangle */
/*          of AF(1:k,1:k) is a partial triangular factor, the entries */
/*          below the diagonal in the first k columns are the Householder */
/*          vectors, and the rest of AF contains a partially updated */
/*          matrix. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A and AF. */

/*  TAU     (input) COMPLEX array, dimension (K) */
/*          Details of the Householder transformations as returned by */
/*          CGEQPF. */

/*  JPVT    (input) INTEGER array, dimension (N) */
/*          Pivot information as returned by CGEQPF. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

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
	xerbla_("CQPT01", &c__10);
	return ret_val;
    }

/*     Quick return if possible */

    if (*m <= 0 || *n <= 0) {
	return ret_val;
    }

    norma = clange_("One-norm", m, n, &a[a_offset], lda, rwork);

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = min(j,*m);
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = (j - 1) * *m + i__;
	    i__4 = i__ + j * af_dim1;
	    work[i__3].r = af[i__4].r, work[i__3].i = af[i__4].i;
/* L10: */
	}
	i__2 = *m;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    i__3 = (j - 1) * *m + i__;
	    work[i__3].r = 0.f, work[i__3].i = 0.f;
/* L20: */
	}
/* L30: */
    }
    i__1 = *n;
    for (j = *k + 1; j <= i__1; ++j) {
	ccopy_(m, &af[j * af_dim1 + 1], &c__1, &work[(j - 1) * *m + 1], &c__1)
		;
/* L40: */
    }

    i__1 = *lwork - *m * *n;
    cunmqr_("Left", "No transpose", m, n, k, &af[af_offset], lda, &tau[1], &
	    work[1], m, &work[*m * *n + 1], &i__1, &info);

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {

/*        Compare i-th column of QR and jpvt(i)-th column of A */

	caxpy_(m, &c_b16, &a[jpvt[j] * a_dim1 + 1], &c__1, &work[(j - 1) * *m 
		+ 1], &c__1);
/* L50: */
    }

    ret_val = clange_("One-norm", m, n, &work[1], m, rwork) / ((
	    real) max(*m,*n) * slamch_("Epsilon"));
    if (norma != 0.f) {
	ret_val /= norma;
    }

    return ret_val;

/*     End of CQPT01 */

} /* cqpt01_ */
