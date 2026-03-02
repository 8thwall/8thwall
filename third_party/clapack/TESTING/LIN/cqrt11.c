#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__7 = 7;
static complex c_b5 = {0.f,0.f};
static complex c_b6 = {1.f,0.f};

doublereal cqrt11_(integer *m, integer *k, complex *a, integer *lda, complex *
	tau, complex *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    real ret_val;
    complex q__1;

    /* Local variables */
    integer j, info;
    extern /* Subroutine */ int cunm2r_(char *, char *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, integer *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    extern /* Subroutine */ int claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *), xerbla_(char *, 
	    integer *);
    real rdummy[1];


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CQRT11 computes the test ratio */

/*        || Q'*Q - I || / (eps * m) */

/*  where the orthogonal matrix Q is represented as a product of */
/*  elementary transformations.  Each transformation has the form */

/*     H(k) = I - tau(k) v(k) v(k)' */

/*  where tau(k) is stored in TAU(k) and v(k) is an m-vector of the form */
/*  [ 0 ... 0 1 x(k) ]', where x(k) is a vector of length m-k stored */
/*  in A(k+1:m,k). */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. */

/*  K       (input) INTEGER */
/*          The number of columns of A whose subdiagonal entries */
/*          contain information about orthogonal transformations. */

/*  A       (input) COMPLEX array, dimension (LDA,K) */
/*          The (possibly partial) output of a QR reduction routine. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  TAU     (input) COMPLEX array, dimension (K) */
/*          The scaling factors tau for the elementary transformations as */
/*          computed by the QR factorization routine. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= M*M + M. */

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
/*     .. Local Arrays .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    ret_val = 0.f;

/*     Test for sufficient workspace */

    if (*lwork < *m * *m + *m) {
	xerbla_("CQRT11", &c__7);
	return ret_val;
    }

/*     Quick return if possible */

    if (*m <= 0) {
	return ret_val;
    }

    claset_("Full", m, m, &c_b5, &c_b6, &work[1], m);

/*     Form Q */

    cunm2r_("Left", "No transpose", m, m, k, &a[a_offset], lda, &tau[1], &
	    work[1], m, &work[*m * *m + 1], &info);

/*     Form Q'*Q */

    cunm2r_("Left", "Conjugate transpose", m, m, k, &a[a_offset], lda, &tau[1]
, &work[1], m, &work[*m * *m + 1], &info);

    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	i__2 = (j - 1) * *m + j;
	i__3 = (j - 1) * *m + j;
	q__1.r = work[i__3].r - 1.f, q__1.i = work[i__3].i;
	work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L10: */
    }

    ret_val = clange_("One-norm", m, m, &work[1], m, rdummy) / ((
	    real) (*m) * slamch_("Epsilon"));

    return ret_val;

/*     End of CQRT11 */

} /* cqrt11_ */
