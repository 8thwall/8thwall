#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__16 = 16;
static integer c__2 = 2;
static integer c__1 = 1;
static real c_b18 = 0.f;
static real c_b19 = 1.f;
static real c_b22 = 2.f;
static integer c__0 = 0;

/* Subroutine */ int sqrt15_(integer *scale, integer *rksel, integer *m, 
	integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *
	ldb, real *s, integer *rank, real *norma, real *normb, integer *iseed, 
	 real *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;
    real r__1;

    /* Local variables */
    integer j, mn;
    real eps;
    integer info;
    real temp;
    extern doublereal snrm2_(integer *, real *, integer *);
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *), 
	    slarf_(char *, integer *, integer *, real *, integer *, real *, 
	    real *, integer *, real *), sgemm_(char *, char *, 
	    integer *, integer *, integer *, real *, real *, integer *, real *
, integer *, real *, real *, integer *);
    extern doublereal sasum_(integer *, real *, integer *);
    real dummy[1];
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    real bignum;
    extern /* Subroutine */ int slascl_(char *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, integer *);
    extern doublereal slarnd_(integer *, integer *);
    extern /* Subroutine */ int slaord_(char *, integer *, real *, integer *), slaset_(char *, integer *, integer *, real *, real *, 
	    real *, integer *), slaror_(char *, char *, integer *, 
	    integer *, real *, integer *, integer *, real *, integer *), slarnv_(integer *, integer *, integer *, real *);
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

/*  SQRT15 generates a matrix with full or deficient rank and of various */
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

/*  A       (output) REAL array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  B       (output) REAL array, dimension (LDB, NRHS) */
/*          A matrix that is in the range space of matrix A. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. */

/*  S       (output) REAL array, dimension MIN(M,N) */
/*          Singular values of A. */

/*  RANK    (output) INTEGER */
/*          number of nonzero singular values of A. */

/*  NORMA   (output) REAL */
/*          one-norm of A. */

/*  NORMB   (output) REAL */
/*          one-norm of B. */

/*  ISEED   (input/output) integer array, dimension (4) */
/*          seed for random number generator. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

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
	xerbla_("SQRT15", &c__16);
	return 0;
    }

    smlnum = slamch_("Safe minimum");
    bignum = 1.f / smlnum;
    eps = slamch_("Epsilon");
    smlnum = smlnum / eps / eps;
    bignum = 1.f / smlnum;

/*     Determine rank and (unscaled) singular values */

    if (*rksel == 1) {
	*rank = mn;
    } else if (*rksel == 2) {
	*rank = mn * 3 / 4;
	i__1 = mn;
	for (j = *rank + 1; j <= i__1; ++j) {
	    s[j] = 0.f;
/* L10: */
	}
    } else {
	xerbla_("SQRT15", &c__2);
    }

    if (*rank > 0) {

/*        Nontrivial case */

	s[1] = 1.f;
	i__1 = *rank;
	for (j = 2; j <= i__1; ++j) {
L20:
	    temp = slarnd_(&c__1, &iseed[1]);
	    if (temp > .1f) {
		s[j] = dabs(temp);
	    } else {
		goto L20;
	    }
/* L30: */
	}
	slaord_("Decreasing", rank, &s[1], &c__1);

/*        Generate 'rank' columns of a random orthogonal matrix in A */

	slarnv_(&c__2, &iseed[1], m, &work[1]);
	r__1 = 1.f / snrm2_(m, &work[1], &c__1);
	sscal_(m, &r__1, &work[1], &c__1);
	slaset_("Full", m, rank, &c_b18, &c_b19, &a[a_offset], lda)
		;
	slarf_("Left", m, rank, &work[1], &c__1, &c_b22, &a[a_offset], lda, &
		work[*m + 1]);

/*        workspace used: m+mn */

/*        Generate consistent rhs in the range space of A */

	i__1 = *rank * *nrhs;
	slarnv_(&c__2, &iseed[1], &i__1, &work[1]);
	sgemm_("No transpose", "No transpose", m, nrhs, rank, &c_b19, &a[
		a_offset], lda, &work[1], rank, &c_b18, &b[b_offset], ldb);

/*        work space used: <= mn *nrhs */

/*        generate (unscaled) matrix A */

	i__1 = *rank;
	for (j = 1; j <= i__1; ++j) {
	    sscal_(m, &s[j], &a[j * a_dim1 + 1], &c__1);
/* L40: */
	}
	if (*rank < *n) {
	    i__1 = *n - *rank;
	    slaset_("Full", m, &i__1, &c_b18, &c_b18, &a[(*rank + 1) * a_dim1 
		    + 1], lda);
	}
	slaror_("Right", "No initialization", m, n, &a[a_offset], lda, &iseed[
		1], &work[1], &info);

    } else {

/*        work space used 2*n+m */

/*        Generate null matrix and rhs */

	i__1 = mn;
	for (j = 1; j <= i__1; ++j) {
	    s[j] = 0.f;
/* L50: */
	}
	slaset_("Full", m, n, &c_b18, &c_b18, &a[a_offset], lda);
	slaset_("Full", m, nrhs, &c_b18, &c_b18, &b[b_offset], ldb)
		;

    }

/*     Scale the matrix */

    if (*scale != 1) {
	*norma = slange_("Max", m, n, &a[a_offset], lda, dummy);
	if (*norma != 0.f) {
	    if (*scale == 2) {

/*              matrix scaled up */

		slascl_("General", &c__0, &c__0, norma, &bignum, m, n, &a[
			a_offset], lda, &info);
		slascl_("General", &c__0, &c__0, norma, &bignum, &mn, &c__1, &
			s[1], &mn, &info);
		slascl_("General", &c__0, &c__0, norma, &bignum, m, nrhs, &b[
			b_offset], ldb, &info);
	    } else if (*scale == 3) {

/*              matrix scaled down */

		slascl_("General", &c__0, &c__0, norma, &smlnum, m, n, &a[
			a_offset], lda, &info);
		slascl_("General", &c__0, &c__0, norma, &smlnum, &mn, &c__1, &
			s[1], &mn, &info);
		slascl_("General", &c__0, &c__0, norma, &smlnum, m, nrhs, &b[
			b_offset], ldb, &info);
	    } else {
		xerbla_("SQRT15", &c__1);
		return 0;
	    }
	}
    }

    *norma = sasum_(&mn, &s[1], &c__1);
    *normb = slange_("One-norm", m, nrhs, &b[b_offset], ldb, dummy)
	    ;

    return 0;

/*     End of SQRT15 */

} /* sqrt15_ */
