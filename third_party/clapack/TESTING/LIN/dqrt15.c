#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__16 = 16;
static integer c__2 = 2;
static integer c__1 = 1;
static doublereal c_b18 = 0.;
static doublereal c_b19 = 1.;
static doublereal c_b22 = 2.;
static integer c__0 = 0;

/* Subroutine */ int dqrt15_(integer *scale, integer *rksel, integer *m, 
	integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, 
	 integer *ldb, doublereal *s, integer *rank, doublereal *norma, 
	doublereal *normb, integer *iseed, doublereal *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    integer j, mn;
    doublereal eps;
    integer info;
    doublereal temp;
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *, 
	    integer *), dlarf_(char *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *), dgemm_(char *, char *, integer *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal dummy[1];
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlascl_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, integer *);
    extern doublereal dlarnd_(integer *, integer *);
    extern /* Subroutine */ int dlaord_(char *, integer *, doublereal *, 
	    integer *), dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    xerbla_(char *, integer *);
    doublereal bignum;
    extern /* Subroutine */ int dlaror_(char *, char *, integer *, integer *, 
	    doublereal *, integer *, integer *, doublereal *, integer *), dlarnv_(integer *, integer *, integer *, 
	    doublereal *);
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

/*  DQRT15 generates a matrix with full or deficient rank and of various */
/*  norms. */

/*  Arguments */
/*  ========= */

/*  SCALE   (input) INTEGER */
/*          SCALE = 1: normally scaled matrix */
/*          SCALE = 2: matrix scaled up */
/*          SCALE = 3: matrix scaled down */

/*  RKSEL   (input) INTEGER */
/*          RKSEL = 1: full rank matrix */
/*          RKSEL = 2: rank-deficient matrix */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. */

/*  N       (input) INTEGER */
/*          The number of columns of A. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of B. */

/*  A       (output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  B       (output) DOUBLE PRECISION array, dimension (LDB, NRHS) */
/*          A matrix that is in the range space of matrix A. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. */

/*  S       (output) DOUBLE PRECISION array, dimension MIN(M,N) */
/*          Singular values of A. */

/*  RANK    (output) INTEGER */
/*          number of nonzero singular values of A. */

/*  NORMA   (output) DOUBLE PRECISION */
/*          one-norm of A. */

/*  NORMB   (output) DOUBLE PRECISION */
/*          one-norm of B. */

/*  ISEED   (input/output) integer array, dimension (4) */
/*          seed for random number generator. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          length of work space required. */
/*          LWORK >= MAX(M+MIN(M,N),NRHS*MIN(M,N),2*N+M) */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --s;
    --iseed;
    --work;

    /* Function Body */
    mn = min(*m,*n);
/* Computing MAX */
    i__1 = *m + mn, i__2 = mn * *nrhs, i__1 = max(i__1,i__2), i__2 = (*n << 1)
	     + *m;
    if (*lwork < max(i__1,i__2)) {
	xerbla_("DQRT15", &c__16);
	return 0;
    }

    smlnum = dlamch_("Safe minimum");
    bignum = 1. / smlnum;
    eps = dlamch_("Epsilon");
    smlnum = smlnum / eps / eps;
    bignum = 1. / smlnum;

/*     Determine rank and (unscaled) singular values */

    if (*rksel == 1) {
	*rank = mn;
    } else if (*rksel == 2) {
	*rank = mn * 3 / 4;
	i__1 = mn;
	for (j = *rank + 1; j <= i__1; ++j) {
	    s[j] = 0.;
/* L10: */
	}
    } else {
	xerbla_("DQRT15", &c__2);
    }

    if (*rank > 0) {

/*        Nontrivial case */

	s[1] = 1.;
	i__1 = *rank;
	for (j = 2; j <= i__1; ++j) {
L20:
	    temp = dlarnd_(&c__1, &iseed[1]);
	    if (temp > .1) {
		s[j] = abs(temp);
	    } else {
		goto L20;
	    }
/* L30: */
	}
	dlaord_("Decreasing", rank, &s[1], &c__1);

/*        Generate 'rank' columns of a random orthogonal matrix in A */

	dlarnv_(&c__2, &iseed[1], m, &work[1]);
	d__1 = 1. / dnrm2_(m, &work[1], &c__1);
	dscal_(m, &d__1, &work[1], &c__1);
	dlaset_("Full", m, rank, &c_b18, &c_b19, &a[a_offset], lda)
		;
	dlarf_("Left", m, rank, &work[1], &c__1, &c_b22, &a[a_offset], lda, &
		work[*m + 1]);

/*        workspace used: m+mn */

/*        Generate consistent rhs in the range space of A */

	i__1 = *rank * *nrhs;
	dlarnv_(&c__2, &iseed[1], &i__1, &work[1]);
	dgemm_("No transpose", "No transpose", m, nrhs, rank, &c_b19, &a[
		a_offset], lda, &work[1], rank, &c_b18, &b[b_offset], ldb);

/*        work space used: <= mn *nrhs */

/*        generate (unscaled) matrix A */

	i__1 = *rank;
	for (j = 1; j <= i__1; ++j) {
	    dscal_(m, &s[j], &a[j * a_dim1 + 1], &c__1);
/* L40: */
	}
	if (*rank < *n) {
	    i__1 = *n - *rank;
	    dlaset_("Full", m, &i__1, &c_b18, &c_b18, &a[(*rank + 1) * a_dim1 
		    + 1], lda);
	}
	dlaror_("Right", "No initialization", m, n, &a[a_offset], lda, &iseed[
		1], &work[1], &info);

    } else {

/*        work space used 2*n+m */

/*        Generate null matrix and rhs */

	i__1 = mn;
	for (j = 1; j <= i__1; ++j) {
	    s[j] = 0.;
/* L50: */
	}
	dlaset_("Full", m, n, &c_b18, &c_b18, &a[a_offset], lda);
	dlaset_("Full", m, nrhs, &c_b18, &c_b18, &b[b_offset], ldb)
		;

    }

/*     Scale the matrix */

    if (*scale != 1) {
	*norma = dlange_("Max", m, n, &a[a_offset], lda, dummy);
	if (*norma != 0.) {
	    if (*scale == 2) {

/*              matrix scaled up */

		dlascl_("General", &c__0, &c__0, norma, &bignum, m, n, &a[
			a_offset], lda, &info);
		dlascl_("General", &c__0, &c__0, norma, &bignum, &mn, &c__1, &
			s[1], &mn, &info);
		dlascl_("General", &c__0, &c__0, norma, &bignum, m, nrhs, &b[
			b_offset], ldb, &info);
	    } else if (*scale == 3) {

/*              matrix scaled down */

		dlascl_("General", &c__0, &c__0, norma, &smlnum, m, n, &a[
			a_offset], lda, &info);
		dlascl_("General", &c__0, &c__0, norma, &smlnum, &mn, &c__1, &
			s[1], &mn, &info);
		dlascl_("General", &c__0, &c__0, norma, &smlnum, m, nrhs, &b[
			b_offset], ldb, &info);
	    } else {
		xerbla_("DQRT15", &c__1);
		return 0;
	    }
	}
    }

    *norma = dasum_(&mn, &s[1], &c__1);
    *normb = dlange_("One-norm", m, nrhs, &b[b_offset], ldb, dummy)
	    ;

    return 0;

/*     End of DQRT15 */

} /* dqrt15_ */
