#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__16 = 16;
static integer c__2 = 2;
static integer c__1 = 1;
static complex c_b22 = {2.f,0.f};
static integer c__0 = 0;

/* Subroutine */ int cqrt15_(integer *scale, integer *rksel, integer *m, 
	integer *n, integer *nrhs, complex *a, integer *lda, complex *b, 
	integer *ldb, real *s, integer *rank, real *norma, real *normb, 
	integer *iseed, complex *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;
    real r__1;

    /* Local variables */
    integer j, mn;
    real eps;
    integer info;
    real temp;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *), clarf_(char *, 
	    integer *, integer *, complex *, integer *, complex *, complex *, 
	    integer *, complex *);
    extern doublereal sasum_(integer *, real *, integer *);
    real dummy[1];
    extern doublereal scnrm2_(integer *, complex *, integer *);
    extern /* Subroutine */ int slabad_(real *, real *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *);
    extern /* Subroutine */ int clascl_(char *, integer *, integer *, real *, 
	    real *, integer *, integer *, complex *, integer *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int csscal_(integer *, real *, complex *, integer 
	    *), claset_(char *, integer *, integer *, complex *, complex *, 
	    complex *, integer *), xerbla_(char *, integer *);
    real bignum;
    extern /* Subroutine */ int claror_(char *, char *, integer *, integer *, 
	    complex *, integer *, integer *, complex *, integer *);
    extern doublereal slarnd_(integer *, integer *);
    extern /* Subroutine */ int slaord_(char *, integer *, real *, integer *), clarnv_(integer *, integer *, integer *, complex *), 
	    slascl_(char *, integer *, integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *);
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

/*  CQRT15 generates a matrix with full or deficient rank and of various */
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

/*  A       (output) COMPLEX array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  B       (output) COMPLEX array, dimension (LDB, NRHS) */
/*          A matrix that is in the range space of matrix A. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. */

/*  S       (output) REAL array, dimension MIN(M,N) */
/*          Singular values of A. */

/*  RANK    (output) INTEGER */
/*          number of nonzero singular values of A. */

/*  NORMA   (output) REAL */
/*          one-norm norm of A. */

/*  NORMB   (output) REAL */
/*          one-norm norm of B. */

/*  ISEED   (input/output) integer array, dimension (4) */
/*          seed for random number generator. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

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
	xerbla_("CQRT15", &c__16);
	return 0;
    }

    smlnum = slamch_("Safe minimum");
    bignum = 1.f / smlnum;
    slabad_(&smlnum, &bignum);
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
	xerbla_("CQRT15", &c__2);
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

	clarnv_(&c__2, &iseed[1], m, &work[1]);
	r__1 = 1.f / scnrm2_(m, &work[1], &c__1);
	csscal_(m, &r__1, &work[1], &c__1);
	claset_("Full", m, rank, &c_b1, &c_b2, &a[a_offset], lda);
	clarf_("Left", m, rank, &work[1], &c__1, &c_b22, &a[a_offset], lda, &
		work[*m + 1]);

/*        workspace used: m+mn */

/*        Generate consistent rhs in the range space of A */

	i__1 = *rank * *nrhs;
	clarnv_(&c__2, &iseed[1], &i__1, &work[1]);
	cgemm_("No transpose", "No transpose", m, nrhs, rank, &c_b2, &a[
		a_offset], lda, &work[1], rank, &c_b1, &b[b_offset], ldb);

/*        work space used: <= mn *nrhs */

/*        generate (unscaled) matrix A */

	i__1 = *rank;
	for (j = 1; j <= i__1; ++j) {
	    csscal_(m, &s[j], &a[j * a_dim1 + 1], &c__1);
/* L40: */
	}
	if (*rank < *n) {
	    i__1 = *n - *rank;
	    claset_("Full", m, &i__1, &c_b1, &c_b1, &a[(*rank + 1) * a_dim1 + 
		    1], lda);
	}
	claror_("Right", "No initialization", m, n, &a[a_offset], lda, &iseed[
		1], &work[1], &info);

    } else {

/*        work space used 2*n+m */

/*        Generate null matrix and rhs */

	i__1 = mn;
	for (j = 1; j <= i__1; ++j) {
	    s[j] = 0.f;
/* L50: */
	}
	claset_("Full", m, n, &c_b1, &c_b1, &a[a_offset], lda);
	claset_("Full", m, nrhs, &c_b1, &c_b1, &b[b_offset], ldb);

    }

/*     Scale the matrix */

    if (*scale != 1) {
	*norma = clange_("Max", m, n, &a[a_offset], lda, dummy);
	if (*norma != 0.f) {
	    if (*scale == 2) {

/*              matrix scaled up */

		clascl_("General", &c__0, &c__0, norma, &bignum, m, n, &a[
			a_offset], lda, &info);
		slascl_("General", &c__0, &c__0, norma, &bignum, &mn, &c__1, &
			s[1], &mn, &info);
		clascl_("General", &c__0, &c__0, norma, &bignum, m, nrhs, &b[
			b_offset], ldb, &info);
	    } else if (*scale == 3) {

/*              matrix scaled down */

		clascl_("General", &c__0, &c__0, norma, &smlnum, m, n, &a[
			a_offset], lda, &info);
		slascl_("General", &c__0, &c__0, norma, &smlnum, &mn, &c__1, &
			s[1], &mn, &info);
		clascl_("General", &c__0, &c__0, norma, &smlnum, m, nrhs, &b[
			b_offset], ldb, &info);
	    } else {
		xerbla_("CQRT15", &c__1);
		return 0;
	    }
	}
    }

    *norma = sasum_(&mn, &s[1], &c__1);
    *normb = clange_("One-norm", m, nrhs, &b[b_offset], ldb, dummy)
	    ;

    return 0;

/*     End of CQRT15 */

} /* cqrt15_ */
