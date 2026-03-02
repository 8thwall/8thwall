#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};
static integer c__16 = 16;
static integer c__2 = 2;
static integer c__1 = 1;
static doublecomplex c_b22 = {2.,0.};
static integer c__0 = 0;

/* Subroutine */ int zqrt15_(integer *scale, integer *rksel, integer *m, 
	integer *n, integer *nrhs, doublecomplex *a, integer *lda, 
	doublecomplex *b, integer *ldb, doublereal *s, integer *rank, 
	doublereal *norma, doublereal *normb, integer *iseed, doublecomplex *
	work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    integer j, mn;
    doublereal eps;
    integer info;
    doublereal temp;
    extern doublereal dasum_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int zlarf_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *), zgemm_(char *, char *, 
	    integer *, integer *, integer *, doublecomplex *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublecomplex *, 
	    doublecomplex *, integer *);
    doublereal dummy[1];
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dznrm2_(integer *, doublecomplex *, integer *), dlamch_(
	    char *);
    extern /* Subroutine */ int dlascl_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, integer *);
    extern doublereal dlarnd_(integer *, integer *);
    extern /* Subroutine */ int dlaord_(char *, integer *, doublereal *, 
	    integer *), xerbla_(char *, integer *);
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    doublereal bignum;
    extern /* Subroutine */ int zdscal_(integer *, doublereal *, 
	    doublecomplex *, integer *), zlascl_(char *, integer *, integer *, 
	     doublereal *, doublereal *, integer *, integer *, doublecomplex *
, integer *, integer *), zlaset_(char *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    integer *), zlaror_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, integer *, doublecomplex *, integer *);
    doublereal smlnum;
    extern /* Subroutine */ int zlarnv_(integer *, integer *, integer *, 
	    doublecomplex *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZQRT15 generates a matrix with full or deficient rank and of various */
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

/*  A       (output) COMPLEX*16 array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  B       (output) COMPLEX*16 array, dimension (LDB, NRHS) */
/*          A matrix that is in the range space of matrix A. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. */

/*  S       (output) DOUBLE PRECISION array, dimension MIN(M,N) */
/*          Singular values of A. */

/*  RANK    (output) INTEGER */
/*          number of nonzero singular values of A. */

/*  NORMA   (output) DOUBLE PRECISION */
/*          one-norm norm of A. */

/*  NORMB   (output) DOUBLE PRECISION */
/*          one-norm norm of B. */

/*  ISEED   (input/output) integer array, dimension (4) */
/*          seed for random number generator. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

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
	xerbla_("ZQRT15", &c__16);
	return 0;
    }

    smlnum = dlamch_("Safe minimum");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
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
	xerbla_("ZQRT15", &c__2);
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

	zlarnv_(&c__2, &iseed[1], m, &work[1]);
	d__1 = 1. / dznrm2_(m, &work[1], &c__1);
	zdscal_(m, &d__1, &work[1], &c__1);
	zlaset_("Full", m, rank, &c_b1, &c_b2, &a[a_offset], lda);
	zlarf_("Left", m, rank, &work[1], &c__1, &c_b22, &a[a_offset], lda, &
		work[*m + 1]);

/*        workspace used: m+mn */

/*        Generate consistent rhs in the range space of A */

	i__1 = *rank * *nrhs;
	zlarnv_(&c__2, &iseed[1], &i__1, &work[1]);
	zgemm_("No transpose", "No transpose", m, nrhs, rank, &c_b2, &a[
		a_offset], lda, &work[1], rank, &c_b1, &b[b_offset], ldb);

/*        work space used: <= mn *nrhs */

/*        generate (unscaled) matrix A */

	i__1 = *rank;
	for (j = 1; j <= i__1; ++j) {
	    zdscal_(m, &s[j], &a[j * a_dim1 + 1], &c__1);
/* L40: */
	}
	if (*rank < *n) {
	    i__1 = *n - *rank;
	    zlaset_("Full", m, &i__1, &c_b1, &c_b1, &a[(*rank + 1) * a_dim1 + 
		    1], lda);
	}
	zlaror_("Right", "No initialization", m, n, &a[a_offset], lda, &iseed[
		1], &work[1], &info);

    } else {

/*        work space used 2*n+m */

/*        Generate null matrix and rhs */

	i__1 = mn;
	for (j = 1; j <= i__1; ++j) {
	    s[j] = 0.;
/* L50: */
	}
	zlaset_("Full", m, n, &c_b1, &c_b1, &a[a_offset], lda);
	zlaset_("Full", m, nrhs, &c_b1, &c_b1, &b[b_offset], ldb);

    }

/*     Scale the matrix */

    if (*scale != 1) {
	*norma = zlange_("Max", m, n, &a[a_offset], lda, dummy);
	if (*norma != 0.) {
	    if (*scale == 2) {

/*              matrix scaled up */

		zlascl_("General", &c__0, &c__0, norma, &bignum, m, n, &a[
			a_offset], lda, &info);
		dlascl_("General", &c__0, &c__0, norma, &bignum, &mn, &c__1, &
			s[1], &mn, &info);
		zlascl_("General", &c__0, &c__0, norma, &bignum, m, nrhs, &b[
			b_offset], ldb, &info);
	    } else if (*scale == 3) {

/*              matrix scaled down */

		zlascl_("General", &c__0, &c__0, norma, &smlnum, m, n, &a[
			a_offset], lda, &info);
		dlascl_("General", &c__0, &c__0, norma, &smlnum, &mn, &c__1, &
			s[1], &mn, &info);
		zlascl_("General", &c__0, &c__0, norma, &smlnum, m, nrhs, &b[
			b_offset], ldb, &info);
	    } else {
		xerbla_("ZQRT15", &c__1);
		return 0;
	    }
	}
    }

    *norma = dasum_(&mn, &s[1], &c__1);
    *normb = zlange_("One-norm", m, nrhs, &b[b_offset], ldb, dummy)
	    ;

    return 0;

/*     End of ZQRT15 */

} /* zqrt15_ */
